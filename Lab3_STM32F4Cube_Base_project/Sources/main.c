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

#include "stm32f4xx_hal.h"
#include "supporting_functions.h"
#include "stm32f4xx_hal_spi.h"
#include "init.h"
#include "lis3dsh.h"
#include "keypad.h"
#include "stdio.h"
#include "display.h"
#include "kalman.h"

#include "math.h"


/* Definitions ---------------------------------------------------------*/
#define POSITIONING_AXIS 1
#define ANGLE_RANGE 5


/* Private variables ---------------------------------------------------------*/
//LIS3DSH_InitTypeDef LISInitStruct;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config	(void);
void convertAccToAngle(float*, float*);
float square(float);
float absolute(float);
void position (int);
extern int MS_PASSED;
int keypad_flag=0, display_flag=0;
TIM_HandleTypeDef tim;
float typed_angle, current_angle;

kalman_state kalman_x, kalman_y,kalman_z;

int positioning_started = 0;
float acc[3],out[4];
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
		// Initialize GPIOs
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
				acc_to_display=targetDegrees;
				
			}
			else {
				buttonPressed = readButton();
				if (buttonPressed != NOREAD) { // button is pressed and positioning has not started
					//printf("Button pressed was: %d\n",buttonPressed);
					if (buttonPressed != 10){ 
					targetDegrees = (targetDegrees * 10) + buttonPressed;
					//printf("current target degres %d", targetDegrees);
					}
					else {
						if (targetDegrees > 180) targetDegrees = targetDegrees % 180;
						positioning_started = 1;
					}
				}			
			}
			updateDisplay();
			MS_PASSED = 0;
		}
		
	}
}

void position(int targetDegrees) {
	// We will be positioning along X axis -> roll
	float angles[3];
	float current_angle;
	// Get angles
	convertAccToAngle(acc, angles);
	current_angle = angles[POSITIONING_AXIS];
	printf("Measured angle:%f; Target Angle:%d\n",current_angle,targetDegrees);
	// if in range
	if (absolute(current_angle - targetDegrees) < ANGLE_RANGE) {
		display_flag = 0;
		acc_to_display = current_angle;
		//positioning_started = 0;
	} else { // if outside
		if (current_angle > targetDegrees) {
			// display -->
			display_flag=-1;
			
		} else {
			// display <--
			display_flag=-2;

		}
	}
}

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

