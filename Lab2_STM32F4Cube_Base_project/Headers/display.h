#include "stm32f4xx_hal.h"
#include "supporting_functions.h"

void updateDisplay(float num);
uint32_t getRegisterLEDValue(int num,int place);

void alarm(void);