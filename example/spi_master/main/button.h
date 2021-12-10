#ifndef _BUTTON_H_
#define _BUTTON_H_

#include "driver/gpio.h"

enum int_level {
    INT_LEVEL_RISE=0,
    INT_LEVEL_DROP,
    INT_LEVEL_BOTH

};

enum message {
    MSG_ENTER=1,
    MSG_UP,
    MSG_DOWN,
    MSG_QUIT

};


typedef struct _button{
    const char * name;
    uint8_t pin_num;
    GPIO_INT_TYPE int_level;
    int8_t pull;//-1 0 1 d n u
    enum message message;
    void (*isr_func)(void);
}BUTTON;


uint8_t button_init(BUTTON * but);



#endif