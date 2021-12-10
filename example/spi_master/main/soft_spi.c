//在于实现一个mode1的spi



#include "sys/unistd.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "soft_spi.h"





void init_pins(void){
    //三个输出，一个输入

    gpio_config_t io_conf;

     io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
     io_conf.mode=GPIO_MODE_OUTPUT;
     io_conf.pin_bit_mask=((1ULL << PIN_NUM_MOSI) | (1ULL << PIN_NUM_CLK) | (1ULL << PIN_NUM_CS));
     io_conf.pull_down_en = 0;
     io_conf.pull_up_en = 0;
     gpio_config(&io_conf);

    io_conf.mode=GPIO_MODE_INPUT;
    io_conf.pin_bit_mask=(1ULL << PIN_NUM_MISO);
    gpio_config(&io_conf);

    gpio_set_level(PIN_NUM_CLK,0);
    gpio_set_level(PIN_NUM_CS,1);


}

uint16_t WR_16B(uint16_t cmd){

    uint16_t data=0;
    CS(0);
    usleep(2);

    for(int i=0;i<16;i++){
        SCK(1);

        if(cmd & 0x8000)MOSI(1);
        else MOSI(0);
        cmd <<= 1;
        
        usleep(2);

        SCK(0);
        usleep(2);

        data <<=1;
        if(MISO())data|=0x0001;

        

    }

    SCK(0);
    usleep(10);
    MOSI(0);

    CS(1);
    usleep(10);

    return data;


}