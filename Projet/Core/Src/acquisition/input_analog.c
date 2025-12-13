/*
 * analog_input.c
 *
 *  Created on: Nov 11, 2025
 *      Author: nicolas
 */

#include "acquisition/input_analog.h"
#include "adc.h"
#include "stm32g4xx.h"
#include "stdio.h"

#define ADC_RESOLUTION        4095.0f  // 12 bits -> 0..4095
#define VREF                  3.3f
#define AMP_GAIN              1     // 1.0 si pas d'amplification
#define ADC_POLL_TIMEOUT_MS   10        // timeout pour le polling

int	input_analog_init(void)
{
	HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
    if (HAL_ADC_Start(&hadc1) != HAL_OK)
    {
        return HAL_ERROR;
    }
    shell_add(&hshell1, "getcurrent", input_analog_get_current, "Get current");
    return HAL_OK;

}

float measure_current_polling(void)
{
    uint32_t raw;
    float v_meas, i_meas;

    raw = HAL_ADC_GetValue(&hadc1);

    v_meas = ((float)raw / ADC_RESOLUTION) * VREF;    // tension lue par l'ADC
    i_meas = (v_meas-1.47)/0.05;                      // courant en A

    return i_meas;
}

float measure_current_DMA(void)
{
    uint32_t raw;
    float v_meas, i_meas;

	HAL_ADC_Start_DMA(&hadc1, &raw, sizeof(raw));     // courant en A

    v_meas = ((float)raw / ADC_RESOLUTION) * VREF;    // tension lue par l'ADC
    i_meas = (v_meas-1.47)/0.05;

    return i_meas;
}

int input_analog_get_current(h_shell_t* h_shell, int argc, char** argv)
{
	int size;

	if(argc!=1)
	{
		size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "Need 1 argument : getcurrent\r\n");
		h_shell->drv.transmit(h_shell->print_buffer, size);
		return HAL_ERROR;
	}

	float measured_current = measure_current_polling();
	size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "measured_current: %f \n\r", measured_current);
	h_shell->drv.transmit(h_shell->print_buffer, size);
	return HAL_OK;
}
