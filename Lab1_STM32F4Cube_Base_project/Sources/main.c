/**
 * @brief Main file for test benches
 * @author Yusaira Khan 
 * @author Shivan Kaul
 */
#include <stdio.h>
#include "arm_math.h"
#include "demo.h"

// Used for initializing the kalman_state struct
#define k_def { DEF_q, DEF_r, DEF_x, DEF_p, DEF_k }

 // Used for defining the global value of length used
#define LEN 5
typedef struct {
	float q; /**< process noise variance */
	float r; /**< measurement noise variance */
	float x; /**< value */
	float p; /**< estimation error variance */
	float k; /**< kalman gain */
} kalman_state;

/**
* @brief Computes the kalman estimation (in Assembly) of output based on previous inputs
* 			> follows calling conventions
*			> the 'extern' keyword is implicit in function declarations so it is not required here
* @file ../Kalmanfilter_asm.s
* @param InputArray reference to the array of measurements
* @param OutputArray reference to the empty where results will be stored
* @param Length length of both arrays 
* @retval 	0	if no error occured
* 			1 	if error occured during calculation
*/
int Kalmanfilter_asm (float* InputArray, float* OutputArray, kalman_state* kstate, int Length);


/**
* @brief Computes the kalman estimation (in C)of output based on previous inputs
* @param InputArray reference to the array of measurements
* @param OutputArray reference to the empty where results will be stored
* @param Length length of both arrays 
* @retval 	0	if no error occured
* 			1 	if error occured during calculation
*/
int Kalmanfilter_C (float* InputArray, float* OutputArray, kalman_state* kstate, int Length) {
	int i;
	kstate->x=OutputArray[0];
	
	for (i = 0;i< Length;i++){

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

/**
* @brief 	Subtracts the values of an array  from another
* @param 	orig 	Address of array of values to be subracted from
* @param 	calculated 	Address of array of values to be subracted from
* @param 	sub 	Address of array of values where resutls are stored
* @retval 	None
*/
void subtraction(float orig[], float calculated[] ,float sub[]) {
	int i;
	for (i = 0; i < LEN; i++) {
		sub[i] = orig[i] - calculated[i];
	}
}

/**
* @brief 	Computes the mean and standard deviation of an array of values
* @param 	array 	Address of array of values calculated
* @param 	meanAndStdDev 	Address of array where mean and stdev are stored
* @retval 	None
*/
void stdev(float array[], float meanAndStdDev[]) {
	
	float mean=0;
	float stdDev=0;
	int i;
	//Comutation of mean
	for (i = 0; i < LEN; i++) {
		mean += array[i];
	}
	mean = mean / LEN;

//Computation of st deviation
	for (i = 0; i < LEN; i++) {
		stdDev += (array[i] - mean) * (array[i] - mean);
	}
	stdDev = stdDev / (LEN-1); 
	arm_sqrt_f32(stdDev, &stdDev);
	
	
	meanAndStdDev[0] = mean;
	meanAndStdDev[1] = stdDev;
	
}

/**
* @brief 	Computes signal Correlation between 2 arrays
* @param 	input 	Address of an Array with values from 1st signal
* @param 	output 	Address of an Array with values from 2nd signal
* @param 	corr 	Address of an Array where correlation signal values will be stored
* @retval 	None
*/
void correlation(float input[], float output[], float corr[]) {
	int i;
	int j;
	float temp=0;
	//Computation of correlation when values appear from the right  (1st half of answers)
	for (i = 0; i < LEN; i++) {
		temp=0;
		for (j = 0; j < i + 1; j++) {
			temp += input[j] * output[LEN - 1 + j - i];
		}			
		corr[i] = temp;
	} 
	//Computation of correlation when values disssappear to the left (2nd half of answers) 
	for (i = 1; i < LEN; i++) {
		temp=0;
		for (j = 0; j < LEN - i; j++) {
			temp += input[i+j] *output[j];
		}			
		corr[LEN + i-1] = temp;
	} 
}
/**
* @brief 	Computes signal Convolution of 2 arrays
* @param 	input 	Address of an Array with values from 1st signal
* @param 	output 	Address of an Array with values from 2nd signal
* @param 	conv 	Address of an Array where convolution signal values will be stored
* @retval 	None
*/
void convolution(float input[], float output[], float conv[]) {
	int n;

	//Computiing both sides of the correlation together
	for (n = 0; n < (2 * LEN - 1); n++) {
		int left, right, k;
	
		conv[n] = 0;
		// Computation of limits on indices for the convolution 
		left = (n >= LEN - 1) ? n - (LEN - 1) : 0;
		right = (n < LEN - 1) ? n : LEN - 1;
	
		for (k = left; k <= right; k++) {
			conv[n] += input[k] * output[n - k];
		}
	}
}



int main()  {
	//Initialing all the arrays used
	float* input = measurements;//{10.0,11.0,12.0,13.0,14.0};
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
	int i,cBool, asmBool;
	kalman_state kC = k_def, kASM = k_def;

	//Checking if any error occured 
	cBool=Kalmanfilter_C(input,outputC,&kC,LEN);
	// call kalman.c
	if (cBool) {
		printf("Encountered a NaN in the C subroutine.\n");
	}
	
	asmBool=Kalmanfilter_asm(input,outputASM,&kASM,LEN);
	// call kalman.asm
	if (asmBool) {
		printf("Encountered a NaN in the assembly subroutine.\n");
	}
	
	// Printing out from the kalman filtering
	for (i = 0; i < LEN; i++) {
		printf("Item %d in outputC is %f\n", i, outputC[i]);
	}
	printf("\n");
	for (i = 0; i < LEN; i++) {
		printf("Item %d in outputASM is %f\n", i, outputASM[i]);
	}
	
	// Part 3 calling
	
	subtraction(input, outputC, subC);
	stdev(subC, meanAndStdDevC);
	correlation(input, outputC, corrC);
	convolution(input, outputC, convC);
	
	// DSP calling
	arm_sub_f32	(input,outputASM,subASM,LEN);
	arm_mean_f32	(subASM,LEN,meanAndStdDevASM); 
	arm_std_f32	(subASM,LEN,&(meanAndStdDevASM[1]));
	arm_correlate_f32 (input, LEN, outputASM, LEN, corrASM);
	arm_conv_f32 (input, LEN, outputASM, LEN, convASM);
	printf("\n");
	
	// Printing substraction
	for (i = 0; i < LEN; i++) {
		printf("Subtraction %d C: %f\tDSP: %f\n", i,subC[i], subASM[i]);
		printf("Convolution %d C: %f\tDSP: %f\n", i,convC[i], convASM[i]);
		printf("Correlation %d C: %f\tDSP: %f\n", i,corrC[i], corrASM[i]);
	}
		printf("STDEV for DSP  :%f\tSTDEV for C:%f\n",meanAndStdDevASM[1], meanAndStdDevC[1]);
		printf("Mean for DSP  :%f\tMean for C:%f\n",meanAndStdDevASM[0], meanAndStdDevC[0]);
		printf("\n");
}

