#include <stdio.h>
#include <fcntl.h>
#include <vector>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>

#include <string.h>

#include <sys/resource.h>

#include <errno.h>

using namespace std;

//#include "convert.h"
#include "mb_vision/BlobImageProcessorYUV.h"
//#include "mb_vision/RGB24Image.h"
#include "mb_vision/YUVImage.h"

#define VIDEO0_BUFFER "/tmp/video0_buffer"
#define VIDEO0_READY "/tmp/video0_ready"

#define VIDEO1_BUFFER "/tmp/video1_buffer"
#define VIDEO1_READY "/tmp/video1_ready"

#define VIDEO0_MARKED_BUFFER "/tmp/video0_marked_buffer"
#define VIDEO0_MARKED_READY "/tmp/video0_marked_ready"

#define VIDEO1_MARKED_BUFFER "/tmp/video1_marked_buffer"
#define VIDEO1_MARKED_READY "/tmp/video1_marked_ready"

#define VIDEO0_ENABLE_0 "/tmp/video0_enable_0"
#define VIDEO0_ENABLE_1 "/tmp/video0_enable_1"
#define VIDEO0_ENABLE_2 "/tmp/video0_enable_2"
#define VIDEO0_ENABLE_3 "/tmp/video0_enable_3"

#define VIDEO1_ENABLE_0 "/tmp/video1_enable_0"
#define VIDEO1_ENABLE_1 "/tmp/video1_enable_1"
#define VIDEO1_ENABLE_2 "/tmp/video1_enable_2"
#define VIDEO1_ENABLE_3 "/tmp/video1_enable_3"

//unsigned char video0_yuv[640*480*3/2];
//unsigned char video1_yuv[176*144*3/2];

//unsigned char video0_rgb[640*480*3];
//unsigned char video1_rgb[176*144*3];

// Keeps track of whether blob tracking is enabled for the cameras; up to 4 color models per camera.
bool video0_enable[4];
bool video1_enable[4];

bool video0_bluemarker[4];
bool video1_bluemarker[4];

// blob tracking data
vector<Blob> video0_blobs_found[4];
vector<Blob> video1_blobs_found[4];

// mb_vision objects

//RGB24Image* video0_visionImageWrapper;
//RGB24Image* video1_visionImageWrapper;

YUVImage* video0_visionImageWrapper;
YUVImage* video1_visionImageWrapper;

BlobImageProcessorYUV* video0_blobTracker[4];
BlobImageProcessorYUV* video1_blobTracker[4];

ColorModelYUV* video0_model[4];
ColorModelYUV* video1_model[4];

Blob *udp_output;

// This function borrowed from libcbc
void msleep(long mtime)
{
    struct timespec time;

    time.tv_sec = (mtime/1000);
    time.tv_nsec = (mtime%1000)*1000000;

    nanosleep(&time, 0);
}

bool video0_enabled_on_any_model()
{
	return video0_enable[0] || video0_enable[1] || video0_enable[2] || video0_enable[3];
}

bool video1_enabled_on_any_model()
{
	return video1_enable[0] || video1_enable[1] || video1_enable[2] || video1_enable[3];
}

int sock, sock_ret;
unsigned int sock_length;
struct sockaddr_in server, from;
struct hostent *hp;

void init_udp()
{
    printf("Init UDP socket...\n");

   sock= socket(AF_INET, SOCK_DGRAM, 0);
   if (sock < 0) printf("socket error\n");

   server.sin_family = AF_INET;
   //hp = gethostbyname("255.255.255.255");
   hp = gethostbyname("192.168.1.2");
   if (hp==0) printf("Unknown host\n");

   memcpy( (char *)&server.sin_addr,
          (char *)hp->h_addr,
         hp->h_length);
   server.sin_port = htons(16720); // 16720 = decimal for "AP" (AR.Pwn)
   sock_length=sizeof(struct sockaddr_in);
}

void send_udp()
{
    sock_ret =sendto(sock,(char *) udp_output,
            512 /*sizeof(udp_output)*/,0,(const struct sockaddr *)&server,sock_length);
   if (sock_ret < 0)
   {
       printf("UDP send error errno=%d\n", errno);
   }
   else
   {
       //printf("%d bytes sent\n", sizeof(udp_output));
   }
}

