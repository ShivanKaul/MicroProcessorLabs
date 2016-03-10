/**
 * @brief Display file - handles all display logic
 * @author Yusaira Khan 
 * @author Shivan Kaul
 */

#include "display.h"

// Flags
int NOW_CHANGE_DISPLAY = 0;
//extern int ALARM_LED;
//int RAISE_ALARM = 0;
//extern int RAISE_ALARM_SEM;
float acc_to_display;

// Define statments, for prettier code
#define LED_EN_0 GPIO_PIN_6 
#define LED_EN_1 GPIO_PIN_4 
#define LED_EN_2 GPIO_PIN_5 
#define LED_DEG GPIO_PIN_12

#define LED_A GPIO_PIN_7
#define LED_B GPIO_PIN_9
#define LED_C GPIO_PIN_14
#define LED_D GPIO_PIN_8
#define LED_E GPIO_PIN_10
#define LED_F GPIO_PIN_11
#define LED_G GPIO_PIN_15
#define LED_DP GPIO_PIN_13

// AlarmLEDs
#define LED_Green GPIO_PIN_12
#define LED_Orange GPIO_PIN_13
#define LED_Red GPIO_PIN_14
#define LED_Blue GPIO_PIN_15

/**
* @brief Update the 7 segment display
* @file display.c
* @param None
* @retval None
*/
void updateDisplay(void) {
	int padded = (int) (acc_to_display), 
		i,
		digit;
	// LED displaying logic
	for(i=0; i< acc_to_display; i++){
		padded /= 10;
	}
	digit = padded % 10;
	
	GPIOB->ODR = getRegisterLEDValue(digit,NOW_CHANGE_DISPLAY);
}

/**
* @brief Get the register LED value
* @file display.c
* @param Number to convert
* @param Place to convert
* @retval Register LED value
*/
uint32_t getRegisterLEDValue(int num,int place) {
	uint32_t val=LED_DEG;
	//up and down arrows
	if(num<0){
		if (num==-1){//down arrow
			return LED_EN_1|LED_EN_2|LED_C|LED_D|LED_E;
		}
		if (num==-2){//up arrow
			return LED_EN_1|LED_EN_2|LED_F|LED_A|LED_B;
		}
	}
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
	switch(place){
		case 0:
			val |= LED_EN_0;
		  break;
		case 1:
			val |= LED_EN_1;
		  break;
		case 2:
			val |= LED_EN_2;//!!num*LED_EN_2;
		  break;
	}
	return val;
	
}

///**
//* @brief Set alarm on - flash LEDs on board
//* @file display.c
//* @param None
//* @retval None
//*/
//void alarm_on(void){
//	switch(ALARM_LED % 4) {
//		case 0: 
//			GPIOD->ODR = LED_Green;
//			break;
//		case 1:
//			GPIOD->ODR = LED_Orange;
//			break;
//		case 2: 
//			GPIOD->ODR = LED_Red;
//			break;
//		case 3: 
//			GPIOD->ODR = LED_Blue;
//			break;
//	}
//}

///**
//* @brief Switch off alarm LEDs
//* @file display.c
//* @param None
//* @retval None
//*/
//void alarm_off(void){
//	GPIOD->ODR = 0;
//}

