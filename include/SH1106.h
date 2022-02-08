#ifndef _SH1106_H_
#define _SH1106_H_

#define PIN_NUM_MISO 25//这个没用到

#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK  19
#define PIN_NUM_CS   22

#define PIN_NUM_DC   21
#define PIN_NUM_RST  18

#define OLED_HEIGHT 64
#define OLED_WIDTH 128


void oled_cmd(spi_device_handle_t spi, const uint8_t cmd);
void oled_data(spi_device_handle_t spi, const uint8_t *data, int len);

void oled_spi_pre_transfer_callback(spi_transaction_t *t);


void oled_clear(spi_device_handle_t spi);
void oled_setpos(spi_device_handle_t spi,unsigned char x, unsigned char y);

void oled_display_on(spi_device_handle_t spi);
void oled_display_off(spi_device_handle_t spi);

void oled_init(spi_device_handle_t spi);

void oled_plot_buf(spi_device_handle_t spi,uint8_t* BUF);




#endif