int main()
{
    bool new_data_available = false;

	// Initialize RGB / YUV tables
	//InitLookupTable();
	//InitConvertTable();

	//video0_visionImageWrapper = new RGB24Image(640, 480);
	//video1_visionImageWrapper = new RGB24Image(176, 144);

    // Set priority to a low value
    setpriority(PRIO_PROCESS, 0, 19); // 19 = least CPU time
	printf("Priority = %d\n", getpriority(PRIO_PROCESS, 0));

    //printf("Start of main()\n");

	// Chroma resolution is half of the raw resolution in each dimension
	video0_visionImageWrapper = new YUVImage(640/2, 480/2);
	video1_visionImageWrapper = new YUVImage(176/2, 144/2);

    //printf("Init vars\n");

    udp_output = new Blob[2*4*4]; // 4 blobs, 4 color channels, 2 cameras

    Blob blankBlob;

    blankBlob.x=0;
    blankBlob.y=0;
    blankBlob.w=0;
    blankBlob.h=0;

	// Initialize variables
	for(int index=0; index<4; index++)
	{
		video0_enable[index] = false;
		video1_enable[index] = false;

        video0_bluemarker[index] = true;
        video1_bluemarker[index] = true;

        // The last argument is for the bluemarker... should be false by default once the config features are done
		video0_blobTracker[index] = new BlobImageProcessorYUV(10, 8, 8, false, video0_bluemarker[index]); // size limit was 12x12
		video1_blobTracker[index] = new BlobImageProcessorYUV(10, 8, 8, false, video1_bluemarker[index]); // size limit was 12x12

        if(index==0)
        {
            video0_model[index] = new ColorModelYUV(75, 202, 21, 117, 10, 106); // green
            video1_model[index] = new ColorModelYUV(75, 202, 21, 117, 10, 106); // green
        }
        else if(index==1)
        {
            // RGB: 150,81,115; 150,81,92; 150,33,91; 150,33,52; 232,51,141; 232,51,81; 232,125,178; 232,125,143

            video0_model[index] = new ColorModelYUV(70, 163, 112, 142, 159, 216); // pink
            video1_model[index] = new ColorModelYUV(70, 163, 112, 142, 159, 216); // pink

            //video0_model[index] = new ColorModelYUV(121, 188, 95, 122, 175, 223); // pink
            //video1_model[index] = new ColorModelYUV(121, 188, 95, 122, 175, 223); // pink
        }
        else
        {
            video0_model[index] = new ColorModelYUV(75, 202, 21, 117, 10, 106); // green
            video1_model[index] = new ColorModelYUV(75, 202, 21, 117, 10, 106); // green
        }


		video0_blobTracker[index]->addColorModel(video0_model[index]);
		video1_blobTracker[index]->addColorModel(video1_model[index]);
	}

    init_udp();

    printf("Start main loop\n");

	// Main loop
	while(1)
	{
		// Check if blob tracking is enabled

		video0_enable[0] = (access(VIDEO0_ENABLE_0, F_OK) != -1);
		video0_enable[1] = (access(VIDEO0_ENABLE_1, F_OK) != -1);
		video0_enable[2] = (access(VIDEO0_ENABLE_2, F_OK) != -1);
		video0_enable[3] = (access(VIDEO0_ENABLE_3, F_OK) != -1);

		video1_enable[0] = (access(VIDEO1_ENABLE_0, F_OK) != -1);
		video1_enable[1] = (access(VIDEO1_ENABLE_1, F_OK) != -1);
		video1_enable[2] = (access(VIDEO1_ENABLE_2, F_OK) != -1);
		video1_enable[3] = (access(VIDEO1_ENABLE_3, F_OK) != -1);

        //printf("Checking video0\n");

		// If video0 is ready
		if( (access(VIDEO0_READY, F_OK) != -1) && video0_enabled_on_any_model() )
		{
		    new_data_available = true;

			//read_video0_yuv();
			int buffer_file = open(VIDEO0_BUFFER, O_RDONLY);
			read(buffer_file, video0_visionImageWrapper->getBuffer(), 640*480*3/2);
			close(buffer_file);

			//ConvertYUV2RGB(video0_yuv, video0_yuv+(640*480), video0_yuv+(640*480*5/4), video0_visionImageWrapper->getBuffer(), 640, 480);

            // For each color channel
			for(int index=0; index<4; index++)
			{
				if(video0_enable[index])
				{

                    // Do blob tracking, get results
					video0_blobTracker[index]->processImage(*video0_visionImageWrapper);
					video0_blobs_found[index] = video0_blobTracker[index]->getBlobs();

                    udp_output[0*4*4+index*4+0] = blankBlob;
                    udp_output[0*4*4+index*4+1] = blankBlob;
                    udp_output[0*4*4+index*4+2] = blankBlob;
                    udp_output[0*4*4+index*4+3] = blankBlob;

					// Print stats
					if(video0_blobs_found[index].size())
					{
						int maxsize = 0;
						int largest_blob = 0;

						for(int count = 0; count < video0_blobs_found[index].size(); count++)
						{
						    if(count<4)
                            {
                                udp_output[0*4*4+index*4+count] = video0_blobs_found[index][count];
                            }

							/*
							if(video0_blobs_found[index][count].w * video0_blobs_found[index][count].h > maxsize)
							{
								printf("Blob %d size = %d\n", count, video0_blobs_found[index][count].w * video0_blobs_found[index][count].h);
								maxsize = video0_blobs_found[index][count].w * video0_blobs_found[index][count].h;
								largest_blob = count;
							}
							*/
						}

						// Show bounding box
						//printf("Largest blob: x=%d y=%d w=%d h=%d\n", video0_blobs_found[index][largest_blob].x, video0_blobs_found[index][largest_blob].y, video0_blobs_found[index][largest_blob].w, video0_blobs_found[index][largest_blob].h);
					}
					else
                    {
                        //printf("No blobs found\n");
                    }

				}
			}

			//clear_video0_buffer();
			remove(VIDEO0_READY);
		}

        //printf("Checking video1\n");

		// If video1 is ready
		if( (access(VIDEO1_READY, F_OK) != -1) && video1_enabled_on_any_model() )
		{
		    new_data_available = true;

			//read_video0_yuv();
			int buffer_file = open(VIDEO1_BUFFER, O_RDONLY);
			read(buffer_file, video1_visionImageWrapper->getBuffer(), 176*144*3/2);
			close(buffer_file);

			//ConvertYUV2RGB(video0_yuv, video0_yuv+(640*480), video0_yuv+(640*480*5/4), video0_visionImageWrapper->getBuffer(), 640, 480);

            // For each color channel
			for(int index=0; index<4; index++)
			{
				if(video1_enable[index])
				{

                    // Do blob tracking, get results
					video1_blobTracker[index]->processImage(*video1_visionImageWrapper);
					video1_blobs_found[index] = video1_blobTracker[index]->getBlobs();

                    udp_output[1*4*4+index*4+0] = blankBlob;
                    udp_output[1*4*4+index*4+1] = blankBlob;
                    udp_output[1*4*4+index*4+2] = blankBlob;
                    udp_output[1*4*4+index*4+3] = blankBlob;

					// Print stats
					if(video1_blobs_found[index].size())
					{
						int maxsize = 0;
						int largest_blob = 0;

						for(int count = 0; count < video1_blobs_found[index].size(); count++)
						{
						    if(count<4)
                            {
                                udp_output[1*4*4+index*4+count] = video1_blobs_found[index][count];
                            }

                            // The below commented out code was not adjusted to use video1
							/*
							if(video0_blobs_found[index][count].w * video0_blobs_found[index][count].h > maxsize)
							{
								printf("Blob %d size = %d\n", count, video0_blobs_found[index][count].w * video0_blobs_found[index][count].h);
								maxsize = video0_blobs_found[index][count].w * video0_blobs_found[index][count].h;
								largest_blob = count;
							}
							*/
						}

						// Show bounding box
						//printf("Largest blob: x=%d y=%d w=%d h=%d\n", video0_blobs_found[index][largest_blob].x, video0_blobs_found[index][largest_blob].y, video0_blobs_found[index][largest_blob].w, video0_blobs_found[index][largest_blob].h);
					}
					else
                    {
                        //printf("No blobs found\n");
                    }

				}
			}

			//clear_video0_buffer();
			remove(VIDEO1_READY);
		}

        //printf("msleep 5\n");

        if(new_data_available)
        {
            send_udp();
        }

        new_data_available = false;

        #ifdef ENABLE_BLUEMARKER
        if(video0_bluemarker[0] || video0_bluemarker[1] || video0_bluemarker[2] || video0_bluemarker[3])
        {
            // If Ready Flag is not present, or if buffer is not present
            if( ! (access(VIDEO0_MARKED_READY, F_OK) != -1) || ! (access(VIDEO0_MARKED_BUFFER, F_OK) != -1) )
            {
                int buffer_file = open(VIDEO0_MARKED_BUFFER, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                write(buffer_file, video0_visionImageWrapper->getBuffer(), 640*480*3/2);
                close(buffer_file);

                int ready_file = open(VIDEO0_MARKED_READY, O_WRONLY | O_CREAT, 0666);
                close(ready_file);
            }
        }

        if(video1_bluemarker[0] || video1_bluemarker[1] || video1_bluemarker[2] || video1_bluemarker[3])
        {
            // If Ready Flag is not present, or if buffer is not present
            if( ! (access(VIDEO1_MARKED_READY, F_OK) != -1) || ! (access(VIDEO1_MARKED_BUFFER, F_OK) != -1) )
            {
                int buffer_file = open(VIDEO1_MARKED_BUFFER, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                write(buffer_file, video1_visionImageWrapper->getBuffer(), 176*144*3/2);
                close(buffer_file);

                int ready_file = open(VIDEO1_MARKED_READY, O_WRONLY | O_CREAT, 0666);
                close(ready_file);
            }
        }
        #endif

        msleep(5);
	}

	return 0;
}
