/*=======================================================
 �ļ���: LCD12864_drv.c
 ����: Һ��12864ģ������c�ļ�
 ����:
=========================================================
-----------------------------------------------------------------------------------------------------------*/
#include "LCD12864_drv.h"

#ifndef CHECK_BUSY
#define CHECK_BUSY
#endif

/**********************************************
���ܣ�
    ��ʱ����
���������
    n: ��ʱʱ��
���������
    ��
����ֵ��
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
���ܣ�
    æ���
���������
    ��
���������
    ��
����ֵ��
    1: æ
    0: ����
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
���ܣ�
    д���LCD12864
���������
    CmdByte: �����ֽ�
���������
    ��
����ֵ��
**********************************************/
void CommandWrite12864(uint8 CmdByte)
{
    while(BusyCheck()); //�ȴ�æ
    LCD12864_RS = 0;
    LCD12864_RW = 0;
    LCD12864_EN = 1;
    LCD12864_DATA = CmdByte;
    _nop_();
    LCD12864_EN = 0;
}

/**********************************************
���ܣ�
    д���ݵ�LCD12864
���������
    DataByte: �����ֽ�
���������
    ��
����ֵ��
**********************************************/
void DataWrite12864(uint8 DataByte)
{
    while(BusyCheck()); //�ȴ�æ
    LCD12864_RS = 1;
    LCD12864_RW = 0;
    LCD12864_EN = 1;
    LCD12864_DATA = DataByte;
    _nop_();
    LCD12864_EN = 0;
}

