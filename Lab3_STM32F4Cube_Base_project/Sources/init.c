/**
 * @brief Initialization file - initializes accelerometer, GPIOs
 * @author Yusaira Khan 
 * @author Shivan Kaul
 */

#include "init.h"
#include "stm32f4xx_hal.h"
#include "lis3dsh.h"

LIS3DSH_InitTypeDef LISInitStruct; // Defined in main.c
LIS3DSH_DRYInterruptConfigTypeDef LISIntConfig;

/**
* @brief Initialize GPIOs - GPIOB for display LEDs and GPIOD for alarm LEDs
* @file init.c
* @param None
* @retval None
*/
void gpioInit(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// GPIO clock for LEDs

	GPIO_InitStructure.Pin = GPIO_PIN_0 ;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStructure.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	HAL_NVIC_SetPriority(EXTI0_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);

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
	
	
	LISIntConfig.Dataready_Interrupt = LIS3DSH_DATA_READY_INTERRUPT_ENABLED;
	LISIntConfig.Interrupt_signal = LIS3DSH_ACTIVE_HIGH_INTERRUPT_SIGNAL;
	LISIntConfig.Interrupt_type = LIS3DSH_INTERRUPT_REQUEST_LATCHED;
	LIS3DSH_DataReadyInterruptConfig(&LISIntConfig);
}

