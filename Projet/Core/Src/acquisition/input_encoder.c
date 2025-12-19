/*
 * input_encoder.c
 *
 *  Created on: Nov 11, 2025
 *      Author: nicolas
 */

#include "acquisition/input_encoder.h"
#include "app.h"
#include "adc.h"
#include "stm32g4xx.h"
#include "tim.h"
#include "stdio.h"

#define ENCODER_CPR  1024.0f   // valeur de l'encodeur (par canal)
#define DT_SEC      0.01f // 10 ms => fTIM7 = 100Hz
#define PI 3.1415926f // Valeur de PI
#define ENCODER_COUNTER_MAX   65536
#define ENCODER_WRAP_DELTA   32768   // 2^15

int16_t count_now;
int16_t count_prev;
float speed_rpm;
float speed_rps;

int input_encoder_init (void)
{
	if (HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL) != HAL_OK)
	{
		return HAL_ERROR;
	}
    __HAL_TIM_SET_COUNTER(&htim3, 0);
    count_prev = 0;
    HAL_TIM_Base_Start_IT(&htim7);
    shell_add(&hshell1, "getspeed", input_encoder_get_speed, "Get motor speed");
    return HAL_OK;
}

void measure_speed (void)
{
    count_now = (int16_t)__HAL_TIM_GET_COUNTER(&htim3);

    int16_t delta = count_now - count_prev;

    /* Gestion wrap-around 16 bits */
    if (delta > ENCODER_WRAP_DELTA)
    {
        delta -= ENCODER_COUNTER_MAX;
    }
    else if (delta < -ENCODER_WRAP_DELTA)
    {
        delta += ENCODER_COUNTER_MAX;
    }

    count_prev = count_now;

    // tours par seconde
    speed_rps = ((float)delta * 2.0f * PI) / (ENCODER_CPR * DT_SEC);

    speed_rpm = speed_rps / 60.0f;
}

int input_encoder_get_speed(h_shell_t* h_shell, int argc, char** argv)
{
	int size;

	if(argc!=1)
	{
		size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "Need 1 argument : getspeedpolling\r\n");
		h_shell->drv.transmit(h_shell->print_buffer, size);
		return HAL_ERROR;
	}

	size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "measured_speed: %f \n\r", speed_rps);
	h_shell->drv.transmit(h_shell->print_buffer, size);
	return HAL_OK;
}
