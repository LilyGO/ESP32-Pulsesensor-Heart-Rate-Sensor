 #include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "esp_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"


#include "esp_system.h"

#include "esp_log.h"
 
#include "pulse_adc.h"


 
 

void adc_init(void){
    adc1_config_width(ADC_WIDTH_BIT_10);
    adc1_config_channel_atten(ADC1_CHANNEL, ADC_ATTEN_DB_11);
    // esp_adc_cal_get_characteristics(V_REF, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_10, &characteristics);

 }
