/**
 * @brief Initialization file - initializes GPIO, ADC, Channel
 * @author Yusaira Khan 
 * @author Shivan Kaul
 */

#include "init.h"
#include "stm32f4xx_hal.h"
extern ADC_HandleTypeDef ADC1_Handle; // Defined in main.c

/**
* @brief Initialize GPIOs - GPIOB for display LEDs and GPIOD for alarm LEDs
* @file init.c
* @param None
* @retval None
*/
void gpioInit(void) {
	GPIO_InitTypeDef GPIO_InitStructure, GPIO_InitStructureAlarm;
	
	// GPIO clock for LEDs
	__HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_InitStructure.Pin = GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13| GPIO_PIN_14 | GPIO_PIN_15
	| GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 ;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	// GPIO clock for alarm LEDs
	__HAL_RCC_GPIOD_CLK_ENABLE();
	GPIO_InitStructureAlarm.Pin = GPIO_PIN_12 | GPIO_PIN_13| GPIO_PIN_14 | GPIO_PIN_15 ;
	GPIO_InitStructureAlarm.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStructureAlarm.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructureAlarm.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStructureAlarm);
}

/**
* @brief Initialize ADC
* @file init.c
* @param None
* @retval None
*/
void ADCInit(void) {
	// ADC Init
	__HAL_RCC_ADC1_CLK_ENABLE(); // Clock enable
	ADC1_Handle.Instance = ADC1; // Instance
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

/**
* @brief Initialize Channel - we use channel 16, which is hard set to be temperature sensor
* @file init.c
* @param None
* @retval None
*/
void ChannelInit(void) {
	// Channel
	ADC_ChannelConfTypeDef ADC_Channel;
	ADC_Channel.Channel = ADC_CHANNEL_16; // Temperature sensor
	ADC_Channel.Rank = 1;
	ADC_Channel.SamplingTime = ADC_SAMPLETIME_480CYCLES;
	ADC_Channel.Offset = 0;
	
	// Config channel
	if (HAL_ADC_ConfigChannel(&ADC1_Handle, &ADC_Channel) != HAL_OK) {
        Error_Handler(ADC_INIT_FAIL);
	}
}
