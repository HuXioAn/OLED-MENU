/*
 * @Author: HuXiaoan 
 * @Date: 2022-02-08 14:42:32 
 * @Last Modified by: HuXiaoan
 * @Last Modified time: 2022-02-08 14:48:14
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "sys/unistd.h"
#include "esp_rom_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_err.h"
#include "driver/gpio.h"
#include "button.h"


static const char *TAG = "button";


extern QueueHandle_t button_qhandle;


static void IRAM_ATTR gpio_isr_handler(void * butt_p){
    static size_t last_tick=0;
    BUTTON* but=(BUTTON*)butt_p;
    
    //去抖
    if((xTaskGetTickCountFromISR()-last_tick)*portTICK_RATE_MS > 200){

        last_tick=xTaskGetTickCountFromISR();
    
        uint8_t msg=(uint8_t)(but->message);
        BaseType_t highertask=pdTRUE;
        xQueueSendFromISR(button_qhandle,&msg,&highertask);

        }
    

}





uint8_t button_init(BUTTON * but){

    esp_err_t ret;
    static esp_err_t service_ret=-1;
    //根据传入的button结构体，初始化一个gpio的中断
    gpio_config_t io_conf;


    io_conf.intr_type = but->int_level;
    io_conf.pin_bit_mask = (1ULL << (but->pin_num));
    io_conf.mode = GPIO_MODE_INPUT;

    if(but->pull == 1){
        io_conf.pull_up_en = 1;//上拉开启
        io_conf.pull_down_en = 0;
    }else if(but->pull == -1){
        io_conf.pull_up_en = 0;
        io_conf.pull_down_en = 1;
    }else{
        io_conf.pull_up_en = 0;
        io_conf.pull_down_en = 0;
    }

    gpio_config(&io_conf);

    if(ESP_OK != service_ret){
    service_ret=gpio_install_isr_service(0);
    ESP_ERROR_CHECK(service_ret);
    }
    ret=gpio_isr_handler_add(but->pin_num, gpio_isr_handler, (void*) but);
    ESP_ERROR_CHECK(ret);
    return 0;


}





