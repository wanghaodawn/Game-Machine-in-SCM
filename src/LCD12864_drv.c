/*=======================================================
 文件名: LCD12864_drv.c
 描述: 液晶12864模块驱动c文件
 作者:
=========================================================
-----------------------------------------------------------------------------------------------------------*/
#include "LCD12864_drv.h"

#ifndef CHECK_BUSY
#define CHECK_BUSY
#endif

/**********************************************
功能：
    延时函数
输入参数：
    n: 延时时长
输出参数：
    无
返回值：
**********************************************/
static void Delay(uint16 n)
{
    uint16 i;
    while(n--)
    {
        i=200;
        while(i--);
    }
}

/**********************************************
功能：
    忙检测
输入参数：
    无
输出参数：
    无
返回值：
    1: 忙
    0: 空闲
**********************************************/
static bit BusyCheck(void)
{
    #ifdef CHECK_BUSY
        bit BusyBit;
        
        LCD12864_DATA = 0xFF;
        LCD12864_RS = 0;
        LCD12864_RW = 1;
        LCD12864_EN = 1;
        _nop_();
        BusyBit = LCD12864_DATA&0x80;
        LCD12864_EN = 0;
        return BusyBit;
    #else
        return 0;
    #endif
}

/**********************************************
功能：
    写命令到LCD12864
输入参数：
    CmdByte: 命令字节
输出参数：
    无
返回值：
**********************************************/
void CommandWrite12864(uint8 CmdByte)
{
    while(BusyCheck()); //等待忙
    LCD12864_RS = 0;
    LCD12864_RW = 0;
    LCD12864_EN = 1;
    LCD12864_DATA = CmdByte;
    _nop_();
    LCD12864_EN = 0;
}

/**********************************************
功能：
    写数据到LCD12864
输入参数：
    DataByte: 数据字节
输出参数：
    无
返回值：
**********************************************/
void DataWrite12864(uint8 DataByte)
{
    while(BusyCheck()); //等待忙
    LCD12864_RS = 1;
    LCD12864_RW = 0;
    LCD12864_EN = 1;
    LCD12864_DATA = DataByte;
    _nop_();
    LCD12864_EN = 0;
}

/**********************************************
功能：
    从LCD12864读取1字节
输入参数：
    无
输出参数：
    无
返回值：
    RdByte: 从LCD12864中读取的字节值
**********************************************/
uint8 DataRead12864(void)
{
    uint8 RdByte;
    
    while(BusyCheck()); //等待忙
    LCD12864_RS = 1;
    LCD12864_RW = 1;
    LCD12864_DATA = 0xff;
    LCD12864_EN = 1;
    _nop_();
    RdByte = LCD12864_DATA;
    LCD12864_EN = 0;
    return RdByte;
}

/**********************************************
功能：
    初始化LCD12864
输入参数：
    无
输出参数：
    无
返回值：
**********************************************/
void Reset12864(void)
{  
	LCD12864_PSB = 1;
	CommandWrite12864(0x30);       //选择基本指令集
	CommandWrite12864(0x0c);       //开显示(无游标、不反白)
	CommandWrite12864(0x01);       //清除显示，并且设定地址指针为00H
	CommandWrite12864(0x06);       //指定在资料的读取及写入时，设定游标的移动方向及指定显示的移位
}

/**********************************************
功能：
    清屏
输入参数：
    无
输出参数：
    无
返回值：
**********************************************/
void ScreenClear12864(void)
{
    CommandWrite12864(CLEAR_SCREEN);
    Delay(2);
}

/**********************************************
功能：
    在指定坐标显示字符串
输入参数：
    x: 横坐标(0——15)
    y: 纵坐标(0——1)
    String: 字符串
输出参数：
    无
返回值：
**********************************************/
void StringDisplay12864(uint8 x, uint8 y, uint8 *String)
{
    if((x<8)&&(y<4))
    {
        //指定数据地址
        switch(y)
        {
            case 0:
                CommandWrite12864(DDRAM_ADR_SET|0x00|x);
                break;
            case 1:
                CommandWrite12864(DDRAM_ADR_SET|0x10|x);
                break;
            case 2:
                CommandWrite12864(DDRAM_ADR_SET|0x08|x);
                break;
            case 3:
                CommandWrite12864(DDRAM_ADR_SET|0x18|x);
                break;
        }
        //显示字符串
        while(*String != '\0')
        {
            DataWrite12864(*String);
            String++;
        }
    }
}

