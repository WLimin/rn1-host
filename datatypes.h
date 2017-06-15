#ifndef DATATYPES_H
#define DATATYPES_H

#include <stdint.h>

#define ANG_2_5_DEG   29826162
#define ANG_1_DEG     11930465
#define ANG_0_5_DEG    5965232
#define ANG_0_25_DEG   2982616
#define ANG_0_125_DEG  1491308
#define ANG_0_1_DEG    1193047
#define ANG_0_05_DEG    596523

#define ANG32TORAD(x) ( ((float)((uint32_t)(x)))/683565275.576432)
#define ANG32TOFDEG(x) ( ((float)((uint32_t)(x)))/11930464.7111111)
#define RADTODEG(x) ((x)*(360.0/(2.0*M_PI)))
#define DEGTORAD(x) ((x)*((2.0*M_PI)/360.0))

void wdbg(char* mesta);


typedef struct
{
	int32_t ang; // int32_t range --> -180..+180 deg; let it overflow freely. 1 unit = 83.81903171539 ndeg
	int32_t x;   // in mm
	int32_t y;
} pos_t;

typedef struct
{
	int valid;
	int32_t x;   // in mm
	int32_t y;
} point_t;

#define LIDAR_SCAN_POINTS 360

typedef struct
{
	int significant_for_mapping;
	int is_invalid; // May be distorted due to excessive robot acceleration (collision, drop, etc.)
	pos_t robot_pos;
	point_t scan[LIDAR_SCAN_POINTS];
} lidar_scan_t;

typedef struct
{
	pos_t   robot_pos;
	point_t scan[3];
} sonar_scan_t;

extern int32_t hwdbg[10];

typedef struct
{
	int bat_mv;
	int bat_percentage;
	int charging;
	int charged;
} pwr_status_t;

typedef struct
{
	int status;
	int id;
	int remaining;
	uint32_t micronavi_stop_flags;
	uint32_t micronavi_action_flags;
	uint32_t feedback_stop_flags;
} xymove_t;

extern pwr_status_t pwr_status;
extern xymove_t cur_xymove;

#define MAP_SIGNIFICANT_IMGS     1
#define MAP_SEMISIGNIFICANT_IMGS 2
extern int map_significance_mode;


#endif
