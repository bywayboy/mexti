#include <stdio.h>
#include <stdlib.h>
//#include <stdint.h>
#if !defined(_MSC_VER)
#include <assert.h>
#endif
#include <string.h>
#include <math.h>
#include "lbs.h"

#define PI		3.14159265358979324L
#define x_pi	(3.14159265358979324L * 3000.0L / 180.0L)

static double transformLat(double x, double y) {
		double ret = -100.0L + 2.0L * x + 3.0L * y + 0.2L * y * y + 0.1L * x * y + 0.2L * sqrt(abs(x));
		ret += (20.0L * sin(6.0L * x * PI) + 20.0L * sin(2.0L * x * PI)) * 2.0L / 3.0L;
		ret += (20.0L * sin(y * PI) + 40.0L * sin(y / 3.0L * PI)) * 2.0L / 3.0L;
		ret += (160.0L * sin(y / 12.0L * PI) + 320L * sin(y * PI / 30.0L)) * 2.0L / 3.0L;
		return ret;
}
static double transformLon(double x, double y) {
	double ret = 300.0L + x + 2.0L * y + 0.1L * x * x + 0.1L * x * y + 0.1L * sqrt(abs(x));
	ret += (20.0L * sin(6.0L * x * PI) + 20.0L * sin(2.0L * x * PI)) * 2.0L / 3.0L;
	ret += (20.0L * sin(x * PI) + 40.0L * sin(x / 3.0L * PI)) * 2.0L / 3.0L;
	ret += (150.0L * sin(x / 12.0L * PI) + 300.0L * sin(x / 30.0L * PI)) * 2.0L / 3.0L;
	return ret;
}

static int outOfChina(double lat, double lng) {
	if (lng < 72.004L || lng > 137.8347L) return -1;
	if (lat < 0.8293L || lat > 55.8271L) return -1;
	return 0;
}

static void delta(double lat,double lng, double * dLat, double * dLon)
{
	double radLat,magic,sqrtMagic,a=6378245.0L, //  a: 卫星椭球坐标投影到平面地图坐标系的投影因子。
	ee = 0.00669342162296594323L, //  ee: 椭球的偏心率。
	dlat = transformLat(lng - 105.0L, lat - 35.0L),
	dlon = transformLon(lng - 105.0L, lat - 35.0L);
	radLat = lat / 180.0L * PI;
	magic = sin(radLat);
	
	magic = 1 - ee * magic * magic;
	sqrtMagic = sqrt(magic);
	*dLat = (dlat * 180.0L) / ((a * (1L - ee)) / (magic * sqrtMagic) * PI);
	*dLon = (dlon * 180.0L) / (a / sqrtMagic * cos(radLat) * PI);
}

void WGS2GCJ(double lat,double lng, double * dlat, double * dlng) {
	if (outOfChina(lat, lng))
	{
		*dlat = lat; * dlng = lng;
	}else{
		double dLat,dLng;
		delta(lat,lng,&dLat, &dLng);
		*dlat = lat + dLat;
		*dlng = lng + dLng;
	}
}

void GCJ2WGS(double lat, double lng, double * dlat, double * dlng)
{
	if (outOfChina(lat, lng))
	{
		*dlat = lat;*dlng = lng;
	}else{
		double dLat,dLng;
		delta(lat,lng,&dLat,&dLng);
		*dlat = lat - dLat;
		*dlng = lng - dLng;
	}
}

void GCJ2BD(double lat, double lng, double * bdLat,double * bdLon){
	double x = lng, y =lat,z, theta;
	z=sqrt(x * x + y * y) + 0.00002L * sin(y * x_pi);
	theta=atan2(y, x) + 0.000003L * cos(x * x_pi);
	*bdLon = z * cos(theta) + 0.0065L;
	*bdLat = z * sin(theta) + 0.006L;
}


void BD2GCJ(double lat,double lng,double * dlat, double * dlng)
{
	double x = lng - 0.0065L, y = lat- 0.006L;
	double z = sqrt(x * x + y * y) - 0.00002L * sin(y * x_pi);
	double theta = atan2(y, x) - 0.000003L * cos(x * x_pi);
	*dlng = z * cos(theta);
	*dlat = z * sin(theta);

}


void GCJ2WGS_EXACT(double lat, double lng, double * dlat, double * dlng) {
	double threshold = 0.000000001L,wgsLat, wgsLng;

	double dLat = 0.01L, dLng = 0.01L;
	double mLat = lat - dLat, mLon = lng - dLng;
	double pLat = lat + dLat, pLon = lng + dLng;
	int i = 0;
	while (1) {
		double tmpLat,tmpLng;
		wgsLat = (mLat + pLat) / 2;
		wgsLng = (mLon + pLon) / 2;
		WGS2GCJ(wgsLat, wgsLng, &tmpLat, &tmpLng);
		dLat = tmpLat - lat;
		dLng = tmpLng - lng;
		if ((fabs(dLat) < threshold) && (fabs(dLng) < threshold))
			break;

		if (dLat > 0) pLat = wgsLat; else mLat = wgsLat;
		if (dLng > 0) pLon = wgsLng; else mLon = wgsLng;

		if (++i > 10000) break;
	}
	*dlat = wgsLat;
	*dlng = wgsLng;
}

void BD2WGS(double lat,double lng, double * dlat, double * dlng){
	double _dlat,_dlng;
	BD2GCJ(lat,lng, &_dlat, &_dlng);
	GCJ2WGS_EXACT(_dlat, _dlng, dlat,dlng);
}
void WGS2BD( double lat, double lng, double * dlat, double * dlng)
{
	double _dlat,_dlng;
	WGS2GCJ(lat,lng, &_dlat, &_dlng);
	GCJ2BD(_dlat, _dlng, dlat,dlng);
}
