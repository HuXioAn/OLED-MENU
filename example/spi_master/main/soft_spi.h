#ifndef _SOFT_SPI_H_
#define _SOFT_SPI_H_

#define PIN_NUM_MISO 25

#define PIN_NUM_MOSI 23

#define PIN_NUM_CLK  19

#define PIN_NUM_CS   22


#define MOSI(x) (gpio_set_level(PIN_NUM_MOSI,x))
#define CS(x) (gpio_set_level(PIN_NUM_CS,x))
#define SCK(x) (gpio_set_level(PIN_NUM_CLK,x))
#define MISO() (gpio_get_level(PIN_NUM_MISO))

void init_pins(void);

uint16_t WR_16B(uint16_t cmd);


#endif