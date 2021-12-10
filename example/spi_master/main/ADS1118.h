#ifndef _ADS1118_H_
#define _ADS1118_H_



#define PIN_NUM_MISO 25

#define PIN_NUM_MOSI 23

#define PIN_NUM_CLK  19

#define PIN_NUM_CS   22





//被测量的通道以及差分选择
enum ADS_MUX_TYPE{
    ADS_MUX_0to1=0,
    ADS_MUX_0to3,
    ADS_MUX_1to3,
    ADS_MUX_2to3,
    ADS_MUX_0toG,
    ADS_MUX_1toG,
    ADS_MUX_2toG,
    ADS_MUX_3toG
};


//FSR 满量程选择
enum ADS_PGA_TYPE {

ADS_PGA_6_144=0,
ADS_PGA_4_096,
ADS_PGA_2_048,
ADS_PGA_1_024,
ADS_PGA_0_512,
ADS_PGA_0_256

};

enum ADS_CONV_MODE{
ADS_CONTINUOUS_MODE=0,
ADS_SINGLE_MODE
};

void ads_conf(enum ADS_MUX_TYPE MUX,enum ADS_PGA_TYPE PGA,enum ADS_CONV_MODE MODE);

uint16_t ads_read_conv(void);

uint16_t ads_get_result(enum ADS_MUX_TYPE MUX);


double to_mvoltage(uint16_t data,enum ADS_PGA_TYPE PGA);









#endif