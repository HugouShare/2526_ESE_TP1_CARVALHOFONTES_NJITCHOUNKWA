/*
 * motor.c
 *
 *  Created on: Nov 11, 2025
 *      Author: nicolas
 */

#include "motor_control/motor.h"
#include "stm32g4xx.h"
#include "stdlib.h"
#include "tim.h"
#include "gpio.h"


#define SET_ARR (4250-1)
#define CCR_MAX (4250-1)


int motor_init (void)
{
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
	motor_rapport_cyclique_50();
	shell_add(&hshell1, "startmotor", motor_start, "Start motor");
	shell_add(&hshell1, "stopmotor", motor_stop, "Stop motor");
	return 1;
}

void motor_rapport_cyclique_50 (void)
{
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 2125);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 2125);
}

void motor_control (int SET_CCR)
{
	int CCR1 = 0;
	int CCR2 = 0;
	CCR1 = SET_CCR;
	CCR2 = SET_ARR - CCR1;
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, CCR1);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, CCR2);
}

int motor_set_ccr(h_shell_t* h_shell, int argc, char** argv)
{
	int size;

	if(argc!=2)
	{
		size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "Need 2 arguments : SETCCR XXXX\r\n");
		h_shell->drv.transmit(h_shell->print_buffer, size);
		return HAL_ERROR;
	}
	if(atoi(argv[1])>CCR_MAX)
	{
		size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "CCR value to high => CCR set to CCR_MAX : %d\r\n",CCR_MAX);
		h_shell->drv.transmit(h_shell->print_buffer, size);
		motor_control(CCR_MAX);
		return HAL_OK;
	}
	if(atoi(argv[1])<0)
	{
		size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "CCR value too low => CCR unchanged\r\n");
		h_shell->drv.transmit(h_shell->print_buffer, size);
		return HAL_OK;
	}
	else if((0<=atoi(argv[1])) && (atoi(argv[1])<=CCR_MAX))
	{
		size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "CCR value OK => CCR set to : %d\r\n",atoi(argv[1]));
		h_shell->drv.transmit(h_shell->print_buffer, size);
		motor_control(atoi(argv[1]));
		return HAL_OK;
	}

	size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "Need 2 arguments : SETCCR XXXX\r\n");
	h_shell->drv.transmit(h_shell->print_buffer, size);
	return HAL_ERROR;
}

int motor_start (h_shell_t* h_shell, int argc, char** argv)
{
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
	motor_rapport_cyclique_50();
	return HAL_OK;
}

int motor_stop (h_shell_t* h_shell, int argc, char** argv)
{
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);
	return HAL_OK;
}
