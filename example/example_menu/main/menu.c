
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "SH1106.h"
#include "menu.h"
#include "button.h"
#include "chara.h"

#define LEFT_MARGIN 16

static const char *TAG = "menu";

uint8_t BUF[1024] = {0};

spi_device_handle_t spi;

extern QueueHandle_t button_qhandle; 

uint8_t test_1(void)
{

    printf("test1\r\n");

    return 0;
}

uint8_t test_2(void)
{

    printf("test2\r\n");

    return 0;
}

uint8_t test_3(void)
{

    printf("test3\r\n");

    return 0;
}

uint8_t test_4(void)
{

    printf("test4\r\n");

    return 0;
}

MENU EMenuListTest[] =
    {   
        {MENU_L_0,       //菜单等级
        "L0",           //中文
        {NULL},
        MENU_TYPE_LIST, //菜单类型
        NULL},           //菜单函数，功能菜单才会执行，有子菜单的不会执行

            {MENU_L_1,       //菜单等级
            "L1",           //中文
            {&yi_t,NULL},
            MENU_TYPE_LIST, //菜单类型
            NULL},           //菜单函数，功能菜单才会执行，有子菜单的不会执行
                {MENU_L_2,       //菜单等级
                "TEST1",        //中文
                {&ni_t,&ni_t,&wo_t,NULL},
                MENU_TYPE_FUN,  //菜单类型
                test_1},         //菜单函数，功能菜单才会执行，有子菜单的不会执行

                {MENU_L_2,      //菜单等级
                "TEST2",       //中文
                {&ni_t,&ni_t,NULL},
                MENU_TYPE_FUN, //菜单类型
                test_2},        //菜单函数，功能菜单才会执行，有子菜单的不会执行
                {MENU_L_2,       //菜单等级
                "TEST1",        //中文
                {&ni_t,&ni_t,&ni_t,NULL},
                MENU_TYPE_FUN,  //菜单类型
                test_1},         //菜单函数，功能菜单才会执行，有子菜单的不会执行
                {MENU_L_2,       //菜单等级
                "TEST1",        //中文
                {&ni_t,&ni_t,&ni_t,&ni_t,NULL},
                MENU_TYPE_FUN,  //菜单类型
                test_1},         //菜单函数，功能菜单才会执行，有子菜单的不会执行
                {MENU_L_2,       //菜单等级
                "TEST1",        //中文
                {&ni_t,&wo_t,&wo_t,NULL},
                MENU_TYPE_FUN,  //菜单类型
                test_1},         //菜单函数，功能菜单才会执行，有子菜单的不会执行

            {MENU_L_1,       //菜单等级
            "L2",           //中文
            {&er_t,NULL},
            MENU_TYPE_LIST, //菜单类型
            NULL},           //菜单函数，功能菜单才会执行，有子菜单的不会执行
                {MENU_L_2,       //菜单等级
                "TEST3",        //中文
                {&er_t,&yi_t,NULL},
                MENU_TYPE_FUN,  //菜单类型
                test_3},         //菜单函数，功能菜单才会执行，有子菜单的不会执行

                {MENU_L_2,      //菜单等级
                "TEST4",       //中文
                {&er_t,&er_t,NULL},
                MENU_TYPE_FUN, //菜单类型
                test_4},        //菜单函数，功能菜单才会执行，有子菜单的不会执行

        /*最后的菜单是结束菜单，无意义*/
        {MENU_L_0,       //菜单等级
        "END",          //中文
        {NULL},
        MENU_TYPE_NULL, //菜单类型
        NULL},           //菜单函数，功能菜单才会执行，有子菜单的不会执行
};