/**********************************************
功能：
    显示一整个位图
输入参数：
    Picture: 指向位图的指针
输出参数：
    无
返回值：
**********************************************/
void PictureDisplay12864(unsigned char code *Picture)
{
    unsigned char i,j,k;
    
    /* 选择扩展指令集，绘图显示开 */
	CommandWrite12864(0x36);
    /* 将位图的所有字节写到液晶的位图RAM中 */
    for(i=0; i<2; i++)                      //GDRAM分上下两页
    {
        for(j=0; j<32; j++)
        {
            /* 写纵坐标y和横坐标x */
            CommandWrite12864(0x80+j);      //写GDRAM的坐标y，即第y行
            if(i==0)
            {
                CommandWrite12864(0x80);    //写GDRAM的坐标x（上页）
            }
            else
            {
                CommandWrite12864(0x88);    //写GDRAM的坐标x（下页）
            }
            /* 将16个字节写入GDRAM */
            for(k=0; k<16; k++)
            {
                DataWrite12864(*Picture++);
            }
        }
    }
    /* 选择基本指令集，绘图显示关 */
    CommandWrite12864(0x30);
}

/***********************************************************************************************
为加速逻辑运算而设置的掩码表，这是以牺牲空间而换取时间的办法
************************************************************************************************/
code unsigned int LcdMaskTab[]={0x0001,0x0002,0x0004,0x0008,0x0010,0x0020,0x0040,0x0080,
							    0x0100,0x0200,0x0400,0x0800,0x1000,0x2000,0x4000,0x8000};

/**********************************************
功能：
    在指定坐标显示字符串
输入参数：
    x: 横坐标(0——15)
    y: 纵坐标(0——1)
    String: 字符串
输出参数：
    无
返回值：
**********************************************/
void Lcd_WriteStr(uint8 x, uint8 y, uint8 *String)
{
    if((x<8)&&(y<4))
    {
        //指定数据地址
        switch(y)
        {
            case 0:
                CommandWrite12864(DDRAM_ADR_SET|0x00|x);
                break;
            case 1:
                CommandWrite12864(DDRAM_ADR_SET|0x10|x);
                break;
            case 2:
                CommandWrite12864(DDRAM_ADR_SET|0x08|x);
                break;
            case 3:
                CommandWrite12864(DDRAM_ADR_SET|0x18|x);
                break;
        }
        //显示字符串
        while(*String != '\0')
        {
            DataWrite12864(*String);
            String++;
        }
    }
}

/**********************************************
功能：
    在指定坐标写入一个象素,颜色有两种,0(白,即无显示);1(黑,即有显示)
输入参数：
    x: 横坐标(0——127)
    y: 纵坐标(0——63)
    Color: 像素颜色
输出参数：
    无
返回值：
**********************************************/
void Lcd_PutPixel(unsigned char x,unsigned char y,unsigned char Color)
{
	unsigned char z,w;
	unsigned int Temp;
	if(x>=128||y>=64)
		return;
	Color=Color%2;
	w=15-x%16;//确定对这个字的第多少位进行操作
	x=x/16;//确定为一行上的第几字

	if(y<32) //如果为上页
		z=0x80;
	else     //否则如果为下页
		z=0x88;

	y=y%32;
	EA=0;
	CommandWrite12864(0x36);
	CommandWrite12864(y+0x80);        //行地址
	CommandWrite12864(x+z);     //列地址 
	Temp=DataRead12864();//先空读一次
	Temp=(unsigned int)DataRead12864()<<8;//再读出高8位
	Temp|=(unsigned int)DataRead12864();//再读出低8位
	EA=1;
	if(Color==1) //如果写入颜色为1
		Temp|=LcdMaskTab[w];//在此处查表实现加速
	else         //如果写入颜色为0
		Temp&=~LcdMaskTab[w];//在此处查表实现加速
	EA=0;
	CommandWrite12864(y+0x80);        //行地址
	CommandWrite12864(x+z);     //列地址
    DataWrite12864(Temp>>8);//先写入高8位，再写入低8位
    DataWrite12864(Temp&0x00ff);
	CommandWrite12864(0x30);
	EA=1;
}

