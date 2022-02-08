#ifndef _MENU_H_
#define _MENU_H_

/*
把每一级菜单统一化，双向链接，方便返回和进入


*/
#define MENU_LANG_BUF_SIZE 16


//extern spi_device_handle_t spi;

typedef enum{
	MENU_TYPE_NULL = 0,//结尾类型

	MENU_TYPE_FUN,//最下层

	MENU_TYPE_LIST,//中间层

}MenuType;

typedef enum{
	MENU_L_0 = 0,
	MENU_L_1,
	MENU_L_2,
	MENU_L_3,
	MENU_L_MAX
}MenuLel;



typedef struct _char_struct
{
	uint8_t width;
	uint8_t height;
	uint8_t indent;
	uint8_t* char_buf;


}char_struct;



typedef struct _strMenu
{
    MenuLel l;     //菜单等级
    char cha[MENU_LANG_BUF_SIZE];  //菜单名称
	char_struct * title[MENU_LANG_BUF_SIZE+1];
    MenuType type;  ///菜单类型
    uint8_t (*func)(void);  //最下层菜单的内容函数
	//TaskHandle_t task;

} MENU;






void menu_display(MENU * menu_list);


//将找到的上下菜单项填充到显示buffer，当前填到第二个。
void list_to_buffer(MENU **UP, MENU **DOWN,uint8_t * buffer);


uint8_t plot_char(char_struct* char_t,int x,int y,uint8_t* buf);

uint8_t plot_string(char_struct ** string_t ,int x,int y,uint8_t* buf);

void task_display();



#endif