void task_display()
{ //将会作为主要函数运行在显示任务中

    //spi初始化
    esp_err_t ret;
    
    spi_bus_config_t buscfg={
        .miso_io_num=PIN_NUM_MISO,
        .mosi_io_num=PIN_NUM_MOSI,
        .sclk_io_num=PIN_NUM_CLK,
        .quadwp_io_num=-1,
        .quadhd_io_num=-1,
        .max_transfer_sz=0,//默认4096
    };
    spi_device_interface_config_t devcfg={

        .clock_speed_hz=10*1000*1000,           //Clock out at 10 MHz
        .mode=3,                                //极性和相位都为1
        .spics_io_num=PIN_NUM_CS,               //CS pin
        .queue_size=5,                          //队列同时等待5个
        .pre_cb=oled_spi_pre_transfer_callback,  //Specify pre-transfer callback to handle D/C line
    };
    ret=spi_bus_initialize(HSPI_HOST, &buscfg, 1);
    ESP_ERROR_CHECK(ret);
    ret=spi_bus_add_device(HSPI_HOST, &devcfg, &spi);
    ESP_ERROR_CHECK(ret);
  
    

    //oled初始化
    oled_init(spi);


    MENU *cur_menu = EMenuListTest; //从头开始
    MENU *menu_path[MENU_L_MAX]={NULL};//这里加入一个路径记录，便于返回
    //在每次enter和quit时进行操作
    uint8_t button=0;

    if (cur_menu->l == MENU_L_0)
    {
        //如果是头,开始显示第一个菜单项
        cur_menu += 1;
    }
    else if ((MENU_L_MAX >= cur_menu->l) && (cur_menu->l > MENU_L_0))
    { //不是开头的一个
        while (cur_menu->l > MENU_L_0)
            cur_menu -= 1;

        cur_menu++;
    }
    else
    { //出现错误
        ESP_LOGE(TAG, "error menulist pointer!\r\n");
    }

    menu_path[0]=cur_menu-1;//找到最头上的0级

    while (1)
    {

        //查找同页面节点实现方法,向上向下查找，如果有等级更大的就继续，一旦遇到等级小的就停止这方向的查找
        MENU *UP[4] = {NULL},*DOWN[4] = {NULL};//放在这里清空
        //这两个都是以cur开头

        //寻找上方条目
        for (int i = 0, g = 0; (cur_menu - i)->l >= (cur_menu->l); i++)
        {
            //向上
            if ((cur_menu - i)->l == (cur_menu->l))
            {
                UP[g] = (cur_menu - i);
                g++;
                if (g == 4)
                    break;
            }
        }

        //寻找下方条目
        for (int i = 0, g = 0; (cur_menu + i)->l >= (cur_menu->l); i++)
        {
            //向上
            if ((cur_menu + i)->l == (cur_menu->l))
            {
                DOWN[g] = (cur_menu + i);
                g++;
                if (g == 4)
                    break;
            }
        }

        //现在有了cur的上下相同
        //把cur放在第二个，可以让up两个，down三个

        //组织图像，需要一个函数，传入buf、内容和位置1-4，是否选中

        /*
        if(UP[1] != NULL)func;
        func;
        if(DOWN[1] != NULL)func;
        if(DOWN[2] != NULL)func;

        换用一整个函数方案
        */
        memset(BUF,0,1024);
        list_to_buffer((MENU**)UP, (MENU**)DOWN, BUF);//准备好了buf
        //准备发送到硬件


    for(int y=0;y<8;y++)
	{   
        uint8_t row[128]={0};
        uint8_t* p= (BUF)+y*128;
		oled_setpos(spi,0,y);
         for(int j=0;j<128;j++){
             uint8_t * byte=p+j/8;
             uint8_t bit=j%8;
             for(int n=0;n<8;n++){
                 row[j] |= (((1 << (7-bit)) & *(byte+OLED_WIDTH/8*n)) >> (7-bit) )<< n;
             }
         }
	    oled_data(spi,row,128);	    	
	    
	}





        //等待按键，这边用消息队列
        uint32_t ret= xQueueReceive(button_qhandle,&button,portMAX_DELAY);
        printf("received:%d\r\n",button);
        if(ret){//有按键事件发生,对button分类处理
            //结果是菜单的，把cur_menu指针指向即可。如果是个末端的任务，就调用并阻塞，显示由具体函数实现
            switch(button){
                case MSG_ENTER:
                    //分为是否是末端项
                    if(cur_menu->type == MENU_TYPE_LIST){
                        for(int i=0;;i++){
                            if(EMenuListTest+i == cur_menu){
                                /*
                                改为记录转出的条目，这样好一些，0级标签永远在头上。
                                如果是任务项目，进入也不需要记录了，cur指针会记录的
                                */
                    
                                menu_path[cur_menu->l]=cur_menu;
                                cur_menu=EMenuListTest+i+1;
                                break;
                            }
                        }
                    }else{
                        //如果是末端函数项
                        //方案：阻塞菜单显示任务，也就是本任务，恢复相应的任务。
                        //vTaskResume(cur_menu->task);
                        //这里清空button queue吗？
                        /*
                        因为具体任务优先级低，所以会在下一句挂起menu的时候，开始运行
                        应该清空but queue，具体应用还是用这个
                        */
                    //    if(pdTRUE != xQueueReset(button_qhandle)){
                    //        //可能会因为有进程被这个队列挂起而无法清空
                    //        ESP_LOGE(TAG,"error empty the button queue~");
                    //    }

                    //     vTaskSuspend(NULL);//挂起menu任务


                    //决定还是不用这种任务绑定的模式，改成函数来处理
                    cur_menu->func();
                    //printf("??????\r\n");
                    }
                    break;
                case MSG_UP:
                    //向上
                    if(UP[1]!=NULL){
                        //如果上一个有同级菜单项
                        cur_menu=UP[1];
                    }else{
                        //如果没有
                        ESP_LOGI(TAG,"there's no upper item.");
                    }
                    break;
                case MSG_DOWN:
                    if(DOWN[1]!=NULL){
                        //如果上一个有同级菜单项
                        cur_menu=DOWN[1];
                    }else{
                        //如果没有
                        ESP_LOGI(TAG,"there's no item below.");
                    }
                    break;
                case MSG_QUIT:
                    if(cur_menu->l == MENU_L_0 +1){
                        //无法返回上一级
                        ESP_LOGI(TAG,"there's no way exit.");
                    }else{
                        //根据menu_path找到上一级
                        if((cur_menu->l == menu_path[cur_menu->l-1]->l+1)){
                            cur_menu=menu_path[cur_menu->l-1];
                            menu_path[cur_menu->l]=NULL;
                            
                        }else{
                            ESP_LOGE(TAG,"error finding the corresbonding item.");
                        }


                    }
                    break;
                
                default:
                    ESP_LOGW(TAG,"button queue received an unknown msg.");
            }

        }else{//意外发生

        ESP_LOGE(TAG,"task menu resumed with a wrong queue message. ");

        }


    }
}

