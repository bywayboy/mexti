#ifndef PHP_LBS_H
#define PHP_LBS_H

void WGS2GCJ(double lat,double lng, double * dlat, double * dlng);
void GCJ2WGS(double lat, double lng, double * dlat, double * dlng);
void GCJ2WGS_EXACT(double lat, double lng, double * dlat, double * dlng);

void BD2GCJ(double lat,double lng,double * dlat, double * dlng);
void GCJ2BD(double lat, double lng, double * bdLat,double * bdLon);



void BD2WGS(double lat,double lng, double * dlat, double * dlng);
void WGS2BD( double lat, double lng, double * dlat, double * dlng);


#endif