/**********************************************
功能：
    从LCD指定坐标读取象素颜色值
输入参数：
    x: 横坐标(0——127)
    y: 纵坐标(0——63)
输出参数：
    无
返回值：
    像素值
**********************************************/
unsigned char Lcd_ReadPixel(unsigned char x,unsigned char y)
{
	unsigned char z,w;
	unsigned int Temp;
	if(x>=128||y>=64)
		return 0;
	w=15-x%16;//确定对这个字的第多少位进行操作
	x=x/16;//确定为一行上的第几字

	if(y<32) //如果为上页
		z=0x80;
	else     //否则如果为下页
		z=0x88;

	y=y%32;
	EA=0;
	CommandWrite12864(0x36);
	CommandWrite12864(y+0x80);        //行地址
	CommandWrite12864(x+z);     //列地址 
	Temp=DataRead12864();//先空读一次
	Temp=(unsigned int)DataRead12864()<<8;//再读出高8位
	Temp|=(unsigned int)DataRead12864();//再读出低8位
	EA=1;
	if((Temp&&LcdMaskTab[w])==0)
		return 0;
	else
		return 1;
}

/**********************************************
功能：
    向指定位置画一条长度为Length的指定颜色的水平线
输入参数：
    x: 横坐标(0——127)
    y: 纵坐标(0——63)
    Length: 长度
    Color: 颜色
输出参数：
    无
返回值：
**********************************************/
void Lcd_HoriLine(unsigned char x,unsigned char y,unsigned char Length,unsigned char Color)
{
	unsigned char i;
	if(Length==0)
		return;
	for(i=0;i<Length;i++)
	{
		Lcd_PutPixel(x+i,y,Color);
	}
}

/**********************************************
功能：
    向指定位置画一条长度为Length的指定颜色的垂直线
输入参数：
    x: 横坐标(0——127)
    y: 纵坐标(0——63)
    Length: 长度
    Color: 颜色
输出参数：
    无
返回值：
**********************************************/
void Lcd_VertLine(unsigned char x,unsigned char y,unsigned char Length,unsigned char Color)
{
	unsigned char i;
	if(Length==0)
		return;
	for(i=0;i<Length;i++)
	{
		Lcd_PutPixel(x,y+i,Color);
	}
}

/**********************************************
功能：
    指定两对角坐标画一个指定颜色的矩形
输入参数：
    x0: 横坐标(0——127)
    y0: 纵坐标(0——63)
    x1: 横坐标(0——127)
    y1: 纵坐标(0——63)
    Color: 颜色
输出参数：
    无
返回值：
**********************************************/
void Lcd_Rectangle(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char Color)
{
	unsigned char Temp;
	if(x0>x1)
	{
		Temp=x0;
		x0=x1;
		x1=Temp;
	}	
	if(y0>y1)
	{
		Temp=y0;
		y0=y1;
		y1=Temp;
	}
	Lcd_VertLine(x0,y0,y1-y0+1,Color);
	Lcd_VertLine(x1,y0,y1-y0+1,Color);
	Lcd_HoriLine(x0,y0,x1-x0+1,Color);
	Lcd_HoriLine(x0,y1,x1-x0+1,Color);
}

/**********************************************
功能：
    绘图模式清屏，即对GDRAM进行清除
输入参数：
    Mode: 清屏模式，0为无任何显示，1为全填充显示
输出参数：
    无
返回值：
**********************************************/
void Lcd_Clear(unsigned char Mode)
{
	unsigned char x,y,ii;
	unsigned char Temp;
	if(Mode%2==0)
		Temp=0x00;
	else
		Temp=0xff;
	CommandWrite12864(0x36);//扩充指令 绘图显示
	for(ii=0;ii<9;ii+=8)
		for(y=0;y<0x20;y++)
			for(x=0;x<8;x++)
			{ 	
				EA=0;
				CommandWrite12864(y+0x80);        //行地址
				CommandWrite12864(x+0x80+ii);     //列地址     
				DataWrite12864(Temp); //写数据 D15－D8 
				DataWrite12864(Temp); //写数据 D7－D0 
				EA=1;
			}
	CommandWrite12864(0x30);
}

