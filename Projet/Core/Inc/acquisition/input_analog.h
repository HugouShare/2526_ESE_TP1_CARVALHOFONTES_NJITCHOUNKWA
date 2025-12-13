/*
 * input_analog.h
 *
 *  Created on: Nov 11, 2025
 *      Author: nicolas
 */

#ifndef INC_INPUT_ANALOG_H_
#define INC_INPUT_ANALOG_H_

#include "user_interface/shell.h"

int	input_analog_init(void);
float measure_current_polling(void);
float measure_current_DMA(void);
int input_analog_get_current(h_shell_t* h_shell, int argc, char** argv);

#endif /* INC_INPUT_ANALOG_H_ */
