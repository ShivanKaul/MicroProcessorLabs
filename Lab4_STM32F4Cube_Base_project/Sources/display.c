/**
 * @brief Display file - handles all display logic
 * @author Yusaira Khan 
 * @author Shivan Kaul
 */

#include "display.h"
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "stm32f4xx_hal.h"
// Flags
extern int DISPLAY_DIGIT,Seg7_MS_PASSED;
//extern float temperature_to_display;
extern int display_flag;

// Define statments, for prettier code
#define LED_EN_0 GPIO_PIN_7 /*pin 7*/
#define LED_EN_1 GPIO_PIN_6 /*pin 2*/
#define LED_EN_2 GPIO_PIN_5 /*pin 1*/
#define LED_DEG GPIO_PIN_8 /*Pin 9*/

#define LED_A GPIO_PIN_15 /*Pin 14*/
#define LED_B GPIO_PIN_13 /*Pin 16*/
#define LED_C GPIO_PIN_11 /*Pin 13*/
#define LED_D GPIO_PIN_14 /*Pin 3*/
#define LED_E GPIO_PIN_12 /*Pin 5*/
#define LED_F GPIO_PIN_10 /*Pin 11*/
#define LED_G GPIO_PIN_9 /*Pin 15*/
#define LED_DP GPIO_PIN_4 /*Pin 7*/

// AlarmLEDs
#define LED_Green GPIO_PIN_12
#define LED_Orange GPIO_PIN_13
#define LED_Red GPIO_PIN_14
#define LED_Blue GPIO_PIN_15


void Thread_7Seg (void const *argument);                 // thread function
osThreadId tid_Thread_7Seg;                              // thread id
osThreadDef(Thread_7Seg, osPriorityNormal, 1, 0);


/*----------------------------------------------------------------------------
 *      Create the thread within RTOS context
 *---------------------------------------------------------------------------*/
osMutexId  disp_mutex; 
osMutexDef (disp_mutex);  
 
int start_Thread_7Seg (void) {

	disp_mutex = osMutexCreate(osMutex(disp_mutex)); 
  tid_Thread_7Seg = osThreadCreate(osThread(Thread_7Seg ), NULL); // Start LED_Thread
  if (!tid_Thread_7Seg) return(-1); 
  return(0);
}
#define flicker_bound 64
int flicker_count;
 /*----------------------------------------------------------------------------
*      Thread  'LED_Thread': Toggles LED
 *---------------------------------------------------------------------------*/
	void Thread_7Seg (void const *argument) {
		while(1){
			
				osDelay(5); //200Hz
			//if (Seg7_MS_PASSED){
				Seg7_MS_PASSED=0;
				updateDisplay();
			DISPLAY_DIGIT++;
			if (DISPLAY_DIGIT > 2) DISPLAY_DIGIT = 0;
			
			flicker_count= (flicker_count + 1) % flicker_bound;
			
		//}
			}
	}
/**
* @brief Update the 7 segment display
* @file display.c
* @param None
* @retval None
*/

	
	

float getSetValue(float,int, int);	
	
int padded_stored, mul,alarm_display_flag=1, display_state=1;


int getSetButton(int, int);	


#define ALARM_THRESHOLD 36
int buttonDisplay = 1;
void updateDisplay(void) {
	int padded,	i,digit;
	
	if (!flicker_count){
		float temperature_for_alarm,read_value;
	// wait for semaphore from keypad
		buttonDisplay = getSetButton(0, 0);
		read_value=getSetValue(0,0,buttonDisplay);
		padded_stored = (int)(read_value *100);
		// LED displaying logic
		// logic for displaying decimal points
		mul = getDecimalPointPosition(padded_stored);
		for (i=mul; i<2;i++){
			padded_stored /= 10;
		}
		if(buttonDisplay==2){
		temperature_for_alarm = read_value;
		}else{
		temperature_for_alarm = getSetValue(0,0,2);}
		if (temperature_for_alarm < ALARM_THRESHOLD){
		//if(!alarm_display_flag ){
			display_state = 1;
		}else {
			display_state = !display_state;
			
		}
	}	
	if (display_state){
		padded = padded_stored;
		// logic for displaying 
		for (i=0; i< DISPLAY_DIGIT; i++){
			padded /= 10;
		}
		digit = padded % 10;
		GPIOB->ODR = getRegisterLEDValue(digit,DISPLAY_DIGIT, mul);
	} else {
			GPIOB->ODR = 0;

	}
}

/**
* @brief The postion of the decimal point for values that are less than 10 or 100
* @file display.c
* @param num	Value of the angle multiplied by 100
* @retval 	0 	when the angle > 100 (abs(num) >10000)
						1 	when the 100> angle > 10 (abs(num) >10)
						2 	when the 10 >angle > 0 (otherwise)
*/
int getDecimalPointPosition(int num) {
	
	num = num > 0 ? num : -num;
	if (num > 10000/*100*100*/) return 0;
	else if(num > 1000 /*10*100*/) return 1;
	else return 2;
}

/**
* @brief Get the register LED value
* @file display.c
* @param num 		Digit to display
* @param place	Place to display at
* @param dp_pos	Position of decimal point in the full number to display
* @retval Register LED bit setting
*/
uint32_t getRegisterLEDValue(int num,int place,int dp_pos) {
	uint32_t val=LED_DEG;

	//bit mapping for Numbers
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
			val |= LED_B|LED_C|LED_F|LED_G;
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
	//sets the display enables for the current digit as well as potential decimal points
	switch(place){
		case 0:
			val |=   LED_EN_0;
		  break;
		case 1:
			val |= (((dp_pos & 1)<<4)& LED_DP) //is less than 100
							| LED_EN_1;
		  break;
		case 2:
			val |= (((dp_pos & 2)<<3)& LED_DP) //is less than 10
							|LED_EN_2;
		  break;
	}
	return val;
	
}
extern TIM_HandleTypeDef TIM_LED_handle;
void TIM2_IRQHandler() {
	HAL_TIM_IRQHandler(&TIM_LED_handle);
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* tim){
		
		Seg7_MS_PASSED=1;

}
