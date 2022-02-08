/*
 * @Author: HuXiaoan 
 * @Date: 2022-02-08 14:42:35 
 * @Last Modified by:   HuXiaoan 
 * @Last Modified time: 2022-02-08 14:42:35 
 */
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/spi_master.h"
#include "SH1106.h"
#include "menu.h"
#include "button.h"

QueueHandle_t button_qhandle;
BUTTON down={
    "down",
    33,
    GPIO_PIN_INTR_NEGEDGE,
    1,
    MSG_DOWN,
    NULL
};

BUTTON up={
    "up",
    32,
    GPIO_PIN_INTR_NEGEDGE,
    1,
    MSG_UP,
    NULL
};

BUTTON enter={
    "enter",
    25,
    GPIO_PIN_INTR_NEGEDGE,
    1,
    MSG_ENTER,
    NULL
};

BUTTON quit={
    "quit",
    26,
    GPIO_PIN_INTR_NEGEDGE,
    1,
    MSG_QUIT,
    NULL
};

void app_main(void)
{
    TaskHandle_t th=NULL;
	button_qhandle = xQueueCreate(10,1);
    //vTaskDelay(5000/portTICK_RATE_MS);
    xTaskCreate(task_display,"显示任务",4096,NULL,3,&th);
	printf("init over\r\n");


    button_init(&down);
    button_init(&up);
    button_init(&enter);
    button_init(&quit);

}
