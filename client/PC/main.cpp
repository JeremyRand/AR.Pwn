#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>

#include "mb_vision/BlobImageProcessorYUV.h"

using namespace std;

int sock, length, n;
socklen_t fromlen;
struct sockaddr_in server;
struct sockaddr_in from;

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
}

int main()
{
    Blob *udp_output;

    udp_output = new Blob[2*4*4]; // 4 blobs, 4 color channels, 2 cameras;

    init_receive_blobs();

    printf("Starting blob receive...\n");

    while(1)
    {
        n = recvfrom(sock,(char*) udp_output, 512 /*sizeof(udp_output)*/,0,(struct sockaddr *)&from,&fromlen);
        if (n < 0) printf ("Error recvfrom\n");

        //if(udp_output[0].h>0 && udp_output[0].w>0)
        //{
            printf("Cam0 Chan0 Blob0: x=%d y=%d w=%d h=%d\n", udp_output[0].x, udp_output[0].y, udp_output[0].w, udp_output[0].h);
        //}
    }




}
