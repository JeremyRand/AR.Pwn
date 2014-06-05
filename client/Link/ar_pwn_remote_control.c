#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>

struct Blob {
	int x, y, w, h;
};

int sock, length, n;
socklen_t fromlen;
struct sockaddr_in server;
struct sockaddr_in from;

struct Blob udp_output[2*4*4]; // 4 blobs, 4 color channels, 2 cameras;

void blob_track()
{
	while(1)
    {
        n = recvfrom(sock,(char*) udp_output, 512 /*sizeof(udp_output)*/,0,(struct sockaddr *)&from,&fromlen);
        if (n < 0) printf ("Error recvfrom\n");
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
		if(a_button())
		{
			if(in_air)
			{
				drone_land();
			}
			else
			{
				drone_takeoff();
			}
			in_air = !in_air;

			while(a_button()) msleep(5);
		}
		if(black_button())
		{
			drone_emergency();
			while(black_button()) msleep(5);
		}
		x_tilt = (float)(b_button()) * (float)(-accel_x() / 65.0 * max_tilt);
		y_tilt = (float)(b_button()) * (float)(-accel_y() / 65.0 * max_tilt);
		yaw_vel = max_yaw_vel * (float)(left_button() - right_button());
		z_vel = max_z_vel * (float)(up_button() - down_button());

		enable_move = b_button() || left_button() || right_button() || up_button() || down_button();

		drone_move(enable_move, x_tilt, y_tilt, yaw_vel, z_vel);		

		cbc_display_clear();
		printf("A=Tkf/Lnd Blk=Emg B=TiltXY Arrow=Yaw/Z\nControl= %d %.2f %.2f %.2f %.2f\nBat=%d Pos=%d %d %d Vel=%d %d %d Yaw=%d\nCam0 Chan0 Blob0: x=%d y=%d w=%d h=%d\n", enable_move, x_tilt, y_tilt, yaw_vel, z_vel, drone_get_battery(), (int)(drone_get_x()), (int)(drone_get_y()), (int)(drone_get_z()), (int)(drone_get_x_vel()), (int)(drone_get_y_vel()), (int)(drone_get_z_vel()), (int)(drone_get_yaw()), udp_output[0].x, udp_output[0].y, udp_output[0].w, udp_output[0].h);

		msleep(50);
	}
}

int main()
{
	printf("Connecting to Drone...\n");
	drone_connect();
	printf("Connected!\n");

    printf("Starting blob receive...\n");

	init_receive_blobs();

	drone_remote_control(0.1, 0.1, 0.1);
}
