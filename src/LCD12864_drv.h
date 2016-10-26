/*=======================================================
 文件名: LCD12864_drv.h
 描述: 液晶12864模块驱动头文件
 作者:
=========================================================
-----------------------------------------------------------------------------------------------------------*/
#ifndef __LCD12864_DRV_H__
#define __LCD12864_DRV_H__

#include <intrins.h>
#include "type.h"
#include "IO_def.h"

/* 定义LCD12864接口对应的端口和引脚 */

#define LCD12864_DATA   P0      //并行数据口

         //开发板
/*
#define LCD12864_RS     P27     //数据/命令选择
#define LCD12864_RW     P26     //读/写选择
#define LCD12864_EN     P25     //使能信号
#define LCD12864_PSB    P24     //并/串接口选择, H并,L串
#define LCD12864_RST    P22     //复位, L有效
*/


/*  自己的板子	  */

#define LCD12864_RS     P25     //数据/命令选择
#define LCD12864_RW     P26     //读/写选择
#define LCD12864_EN     P27     //使能信号
#define LCD12864_PSB    P24     //并/串接口选择, H并,L串
#define LCD12864_RST    P22     //复位, L有效



/* 定义控制指令集 */

#define CLEAR_SCREEN    0x01
#define CURSOR_BACK     0x02
#define MODE_SET        0x04
#define SWITCH_CTL      0x08
#define SHIFT           0x10
#define FUNCTION_SET    0x20
#define RAM_ADR_SET	    0x40
#define DDRAM_ADR_SET   0x80

/* 接口函数定义 */
void CommandWrite12864(uint8 CmdByte);
void DataWrite12864(uint8 DataByte);
uint8 DataRead12864(void);
void Reset12864(void);
void ScreenClear12864(void);
void StringDisplay12864(uint8 x, uint8 y, uint8 *String);
void PictureDisplay12864(unsigned char code *Picture);

/* 扩展函数定义 */
void Lcd_WriteStr(uint8 x, uint8 y, uint8 *String);
void Lcd_PutPixel(unsigned char x,unsigned char y,unsigned char Color);
unsigned char Lcd_ReadPixel(unsigned char x,unsigned char y);
void Lcd_HoriLine(unsigned char x,unsigned char y,unsigned char Length,unsigned char Color);
void Lcd_VertLine(unsigned char x,unsigned char y,unsigned char Length,unsigned char Color);
void Lcd_Rectangle(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char Color);
void Lcd_Clear(unsigned char Mode);

#endif