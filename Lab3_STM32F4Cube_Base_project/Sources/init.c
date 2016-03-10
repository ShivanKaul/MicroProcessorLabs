/**
 * @brief Initialization file - initializes accelerometer, GPIOs
 * @author Yusaira Khan 
 * @author Shivan Kaul
 */

#include "init.h"
#include "stm32f4xx_hal.h"
#include "lis3dsh.h"
#include "stdio.h"

LIS3DSH_InitTypeDef LISInitStruct; 
LIS3DSH_DRYInterruptConfigTypeDef LISIntConfig;

/**
* @brief Initialize GPIOs - GPIOB for display LEDs and GPIOD for alarm LEDs
* @file init.c
* @param None
* @retval None
*/
void gpioInit(void) {
	GPIO_InitTypeDef GPIO_Init_Acc, GPIO_Init_Disp;//, GPIO_Init_Keypad_Input,GPIO_Init_Keypad_Output ;
	
	// GPIO clock for LEDs

	GPIO_Init_Acc.Pin = GPIO_PIN_0 ;
	GPIO_Init_Acc.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_Init_Acc.Mode = GPIO_MODE_IT_RISING;
	GPIO_Init_Acc.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOE, &GPIO_Init_Acc);
	
	HAL_NVIC_SetPriority(EXTI0_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
	
	__HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_Init_Disp.Pin = GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13| GPIO_PIN_14 | GPIO_PIN_15
	| GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 ;
	GPIO_Init_Disp.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_Init_Disp.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Init_Disp.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_Init_Disp);

	
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
	uint8_t offsetX, offsetY, offsetZ;
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
	LISIntConfig.Interrupt_type = LIS3DSH_INTERRUPT_REQUEST_PULSED;
	LIS3DSH_DataReadyInterruptConfig(&LISIntConfig);
	
	LIS3DSH_Read(&offsetX, LIS3DSH_OFF_X, 1);
  LIS3DSH_Read(&offsetY, LIS3DSH_OFF_Y, 1);
  LIS3DSH_Read(&offsetZ, LIS3DSH_OFF_Z, 1);
	printf("x:%d,y:%d,z:%d\n",offsetX,offsetY,offsetZ);
}

