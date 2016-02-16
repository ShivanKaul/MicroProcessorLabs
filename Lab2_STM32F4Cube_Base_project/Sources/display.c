#include "display.h"


// Update 7 segment display
void updateDisplay(float num) {
	
	
}
#define LED_EN_0 GPIO_PIN_4
#define LED_EN_1 GPIO_PIN_5
#define LED_EN_2 GPIO_PIN_6
#define LED_DEG GPIO_PIN_12


#define LED_A GPIO_PIN_7
#define LED_B GPIO_PIN_9
#define LED_C GPIO_PIN_14
#define LED_D GPIO_PIN_8
#define LED_E GPIO_PIN_10
#define LED_F GPIO_PIN_11
#define LED_G GPIO_PIN_15
#define LED_DP GPIO_PIN_13


uint32_t getRegisterLEDValue(int num,int place) {
	uint32_t val=0;
	val=LED_DEG;
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
			val |= LED_B|LED_C|LED_D|LED_F|LED_G;
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
		case 1:
			val |= LED_EN_1|LED_DP;
		case 2:
			val |= LED_EN_2;
	}
	return val;
	
}
