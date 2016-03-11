#include "mathhelper.h"
#include "math.h"

float getArcTan(float num, float denom1, float denom2){
	float angle=atan2(sqrt(denom1*denom1 + denom2*denom2),num )*180/3.14159265;
	angle=angle-90;
	if (denom2 < 0){
		angle= -angle;
	}
	if (angle < 0){
		return angle + 180;
	}
	return angle;
}
void convertAccToAngle(float* acc, float* angles) {
	angles[0] = getArcTan(acc[0],acc[1], acc[2]);
	angles[1] = getArcTan(acc[1],acc[0], acc[2]);
	angles[2] = getArcTan(acc[2],acc[0], acc[1]);
}


float square(float x) {
	return x*x;
}
	
float absolute(float x) {
	return x >= 0 ? x : -x;
}
