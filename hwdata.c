
#include <stdint.h>
#include "datatypes.h"
#include "uart.h"

#include "../rn1-brain/comm.h" // For the convenient 7-bit data handling macros.
#define I14x2_I16(msb,lsb) ((int16_t)( ( ((uint16_t)(msb)<<9) | ((uint16_t)(lsb)<<2) ) ))


#define LIDAR_RING_BUF_LEN 8
#define SONAR_RING_BUF_LEN 16

int lidar_wr = 0;
int lidar_rd = 0;
int sonar_wr = 0;
int sonar_rd = 0;

lidar_scan_t lidars[LIDAR_RING_BUF_LEN];
sonar_scan_t sonars[SONAR_RING_BUF_LEN];

int32_t hwdbg[10];

lidar_scan_t* get_lidar()
{
	if(lidar_wr == lidar_rd)
	{
		return 0;
	}
	
	lidar_scan_t* ret = &lidars[lidar_rd];
	lidar_rd++; if(lidar_rd >= LIDAR_RING_BUF_LEN) lidar_rd = 0;
	return ret;
}

sonar_scan_t* get_sonar()
{
	if(sonar_wr == sonar_rd)
	{
		return 0;
	}
	
	sonar_scan_t* ret = &sonars[sonar_rd];
	sonar_rd++; if(sonar_rd >= SONAR_RING_BUF_LEN) sonar_rd = 0;
	return ret;
}


int parse_uart_msg(uint8_t* buf, int len)
{
	switch(buf[0])
	{
		case 0x84:
		{
			/*
			 Lidar-based 2D MAP on uart:

			num_bytes
			 1	uint8 start byte
			 1	uint7 status
			 2	int14 cur_ang (at the middle point of the lidar scan)  (not used for turning the image, just to include robot coords)
			 5	int32 cur_x   ( " " )
			 5	int32 cur_y   ( " " )
			 1	int7  correction return value
			 2	int14 ang_corr (for information only)
			 2	int14 x_corr (for information only)
			 2	int14 y_corr (for information only) 
			1440	360 * point
				  2	int14  x referenced to cur_x
				  2	int14  y referenced to cur_y

				Total: 1461
				Time to tx at 115200: ~130 ms

			*/

			lidars[lidar_wr].significant_for_mapping = buf[1];
			lidars[lidar_wr].robot_pos.ang = (I7I7_U16_lossy(buf[2], buf[3]))<<16;
			int mid_x = lidars[lidar_wr].robot_pos.x = I7x5_I32(buf[4],buf[5],buf[6],buf[7],buf[8]);
			int mid_y = lidars[lidar_wr].robot_pos.y = I7x5_I32(buf[9],buf[10],buf[11],buf[12],buf[13]);

			for(int i = 0; i < 360; i++)
			{
				int x = I14x2_I16(buf[21+4*i+0], buf[21+4*i+1])>>2;
				int y = I14x2_I16(buf[21+4*i+2], buf[21+4*i+3])>>2;
				if(x != 0 && y != 0)
				{
					lidars[lidar_wr].scan[i].x = x + mid_x;
					lidars[lidar_wr].scan[i].y = y + mid_y;
					lidars[lidar_wr].scan[i].valid = 1;
				}
				else
				{
					lidars[lidar_wr].scan[i].valid = 0;
				}
			}

			lidar_wr++; if(lidar_wr >= LIDAR_RING_BUF_LEN) lidar_wr = 0;
		}
		break;

		case 0x85:
		{
			/*
				Sonar-based 2D map
			*/
			for(int i = 0; i < 3; i++)
			{
				sonars[sonar_wr].scan[i].valid = (buf[1]&(1<<i))?1:0;
				sonars[sonar_wr].scan[i].x = I7x5_I32(buf[2+10*i],buf[3+10*i],buf[4+10*i],buf[5+10*i],buf[6+10*i]);
				sonars[sonar_wr].scan[i].y = I7x5_I32(buf[7+10*i],buf[8+10*i],buf[9+10*i],buf[10+10*i],buf[11+10*i]);
			}

			sonar_wr++; if(sonar_wr >= SONAR_RING_BUF_LEN) sonar_wr = 0;

		}
		break;

		case 0xd2:
		{
			for(int i=0; i<10; i++)
			{
				hwdbg[i] = I7x5_I32(buf[i*5+1],buf[i*5+2],buf[i*5+3],buf[i*5+4],buf[i*5+5]);
			}
		}
		break;
	}

	return 0;
}

void send_keepalive()
{
	uint8_t buf[3] = {0x8f, 0x00, 0xff};
	send_uart(buf, 3);
}

void move_to(int32_t x, int32_t y)
{
	uint8_t buf[12];

	buf[0] = 0x82;
	buf[1] = I32_I7_4(x);
	buf[2] = I32_I7_3(x);
	buf[3] = I32_I7_2(x);
	buf[4] = I32_I7_1(x);
	buf[5] = I32_I7_0(x);
	buf[6] = I32_I7_4(y);
	buf[7] = I32_I7_3(y);
	buf[8] = I32_I7_2(y);
	buf[9] = I32_I7_1(y);
	buf[10] = I32_I7_0(y);
	buf[11] = 0xff;
	send_uart(buf, 12);
}
