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

#include "kalman.h"

#include "math.h"


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
extern int MS_PASSED;
int keypad_flag=0, display_flag=0;
TIM_HandleTypeDef tim;
float typed_angle, current_angle;

kalman_state kalman_x, kalman_y,kalman_z;

int positioning_started = 0;
float acc[3],out[4];


int main(void)
{	
	int buttonPressed;
	int targetDegrees;
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
		if(MS_PASSED){
			
			keypad_flag = !keypad_flag;
			buttonPressed = readButton();
			if (buttonPressed != NOREAD) {
				if (buttonPressed != 10 && !positioning_started) targetDegrees = (targetDegrees * 10) + buttonPressed;
				else {
					positioning_started = 1;
					position(targetDegrees);
				}
				printf("%d\n",buttonPressed);	
			}			
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
	
	// if in range
	if (absolute(current_angle - targetDegrees) < ANGLE_RANGE) {
		display_flag = 0;
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
	angles[0] = atan2(acc[0], sqrt(square((acc[1]) + square(acc[2]))));
	angles[1] = atan2(acc[1], sqrt(square((acc[0]) + square(acc[2]))));
	angles[2] = atan2(acc[2], sqrt(square((acc[1]) + square(acc[0]))));
}

float square(float x) {
	return x*x;
}
	
float absolute(float x) {
	return x >= 0 ? x : -x;
}

/** System Clock Configuration*/
void SystemClock_Config(void){

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  __PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType 	= RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState 			 	= RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState 		= RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource 	= RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM 				= 8;
  RCC_OscInitStruct.PLL.PLLN 				= 336;
  RCC_OscInitStruct.PLL.PLLP 				= RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ 				= 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK){Error_Handler(RCC_CONFIG_FAIL);};

  RCC_ClkInitStruct.ClockType 			= RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource 		= RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider 	= RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider 	= RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider 	= RCC_HCLK_DIV2;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5)!= HAL_OK){Error_Handler(RCC_CONFIG_FAIL);};
	
	/*Configures SysTick to provide 1ms interval interrupts. SysTick is already 
	  configured inside HAL_Init, I don't kow why the CubeMX generates this call again*/
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

	/* This function sets the source clock for the internal SysTick Timer to be the maximum,
	   in our case, HCLK is now 168MHz*/
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
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

