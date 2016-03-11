/**
 * @brief Helper for Math Equations
 * @author Yusaira Khan 
 * @author Shivan Kaul
 */
#include "mathhelper.h"
#include "math.h"

/**
* @brief Get an Angle through arctan depending on all 3 accelerations for accuracy
* @file mathhelper.c
* @param num	numerator for the tangent
* @param denom1	one of the axes in the denominator
* @param denom1	the other axes in the denominator
* @retval angle that was calculated
*/
float getArcTan(float num, float denom1, float denom2){
	//getting arc COT of angle
	float angle=atan2(sqrt(denom1*denom1 + denom2*denom2),num )*180/3.14159265;
	//setting 90 as vertical
	angle=angle-90;
	if (denom2 < 0){
		angle= -angle;
	}
	
	//getting rid of negative angles since tan x == tan (180 +x)
	if (angle < 0){
		return angle + 180;
	}
	return angle;
}
/**
* @brief Get an Angle through arctan depending on all 3 accelerations for accuracy
* @file mathhelper.c
* @param acc accelaration array pointer
* @param angles results pointers
* @retval None
*/
void convertAccToAngle(float* acc, float* angles) {
	angles[0] = getArcTan(acc[0],acc[1], acc[2]); //roll calculation
	
	
	//Used in pitch and yaw calculations
	// angles[1] = getArcTan(acc[1],acc[0], acc[2]);
	// angles[2] = getArcTan(acc[2],acc[0], acc[1]);
}
	
float absolute(float x) {
	return x >= 0 ? x : -x;
}
