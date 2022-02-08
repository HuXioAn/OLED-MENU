# OLED-MENU

一个简单的单片机单色OLED菜单与显示组件

----

## 特点：

1. 单色OLED，使用**1bpp**的buffer，不浪费运行时RAM空间
2. 任意位置放置贴图、文字，**不需要8位字节对齐**
3. 理论支持无限层级、无限数量的菜单项，且运算复杂度不变
4. 菜单项管理统一，增减便利
5. 支持文字宽度缩进排版，利于中英文混合
6. 支持多种屏幕尺寸，自动处理边界超出

## 参考项目：

项目中**菜单项存储结构**借鉴于文章，非常感谢：

https://mp.weixin.qq.com/s/8X0cbD4b9KWOKdzlmWxV2g

对应仓库：https://github.com/wujique/stm32f407/tree/sw_arch

保留了原作者菜单数组原名，删改了一些菜单结构体成员，感谢这位老师的文章与项目。

其余代码实现没有参考任何项目。

## 文件结构

[./src/menu.c](./src/menu.c) 菜单注册结构体，菜单显示任务函数等实现

[./src/button.c](./src/menu.c) 物理按键注册初始化，按键中断回调函数，通过queue解除阻塞的现实任务

[./src/chara.c](./src/chara.c) 字模结构体定义

[./src/SH1106.c](./src/SH1106.c) SH1106芯片的OLED驱动