/**********************************************
���ܣ�
    ��LCD12864��ȡ1�ֽ�
���������
    ��
���������
    ��
����ֵ��
    RdByte: ��LCD12864�ж�ȡ���ֽ�ֵ
**********************************************/
uint8 DataRead12864(void)
{
    uint8 RdByte;
    
    while(BusyCheck()); //�ȴ�æ
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
���ܣ�
    ��ʼ��LCD12864
���������
    ��
���������
    ��
����ֵ��
**********************************************/
void Reset12864(void)
{  
	LCD12864_PSB = 1;
	CommandWrite12864(0x30);       //ѡ�����ָ�
	CommandWrite12864(0x0c);       //����ʾ(���αꡢ������)
	CommandWrite12864(0x01);       //�����ʾ�������趨��ַָ��Ϊ00H
	CommandWrite12864(0x06);       //ָ�������ϵĶ�ȡ��д��ʱ���趨�α���ƶ�����ָ����ʾ����λ
}

/**********************************************
���ܣ�
    ����
���������
    ��
���������
    ��
����ֵ��
**********************************************/
void ScreenClear12864(void)
{
    CommandWrite12864(CLEAR_SCREEN);
    Delay(2);
}

/**********************************************
���ܣ�
    ��ָ��������ʾ�ַ���
���������
    x: ������(0����15)
    y: ������(0����1)
    String: �ַ���
���������
    ��
����ֵ��
**********************************************/
void StringDisplay12864(uint8 x, uint8 y, uint8 *String)
{
    if((x<8)&&(y<4))
    {
        //ָ�����ݵ�ַ
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
        //��ʾ�ַ���
        while(*String != '\0')
        {
            DataWrite12864(*String);
            String++;
        }
    }
}

/**********************************************
���ܣ�
    ��ʾһ����λͼ
���������
    Picture: ָ��λͼ��ָ��
���������
    ��
����ֵ��
**********************************************/
void PictureDisplay12864(unsigned char code *Picture)
{
    unsigned char i,j,k;
    
    /* ѡ����չָ�����ͼ��ʾ�� */
	CommandWrite12864(0x36);
    /* ��λͼ�������ֽ�д��Һ����λͼRAM�� */
    for(i=0; i<2; i++)                      //GDRAM��������ҳ
    {
        for(j=0; j<32; j++)
        {
            /* д������y�ͺ�����x */
            CommandWrite12864(0x80+j);      //дGDRAM������y������y��
            if(i==0)
            {
                CommandWrite12864(0x80);    //дGDRAM������x����ҳ��
            }
            else
            {
                CommandWrite12864(0x88);    //дGDRAM������x����ҳ��
            }
            /* ��16���ֽ�д��GDRAM */
            for(k=0; k<16; k++)
            {
                DataWrite12864(*Picture++);
            }
        }
    }
    /* ѡ�����ָ�����ͼ��ʾ�� */
    CommandWrite12864(0x30);
}

/***********************************************************************************************
Ϊ�����߼���������õ�����������������ռ����ȡʱ��İ취
************************************************************************************************/
code unsigned int LcdMaskTab[]={0x0001,0x0002,0x0004,0x0008,0x0010,0x0020,0x0040,0x0080,
							    0x0100,0x0200,0x0400,0x0800,0x1000,0x2000,0x4000,0x8000};

/**********************************************
���ܣ�
    ��ָ��������ʾ�ַ���
���������
    x: ������(0����15)
    y: ������(0����1)
    String: �ַ���
���������
    ��
����ֵ��
**********************************************/
void Lcd_WriteStr(uint8 x, uint8 y, uint8 *String)
{
    if((x<8)&&(y<4))
    {
        //ָ�����ݵ�ַ
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
        //��ʾ�ַ���
        while(*String != '\0')
        {
            DataWrite12864(*String);
            String++;
        }
    }
}

/**********************************************
���ܣ�
    ��ָ������д��һ������,��ɫ������,0(��,������ʾ);1(��,������ʾ)
���������
    x: ������(0����127)
    y: ������(0����63)
    Color: ������ɫ
���������
    ��
����ֵ��
**********************************************/
void Lcd_PutPixel(unsigned char x,unsigned char y,unsigned char Color)
{
	unsigned char z,w;
	unsigned int Temp;
	if(x>=128||y>=64)
		return;
	Color=Color%2;
	w=15-x%16;//ȷ��������ֵĵڶ���λ���в���
	x=x/16;//ȷ��Ϊһ���ϵĵڼ���

	if(y<32) //���Ϊ��ҳ
		z=0x80;
	else     //�������Ϊ��ҳ
		z=0x88;

	y=y%32;
	EA=0;
	CommandWrite12864(0x36);
	CommandWrite12864(y+0x80);        //�е�ַ
	CommandWrite12864(x+z);     //�е�ַ 
	Temp=DataRead12864();//�ȿն�һ��
	Temp=(unsigned int)DataRead12864()<<8;//�ٶ�����8λ
	Temp|=(unsigned int)DataRead12864();//�ٶ�����8λ
	EA=1;
	if(Color==1) //���д����ɫΪ1
		Temp|=LcdMaskTab[w];//�ڴ˴����ʵ�ּ���
	else         //���д����ɫΪ0
		Temp&=~LcdMaskTab[w];//�ڴ˴����ʵ�ּ���
	EA=0;
	CommandWrite12864(y+0x80);        //�е�ַ
	CommandWrite12864(x+z);     //�е�ַ
    DataWrite12864(Temp>>8);//��д���8λ����д���8λ
    DataWrite12864(Temp&0x00ff);
	CommandWrite12864(0x30);
	EA=1;
}

/**********************************************
���ܣ�
    ��LCDָ�������ȡ������ɫֵ
���������
    x: ������(0����127)
    y: ������(0����63)
���������
    ��
����ֵ��
    ����ֵ
**********************************************/
unsigned char Lcd_ReadPixel(unsigned char x,unsigned char y)
{
	unsigned char z,w;
	unsigned int Temp;
	if(x>=128||y>=64)
		return 0;
	w=15-x%16;//ȷ��������ֵĵڶ���λ���в���
	x=x/16;//ȷ��Ϊһ���ϵĵڼ���

	if(y<32) //���Ϊ��ҳ
		z=0x80;
	else     //�������Ϊ��ҳ
		z=0x88;

	y=y%32;
	EA=0;
	CommandWrite12864(0x36);
	CommandWrite12864(y+0x80);        //�е�ַ
	CommandWrite12864(x+z);     //�е�ַ 
	Temp=DataRead12864();//�ȿն�һ��
	Temp=(unsigned int)DataRead12864()<<8;//�ٶ�����8λ
	Temp|=(unsigned int)DataRead12864();//�ٶ�����8λ
	EA=1;
	if((Temp&&LcdMaskTab[w])==0)
		return 0;
	else
		return 1;
}

/**********************************************
���ܣ�
    ��ָ��λ�û�һ������ΪLength��ָ����ɫ��ˮƽ��
���������
    x: ������(0����127)
    y: ������(0����63)
    Length: ����
    Color: ��ɫ
���������
    ��
����ֵ��
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
���ܣ�
    ��ָ��λ�û�һ������ΪLength��ָ����ɫ�Ĵ�ֱ��
���������
    x: ������(0����127)
    y: ������(0����63)
    Length: ����
    Color: ��ɫ
���������
    ��
����ֵ��
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
���ܣ�
    ָ�����Խ����껭һ��ָ����ɫ�ľ���
���������
    x0: ������(0����127)
    y0: ������(0����63)
    x1: ������(0����127)
    y1: ������(0����63)
    Color: ��ɫ
���������
    ��
����ֵ��
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
���ܣ�
    ��ͼģʽ����������GDRAM�������
���������
    Mode: ����ģʽ��0Ϊ���κ���ʾ��1Ϊȫ�����ʾ
���������
    ��
����ֵ��
**********************************************/
void Lcd_Clear(unsigned char Mode)
{
	unsigned char x,y,ii;
	unsigned char Temp;
	if(Mode%2==0)
		Temp=0x00;
	else
		Temp=0xff;
	CommandWrite12864(0x36);//����ָ�� ��ͼ��ʾ
	for(ii=0;ii<9;ii+=8)
		for(y=0;y<0x20;y++)
			for(x=0;x<8;x++)
			{ 	
				EA=0;
				CommandWrite12864(y+0x80);        //�е�ַ
				CommandWrite12864(x+0x80+ii);     //�е�ַ     
				DataWrite12864(Temp); //д���� D15��D8 
				DataWrite12864(Temp); //д���� D7��D0 
				EA=1;
			}
	CommandWrite12864(0x30);
}

