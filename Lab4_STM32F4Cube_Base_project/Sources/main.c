/*******************************************************************************
  * @file    main.c
  * @author  Ashraf Suyyagh
	* @version V1.2.0
  * @date    17-January-2016
  * @brief   This file demonstrates flasing one LED at an interval of one second
	*          RTX based using CMSIS-RTOS 
  ******************************************************************************
  */

#include "stm32f4xx_hal.h"              // Keil::Device:STM32Cube HAL:Common
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "RTE_Components.h"             // Component selection
#include "init.h"
#include "kalman.h"


extern int start_Thread_ADC			(void);
extern void Thread_ADC(void const *argument);
extern osThreadId tid_Thread_ADC;
extern int start_Thread_7Seg			(void);
extern void Thread_7Seg(void const *argument);
extern osThreadId tid_Thread_7Seg;
extern int start_Thread_Accelerometer	(void);
extern void Thread_Accelerometer(void const *argument);
extern osThreadId tid_Thread_Accelerometer;
/**
	These lines are mandatory to make CMSIS-RTOS RTX work with te new Cube HAL
*/
#ifdef RTE_CMSIS_RTOS_RTX
extern uint32_t os_time;

uint32_t HAL_GetTick(void) { 
  return os_time; 
}
#endif

kalman_state kalman_temp=INIT_KALMAN;

/**
  * Main function
  */
int main (void) {
	osThreadId main_id;
  osKernelInitialize();                     /* initialize CMSIS-RTOS          */

  HAL_Init();                               /* Initialize the HAL Library     */

  SystemClock_Config();                     /* Configure the System Clock     */
	
	//TIMInit();
	LISInit();
	ADCInit();
	kalman_init();
	matrix_init();
	gpioInit();
	
	main_id = osThreadGetId();
	//single tear : ' (
	/* User codes goes here*/
  start_Thread_ADC();                       /* Create LED thread              */
	start_Thread_7Seg();
	start_Thread_Accelerometer();
	
	/* User codes ends here*/
  
	osKernelStart();                          /* start thread execution         */
	osThreadTerminate(main_id);
}
