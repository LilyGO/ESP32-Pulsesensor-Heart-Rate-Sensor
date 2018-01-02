

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "soc/uart_struct.h"
#include "string.h"





static const int RX_BUF_SIZE = 1024;
static char *TAG="[SERVO]";
#define TXD_PIN 22
#define RXD_PIN 23

void uart_init(void) {
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    uart_param_config(UART_NUM_2, &uart_config);
    uart_set_pin(UART_NUM_2, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    // We won't use a buffer for sending data.
    uart_driver_install(UART_NUM_2, RX_BUF_SIZE * 2, 0, 0, NULL, 0);

   // servo_read_info(0,62,1);
   // servo_read_info(1,46,2);
   // servo_read_info(2,60,2);
   // servo_read_info(3,56,2);
}

void pulse_send(char symbol,int dat)
{
 const char data[10]={'\0'};
 sprintf(data,"%c%d\r\n",symbol,dat);
 uart_write_bytes(UART_NUM_2,data,strlen(data));  
}
// int rxBytes = uart_read_bytes(UART_NUM_2,(const char*)response,6, 1000 / portTICK_RATE_MS);
// int txBytes=uart_write_bytes(UART_NUM_2,(const char*)cmdpackage,8);