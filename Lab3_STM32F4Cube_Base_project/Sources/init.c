/**
 * @brief Initialization file - initializes accelerometer, GPIOs
 * @author Yusaira Khan 
 * @author Shivan Kaul
 */

#include "init.h"
#include "stm32f4xx_hal.h"
#include "lis3dsh.h"

extern LIS3DSH_InitTypeDef LISInitStruct; // Defined in main.c


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
void LISInit(void) {
	
	// The Anti Aliasing filter is kept to make sure that all 
	// frequencies > 2x our sampling rate which would lead to 
	// aliasing our filtered out.
	// This is the minimum that is available to us
	LISInitStruct.AA_Filter_BW = LIS3DSH_AA_BW_50;
	// We want to enable all axes
	LISInitStruct.Axes_Enable = LIS3DSH_XYZ_ENABLE;
	// We want to make sure that the output register is not updated until 
	// MSB and LSB readings are completed.
	LISInitStruct.Continous_Update = 0;
	// We want to keep our range +/- 2g
	LISInitStruct.Full_Scale = LIS3DSH_FULLSCALE_2;
	// As mandated by lab handout
	LISInitStruct.Power_Mode_Output_DataRate = LIS3DSH_DATARATE_25;
	// TODO: http://www.edn.com/design/analog/4390843/1/The-embedded-self-test-feature-in-MEMS-inertial-sensors
	LISInitStruct.Self_Test = LIS3DSH_SELFTEST_NORMAL;
	LIS3DSH_Init(&LISInitStruct);
}

