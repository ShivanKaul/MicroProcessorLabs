#include "stm32f4xx_hal.h"
// #include "lis3dsh.h"
#include "keypad.h"
GPIO_InitTypeDef GPIO_Init_Keypad_Row,GPIO_Init_Keypad_Col ;

#define keypadGPIO GPIOD
#define negO GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3

void init_keypad(void){
	GPIO_Init_Keypad_Row.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3 ;
	setInput(&GPIO_Init_Keypad_Row);
	
	GPIO_Init_Keypad_Col.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11 ;
	setOutput(&GPIO_Init_Keypad_Col);
}

void readButton(){

}

void setInput(GPIO_InitTypeDef *Keypad_Input){
	Keypad_Input->Speed = GPIO_SPEED_FREQ_LOW;
	Keypad_Input->Mode = GPIO_MODE_INPUT;
	Keypad_Input->Pull = GPIO_PULLUP;
	HAL_GPIO_Init(keypadGPIO, Keypad_Input);
}

void setOutput(GPIO_InitTypeDef* Keypad_Output){
	Keypad_Output->Speed = GPIO_SPEED_FREQ_LOW;
	Keypad_Output->Mode = GPIO_MODE_OUTPUT_PP;
	Keypad_Output->Pull = GPIO_NOPULL;
	HAL_GPIO_Init(keypadGPIO, Keypad_Output);
	
}
