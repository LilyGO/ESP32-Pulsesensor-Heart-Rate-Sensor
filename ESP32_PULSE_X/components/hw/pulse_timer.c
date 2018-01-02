
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "esp_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "soc/timer_group_struct.h"
#include "driver/periph_ctrl.h"
#include "driver/timer.h"
#include "esp_log.h"
#include "esp_system.h"
#include "pulse_timer.h"
#include "pulse_adc.h"

#define TIMER_DIVIDER         16  //  Hardware timer clock divider
#define TIMER_SCALE           (TIMER_BASE_CLK / TIMER_DIVIDER)  // convert counter value to seconds
#define TIMER_INTERVAL0_SEC   (0.002) // sample test interval for the first timer
#define TEST_WITH_RELOAD      1        // testing will be done with auto reload

 xQueueHandle my_queue;
static char*TAG="[PULSE]";

 void IRAM_ATTR timer_group0_isr(void *para)
 {
    int timer_idx = (int) para;

    uint32_t pulse_val=adc1_get_raw(ADC1_CHANNEL);          // read the Pulse Senso
  
  
/*以下为定时器配置、重载*/
      
        TIMERG0.hw_timer[timer_idx].update = 1;
        if (TIMERG0.int_st_timers.val & BIT(timer_idx)) {
        TIMERG0.int_clr_timers.t0 = 1;    } 
        TIMERG0.hw_timer[timer_idx].config.alarm_en = TIMER_ALARM_EN;
 
 
        xQueueSendFromISR(my_queue, &pulse_val , NULL);
}

