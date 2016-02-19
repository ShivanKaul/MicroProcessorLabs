#include "kalman.h"

int Kalmanfilter_C (float* InputArray, float* OutputArray, kalman_state* kstate, int Length) {
	int i;
	kstate->x=OutputArray[0];
	
	for (i = 0;i< Length;i++){
		//Equation as given in class
		kstate->p = kstate->p + kstate->q;
		kstate->k = kstate->p / (kstate->p + kstate->r);
		kstate->x = kstate->x  + kstate->k  *  (InputArray[i] - kstate->x);
		kstate->p = (1.0f - kstate->k) * kstate->p;
		OutputArray[i]=kstate->x;
		
		//If any NaN is found, abort calculation and return that error has occured
		if (OutputArray[i] != OutputArray[i]) { // NaN check
			return 1;
		}
		
	}
	return 0;
}
