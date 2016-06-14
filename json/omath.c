#include "espressif/esp_common.h"

double floor(double x)
{
    return (double) (x < 0.f ? (((int) x) - 1) : ((int) x));
}

double pow(double x, double y){
	double ret=1;
	int i;
	for(i=1; i<(int)y; i++){
		ret *= x;
	}
	return ret;
}

double  fabs(double x){
	if(x<0)
		return (-1*x);
	else
		return x;
}
