#include <stdio.h>
//#include "arm_math.h"

#define k_def {0.0f,0.1f,0.1f,0.1f,0.0f };
typedef struct {
	float x, p, q, r, k;
} kalman_state;

int Kalmanfilter_C (float* InputArray, float* OutputArray, kalman_state* kstate, int Length) {
					
	return 1;
}

int main(){
	float input[]={10.0,11.0,12.0,13.0,14.0}, output[5];
	int i =0;
	kalman_state k= k_def;
	for (;i<5; i++ ){
		output[i] = input[i]+1.0f;
	}
	Kalmanfilter_C(input,output,&k,5);
}

