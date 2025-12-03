/*
 * app.h
 *
 *  Created on: Nov 11, 2025
 *      Author: nicolas
 */

#ifndef INC_APP_H_
#define INC_APP_H_

#include "stm32g4xx_hal.h"
#include "usart.h"

void init_device();
void loop();

uint8_t shell_uart2_transmit(const char *pData, uint16_t size);
uint8_t shell_uart2_receive(char *pData, uint16_t size);

#endif /* INC_APP_H_ */
