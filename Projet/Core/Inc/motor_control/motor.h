/*
 * motor.h
 *
 *  Created on: Nov 11, 2025
 *      Author: nicolas
 */

#ifndef INC_MOTOR_CONTROL_MOTOR_H_
#define INC_MOTOR_CONTROL_MOTOR_H_

#include "user_interface/shell.h"

int motor_init (void);
void motor_rapport_cyclique_60 (void);
void motor_control (int SET_CCR);
int motor_set_ccr(h_shell_t* h_shell, int argc, char** argv);

#endif /* INC_MOTOR_CONTROL_MOTOR_H_ */
