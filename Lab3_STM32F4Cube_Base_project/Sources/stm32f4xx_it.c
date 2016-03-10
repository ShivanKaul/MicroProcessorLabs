/**
  ******************************************************************************
  * @file    HAL/HAL_TimeBase/Src/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version   V1.2.4
  * @date      13-November-2015
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#define ARM_MATH_CM4
#include "main.h"
#include "stm32f4xx_it.h"
#include "stm32f4xx_hal.h"
#include "stdio.h"
#include "lis3dsh.h"
#include "kalman.h"
#include "arm_math.h"


/** @addtogroup STM32F4xx_HAL_Examples
  * @{
  */

/** @addtogroup GPIO_EXTI
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}
int MS_PASSED=0;
/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
			// Millisecond passed
		MS_PASSED = 1;
		// Alarm counter
		//RAISE_ALARM_SEM++;
		// Every 250 ticks of systick, move to next LED.
	 // if (RAISE_ALARM_SEM % 250 == 0) {
			//ALARM_LED++;
		//}
	HAL_IncTick();
}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/
/**
  * @brief  This function handles EXTI0 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI0_IRQHandler(void){
 HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

extern kalman_state kalman_x, kalman_y,kalman_z;
extern arm_matrix_instance_f32 x_matrix,w_matrix,y_matrix;
extern float acc[3],out[4];
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if (GPIO_Pin==GPIO_PIN_0){
		
		LIS3DSH_ReadACC(out);
		//printf("%f,%f,%f\n",w_matrix.pData[0],w_matrix.pData[1],w_matrix.pData[2]);
		Kalmanfilter_C (out, out, &kalman_x, 1);
		Kalmanfilter_C (out+1, out+1, &kalman_y, 1);
		Kalmanfilter_C (out+2, out+2, &kalman_z, 1);
		
		arm_mat_mult_f32(&w_matrix,&x_matrix,&y_matrix);
		
		//printf("X: %f,%f,%f,%f\n",x_matrix.pData[0],x_matrix.pData[1],x_matrix.pData[2],x_matrix.pData[3]);
		//printf("Y: %f,%f,%f\n",y_matrix.pData[0],y_matrix.pData[1],y_matrix.pData[2]);
		//out[3]=1;
		
		
	}
}

//float x_matrix_values[]= { -4.76541983e-05 ,-2.76761579e-07,-3.52490485e-07,
//  2.45104913e-06,-9.59368470e-04,5.16623301e-05,
// -2.34181711e-05,  -2.14577670e-05,  -9.78816908e-04,
//  1.98397753e-01,   9.01443456e-03,   5.32591158e-02};
void mult(void ){}


extern TIM_HandleTypeDef TIM_LED_handle;
void TIM3_IRQHandler(){
	HAL_TIM_IRQHandler(&TIM_LED_handle);
}
extern int DISPLAY_DIGIT;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* tim){
	DISPLAY_DIGIT++;
  if (DISPLAY_DIGIT > 2) DISPLAY_DIGIT = 0; // Wrap around for 3 digits

}
/**
  * @}
  */ 

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
