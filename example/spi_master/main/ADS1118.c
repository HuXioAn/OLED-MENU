

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "soft_spi.h"
#include "sys/unistd.h"
#include "ADS1118.h"

//寄存器的默认值，默认ch0到GND，量程4.096V，连续转换
static uint16_t conf=0xC2EA;
//其中的15位SS，在连续转换下无效，而且只能在掉电下写入1

static double resolution[6]={0.1875,0.125,0.0625,0.03125,0.015625,0.0078125};



void ads_conf(enum ADS_MUX_TYPE MUX,enum ADS_PGA_TYPE PGA,enum ADS_CONV_MODE MODE){
    //先形成一个16位的数据，再发送
    //0 mux(3) pga(3) mode(1) 100 0 1 010

    //uint16_t conf=0x008A;
    conf=conf & (0x80FF);
    conf=conf | (MUX << 12) | (PGA << 9) | (MODE << 8);
    //printf("%x\r\n",conf);
    
    
    WR_16B(conf);

    vTaskDelay(15/portTICK_RATE_MS);
   

}

uint16_t ads_read_conv(void){
    //通过nop指令
    uint16_t cmd=conf & 0xfff9;

    CS(0);
    while(MISO());

    return WR_16B(cmd);
}

uint16_t ads_get_result(enum ADS_MUX_TYPE MUX){
    conf=conf & 0x8FFF;
    conf=conf | (MUX << 12);
    WR_16B(conf);


    //usleep(10);
    vTaskDelay(13/portTICK_RATE_MS);//不能太短（10ms），不然结果来源不变
    CS(0);
    while(MISO());



    return WR_16B(conf);
}

double to_mvoltage(uint16_t data,enum ADS_PGA_TYPE PGA){
    double voltage=0.0;
    if(data & 0x8000){//负值
        data &= 0x7fff;
        data=0x8000-data;
        voltage=-1*data*resolution[PGA];

        return voltage;

    }
    else{//正值
        voltage=data*resolution[PGA];
        return voltage;
    }
}













