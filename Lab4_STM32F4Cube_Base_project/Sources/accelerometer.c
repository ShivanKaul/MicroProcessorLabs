#define ARM_MATH_CM4
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "stm32f4xx_hal.h"
#include "kalman.h"
#include "lis3dsh.h"
#include "arm_math.h"
#include "math.h"


osThreadId tid_Thread_Accelerometer;
void Thread_Accelerometer(void const *argument);
osThreadDef(Thread_Accelerometer, osPriorityNormal, 1, 0);
void convertAccToAngle(float* acc, float* angles);
extern arm_matrix_instance_f32 x_matrix,w_matrix,y_matrix;
void calculateAngles (void);
float current_angle;

#define data_ready_flag 1
int start_Thread_Accelerometer	(void){
	tid_Thread_Accelerometer = osThreadCreate(osThread(Thread_Accelerometer ), NULL); // Start LED_Thread
  if (!tid_Thread_Accelerometer) return(-1); 
  return(0);
}
#define ohai // ;) ^_.
void Thread_Accelerometer(void const *argument){
	osSignalWait (data_ready_flag,osWaitForever); 
	osSignalClear(tid_Thread_Accelerometer,data_ready_flag); 
	calculateAngles();
	
	
}
extern float getSetValue(float newValue,int setmode, int index);
extern float out[], acc[];
extern kalman_state kalman_x, kalman_y,kalman_z;
/**
   * @brief Calculates angles - filter them and use calibration matrix
   * POSITIONING_AXIS specifies which axis to use for positioning
   * @param None
   * @retval None
   */
void calculateAngles (void) {
	float angles[3];
	// NOTE1: We initially read acceleration in the interrupt, but on discussion
	// with the TA on Friday, we've pushed it over here for the purposes of 
	// the code submission.
	LIS3DSH_ReadACC(out);
	Kalmanfilter_C (out, out, &kalman_x, 1);
	Kalmanfilter_C (out+1, out+1, &kalman_y, 1);
	Kalmanfilter_C (out+2, out+2, &kalman_z, 1);
	arm_mat_mult_f32(&w_matrix,&x_matrix,&y_matrix);
	convertAccToAngle(acc, angles);
	// Get angles
	getSetValue(angles[0],1,0);
	getSetValue(angles[1],1,1);
}

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
	angles[1] = getArcTan(acc[1],acc[0], acc[2]);
	// angles[2] = getArcTan(acc[2],acc[0], acc[1]);
}
	
float absolute(float x) {
	return x >= 0 ? x : -x;
}

/**
  * @brief  This function handles EXTI0 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI0_IRQHandler(void){
 HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

// Accelerometer interrupt
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if (GPIO_Pin==GPIO_PIN_0){
		 osSignalSet ( tid_Thread_Accelerometer, data_ready_flag);
	}
}
