/* SPI Master example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/unistd.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "SH1106.h"
#include "menu.h"
#include "button.h"
#include "ADS1118.h"
#include "menu.h"


#include "driver/periph_ctrl.h"
#include "driver/timer.h"





QueueHandle_t button_qhandle;



//QueueHandle_t button_qhandle;//在app_main里建立队列


//spi声明在menu.c,初始化在menu任务头里

//static const char TAG[]="MAIN";


BUTTON down={
    "down",
    4,
    GPIO_PIN_INTR_NEGEDGE,
    1,
    MSG_DOWN,
    NULL
};






void app_main()
{   TaskHandle_t th=NULL;
	button_qhandle = xQueueCreate(10,1);
    //vTaskDelay(5000/portTICK_RATE_MS);
    xTaskCreate(task_display,"显示任务",4096,NULL,3,&th);
	printf("init over\r\n");

    //vTaskDelay(2000/portTICK_RATE_MS);

    // uint8_t msg=MSG_DOWN;
    // for(int i=0;i<80;i++){
    //     xQueueSend(button_qhandle,&msg,0);
    //     printf("%d\r\n",i);
    // }

    button_init(&down);






    // vTaskDelay(5000/portTICK_RATE_MS);
    // uint8_t msg=MSG_DOWN;
    // xQueueSend(button_qhandle,&msg,0);
    // vTaskDelay(1000/portTICK_RATE_MS);
    // msg=MSG_ENTER;
    // xQueueSend(button_qhandle,&msg,0);
    //     vTaskDelay(1000/portTICK_RATE_MS);
    // msg=MSG_DOWN;
    // xQueueSend(button_qhandle,&msg,0);
    //     vTaskDelay(1000/portTICK_RATE_MS);
    //         msg=MSG_ENTER;
    // xQueueSend(button_qhandle,&msg,0);
    //     vTaskDelay(1000/portTICK_RATE_MS);
    // msg=MSG_QUIT;
    // xQueueSend(button_qhandle,&msg,0);
    //     vTaskDelay(1000/portTICK_RATE_MS);
    // msg=MSG_UP;
    // xQueueSend(button_qhandle,&msg,0);
    // while(1)vTaskDelay(1000/portTICK_RATE_MS);
}
