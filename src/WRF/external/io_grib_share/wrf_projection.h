#define WRF_LATLON 0
#define WRF_LAMBERT 1
#define WRF_POLAR_STEREO 2
#define WRF_MERCATOR 3
#define WRF_GAUSS 4
#define WRF_CYL 5
#define WRF_CASSINI 6
#define WRF_PS_WGS84 102
#define WRF_ALBERS_NAD83 105
#define WRF_ROTLL 203

/*#define WRF_EARTH_RADIUS 6370*/
/* Use the value in HYCOM, P. Velissariou */
#define WRF_EARTH_RADIUS 6371
#define PI 3.141593
#define KM_TO_DEGREES 360. / (2. * PI * WRF_EARTH_RADIUS)
