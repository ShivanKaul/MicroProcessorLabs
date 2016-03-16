/*******************************************************************************
  * @file    Thread_LED.c
  * @author  Ashraf Suyyagh
	* @version V1.0.0
  * @date    17-January-2016
  * @brief   This file initializes one LED as an output, implements the LED thread 
  *					 which toggles and LED, and function which creates and starts the thread	
  ******************************************************************************
  */
	
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "stm32f4xx_hal.h"
#include "kalman.h"
void Thread_ADC (void const *argument);                 // thread function
osThreadId tid_Thread_ADC;                              // thread id
osThreadDef(Thread_ADC, osPriorityNormal, 1, 0);
void poll(void);

/*----------------------------------------------------------------------------
 *      Create the thread within RTOS context
 *---------------------------------------------------------------------------*/
int start_Thread_ADC (void) {

  tid_Thread_ADC = osThreadCreate(osThread(Thread_ADC ), NULL); // Start LED_Thread
  if (!tid_Thread_ADC) return(-1); 
  return(0);
}

 /*----------------------------------------------------------------------------
*      Thread  'LED_Thread': Toggles LED
 *---------------------------------------------------------------------------*/
	void Thread_ADC (void const *argument) {
		while(1){
				osDelay(10); //100 Hz
				poll();
			}
	}
	
	
int NOW_CONVERT,NOW_CHANGE_TEMP,ALARM;
#define ALARM_THRESHOLD 35
float temperature_to_display;
extern ADC_HandleTypeDef	ADC1_Handle;
extern kalman_state kalman_temp;
/**
* @brief Polls the ADC for temperature digitization, and filters using Kalman filter
* 		> Set alarm if temperature above threshold
* @file main.c
* @param None
* @retval None
*/
void poll() { //~10 us to complete?
	float  voltage, temperature, filtered_temp;
	HAL_ADC_Start(&ADC1_Handle);
	if (HAL_ADC_PollForConversion(&ADC1_Handle, 10) == HAL_OK)
	{
			voltage = HAL_ADC_GetValue(&ADC1_Handle);
			voltage *= (3000.0f/0xfff); // getting resolution in mV
			temperature = (voltage -760.0f) / 2.5f; // normalizing around 25C voltage and  average slope 
			temperature += 25.0f;
			
			// Use Kalman filter to get filtered value and store in 'filtered_temp'
			Kalmanfilter_C(&temperature, &filtered_temp, &kalman_temp, 1);
		
			printf("%f\n", filtered_temp);
			if (NOW_CHANGE_TEMP) {
				temperature_to_display = filtered_temp;
				NOW_CHANGE_TEMP = 0;
			}
			if (filtered_temp > ALARM_THRESHOLD){
				ALARM = 1;
			} else ALARM = 0;
	}
}
/*----------------------------------------------------------------------------
 *      Initialize the GPIO associated with the LED
 *---------------------------------------------------------------------------*/

	
/*----------------------------------------------------------------------------
 *      
 *---------------------------------------------------------------------------*/