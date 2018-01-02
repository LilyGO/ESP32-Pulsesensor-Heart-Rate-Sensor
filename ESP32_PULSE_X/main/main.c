
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <nvs.h>
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "driver/gpio.h"
#include "driver/i2c.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/api.h"
#include "lwip/tcp.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "apps/sntp/sntp.h"
#include "pulse_uart.h"
#include "pulse_timer.h"
#include "pulse_adc.h"
#include "pulse_i2c.h"

#include "oled.h"

/////////////////////////////////////////////////////
///////////////////////////
 static char *TAG="[PULSE]";
#define EXAMPLE_WIFI_SSID CONFIG_WIFI_SSID
#define EXAMPLE_WIFI_PASS CONFIG_WIFI_PASSWORD
static EventGroupHandle_t wifi_event_group;

const int CONNECTED_BIT = BIT0;


static void obtain_time(void);
static void initialize_sntp(void);
static void initialise_wifi(void);
static esp_err_t event_handler(void *ctx, system_event_t *event);

static char timeinfo[64];
 
void OLED_DISPLAY(void)
{
      
    char BPM[4]={0};
    sprintf(BPM,"%d",PULSESENSOR.BPM);

    OLED_Clear();
    OLED_ShowString(0,0,PULSESENSOR.rtt_time,24);
    if(PULSESENSOR.QS)
    {
 
     if(memcmp(timeinfo,PULSESENSOR.rtt_time,strlen(PULSESENSOR.rtt_time))!=0)
      {
        OLED_ShowBMP(25,27);
        memcpy(timeinfo,PULSESENSOR.rtt_time,strlen(PULSESENSOR.rtt_time));
      }
    
      OLED_ShowString(60, 28,BPM,24);   
    }
    /* Update screen, send changes to LCD */
     
    OLED_Refresh_Gram();//更新显示
   
}

void device_time_init(void)
{
  

    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    // Is time set? If not, tm_year will be (1970 - 1900).
    if (timeinfo.tm_year < (2016 - 1900)) {
        ESP_LOGI(TAG, "Time is not set yet. Connecting to WiFi and getting time over NTP.");
        obtain_time();
        // update 'now' variable with current time
    }

}

void RTT_TIME_GET(void *pvParameters)
{
    time_t now;
    struct tm timeinfo;
    for (;;)
    {
      time(&now);
      setenv("TZ", "CST-8", 1);
      tzset();
      localtime_r(&now, &timeinfo);
      strftime(PULSESENSOR.rtt_time, sizeof(PULSESENSOR.rtt_time), "%T", &timeinfo);
      OLED_DISPLAY();
      vTaskDelay(50 / portTICK_PERIOD_MS);
      
    } 
}




static void obtain_time(void)
{
    ESP_ERROR_CHECK( nvs_flash_init() );
    initialise_wifi();
    xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT,
                        false, true, portMAX_DELAY);
    initialize_sntp();

    // wait for time to be set
    time_t now = 0;
    struct tm timeinfo = { 0 };
    int retry = 0;
    const int retry_count = 10;
    while(timeinfo.tm_year < (2016 - 1900) && ++retry < retry_count) {
        ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        time(&now);
        localtime_r(&now, &timeinfo);
    }

    ESP_ERROR_CHECK( esp_wifi_stop() );
}

static void initialize_sntp(void)
{
    ESP_LOGI(TAG, "Initializing SNTP");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_init();
}

static void initialise_wifi(void)
{
    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_WIFI_SSID,
            .password = EXAMPLE_WIFI_PASS,
        },
    };
    ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
}

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
    
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
        break;
    default:
        break;
    }
    return ESP_OK;
}
void app_main()
{
   char *start="HELLO";
    device_time_init();
    i2c_init();
    OLED_Init();
    
    OLED_Clear();
    OLED_ShowString(30,20,start,24);
 
    /* Update screen, send changes to LCD */
     
    OLED_Refresh_Gram();//更新显示
    
    vTaskDelay(500 / portTICK_PERIOD_MS);
    // ESP_LOGI(TAG, "app_main stack: %d\n", uxTaskGetStackHighWaterMark(NULL));
    uart_init();
    adc_init();
    pulsesensor_init();
    pulse_timer_init();

    

 xTaskCreate(RTT_TIME_GET,"RTT_TIME_GET",2048,NULL,7,NULL);
 while(1)
 {
         pulse_send('S',PULSESENSOR.Signal);
         if(PULSESENSOR.QS)
         {
         pulse_send('B',PULSESENSOR.BPM);
         pulse_send('Q',PULSESENSOR.IBI);   
         }
         vTaskDelay(20 / portTICK_PERIOD_MS);

 }

}
