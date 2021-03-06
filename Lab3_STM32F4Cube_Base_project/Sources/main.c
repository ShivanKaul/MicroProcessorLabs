/**
 * @brief Main file
 * @author Yusaira Khan 
 * @author Shivan Kaul
 */
	
/* Includes ------------------------------------------------------------------*/
/* ARM was complaining about not finding this */
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
#define ENTER 10

/* Private variables ---------------------------------------------------------*/
extern int MS_PASSED;
int keypad_flag = 0, display_flag = 0,ACCELERATION_FLAG = 1,angle_flag;
TIM_HandleTypeDef tim;
float current_angle;
int HOLD_VALUE = 0;
kalman_state kalman_x, kalman_y,kalman_z;
int positioning_started = 0;
float acc[3], out[4];
extern float acc_to_display;
extern kalman_state kalman_x, kalman_y,kalman_z;
extern arm_matrix_instance_f32 x_matrix,w_matrix,y_matrix;
extern float acc[3],out[4];
int buttonPressed = 0;
int targetDegrees = 0;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config	(void);
void convertAccToAngle(float*, float*);
float square(float);
float absolute(float);
void position (int);
void calculateAngles (void);

int main(void)
{	
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
	
	// Super loop
	while (1) {
		if (MS_PASSED){
			keypad_flag = !keypad_flag;
			display_flag = 0;
			
			// If the user has entered an angle and pressed Enter
			if (positioning_started) {
				position(targetDegrees);
			}
			else {
				buttonPressed = readButton();
				if (buttonPressed != NOREAD) { // Button is pressed and positioning has not started
					if (buttonPressed != ENTER){ // We're still reading values from the keypad
						// Collect keypad presses
						printf("Pressed: %d\n", buttonPressed);
						targetDegrees = (targetDegrees * 10) + buttonPressed;
					} else {
						// We have an Enter!
						positioning_started = 1;
						// Angle wraparound
						targetDegrees = targetDegrees % 180;
					}
				}			
			}
			
			if (ACCELERATION_FLAG){ // Every 25 Hz set by Systick
				calculateAngles();
				ACCELERATION_FLAG = 0;
			}
			updateDisplay();
			MS_PASSED = 0;
		}
	}
}

/**
   * @brief Handles the positioning logic
   * @param targetDegrees: the degrees to position to
   * @retval None
   */
void position(int targetDegrees) {
	// We will be positioning along X axis -> roll
	printf("Measured angle:%f; Target Angle:%d;\n",current_angle,targetDegrees);
	// If in range
	if (absolute(current_angle - targetDegrees) < ANGLE_RANGE) {
		// Check if we want to be holding the value for the display
		//if (!HOLD_VALUE) {
			acc_to_display = current_angle;
			//HOLD_VALUE = 1;
			display_flag = 0;
		//}
	} else { // if outside range
		if (current_angle > targetDegrees) {
			//if (HOLD_VALUE) HOLD_VALUE = 0;
			// display vv
			display_flag=-1;
		} else {
			// display ^^
			display_flag=-2;
		}
	}
}

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

