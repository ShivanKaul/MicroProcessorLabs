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


LIS3DSH_InitTypeDef LISInitStruct; 
LIS3DSH_DRYInterruptConfigTypeDef LISIntConfig;

/**
* @brief Initialize GPIOs - GPIOB for display LEDs and GPIOD for acceloremter intterupt
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
* @brief Initialize Accelormeter
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
	//printf("x:%d,y:%d,z:%d\n",offsetX,offsetY,offsetZ);
}


TIM_HandleTypeDef TIM_LED_handle;
void TIMInit(void)
{	
	TIM_Base_InitTypeDef Timinit;
	__TIM3_CLK_ENABLE();
	Timinit.Period = 1000-1; /* 1 MHz to 1 kHz */
	Timinit.Prescaler = 42-1; /* 42 MHz to 1 MHz */
	Timinit.CounterMode = TIM_COUNTERMODE_UP;
	Timinit.ClockDivision = TIM_CLOCKDIVISION_DIV1; //default
	
	TIM_LED_handle.Instance = TIM3;
	TIM_LED_handle.Init = Timinit;
	TIM_LED_handle.Channel = HAL_TIM_ACTIVE_CHANNEL_CLEARED; //default
	TIM_LED_handle.Lock = HAL_UNLOCKED;  //default
	TIM_LED_handle.State = HAL_TIM_STATE_RESET; //default

	HAL_TIM_Base_MspInit(&TIM_LED_handle);
	
	HAL_TIM_Base_Init(&TIM_LED_handle);
	HAL_TIM_Base_Start_IT(&TIM_LED_handle);
	HAL_NVIC_SetPriority(TIM3_IRQn, 2,0);
	HAL_NVIC_EnableIRQ(TIM3_IRQn);
	

}
extern kalman_state kalman_x, kalman_y,kalman_z;
void kalman_init(void){
	kalman_x.p= kalman_y.p=kalman_z.p=1000;
	kalman_x.r= kalman_y.r=kalman_z.r=50; 
	kalman_x.q= kalman_y.q=kalman_z.q=0.1; 

}
extern float acc[],out[];
float x_matrix_values[]= { -4.76541983e-05 ,-2.76761579e-07,-3.52490485e-07,
  2.45104913e-06,-9.59368470e-04,5.16623301e-05,
 -2.34181711e-05,  -2.14577670e-05,  -9.78816908e-04,
  1.98397753e-01,   9.01443456e-03,   5.32591158e-02};
float id[] ={1,1,1, 1,1,1, 1,1,1, 1,1,1};
arm_matrix_instance_f32 x_matrix,w_matrix,y_matrix;
void matrix_init(void){
//	x_matrix.numRows=4;
//	x_matrix.numCols=3;
//	x_matrix.pData=x_matrix_values;
//	x_matrix.pData=id;
	arm_mat_init_f32(&x_matrix,4,3,x_matrix_values);
	out[3]=1;
//	w_matrix.numRows=1;
//	w_matrix.numCols=4;
//	w_matrix.pData=out;
	arm_mat_init_f32(&w_matrix,1,4,out);
	
//	y_matrix.numRows=3;
//	y_matrix.numCols=1;
//	
//	y_matrix.pData=acc;
	arm_mat_init_f32(&y_matrix,1,3,acc);
	//printf();
	

}
