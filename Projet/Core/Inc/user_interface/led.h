/*
 * led.h
 *
 *  Created on: Nov 11, 2025
 *      Author: nicolas
 */

#ifndef INC_LED_H_
#define INC_LED_H_

#include "user_interface/shell.h"

int led_control(h_shell_t* h_shell, int argc, char** argv);
int led_init();

#endif /* INC_LED_H_ */
