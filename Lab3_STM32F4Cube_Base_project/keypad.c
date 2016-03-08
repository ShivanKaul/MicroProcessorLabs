#include "stm32f4xx_hal.h"
// #include "lis3dsh.h"
#include "keypad.h"
GPIO_InitTypeDef GPIO_Init_Keypad_Row,GPIO_Init_Keypad_Col ;

#define keypadGPIO GPIOD

void init_keypad(void){
	GPIO_Init_Keypad_Row.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3 ;
	setInput(&GPIO_Init_Keypad_Row);
	GPIO_Init_Keypad_Col.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11 ;
	setOutput(&GPIO_Init_Keypad_Col);
}
//complement of pin combination
#define button1 ((~GPIO_PIN_0)&(~GPIO_PIN_8))
#define button2 ((~GPIO_PIN_0)&(~GPIO_PIN_9))
#define button3 ((~GPIO_PIN_0)&(~GPIO_PIN_10))
#define buttonC ((~GPIO_PIN_0)&(~GPIO_PIN_11))
#define button4 ((~GPIO_PIN_1)&(~GPIO_PIN_8))
#define button5 ((~GPIO_PIN_1)&(~GPIO_PIN_9))
#define button6 ((~GPIO_PIN_1)&(~GPIO_PIN_10))
#define buttonD ((~GPIO_PIN_1)&(~GPIO_PIN_11))
#define button7 ((~GPIO_PIN_2)&(~GPIO_PIN_8))
#define button8 ((~GPIO_PIN_2)&(~GPIO_PIN_9))
#define button9 ((~GPIO_PIN_2)&(~GPIO_PIN_10))
#define buttonE ((~GPIO_PIN_2)&(~GPIO_PIN_11))
#define buttonA ((~GPIO_PIN_3)&(~GPIO_PIN_8))
#define button0 ((~GPIO_PIN_3)&(~GPIO_PIN_9))
#define buttonB ((~GPIO_PIN_3)&(~GPIO_PIN_10))
#define buttonF ((~GPIO_PIN_3)&(~GPIO_PIN_11))

//needs delays somewhere
uint8_t readButton(){
	uint16_t read =(~0); //all 1s
	read&= (keypadGPIO->IDR) | GPIO_Init_Keypad_Row.Pin; //read only row pins and get all 0s
	
	setInput(&GPIO_Init_Keypad_Col);
	setOutput(&GPIO_Init_Keypad_Row);
	read&= (keypadGPIO->IDR) | GPIO_Init_Keypad_Col.Pin; //read only col pins and get all 0s
	setInput(&GPIO_Init_Keypad_Row);
	setOutput(&GPIO_Init_Keypad_Col);
	
	switch (read){
	case button1: return 1;
	case button2: return 2;
	case button3: return 3;
	case button4: return 4;
	case button5: return 5;
	case button6: return 6;
	case button7: return 7;
	case button8: return 8;
	case button9: return 9;
	case button0: return 0;
	case buttonA: return 10;
	case buttonB: return 11;
	case buttonC: return 12;
	case buttonD: return 13;
	case buttonE: return 14;
	case buttonF: return 15;
	default: return 255;
	}
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
	keypadGPIO->ODR &= (~GPIO_Init_Keypad_Row.Pin); //zeros in the row positions while outputting
	keypadGPIO->ODR &= (~GPIO_Init_Keypad_Col.Pin); //zeros in the column poisions while outputting
}
