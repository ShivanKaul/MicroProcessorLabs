#include "cmsis_os.h" 
#include "interthread.h"
extern osMutexId  disp_mutex; 

extern osMutexId  alarm_mutex; 

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

int alarm_flag;
int getSetAlarm(int newValue, int setmode){
	osMutexWait(alarm_mutex,osWaitForever); 
	if (setmode){
		alarm_flag=newValue;
	}
	newValue = alarm_flag;
	osMutexRelease(alarm_mutex); 
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

