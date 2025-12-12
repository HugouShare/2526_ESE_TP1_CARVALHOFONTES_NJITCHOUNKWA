/*
 * led.c
 *
 *  Created on: Nov 11, 2025
 *      Author: nicolas
 */

#include "user_interface/led.h"

int led_init(){
	return shell_add(&hshell1, "led", led_control, "Control LED");
}

int led_control(h_shell_t* h_shell, int argc, char** argv)
{
	int size;

	if(argc!=2){
		size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "Need 2 arguments : LED on/off/toggle\r\n");
		h_shell->drv.transmit(h_shell->print_buffer, size);
		return HAL_ERROR;
	}
	if(strcmp(argv[1],"on")==0){
		HAL_GPIO_WritePin(USR_LED_GPIO_Port, USR_LED_Pin, SET);
		size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "LED ON\r\n");
		h_shell->drv.transmit(h_shell->print_buffer, size);
		return HAL_OK;
	}
	else if(strcmp(argv[1],"off")==0){
		HAL_GPIO_WritePin(USR_LED_GPIO_Port, USR_LED_Pin, RESET);
		size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "LED OFF\r\n");
		h_shell->drv.transmit(h_shell->print_buffer, size);
		return HAL_OK;
	}
	else if(strcmp(argv[1],"toggle")==0){
		HAL_GPIO_TogglePin(USR_LED_GPIO_Port, USR_LED_Pin);
		size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "LED TOGGLE\r\n");
		h_shell->drv.transmit(h_shell->print_buffer, size);
		return HAL_OK;
	}

	size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "Need 2 arguments : LED on/off/toggle\r\n");
	h_shell->drv.transmit(h_shell->print_buffer, size);
	return HAL_ERROR;
}
