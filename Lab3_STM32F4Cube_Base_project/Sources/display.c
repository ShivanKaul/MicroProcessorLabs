/**
 * @brief Display file - handles all display logic
 * @author Yusaira Khan 
 * @author Shivan Kaul
 */

#include "display.h"

// Flags
int DISPLAY_DIGIT = 0;
//extern int ALARM_LED;
//int RAISE_ALARM = 0;
//extern int RAISE_ALARM_SEM;
float acc_to_display=0.0f;

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

/**
* @brief Update the 7 segment display
* @file display.c
* @param None
* @retval None
*/
extern int display_flag;
void updateDisplay(void) {
	int padded = (int) (acc_to_display), 
		i,
		digit;
	// LED displaying logic
	if(!display_flag) {
		for (i=0; i< acc_to_display; i++){
			padded /= 10;
		}
		digit = padded % 10;
		GPIOB->ODR = getRegisterLEDValue(digit,DISPLAY_DIGIT,0);
	} else { GPIOB->ODR = getRegisterLEDValue(display_flag, 0,0);} // Display arrow!
}

/**
* @brief Get the register LED value
* @file display.c
* @param Number to convert
* @param Place to convert
* @retval Register LED value
*/
uint32_t getRegisterLEDValue(int num,int place,int dp_pos) {
	uint32_t val=LED_DEG;
	//up and down arrows
	if (num < 0){
		if (num == -1){ // left arrow
			return LED_EN_1|LED_EN_2|LED_A|LED_D|LED_E|LED_F;
		}
		if (num == -2){ // right arrow
			return LED_EN_1|LED_EN_2|LED_D|LED_A|LED_B|LED_C;
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
			val |= /*(((dp_pos & 1)<<4)& LED_DP)|*/  LED_EN_0;
		  break;
		case 1:
			val |= /*(((dp_pos >> 1 & 1)<<4)& LED_DP)|*/LED_EN_1;
		  break;
		case 2:
			val |= LED_EN_2;
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

