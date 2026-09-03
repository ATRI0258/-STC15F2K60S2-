#include "stc15f2k60s2.h"
#include "string.h"

#define uint8_t unsigned char 
#define uint16_t unsigned int 

#define Port P0 
#define Dianji_Zhengzhuan    DJ_Zuo=1,DJ_You=0;
#define Dianji_Fanzhuan      DJ_Zuo=0,DJ_You=1;
#define Dianji_Ting          DJ_Zuo=0,DJ_You=0;

sbit LCD_RS = P3^5;
sbit LCD_RW = P3^6;
sbit LCD_E = P3^7;
sbit LCD_BF = P0^7;

sbit Ren=P1^1;
sbit LED=P1^4;

sbit Key_DJ_Zheng=P2^2;
sbit Key_DJ_Fan=P2^3;

sbit DJ_Zuo=P1^2;
sbit DJ_You=P1^3;

uint8_t Disp_Interface;
uint16_t Disp_Time;
uint16_t Key_Time;

uint8_t PWM,PWM_Count;
uint16_t ADC_Dat;
uint16_t Shezhi_liumingzhi=300;

uint16_t Ds1s;

bit Moshi;
bit Deng_bit;
bit Shezhi;

bit Busy_1602 ()
{
	bit busy = 0;
	Port = 0XFF;
	LCD_RS  = 0;        LCD_RW  = 1;
	LCD_E   = 1;        LCD_E   = 1;
	busy    = LCD_BF;   LCD_E   = 0;
	return busy;
}

void Write_Com (uint8_t dat)
{
	while(Busy_1602 ());
	LCD_RS = 0; LCD_RW = 0;
	Port = dat;
	LCD_E = 1; LCD_E = 0;
}

void Write_Dat (uint8_t dat)
{
	while(Busy_1602 ());
	LCD_RS = 1; LCD_RW = 0;
	Port = dat;
	LCD_E = 1; LCD_E = 0;
}

void Init_1602()
{
	Write_Com(0x38);//8位,两行,5x7点阵
	Write_Com(0x0c);//显示开,光标关
	Write_Com(0x06);//默认,光标右移
	Write_Com(0x01);//清显示
}

void Write_1602_String(uint8_t row,col,uint8_t *dat)
{
	uint8_t i = 0;
	row %= 2; col %= 40;
	Write_Com(0x80+row * 0x40+col);
	for(;col<40&&dat[i]!=0;i++,col++)Write_Dat(dat[i]);
}
void Init_Disp (uint8_t interface)
{	
	uint8_t idata buff[30];
	
	if(interface != Disp_Interface)
	{	
		Write_Com(0x01);		
	}

    if(Ds1s < 5000)
    {
        sprintf(buff, "Lumen:%d  ",(int)600-ADC_Dat);		
        Write_1602_String (1, 0, buff);	
        
    }else 
    {
        Write_1602_String (1, 0, "         ");
        if(Ds1s == 10000)Ds1s=0;
    }
	
    if(Moshi == 0)//手动模式
    {        
        Write_1602_String (0, 0, "Manual   ");	
        
    }else //自动模式
    {        
        Write_1602_String (0, 0, "Automatic");	
    }       
    memset (buff, 0, sizeof(buff));
}

void Update_Display ()
{
	static uint8_t interface = 255;
	
	switch(Disp_Interface)
	{
		case 0 : Init_Disp (interface); break;
		default : break;
	}
	
	interface = Disp_Interface;	
}

void Timer0_Init(void)	
{
	AUXR &= 0x7F;			//定时器时钟12T模式
	TMOD &= 0xF0;			//设置定时器模式
	TL0 = 0xA4;				//设置定时初始值
	TH0 = 0xFF;				//设置定时初始值
	TF0 = 0;				//清除TF0标志
	TR0 = 1;				//定时器0开始计时
	
	EA=ET0=1;
}
void Init_ADC()
{
	P1ASF |= 0x01;     //选择输入IO口
	ADC_CONTR |= 0XE0; // 1110 0000 打开ADC电源，选择AD转换速度 结束标志位为0
	
	EADC=EA=1;//开启ADC中断
}

void Read_Key ()
{
	static uint8_t key2_x;
	uint8_t dat;
	
    dat=(P2&0x7f)^0x7f; 
	
	switch(key2_x)
	{ 
		case 0X00 : if(dat != 0X00) key2_x = 1;  break;
		
		case 0X01 : if(dat == 0X00) key2_x = 0;
								else
                {
                  key2_x = 2;
                  switch(dat) 
                  {
                    case 0x01 : Moshi=~Moshi; break;
                    case 0x02 : if(Moshi == 0){ Deng_bit=~Deng_bit; } break;                      
                    case 0x10 : Shezhi=~Shezhi;Ds1s=0; break;
                    case 0x20 : if(Shezhi == 1 && ( Shezhi_liumingzhi+50 < 600 ))   Shezhi_liumingzhi+=50; break;
                    case 0x40 : if(Shezhi == 1 && ( Shezhi_liumingzhi - 50 > 0 ))   Shezhi_liumingzhi-=50; break;                       
                    default : break;
                  }
                } break;		
		case 0X02 :  if(dat == 0X00){ key2_x = 0; } break;
	}
}

void Control()
{
    if(Moshi == 0)//手动模式
    {
        if(Deng_bit == 1)PWM=Shezhi_liumingzhi/6;
        else PWM=0;
        
    }else //自动模式
    {
        if(Ren == 1)PWM=Shezhi_liumingzhi/6;
        else PWM=0;               
    } 
    if(Key_DJ_Zheng == 0 && Key_DJ_Fan == 1)    Dianji_Zhengzhuan
    if(Key_DJ_Fan == 0 && Key_DJ_Zheng == 1)    Dianji_Fanzhuan
    if(Key_DJ_Zheng == 1 && Key_DJ_Fan == 1)    Dianji_Ting
}


void main()
{
    Timer0_Init();
    Init_ADC();
    Init_1602();
    
    Dianji_Ting
    while(1)
    {
        if(Disp_Time == 100){ Update_Display(); Disp_Time=0; }
        else if(Key_Time == 100){ Read_Key(); Key_Time=0; }
        ADC_CONTR |= 0X08;
        Control();
    }
}

void Timer_Interr() interrupt 1
{
    if(Disp_Time != 100)Disp_Time++;
    if(Key_Time != 100)Key_Time++;
    
    if(PWM > PWM_Count)LED=0;else LED=1;
    if(Ds1s != 10000 && Shezhi == 1 )Ds1s++;
    
    
    if(++PWM_Count == 100)PWM_Count=0;
}

void Interrupt_ADC() interrupt 5
{
	ADC_CONTR&=0XEF;
	ADC_Dat = ADC_RES;  //读取ADC的数据	
	
	ADC_Dat = (ADC_Dat /255.)*600  ;
}
