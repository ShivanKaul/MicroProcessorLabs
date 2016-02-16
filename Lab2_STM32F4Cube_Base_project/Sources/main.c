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

#define INIT {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef ADC1_Handle;
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config	(void);


int g_MeasurementNumber;
int NOW_CONVERT;

void gpioInit(void);
void ADCInit(void);
void ChannelInit(void);
	

void poll(void);
void updateDisplay(float);

int main(void)
{
	
  /* MCU Configuration----------------------------------------------------------*/
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();
	/* Configure the system clock */
  SystemClock_Config();
	
	NOW_CONVERT = 0;
	
	gpioInit();
	ADCInit();
	ChannelInit();
	
	// Infinite run loop
	while (1) {
		if (NOW_CONVERT) poll();
	}
	// Stop ADC
	// HAL_ADC_Stop(&ADC1_Handle);
}

void gpioInit() {
	// GPIO clock
	__HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.Pin = GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13| GPIO_PIN_14 | GPIO_PIN_15
	| GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 ;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void ADCInit() {
	// ADC Init
	__HAL_RCC_ADC1_CLK_ENABLE(); // Clock enable
	ADC1_Handle.Instance = ADC1; // Intance
	// Init state
	ADC1_Handle.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV4;
	ADC1_Handle.Init.Resolution = ADC_RESOLUTION_12B;
	ADC1_Handle.Init.ScanConvMode = DISABLE;
	ADC1_Handle.Init.ContinuousConvMode = ENABLE;
	ADC1_Handle.Init.DiscontinuousConvMode = DISABLE;
	ADC1_Handle.Init.NbrOfDiscConversion = 0;
	ADC1_Handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	ADC1_Handle.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T1_CC1;
	ADC1_Handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	ADC1_Handle.Init.NbrOfConversion = 1;
	ADC1_Handle.Init.DMAContinuousRequests = ENABLE;
	ADC1_Handle.Init.EOCSelection = DISABLE;
	// ADC init
	if (HAL_ADC_Init(&ADC1_Handle) != HAL_OK){
		Error_Handler(ADC_INIT_FAIL);
	}
}

void ChannelInit() {
	// Channel
	ADC_ChannelConfTypeDef ADC_Channel;
	ADC_Channel.Channel = ADC_CHANNEL_16; // Temperature sensor
	ADC_Channel.Rank = 1;
	ADC_Channel.SamplingTime = ADC_SAMPLETIME_480CYCLES; // TODO: Ask Ashraf why
	// TODO: how do we know APB2 for ADC
	// TDO: how do we check if running on board
	// TODO: Systick
	ADC_Channel.Offset = 0;
	
	// Config channel
	if (HAL_ADC_ConfigChannel(&ADC1_Handle, &ADC_Channel) != HAL_OK) {
        Error_Handler(ADC_INIT_FAIL);
	}
}

// Get values from temperature sensor
void poll() {
	float g_ADCValue;
	NOW_CONVERT = 0;
	HAL_ADC_Start(&ADC1_Handle);
	if (HAL_ADC_PollForConversion(&ADC1_Handle, 10) == HAL_OK)
	{
			g_ADCValue = HAL_ADC_GetValue(&ADC1_Handle);
			g_ADCValue *= (3000/0xfff); //getting resolution
			g_ADCValue /= 1000.0f;
			g_ADCValue -= 0.76f;
			g_ADCValue /= 0.0025f;
			g_ADCValue += 25.0f;
			updateDisplay(g_ADCValue);
			//g_MeasurementNumber++;
	}
}

// Update 7 segment display
void updateDisplay(float num) {
	
	
}
#define LED_EN_0 GPIO_PIN_4
#define LED_EN_1 GPIO_PIN_5
#define LED_EN_2 GPIO_PIN_6
#define LED_DEG GPIO_PIN_14


#define LED_A GPIO_PIN_7
#define LED_B GPIO_PIN_8
#define LED_C GPIO_PIN_9
#define LED_D GPIO_PIN_10
#define LED_E GPIO_PIN_11
#define LED_F GPIO_PIN_12
#define LED_G GPIO_PIN_13
#define LED_DP GPIO_PIN_13


uint32_t getRegisterLEDValue(int num,int place){
	uint32_t val=0;
	val=LED_DEG;
	switch(num){
		case 0:
			val |= LED_A|LED_B|LED_C|LED_D|LED_E|LED_F;
			break;
		case 1:
			val |= LED_B|LED_C;
			break;
		case 2:
			val |= LED_A|LED_B|LED_D|LED_E|LED_G;
			break;
		case 3:
			val |= LED_A|LED_B|LED_C|LED_D|LED_G;
			break;
		case 4:
			val |= LED_B|LED_C|LED_D|LED_F|LED_G;
			break;
		case 5:
			val |= LED_A|LED_C|LED_D|LED_F|LED_G;
			break;
		case 6:
			val |= LED_A|LED_C|LED_D|LED_E|LED_F|LED_G;
			break;
		case 7:
			val |= LED_A|LED_B|LED_C;
			break;
		case 8:
			val |= LED_A|LED_B|LED_C|LED_D|LED_E|LED_F|LED_G;
			break;
		case 9:
			val |= LED_A|LED_B|LED_C|LED_D|LED_F|LED_G;
			break;
			
	}
	switch(place){
		case 0:
			val |= LED_EN_0;
		case 1:
			val |= LED_EN_1|LED_DP;
		case 2:
			val |= LED_EN_2;
	}
	return val;
	
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
	
	/*Configures SysTick to provide 10ms interval interrupts.*/
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/100);

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
