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

volatile uint8_t adc_ready = 0;
uint16_t adc_raw;

int	input_analog_init(void)
{
	HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
    if (HAL_ADC_Start(&hadc1) != HAL_OK)
    {
        return HAL_ERROR;
    }
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&adc_raw, 1);
    shell_add(&hshell1, "getcurrentpolling", input_analog_get_current_polling, "Get current through polling");
    shell_add(&hshell1, "getcurrentdma", input_analog_get_current_DMA, "Get current through DMA");
    return HAL_OK;
}

float measure_current_polling(void)
{
    float v_meas, i_meas;

    adc_raw = HAL_ADC_GetValue(&hadc1);

    v_meas = ((float)adc_raw / ADC_RESOLUTION) * VREF;    // tension lue par l'ADC
    i_meas = (v_meas-1.47)/0.05;                      // courant en A

    return i_meas;
}

float measure_current_DMA(void)
{
    float v_meas, i_meas;

    if (!adc_ready)
        return 0.0f;
    adc_ready = 0;

    v_meas = ((float)adc_raw / ADC_RESOLUTION) * VREF;
    i_meas = (v_meas - 1.47f) / 0.05f;

    return i_meas;
}

int input_analog_get_current_polling(h_shell_t* h_shell, int argc, char** argv)
{
	int size;

	if(argc!=1)
	{
		size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "Need 1 argument : getcurrentpolling\r\n");
		h_shell->drv.transmit(h_shell->print_buffer, size);
		return HAL_ERROR;
	}

	float measured_current = measure_current_polling();
	size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "measured_current: %f \n\r", measured_current);
	h_shell->drv.transmit(h_shell->print_buffer, size);
	return HAL_OK;
}

int input_analog_get_current_DMA(h_shell_t* h_shell, int argc, char** argv)
{
	int size;

	if(argc!=1)
	{
		size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "Need 1 argument : getcurrentdma\r\n");
		h_shell->drv.transmit(h_shell->print_buffer, size);
		return HAL_ERROR;
	}

	float measured_current = measure_current_DMA();
	size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "measured_current: %f \n\r", measured_current);
	h_shell->drv.transmit(h_shell->print_buffer, size);
	return HAL_OK;
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    if (hadc->Instance == ADC1)
    {
        adc_ready = 1;
    }
}
