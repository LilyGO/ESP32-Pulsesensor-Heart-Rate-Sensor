
#ifndef _PULSE_ADC_H
#define _PULSE_ADC_H
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#define V_REF   1100
#define ADC1_CHANNEL (ADC1_CHANNEL_6)      //GPIO 34
void adc_init(void);
#endif