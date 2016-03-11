/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program subroutine
	* Author						 : Ashraf Suyyagh
	* Version            : 1.0.0
	* Date							 : January 14th, 2016
  ******************************************************************************
  */
	
/* Includes ------------------------------------------------------------------*/
#define ARM_MATH_CM4            
#include "stm32f4xx_hal.h"
#include "supporting_functions.h"
#include "stm32f4xx_hal_spi.h"
#include "init.h"
#include "lis3dsh.h"
#include "keypad.h"
#include "stdio.h"
#include "display.h"
#include "kalman.h"
#include "arm_math.h"
#include "mathhelper.h"



/* Definitions ---------------------------------------------------------*/
#define POSITIONING_AXIS 0
#define ANGLE_RANGE 5


/* Private variables ---------------------------------------------------------*/
//LIS3DSH_InitTypeDef LISInitStruct;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config	(void);
void convertAccToAngle(float*, float*);
float square(float);
float absolute(float);
void position (int);
void calculateAngles (void);
extern int MS_PASSED;
int keypad_flag=0, display_flag=0,ACCELERATION_FLAG=1,angle_flag;
TIM_HandleTypeDef tim;
float typed_angle, current_angle;

kalman_state kalman_x, kalman_y,kalman_z;

int positioning_started = 0;
float acc[3],out[4], current_angle;
extern float acc_to_display;

int main(void)
{	
	int buttonPressed=0;
	int targetDegrees=0;
  /* MCU Configuration----------------------------------------------------------*/
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();
	
  /* Initialize all configured peripherals */
	gpioInit();
	init_keypad();
	// Initialize accelerometer
	LISInit();
	TIMInit();
	kalman_init();
	matrix_init();

	while (1){
		if (MS_PASSED){
			keypad_flag = !keypad_flag;
			//acc_to_display=128;
			display_flag = 0;
			//printf("%f\n",acc_to_display);
			if (positioning_started) {
				position(targetDegrees);
				acc_to_display=current_angle;
			}
			else {
				buttonPressed = readButton();
				if (buttonPressed != NOREAD) { // button is pressed and positioning has not started
					if (buttonPressed != 10){ 
					targetDegrees = (targetDegrees * 10) + buttonPressed;
					}else {
						positioning_started = 1;
						if (targetDegrees > 180) targetDegrees = targetDegrees % 180;
					}
				}			
			}
			if(ACCELERATION_FLAG){//Every 25 Hz
				calculateAngles();
				ACCELERATION_FLAG=0;
			}
			updateDisplay();
			MS_PASSED = 0;
		}
	}
}

void position(int targetDegrees) {
	// We will be positioning along X axis -> roll

	//printf("Measured angle:%f; Target Angle:%d\n",current_angle,targetDegrees);
	// if in range
	if (absolute(current_angle - targetDegrees) < ANGLE_RANGE) {
		display_flag = 0;
		acc_to_display = current_angle;
		//positioning_started = 0;
	} else { // if outside
		if (current_angle > targetDegrees) {
			// display vv
			display_flag=-1;
		} else {
			// display ^^
			display_flag=-2;
		}
	}
}
extern kalman_state kalman_x, kalman_y,kalman_z;
extern arm_matrix_instance_f32 x_matrix,w_matrix,y_matrix;
extern float acc[3],out[4];

void calculateAngles( void ){
	float angles[3];

	//printf("%f,%f,%f\n",w_matrix.pData[0],w_matrix.pData[1],w_matrix.pData[2]);
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


#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line){
}
#endif

