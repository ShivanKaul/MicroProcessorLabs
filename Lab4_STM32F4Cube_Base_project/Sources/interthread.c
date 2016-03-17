#include "cmsis_os.h" 
extern osMutexId  disp_mutex; 

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
