#include <stdio.h>
#include "arm_math.h"

#define k_def { 0.1f, 0.1f, 0.0f, 0.1f, 0.0f }
#define LEN 5
typedef struct {
	float q, r, x, p,  k;
} kalman_state;

int Kalmanfilter_asm (float* InputArray, float* OutputArray, kalman_state* kstate, int Length);

int Kalmanfilter_C (float* InputArray, float* OutputArray, kalman_state* kstate, int Length) {
	int i;
	kstate->x=OutputArray[0];
	
	for (i = 0;i< Length;i++){

		kstate->p = kstate->p + kstate->q;
		kstate->k = kstate->p / (kstate->p + kstate->r);
		kstate->x = kstate->x  + kstate->k  *  (InputArray[i] - kstate->x);
		kstate->p = (1.0f - kstate->k) * kstate->p;
		OutputArray[i]=kstate->x;
		
		if (OutputArray[i] != OutputArray[i]) { // NaN
			return 1;
		}
		
	}
	return 0;
}

void subtraction(float orig[], float calculated[] ,float sub[]) {
	int i;
	for (i = 0; i < LEN; i++) {
		sub[i] = orig[i] - calculated[i];
	}
}

void stdev(float array[], float meanAndStdDev[]) {
	
	float mean;
	float stdDev;
	int i;
	for (i = 0; i < LEN; i++) {
		mean += array[i];
	}
	mean = mean / LEN;
	for (i = 0; i < LEN; i++) {
		stdDev += (array[i] - mean) * (array[i] - mean);
	}
	arm_sqrt_f32(stdDev, &stdDev);
	
	
	meanAndStdDev[0] = mean;
	meanAndStdDev[1] = stdDev;
	
}

void correlation(float input[], float output[], float corr[]) {
	int i;
	int j;
	float temp=0;
	for (i = 0; i < LEN; i++) {
		temp=0;
		for (j = 0; j < i + 1; j++) {
			temp += input[j] * output[LEN - 1 + j - i];
		}			
		corr[i] = temp;
	} 
	for (i = 1; i < LEN; i++) {
		temp=0;
		for (j = 0; j < LEN - i; j++) {
			temp += input[i+j] *output[j];
		}			
		corr[LEN + i-1] = temp;
	} 
}

void convolution(float input[], float output[], float conv[]) {
	int n;
	for (n = 0; n < 2 * LEN - 1; n++) {
	    int kmin, kmax, k;
	
	    conv[n] = 0;
	
	    kmin = (n >= LEN - 1) ? n - (LEN - 1) : 0;
	    kmax = (n < LEN - 1) ? n : LEN - 1;
	
	    for (k = kmin; k <= kmax; k++) {
	      conv[n] += input[k] * output[n - k];
	    }
  }
}



int main(){
	float input[]={10.0,11.0,12.0,13.0,14.0};
	float	outputC[LEN];
	float	outputASM[LEN];
	float subC[LEN];
	float subASM[LEN];
	float meanAndStdDevC[2];
	float meanAndStdDevASM[2];
	float corrC[2 * LEN - 1];
	float convC[2 * LEN - 1];
	float corrASM[2 * LEN - 1];
	float convASM[2 * LEN - 1];
	int i;
	kalman_state kC = k_def, kASM = k_def;
	
	// call kalman.c
	if (Kalmanfilter_C(input,outputC,&kC,5)) {
		printf("Encountered a NaN in the C subroutine.\n");
	}
	
	// call kalman.asm
	if (Kalmanfilter_asm(input,outputASM,&kASM,5)) {
		printf("Encountered a NaN in the assembly subroutine.\n");
	}
	
	for (i = 0; i < LEN; i++) {
		printf("Item %d in outputC is %f\n", i, outputC[i]);
	}
	printf("\n");
	for (i = 0; i < LEN; i++) {
		printf("Item %d in outputASM is %f\n", i, outputASM[i]);
	}
	
	// Part 3
	
	subtraction(input, outputC, subC);
	stdev(subC, meanAndStdDevC);
	correlation(input, outputC, corrC);
	convolution(input, outputC, convC);
	
	arm_sub_f32	(	input,outputASM,subASM,LEN);
	arm_mean_f32	(subASM,LEN,meanAndStdDevASM); 
	arm_std_f32	(subASM,LEN,&(meanAndStdDevASM[1]));
	arm_correlate_f32 (input, LEN, outputASM, LEN, corrASM);
	arm_conv_f32 (input, LEN, outputASM, LEN, convASM);
	for (i = 0; i < 2*LEN-1; i++) {
		printf("Item %d in corr  is c:%f\ta:%f\n", i,corrC[i], corrASM[i]);
	}
		
}

