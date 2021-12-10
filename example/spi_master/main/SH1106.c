

#include "driver/spi_master.h"
#include "driver/gpio.h"
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include <string.h>
#include "freertos/task.h"
#include "esp_system.h"
#include "SH1106.h"
void oled_cmd(spi_device_handle_t spi, const uint8_t cmd)//代替发送指令
{
    //先控制DC引脚为cmd（拉低）,使用pre_cb
    //发送
    //最后把CS拉高解除，自动实现

    //gpio_set_level(PIN_NUM_DC, 0);//进入cmd模式


    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.length=8;                     //Command is 8 bits
    t.tx_buffer=&cmd;               //The data is the cmd itself
    t.user=(void*)0;                //D/C 低是命令
    ret=spi_device_polling_transmit(spi, &t);  //Transmit!
    assert(ret==ESP_OK);            //Should have had no issues.




}

void oled_data(spi_device_handle_t spi, const uint8_t *data, int len)
{
    esp_err_t ret;
    spi_transaction_t t;
    if (len==0) return;             //no need to send anything
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.length=len*8;                 //Len is in bytes, transaction length is in bits.
    t.tx_buffer=data;               //Data
    t.user=(void*)1;                //D/C 高是数据
    ret=spi_device_polling_transmit(spi, &t);  //Transmit!
    assert(ret==ESP_OK);            //Should have had no issues.
}

//This function is called (in irq context!) just before a transmission starts. It will
//set the D/C line to the value indicated in the user field.
void oled_spi_pre_transfer_callback(spi_transaction_t *t)
{
    int dc=(int)t->user;
    gpio_set_level(PIN_NUM_DC, dc);
}

void oled_clear(spi_device_handle_t spi)  
{  
	uint8_t i,n;
    uint8_t blank=0;		    
	for(i=0;i<8;i++)  
	{  
		oled_cmd(spi, 0xb0+i);    //设置页地址（0~7）
		oled_cmd(spi, 0x02);     //设置显示位置—列低地址
		oled_cmd(spi, 0x10);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)oled_data(spi,&blank,1); 
	} //更新显示
}

void oled_setpos(spi_device_handle_t spi,unsigned char x, unsigned char y) 
{ 
	oled_cmd(spi,0xb0+y);
	oled_cmd(spi,(((x+2)&0xf0)>>4)|0x10);
	oled_cmd(spi,((x+2)&0x0f)); 
}   

void oled_display_on(spi_device_handle_t spi)
{
	 oled_cmd(spi,0X8D );  //SET DCDC命令
	 oled_cmd(spi,0X14 );  //DCDC ON
	 oled_cmd(spi,0XAF );  //DISPLAY ON
}
//关闭OLED显示     
void oled_display_off(spi_device_handle_t spi)
{
	 oled_cmd(spi,0X8D );  //SET DCDC命令
	 oled_cmd(spi,0X10 );  //DCDC OFF
	 oled_cmd(spi,0XAE );  //DISPLAY OFF
}	


//Initialize the display
void oled_init(spi_device_handle_t spi)
{


    //Initialize non-SPI GPIOs
    gpio_set_direction(PIN_NUM_DC, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_NUM_RST, GPIO_MODE_OUTPUT);
  

    //Reset the display
    gpio_set_level(PIN_NUM_RST, 1);//先拉高
    vTaskDelay(100 / portTICK_RATE_MS);
    gpio_set_level(PIN_NUM_RST, 0);//低电平复位
    vTaskDelay(100 / portTICK_RATE_MS);
    gpio_set_level(PIN_NUM_RST, 1);//恢复高电平
    vTaskDelay(100 / portTICK_RATE_MS);

    
    oled_cmd(spi,0xAE);//--turn off oled panel
	oled_cmd(spi,0x02);//---set low column address
	oled_cmd(spi,0x10);//---set high column address
	oled_cmd(spi,0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	oled_cmd(spi,0x81);//--set contrast control register
	oled_cmd(spi,0xff); // Set SEG Output Current Brightness
	oled_cmd(spi,0xA1);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
	oled_cmd(spi,0xC8);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
	oled_cmd(spi,0xA6);//--set normal display
	oled_cmd(spi,0xA8);//--set multiplex ratio(1 to 64)
	oled_cmd(spi,0x3f);//--1/64 duty
	oled_cmd(spi,0xD3);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	oled_cmd(spi,0x00);//-not offset
	oled_cmd(spi,0xd5);//--set display clock divide ratio/oscillator frequency
	oled_cmd(spi,0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
	oled_cmd(spi,0xD9);//--set pre-charge period
	oled_cmd(spi,0xF1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	oled_cmd(spi,0xDA);//--set com pins hardware configuration
	oled_cmd(spi,0x12);
	oled_cmd(spi,0xDB);//--set vcomh
	oled_cmd(spi,0x40);//Set VCOM Deselect Level
	oled_cmd(spi,0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)
	oled_cmd(spi,0x02);//
	oled_cmd(spi,0x8D);//--set Charge Pump enable/disable
	oled_cmd(spi,0x14);//--set(0x10) disable
	oled_cmd(spi,0xA4);// Disable Entire Display On (0xa4/0xa5)
	oled_cmd(spi,0xA6);// Disable Inverse Display On (0xa6/a7) 
	oled_cmd(spi,0xAF);//--turn on oled panel
	
	oled_cmd(spi,0xAF); /*display ON*/ 


    oled_clear(spi);
    oled_setpos(spi,0,0);

 
}

