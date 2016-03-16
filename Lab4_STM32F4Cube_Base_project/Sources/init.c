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
kalman_state kalman_x, kalman_y,kalman_z, kalman_temp;
float acc[3],out[4];
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
	__TIM2_CLK_ENABLE();
	Timinit.Period = 1000; /* 1 MHz to 1 kHz */
	Timinit.Prescaler = 42; /* 42 MHz to 1 MHz */
	Timinit.CounterMode = TIM_COUNTERMODE_UP;
	Timinit.ClockDivision = TIM_CLOCKDIVISION_DIV1; // default
	
	// As mandated by lab
	TIM_LED_handle.Instance = TIM2;
	TIM_LED_handle.Init = Timinit;
	TIM_LED_handle.Channel = HAL_TIM_ACTIVE_CHANNEL_CLEARED; //default
	TIM_LED_handle.Lock = HAL_UNLOCKED;  //default
	TIM_LED_handle.State = HAL_TIM_STATE_RESET; //default

	HAL_TIM_Base_MspInit(&TIM_LED_handle);
	
	HAL_TIM_Base_Init(&TIM_LED_handle);
	HAL_TIM_Base_Start_IT(&TIM_LED_handle);

	// Set up NVIC
	HAL_NVIC_SetPriority(TIM2_IRQn, 2,0);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);
	

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
	kalman_temp.p =1000;
	kalman_temp.q=0.1;
	kalman_temp.r=50;
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
/**
* @brief Initialize ADC
* @file init.c
* @param None
* @retval None
*/

ADC_HandleTypeDef ADC1_Handle;
void ADCInit(void) {
	ADC_ChannelConfTypeDef ADC_Channel;
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
	
	//Initialize Channel - we use channel 16, which is hard set to be temperature sensor
	ADC_Channel.Channel = ADC_CHANNEL_16; // Temperature sensor
	ADC_Channel.Rank = 1;
	ADC_Channel.SamplingTime = ADC_SAMPLETIME_480CYCLES;
	ADC_Channel.Offset = 0;
	
	// Config channel
	if (HAL_ADC_ConfigChannel(&ADC1_Handle, &ADC_Channel) != HAL_OK) {
        Error_Handler(ADC_INIT_FAIL);
	}
}
