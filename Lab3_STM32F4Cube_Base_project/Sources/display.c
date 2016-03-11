/**
 * @brief Display file - handles all display logic
 * @author Yusaira Khan 
 * @author Shivan Kaul
 */

#include "display.h"

// Flags
int DISPLAY_DIGIT = 0;
float acc_to_display = 0;
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

/**
* @brief Update the 7 segment display
* @file display.c
* @param None
* @retval None
*/

void updateDisplay(void) {
	int padded = (int) (acc_to_display * 100),
		mul,
		i,
		digit;
	
	if(!display_flag) {
		// LED displaying logic
		// logic for displaying decimal points
		mul = getDecimalPointPosition(padded);
		for (i=mul; i<2;i++){
			padded /= 10;
		}
		// logic for displaying 
		for (i=0; i< DISPLAY_DIGIT; i++){
			padded /= 10;
		}
		digit = padded % 10;
		GPIOB->ODR = getRegisterLEDValue(digit,DISPLAY_DIGIT, mul);
	} else { GPIOB->ODR = getRegisterLEDValue(display_flag, 0, 0);} // Display arrow!
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
	//up and down arrows
	if (num < 0){
		if (num == -1){ // up arrow
			return LED_EN_1|LED_EN_0|LED_A|LED_B|LED_F;
		}
		if (num == -2){ // down arrow
			return LED_EN_1|LED_EN_0|LED_D|LED_E|LED_C;
		}
	}
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
			val |= (((dp_pos & 1)<<4)& LED_DP) //is less than 10
							| LED_EN_1;
		  break;
		case 2:
			val |= (((dp_pos & 2)<<3)& LED_DP) //is less than 1
							|LED_EN_2;
		  break;
	}
	return val;
	
}
