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

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config	(void);
void convertAccToAngle(float*, float*);
float square(float);
float absolute(float);
void position (int);
void calculateAngles (void);

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

	// Super loop
	while (1) {
		// Millisecond has passed
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

void position(int targetDegrees) {
	// We will be positioning along X axis -> roll
	printf("Measured angle:%f; Target Angle:%d;\n",current_angle,targetDegrees);
	// If in range
	if (absolute(current_angle - targetDegrees) < ANGLE_RANGE) {
		if (!HOLD_VALUE) {
			acc_to_display = current_angle;
			HOLD_VALUE = 1;
			display_flag = 0;
		}
		//positioning_started = 0;
	} else { // if outside
		if (current_angle > targetDegrees) {
			if (HOLD_VALUE) HOLD_VALUE = 0;
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

	//printf("W:%f,%f,%f\n",w_matrix.pData[0],w_matrix.pData[1],w_matrix.pData[2]);
	Kalmanfilter_C (out, out, &kalman_x, 1);
	Kalmanfilter_C (out+1, out+1, &kalman_y, 1);
	Kalmanfilter_C (out+2, out+2, &kalman_z, 1);
	//printf("Y:%f,%f,%f\n",y_matrix.pData[0],y_matrix.pData[1],y_matrix.pData[2]);
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

