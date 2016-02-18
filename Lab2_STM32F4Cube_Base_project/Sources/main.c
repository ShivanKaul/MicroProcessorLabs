/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : A program which showcases ADC and TIM3 under the new firmware
	                       The ADC
	* Author						 : Ashraf Suyyagh
	* Version            : 1.0.0
	* Date							 : January 14th, 2016
  ******************************************************************************
  */
	
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "supporting_functions.h"
#include <stdio.h>
#include "init.h"
#include "display.h"

#define INIT {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
#define SAMPLING_DELAY 10
#define ALARM_THRESHOLD 31
extern float temperature_to_display;
extern int NOW_CHANGE_DISPLAY;


/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef ADC1_Handle;
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config	(void);


int g_MeasurementNumber;
int NOW_CONVERT=0;
int NOW_CHANGE_TEMP=0;
uint32_t last_sample_time;	
void poll(void);
int ALARM;


int main(void){
  /* MCU Configuration----------------------------------------------------------*/
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();
	/* Configure the system clock */
  SystemClock_Config();
	last_sample_time=  HAL_GetTick();//initializing ticks for clock;
	//NOW_CONVERT = 0;
	
	// Init all
	gpioInit();
	ADCInit();
	ChannelInit();
	
	// Infinite run loop
	while (1) {
		if (NOW_CONVERT) poll();
		updateDisplay();
		if (ALARM) alarm();
	}
}

// Get values from temperature sensor
void poll() {
	float  voltage, temperature;
	NOW_CONVERT = 0;
	HAL_ADC_Start(&ADC1_Handle);
	if (HAL_ADC_PollForConversion(&ADC1_Handle, 10) == HAL_OK)
	{
			voltage = HAL_ADC_GetValue(&ADC1_Handle);
			voltage *= (3000.0f/0xfff); //getting resolution in milivolts
			temperature = (voltage -760.0f) / 2.5f; // normalizing around 25C voltage and  average slope 
			temperature += 25.0f;
			printf("%f\n", temperature);
			
			if (NOW_CHANGE_TEMP) {
				temperature_to_display = temperature;
				NOW_CHANGE_TEMP=0;
			}
			if (temperature > ALARM_THRESHOLD){
				ALARM = 1;
			} else ALARM = 0;
			//g_MeasurementNumber++;
	}
}

/** System Clock Configuration */
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
	
	/*Configures SysTick to provide 1ms interval interrupts.*/
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

	/* This function sets the source clock for the internal SysTick Timer to be the maximum,
	   in our case, HCLK is now 168MHz*/
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/**
   * @brief A function used to setup the ADC to sample Channel 16
   * @retval None
   */

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
