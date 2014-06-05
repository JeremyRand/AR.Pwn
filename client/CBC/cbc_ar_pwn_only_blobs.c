#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>

// Front camera 6.1 fps, 7273 7124 6943 6782 6622 6459
//                       5002 4867 4681 4520 4377 4201
// With -O3,    9.2fps  6288 6180 6082 5968 5858 5746

struct Blob {
	int x, y, w, h;
};

int sock, length, n;
socklen_t fromlen;
struct sockaddr_in server;
struct sockaddr_in from;

struct Blob udp_output[2*4*4]; // 4 blobs, 4 color channels, 2 cameras;
long blob_last_timestamp = 0;

void blob_track()
{
	while(1)
    {
        n = recvfrom(sock,(char*) udp_output, 512 /*sizeof(udp_output)*/,0,(struct sockaddr *)&from,&fromlen);
        if (n < 0) printf ("Error recvfrom\n");
		blob_last_timestamp = (long) (seconds() * 1000.0);
	}
}

void init_receive_blobs()
{
   sock=socket(AF_INET, SOCK_DGRAM, 0);
   if (sock < 0) printf("Error Opening socket\n");
   length = sizeof(server);
   bzero(&server,length);
   server.sin_family=AF_INET;
   server.sin_addr.s_addr=INADDR_ANY;
   server.sin_port=htons(16720);
   if (bind(sock,(struct sockaddr *)&server,length)<0)
       printf("Error binding\n");
   fromlen = sizeof(struct sockaddr_in);
   
   start_process(blob_track);
}

void drone_remote_control(float max_tilt, float max_yaw_vel, float max_z_vel)
{
	int in_air = 0;
	int enable_move;
	float x_tilt, y_tilt, yaw_vel, z_vel;
	
	// This loop will continue forever; put it in a dedicated program and use the CBC's E-Stop to exit.
	while(1)
	{
		//cbc_display_clear();
		printf("Cam0 Chan0 Blob0: x=%d y=%d w=%d h=%d, ms=%d\n", udp_output[0].x, udp_output[0].y, udp_output[0].w, udp_output[0].h, blob_last_timestamp);

		msleep(50);
	}
}

int main()
{
	printf("Hooking firmware...\n");
	
	system("(\nsleep 5\necho \"/update/snarc/run_hook.sh\"\nsleep 5\n) | nc 192.168.1.1 23 &");
	
	printf("Press A when lights turn green.\n");
	
	while(! a_button() ) msleep(5);
	while(a_button() ) msleep(5);
	
	printf("Starting blob tracker...\n");
	
	system("(\nsleep 5\necho \"touch /tmp/video0_enable_0; touch /tmp/video1_enable_0; /update/snarc/TelnetInterface\"\nsleep 1\n) | telnet -l root 192.168.1.1 23 &");
	
    printf("Starting blob receive...\n");

	msleep(10000);

	init_receive_blobs();

	drone_remote_control(0.1, 0.1, 0.1);
}