void pulse_timer_init(void)
{
    /* Select and initialize basic parameters of the timer */
    timer_config_t config;
    config.divider = TIMER_DIVIDER;
    config.counter_dir = TIMER_COUNT_UP;
    config.counter_en = TIMER_PAUSE;
    config.alarm_en = TIMER_ALARM_EN;
    config.intr_type = TIMER_INTR_LEVEL;
    config.auto_reload = TEST_WITH_RELOAD;
    timer_init(TIMER_GROUP_0, TIMER_0, &config);

    /* Timer's counter will initially start from value below.
       Also, if TEST_WITH_RELOAD is set, this value will be automatically reload on alarm */
    timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0x00000000ULL);

    /* Configure the alarm value and the interrupt on alarm. */
    timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, TIMER_INTERVAL0_SEC * TIMER_SCALE);
    timer_enable_intr(TIMER_GROUP_0, TIMER_0);
    timer_isr_register(TIMER_GROUP_0, TIMER_0, timer_group0_isr,(void *) TIMER_0, ESP_INTR_FLAG_IRAM, NULL);
    my_queue = xQueueCreate(1000, sizeof(uint32_t)); 
    xTaskCreate(PULSE_PROCESS,"PULSE_PROCESS",2048,NULL,9,NULL);
    timer_start(TIMER_GROUP_0, TIMER_0);


}
 
 void PULSE_PROCESS(void * pvParameters)
{
   int pulse_val;
   
    while (1) {
     xQueueReceive(my_queue, &pulse_val, portMAX_DELAY);
    PULSESENSOR.Signal=pulse_val;          // read the Pulse Senso
    PULSESENSOR.sampleCounter += 2;                         // keep track of the time in mS with this variable
    PULSESENSOR.Num = PULSESENSOR.sampleCounter - PULSESENSOR.lastBeatTime;       // monitor the time since the last beat to avoid noise


    //  find the peak and trough of the pulse wave
  if(PULSESENSOR.Signal < PULSESENSOR.thresh && PULSESENSOR.Num > (PULSESENSOR.IBI/5)*3){       // avoid dichrotic noise by waiting 3/5 of last IBI
    if (PULSESENSOR.Signal < PULSESENSOR.T){                        // T is the trough
      PULSESENSOR.T = PULSESENSOR.Signal;                         // keep track of lowest point in pulse wave 
    }
  }

  if(PULSESENSOR.Signal > PULSESENSOR.thresh && PULSESENSOR.Signal > PULSESENSOR.P){          // thresh condition helps avoid noise
    PULSESENSOR.P = PULSESENSOR.Signal;                             // P is the peak
  }                                        // keep track of highest point in pulse wave

  //  NOW IT'S TIME TO LOOK FOR THE HEART BEAT
  // signal surges up in value every time there is a pulse
  if (PULSESENSOR.Num > 250){                                   // avoid high frequency noise
    if ( (PULSESENSOR.Signal > PULSESENSOR.thresh) && (PULSESENSOR.Pulse == false) && (PULSESENSOR.Num > (PULSESENSOR.IBI/5)*3) ){        
      PULSESENSOR.Pulse = true;                               // set the Pulse flag when we think there is a pulse
      
      PULSESENSOR.IBI = PULSESENSOR.sampleCounter - PULSESENSOR.lastBeatTime;         // measure time between beats in mS
      PULSESENSOR.lastBeatTime = PULSESENSOR.sampleCounter;               // keep track of time for next pulse

      if(PULSESENSOR.secondBeat){                        // if this is the second beat, if secondBeat == TRUE
        PULSESENSOR.secondBeat = false;                  // clear secondBeat flag
        for(int i=0; i<=9; i++){             // seed the running total to get a realisitic BPM at startup
          PULSESENSOR.rate[i] = PULSESENSOR.IBI;                      
        }
      }

      if(PULSESENSOR.firstBeat){                         // if it's the first time we found a beat, if firstBeat == TRUE
        PULSESENSOR.firstBeat = false;                   // clear firstBeat flag
        PULSESENSOR.secondBeat = true;                   // set the second beat flag
 //       sei();                               // enable interrupts again
         continue;                            // IBI value is unreliable so discard it
      }   


      // keep a running total of the last 10 IBI values
      unsigned int runningTotal= 0;                  // clear the runningTotal variable    

      for(int i=0; i<=8; i++){                // shift data in the rate array
        PULSESENSOR.rate[i] = PULSESENSOR.rate[i+1];                  // and drop the oldest IBI value 
        runningTotal += PULSESENSOR.rate[i];              // add up the 9 oldest IBI values
      }

      PULSESENSOR.rate[9] = PULSESENSOR.IBI;                          // add the latest IBI to the rate array
      runningTotal += PULSESENSOR.rate[9];                // add the latest IBI to runningTotal
      runningTotal /= 10;                     // average the last 10 IBI values 
      PULSESENSOR.BPM = 60000/runningTotal;               // how many beats can fit into a minute? that's BPM!
      PULSESENSOR.QS = true;                              // set Quantified Self flag 
      // QS FLAG IS NOT CLEARED INSIDE THIS ISR
    }                       
  }
  if (PULSESENSOR.Signal < PULSESENSOR.thresh && PULSESENSOR.Pulse == true){   // when the values are going down, the beat is over
     
    PULSESENSOR.Pulse = false;                         // reset the Pulse flag so we can do it again
    PULSESENSOR.amp = PULSESENSOR.P - PULSESENSOR.T;                           // get amplitude of the pulse wave
    PULSESENSOR.thresh = PULSESENSOR.amp/2 + PULSESENSOR.T;                    // set thresh at 50% of the amplitude
    PULSESENSOR.P = PULSESENSOR.thresh;                            // reset these for next time
    PULSESENSOR.T = PULSESENSOR.thresh;
  }

  if (PULSESENSOR.Num > 2500){                           // if 2.5 seconds go by without a beat
    PULSESENSOR.thresh = 512;                          // set thresh default
    PULSESENSOR.P = 512;                               // set P default
    PULSESENSOR.T = 512;                               // set T default
    PULSESENSOR.lastBeatTime = PULSESENSOR.sampleCounter;          // bring the lastBeatTime up to date        
    PULSESENSOR.firstBeat = true;                      // set these to avoid noise
    PULSESENSOR.secondBeat = false;                    // when we get the heartbeat back
  }

 }
}


void pulsesensor_init(void)
{
PULSESENSOR.IBI = 600;             // holds the time between beats, must be seeded! 
PULSESENSOR.Pulse = false;     // true when pulse wave is high, false when it's low
PULSESENSOR.QS = false;        // becomes true when Arduoino finds a beat.
PULSESENSOR.sampleCounter = 0;          // used to determine pulse timing
PULSESENSOR.lastBeatTime = 0;           // used to find IBI
PULSESENSOR.P =512;                      // used to find peak in pulse wave, seeded
PULSESENSOR.T = 512;                     // used to find in pulse wave, seeded
PULSESENSOR.thresh = 512;                // used to find instant moment of heart beat, seeded
PULSESENSOR.amp = 100;                   // used to hold amplitude of pulse waveform, seeded
PULSESENSOR.firstBeat = true;        // used to seed rate array so we startup with reasonable BPM
PULSESENSOR.secondBeat = false;      // used to seed rate array so we startup with reasonable BPM 
}
 