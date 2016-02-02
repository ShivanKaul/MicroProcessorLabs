#include <stdio.h>
//#include "arm_math.h"

#define k_def { 0.1f, 0.1f, 0.0f, 0.1f, 0.0f };
#define NaN 0.0
typedef struct {
	float q, r, x, p,  k;
} kalman_state;

int Kalmanfilter_C (float* InputArray, float* OutputArray, kalman_state* kstate, int Length) {
	int i;
	kstate->x=OutputArray[0];
	
	for (i = 0;i< Length;i++){

		kstate->p = kstate->p + kstate->q;
		kstate->k = kstate->p / (kstate->p + kstate->r);
		kstate->x = kstate->x  + kstate->k  *  (InputArray[i] - kstate->x);
		kstate->p = (1.0f - kstate->k) * kstate->p;
		OutputArray[i]=kstate->x;
		
		if (OutputArray[i] != OutputArray[i]) { 
			return 0;
		}
		
	}
	return 1;
}

int main(){
	float input[]={10.0,11.0,12.0,13.0,14.0};
	float	output[5];
	int i =0;
	kalman_state k = k_def;
	
	if (Kalmanfilter_C(input,output,&k,5)) {
		for (;i<5; i++ ){
			printf("ohai %f\n",output[i]);
		}
		return 0;
	}
	else return 1;
}

