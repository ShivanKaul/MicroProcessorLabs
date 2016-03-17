#include "cmsis_os.h" 
extern osMutexId  disp_mutex; 
extern osMutexId  button_mutex;

float displayed_values[3]; 
float getSetValue(float newValue,int setmode, int index){
	osMutexWait(disp_mutex,osWaitForever); 
	if (setmode){
		displayed_values[index]=newValue;
	}
	newValue = displayed_values[index];
	osMutexRelease(disp_mutex); 
	return newValue;
}

int buttonLastPressed;
int getSetButton(int button, int setmode){
	osMutexWait(button_mutex,osWaitForever); 
	// set by keypad
	if (setmode){
		buttonLastPressed = button;
	}
	button = buttonLastPressed;
	osMutexRelease(button_mutex); 
	return button;
}