void list_to_buffer(MENU **UP, MENU **DOWN, uint8_t *buffer)
{
    /*
    把上下数组中的数据读出判断，如果有，就在对应位置生成文字
    默认四行，当前处于第二行
    */
   


    if (UP[1] != NULL)
    {
        //在第一行填充文字 
        if(plot_string(UP[1]->title,1+LEFT_MARGIN,1,buffer))ESP_LOGW(TAG,"Ops,string is out of bound. ");
    }

    for (int i = 0; i < 3; i++)
    {
        if ((DOWN[i]) != NULL)
        {
            //在第i+2行填充文字
            if(plot_string(DOWN[i]->title,1+LEFT_MARGIN,1+16*(i+1),buffer))ESP_LOGW(TAG,"Ops,string is out of bound. ");
        }
    }

    //加入选中指示符
    plot_char(&triangle_t,1,16,buffer);


}




uint8_t plot_char(char_struct *char_t, int x, int y, uint8_t *buf)
{
    //用于在buf的指定坐标（x,y）绘制字符char_t,会根据屏幕尺寸宏来判断边界

    if ((x + char_t->width > OLED_WIDTH+1) || (y + char_t->height > OLED_HEIGHT+1))
        //(x,y)将会被图案覆盖，所以这里要加一
        return 1;


    x -= 1;
    y -= 1;
    uint8_t *p = buf + (y * OLED_WIDTH + x) / 8; //找到字符左上角属于的byte
    uint8_t bit = x % 8;
    uint8_t pre_bit = (8 - bit) % 8;
    uint8_t whole_byte = (char_t->width - pre_bit) / 8;
    uint8_t post_bit = char_t->width - pre_bit - 8 * whole_byte;
    //以上都是个数   pre_bit | whole_byte*8 | post_bit

    if (pre_bit == 0 && (((char_t->width) % 8 )== 0))
    {
        //为对齐情况优化
        for (int i = 0; i < char_t->height; i++)
        {

            memcpy(p + (i * OLED_WIDTH/8), char_t->char_buf + i * (char_t->width)/8, char_t->width / 8);
        }
        return 0;
    }
    else
    {

        for (int i = 0; i < char_t->height; i++)
        {
            //先找出来要复制的两个开头指针
            uint8_t *dst = p + (i * OLED_WIDTH/8);
            uint8_t *src = char_t->char_buf + i * (char_t->width/8);

            //分三部分复制一行
            //pre:将dst所在字节的后pre个位
            (*dst) &= (0xff << pre_bit);         //清零
            (*dst) |= ((*src) >> (8 - pre_bit)); //这里要注意

            //whole_byte是针对dst来说的，对于src是破碎的
            for (int g = 0; g < whole_byte; g++)
            {
                //每一个byte：(8-pre)|pre
                //这里考虑pre>0
                *(dst + g + 1) = 0;
                *(dst + g + 1) = (*(src + g) << pre_bit) | (*(src + g + 1) >> (8 - pre_bit));
            }

            (*(dst + whole_byte + 1)) &= 0xff >> post_bit;
            (*(dst + whole_byte + 1)) |= (*(src + (char_t->width / 8 - 1)) << (8 - post_bit));
        }

        return 0;
    }


}





uint8_t plot_string(char_struct ** string_t ,int x,int y,uint8_t* buf){
    //由于plot_char会检查边界，这里就只检查返回值了。
    for(int i=0;string_t[i];i++){//string_t内容结束后是空指针，做判断条件
        if(plot_char(string_t[i],x,y,buf) != 0)return 1;
        x+=string_t[i]->width-string_t[i]->indent;//消除字节对齐的空白
    }
    return 0;

}