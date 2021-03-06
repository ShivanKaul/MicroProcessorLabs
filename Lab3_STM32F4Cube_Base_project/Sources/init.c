/**
 * @brief Initialization file - initializes accelerometer, GPIOs
 * @author Yusaira Khan 
 * @author Shivan Kaul
 */
#define ARM_MATH_CM4
#include "init.h"
#include "stm32f4xx_hal.h"
#include "lis3dsh.h"
#include "stdio.h"
#include "kalman.h"
#include "arm_math.h"

/* Variables */
LIS3DSH_InitTypeDef LISInitStruct; 
LIS3DSH_DRYInterruptConfigTypeDef LISIntConfig;
TIM_HandleTypeDef TIM_LED_handle;
extern kalman_state kalman_x, kalman_y,kalman_z;
extern float acc[],out[];
arm_matrix_instance_f32 x_matrix,w_matrix,y_matrix;

/**
* @brief Initialize GPIOs - GPIOB for display LEDs and GPIOE for acceloremter intterupt
* @file init.c
* @param None
* @retval None
*/
void gpioInit(void) {
	GPIO_InitTypeDef GPIO_Init_Acc, GPIO_Init_Disp;//, GPIO_Init_Keypad_Input,GPIO_Init_Keypad_Output ;
	
	// Accelerometer
	// E0
	GPIO_Init_Acc.Pin = GPIO_PIN_0 ;
	// Set to be 2 MHz -> don't need higher
	GPIO_Init_Acc.Speed = GPIO_SPEED_FREQ_LOW;
	// Generate interrupt on rising edge
	GPIO_Init_Acc.Mode = GPIO_MODE_IT_RISING;
	// To ensure that active high -> default is set to be low
	GPIO_Init_Acc.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOE, &GPIO_Init_Acc);
	
	// Set priority for the accelerometer
	HAL_NVIC_SetPriority(EXTI0_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
	
	// 7 segment display
	// Need the clock to enable gating
	__HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_Init_Disp.Pin = GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13| GPIO_PIN_14 | GPIO_PIN_15
	| GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 ;
	GPIO_Init_Disp.Speed = GPIO_SPEED_FREQ_LOW;
	// To make sure output is not in pull up or pull down mode
	GPIO_Init_Disp.Mode = GPIO_MODE_OUTPUT_PP;
	// We manually set the values for the display logic
	GPIO_Init_Disp.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_Init_Disp);
}



/**
* @brief Initialize accelerometer
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
	LISInitStruct.Self_Test = LIS3DSH_SELFTEST_NORMAL;
	LIS3DSH_Init(&LISInitStruct);
	
	// To configure the interrupt logic for the accelerometer
	LISIntConfig.Dataready_Interrupt = LIS3DSH_DATA_READY_INTERRUPT_ENABLED;
	LISIntConfig.Interrupt_signal = LIS3DSH_ACTIVE_HIGH_INTERRUPT_SIGNAL;
	// Non-latched as mandated in lab handout
	LISIntConfig.Interrupt_type = LIS3DSH_INTERRUPT_REQUEST_PULSED;
	LIS3DSH_DataReadyInterruptConfig(&LISIntConfig);	
}

/**
* @brief Initialize TIM
* @file init.c
* @param None
* @retval None
*/
void TIMInit(void)
{	
	// Period is 1 ms for display
	TIM_Base_InitTypeDef Timinit;
	__TIM3_CLK_ENABLE();
	Timinit.Period = 1000; /* 1 MHz to 1 kHz */
	Timinit.Prescaler = 42; /* 42 MHz to 1 MHz */
	Timinit.CounterMode = TIM_COUNTERMODE_UP;
	Timinit.ClockDivision = TIM_CLOCKDIVISION_DIV1; // default
	
	// As mandated by lab
	TIM_LED_handle.Instance = TIM3;
	TIM_LED_handle.Init = Timinit;
	TIM_LED_handle.Channel = HAL_TIM_ACTIVE_CHANNEL_CLEARED; //default
	TIM_LED_handle.Lock = HAL_UNLOCKED;  //default
	TIM_LED_handle.State = HAL_TIM_STATE_RESET; //default

	HAL_TIM_Base_MspInit(&TIM_LED_handle);
	
	HAL_TIM_Base_Init(&TIM_LED_handle);
	HAL_TIM_Base_Start_IT(&TIM_LED_handle);

	// Set up NVIC
	HAL_NVIC_SetPriority(TIM3_IRQn, 2,0);
	HAL_NVIC_EnableIRQ(TIM3_IRQn);
	

}

/**
* @brief Initialize Kalman
* @file init.c
* @param None
* @retval None
*/
void kalman_init(void){
	kalman_x.p= kalman_y.p=kalman_z.p=1000;
	kalman_x.r= kalman_y.r=kalman_z.r=100; 
	kalman_x.q= kalman_y.q=kalman_z.q=10; 
}


float x_matrix_values[]= { 
	 -9.66498919e-04,	 8.98969238e-06,	-4.04285049e-05,
	  5.30385665e-05,	-9.57997244e-04,	 1.82331830e-05,
	 -1.14033047e-05,	 8.36593441e-06,	-9.84449750e-04,
	  3.18412356e-02,	 8.25963175e-03,	 6.23445509e-02
};

/**
* @brief Initialize matrices to be used for calibration
* @file init.c
* @param None
* @retval None
*/
void matrix_init(void){
	arm_mat_init_f32(&x_matrix,4,3,x_matrix_values);
	out[3]=1;
	arm_mat_init_f32(&w_matrix,1,4,out);
	arm_mat_init_f32(&y_matrix,1,3,acc);
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
