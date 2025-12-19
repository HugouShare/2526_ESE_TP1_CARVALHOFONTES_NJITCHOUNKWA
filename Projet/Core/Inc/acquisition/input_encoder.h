/*
 * input_encoder.h
 *
 *  Created on: Nov 11, 2025
 *      Author: nicolas
 */

#ifndef INC_ACQUISITION_INPUT_ENCODER_H_
#define INC_ACQUISITION_INPUT_ENCODER_H_

#include "user_interface/shell.h"

int input_encoder_init (void);
void measure_speed (void);
int input_encoder_get_speed(h_shell_t* h_shell, int argc, char** argv);

#endif /* INC_ACQUISITION_INPUT_ENCODER_H_ */
