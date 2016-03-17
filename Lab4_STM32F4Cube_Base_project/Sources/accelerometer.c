#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "stm32f4xx_hal.h"
#include "kalman.h"
#include "lis3dsh.h"
#include "arm_math.h"


osThreadId tid_Thread_Accelerometer;
void Thread_Accelerometer(void const *argument);
osThreadDef(Thread_Accelerometer, osPriorityNormal, 1, 0);
#define data_ready_flag 1ss
int start_Thread_Accelerometer	(void){
	tid_Thread_Accelerometer = osThreadCreate(osThread(Thread_Accelerometer ), NULL); // Start LED_Thread
  if (!tid_Thread_Accelerometer) return(-1); 
  return(0);
}
#define ohai // ;) ^_.
void Thread_Accelerometer(void const *argument){
	
	
	
	
}

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
	// NOTE2: Similarly, it was discussed whether filtering before calibrating 
	// is better than calibrating before filtering. We had filtering before 
	// calibrating. We were told that we should be calibrating first, and 
	// then filtering. We are not entirely convinced. We spoke to Harsh, and
	// we talked about how we are NOT using different filtering params
	// for the Kalman filter for the 3 axes (refer to function kalman_init in 
	// init.c, and hence calibration should be a linear transformation.
	Kalmanfilter_C (out, out, &kalman_x, 1);
	Kalmanfilter_C (out+1, out+1, &kalman_y, 1);
	Kalmanfilter_C (out+2, out+2, &kalman_z, 1);
	arm_mat_mult_f32(&w_matrix,&x_matrix,&y_matrix);
	convertAccToAngle(acc, angles);
	// Get angles
	if (angle_flag){
		current_angle = angles[POSITIONING_AXIS];
		angle_flag=0;
	}
}