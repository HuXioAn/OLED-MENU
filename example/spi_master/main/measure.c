#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "SH1106.h"
#include "menu.h"
#include "button.h"
#include "ADS1118.h"

static const char TAG[]="measure";
extern QueueHandle_t button_qhandle;

/*
这个文件，负责实现三个测量：电压、电流、电阻任务
这些任务的优先级很低而且常常处于自己挂起suspend中，等待menu的enter来resume。
进入之前，menu会尝试清空button queue，但有几率不成功（理论上没有）。所以要检测

还是通过读取按键队列来改变状态，在quit主动退出之后，
要恢复menu任务，主动挂起自己等待下一次唤醒

在循环方面，为后期统一化做准备，目前先由函数自己管理。

*/




void task_vol(void){
    //读取对应通道值，换算，显示。
    enum ADS_MUX_TYPE MUX=ADS_MUX_0toG;
    enum ADS_PGA_TYPE PGA=ADS_PGA_4_096;
    uint16_t result=0;
    double mvoltage=0;
    enum message button;
    vTaskSuspend(NULL);
    while(1){
        //ads_conf
        ads_conf(MUX,PGA,ADS_CONTINUOUS_MODE);
        //显示固定的文字

        //读取结果，换算
        result=ads_read_conv();
        mvoltage=to_mvoltage(result,PGA);
        //显示数据


        //轮询buttonqueue
        if(pdTRUE == xQueueReceive(button_qhandle,&button,0)){
            //接受到了动作
            if(QUIT == button){
                //退出
                vTaskSuspend(NULL);
                continue;
            }
        }
        
    }
}


void task_amp(void){
    //读取对应通道值，换算，显示。
    enum ADS_MUX_TYPE MUX=ADS_MUX_1toG;
    enum ADS_PGA_TYPE PGA=ADS_PGA_4_096;
    uint16_t result=0;
    double mvoltage=0;
    enum message button;
    vTaskSuspend(NULL);
    while(1){
        //ads_conf
        ads_conf(MUX,PGA,ADS_CONTINUOUS_MODE);
        //显示固定的文字

        //读取结果，换算
        result=ads_read_conv();
        mvoltage=to_mvoltage(result,PGA);
        //显示数据
        

        //轮询buttonqueue
        if(pdTRUE == xQueueReceive(button_qhandle,&button,0)){
            //接受到了动作
            if(QUIT == button){
                //退出
                vTaskSuspend(NULL);
                continue;
            }
        }
    }
}


void task_res(void){
    //读取对应通道值，换算，显示。
    enum ADS_MUX_TYPE MUX=ADS_MUX_2to3;
    enum ADS_PGA_TYPE PGA=ADS_PGA_2_048;
    uint16_t result=0;
    double mvoltage=0;
    enum message button;
    vTaskSuspend(NULL);
    while(1){
        //ads_conf
        ads_conf(MUX,PGA,ADS_CONTINUOUS_MODE);
        //显示固定的文字

        //读取结果，换算
        result=ads_read_conv();
        mvoltage=to_mvoltage(result,PGA);
        //显示数据
        

        //轮询buttonqueue
        if(pdTRUE == xQueueReceive(button_qhandle,&button,0)){
            //接受到了动作
            if(QUIT == button){
                //退出
                vTaskSuspend(NULL);
                continue;
            }
        }       
    }
}


/*
void task_measure(void){
    while(1){
        switch(mode){
            case VOLTAGE:
                task_vol();
                break;
            case CURRENT:
                task_amp();
                break;
            case RESISTOR:
                task_res();
                break;
            default:

        }
    }
}

*/


