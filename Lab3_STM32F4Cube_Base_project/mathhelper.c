#include "mathhelper.h"
#include "math.h"
void convertAccToAngle(float* acc, float* angles) {
	angles[0] = atan2(acc[0], sqrt(square((acc[1]) + square(acc[2]))))*180/3.14159265;
	angles[1] = atan2(acc[1], sqrt(square((acc[0]) + square(acc[2]))))*180/3.14159265;
	angles[2] = atan2(acc[2], sqrt(square((acc[1]) + square(acc[0]))))*180/3.14159265;
}

float square(float x) {
	return x*x;
}
	
float absolute(float x) {
	return x >= 0 ? x : -x;
}
