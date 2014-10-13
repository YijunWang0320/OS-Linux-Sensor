#define TIME_INTERVAL 200
#define NOISE 0
#define WINDOW 20
struct acc_motion {
	unsigned int dlt_x; /* +/- around X-axis */
	unsigned int dlt_y; /* +/- around Y-axis */
	unsigned int dlt_z; /* +/- around Z-axis */
	unsigned int frq;   /* Number of asmples that satisfies:
				sum_each_sample(dlt_x+dlt_y+dlt_z)>NOISE */
};
struct dev_acceleration {
	int x;
	int y;
	int z;
};
