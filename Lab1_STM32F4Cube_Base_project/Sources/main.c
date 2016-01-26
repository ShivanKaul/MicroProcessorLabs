#include <stdio.h>
//#include "arm_math.h"


typedef struct {
	float x, p, q, r, k;
} kalman_state;

int main(){
	float input[]={10.0,11.0,12.0,13.0,14.0}, output[5];
	int i =0;
	for (;i<5; i++ ){
		output[i] = input[i]+1.0f;
	}
}
int Kalmanfilter_C (float* InputArray, float* OutputArray, kalman_state* kstate, int Length) {
					printf("o/\n");
	return 1;
}
