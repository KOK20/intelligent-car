#include  "delay.h"
#include "ps2.h"
#include "stdio.h"
#include "15W4KxxS4.h"
#include "uart.h"
#include "beep.h"
/**********引脚设置***********/
sbit IN1=P2^6;
sbit IN2=P0^2;
sbit IN3=P2^0;
sbit IN4=P2^4;
sbit OUTR=P0^0;
sbit OUTL=P0^1;
sbit led1=P0^6;
sbit led2=P0^7;
sbit ENA =P1^0;
sbit ENB =P1^1;
 /*********变量设置**********/
volatile uint8 Flag1=FALSE;
uint8 uart2temp,sig;
int g_carstate = 0;
int pwm_control=0;
int flag=0;
/************函数调用********************/
void TIME0_Init(void);//定时器设置
/************枚举按键状态****************/
 enum{
  enSTOP = 0,
  enRUN,
  enBACK,
  enLEFT,
  enRIGHT,
	enTRIANGLE,
	enCIRCLE,
	enCROSS,
	enSQUARE,
	enR2,
 }enCarState;
/**************定时器设置**************/
void TIME0_Init(void)
{
		
		
	TMOD &= 0xF0;		//设置定时器模式
	TMOD |= 0x01;		//设置定时器模式
  TL0 = (65526-1000/1.085)/256;		//设置定时初始值
	TH0 = 65526-1000/1.085;		//设置定时重载值
	TR0= 1;		    //启动T0工作
	TF0=0;
	ET0= 1;		    //允许T0中断，定时器中断开
	EA = 1;	        //开总中断
}
/********** 串口2初始化函数************/
void Uart2_Init(void)
{	
	P_SW2|=S2_S;   //选择P46 P47为串口2
		
	S2CON = 0x50;		//8位数据,可变波特率，启动串行接收器	
	AUXR |= 0x04;		//定时器2时钟为Fosc,即1T
	T2L = 0xE0;		  //设定定时初值
	T2H = 0xFE;     //设定定时初值
	AUXR |= 0x10;   //启动定时器2
	IE2 |= 0x01;    //串口2中断打开
}
/**************运动状态函数设定*********/
void run(void)
{
	IN1=0;
	IN2=1;
	IN3=0;
	IN4=1;
	led1=0;
	led2=0;
	return;
}
void back(void)
{
	IN1=1;
	IN2=0;
	IN3=1;
	IN4=0;
	led1=1;
	led2=1;
	return;
}
void left(void)
{
    IN1=1;
	  IN2=0;
	  IN3=0;
	  IN4=1;
    led1=0;
		led2=1;
	return;
}

void right(void)
{
	 IN1=0;
	 IN2=1;
	 IN3=1;
	 IN4=0;
	 led1=1;
	 led2=0;
	return;
}
void stop(void)
{
	IN1=0;
	IN2=0;
	IN3=0;
	IN4=0;
	return;
}
/**********串口2中断服务函数********************/
void Uart2() interrupt UART2_VECTOR using 1
{
	IE2 &= 0xFE; 			               // 串口2中断关闭
	Flag1=TRUE;                       //接收到数据,接收标识符有效

	led1=0;                           //串口2指示灯亮，红灯
		
	if (S2CON & S2RI)                //串行接收到停止位的中间时刻时，该位置1
  {
      S2CON &= ~S2RI;              //清除S2CON寄存器对应S2RI位（该位必须软件清零）
			uart2temp = S2BUF;       
   }
	 switch(uart2temp)
    {
			 case 0x01:   //收到“1”
       run();
			 delay_ms(300);
			 stop();
            break;
        
        case 0x02:    //收到“2”
       back();
				 delay_ms(300);
				stop();
            break;
        
        case 3:    //收到“3”
       left();
				 delay_ms(300);
				stop();
            break;
        
        case 4:    //收到“4”
       right();
				 delay_ms(300);
				stop();
            break;
        
        default:
            P2=0XFF;
            break;
    }
  if (S2CON & S2TI)                //在停止位开始发送时，该位置1
   {
      S2CON &= ~S2TI;   			     //清除S2CON寄存器对应S2TI位（该位必须软件清零）
   }
	 IE2 |= 0x01;                    // 串口2中断打开	
}
/**************定时器0中断***************/
void timer0()interrupt 1 
{
    
   TL0 = (65526-1000/1.085)/256;		//设置定时初始值
	TH0 = 65526-1000/1.085;
		if(pwm_control>10)
		{
			pwm_control=0;
		}
		if(flag==3)
		{		
		if(pwm_control<5)
		{
		 if(OUTR==0&&OUTL==0)
			{	
			 back();	
			
			}
		
     	else if(OUTL==0)
			{

				right();
		
			}
			else if(OUTR==0)
			{			
			
			left();
				
			}				
			
      if(OUTR==1&&OUTL==1)		
			
			{	
				run();
			}		
		}
		else
		{
			stop();
		}
	
	}
	
	 pwm_control++;
 }
//******************ps2按键控制***************//
 void PS2_control()
{
        //PS2按键变量定义
        stPS2KeyValue * PS2;
    	
		PS2 = ReadPS2KeyValue();	    //检测手柄按键之后获得的值
		//由data[3]和data[4]返回来的组合成16位的数组
		//如data[4]data[3]:当select按下时
		//FunBtn：0x1111 1111 1111 1110
		if((PS2->FunBtn & PSB_SELECT) == 0)
		{
		 
		}
		
		if((PS2->FunBtn & PSB_START) == 0)
		{
		  
		}
		 
		if((PS2->FunBtn & PSB_L3) == 0)
		{
			
			 g_carstate = enSTOP;      //将小车定义为停止状态
		}

		if((PS2->FunBtn & PSB_R3) == 0)
		{
			
			 g_carstate = enSTOP;      //将小车定义为停止状态
		}

		if((PS2->FunBtn & PSB_PAD_UP) == 0)
		{
			
			 g_carstate = enRUN;       //将小车定义为前进状态
		}

		else if((PS2->FunBtn & PSB_PAD_RIGHT) == 0)
		{
			 
			 g_carstate = enRIGHT;     //将小车定义为状态右转
		}
		else if((PS2->FunBtn & PSB_PAD_DOWN) == 0)
		{
			
			g_carstate = enBACK;       //将小车定义为后退状态
		}
		else if((PS2->FunBtn & PSB_PAD_LEFT) == 0)
		{
			 
			 g_carstate = enLEFT;       //将小车定义为左转状态
		}
	   else if((PS2->FunBtn & PSB_TRIANGLE) == 0) // 遥控器操作模式
		{
			  BEEP_on();
		delay_ms(10);
			g_carstate= enTRIANGLE;
			 
		} 
		 
		 else if((PS2->FunBtn & PSB_CIRCLE) == 0)	// 声控模式
		{
			BEEP_on();
		 delay_ms(10);
			  g_carstate= enCIRCLE;
		}  
	
		else if((PS2->FunBtn & PSB_CROSS) == 0) // 自动避障模式
		{
			delay_ms(10);
	     g_carstate= enCROSS;
		BEEP_on();
		 
		} 
		 
		else if((PS2->FunBtn & PSB_SQUARE) == 0)	  //停止
		{
			g_carstate= enSQUARE;
		}  
		else if((PS2->FunBtn & PSB_R2) == 0)
		{
			g_carstate= enR2;
		}
		else                           //去掉则按下一直执行，加此段则放开停止
		{
		  g_carstate = enSTOP;        //将小车定义为停止状态
		}
		
 	
}
/**********串口2发送数据函数*****************/
void SendDataByUart2(uint8 dat)
{
    S2BUF = dat;                 //写数据到UART数据寄存器
		while(!(S2CON&S2TI));        //在停止位没有发送时，S2TI为0即一直等待
    S2CON&=~S2TI;                //清除S2CON寄存器对应S2TI位（该位必须软件清零）
}
/************ 串口2接收到数据后发送出去****************/
void UART2_Tx_Puts(void)
{
  if(Flag1)      //有新数据通过串口被接收到
	{
		
	
		IE2 &= 0xFE; 			                     // 串口2中断关闭	
		SendDataByUart2(uart2temp);            //发送字符 
    SendDataByUart2(0x0D);                 //发送换行符
    SendDataByUart2(0x0A);                 //发送换行符						
		IE2 |= 0x01;                           // 串口2中断打开													
		Flag1=FALSE;                            //清除接收标识符
  }
}
 /***********主函数***************/
int main()
{	
	int i=0;
	int pwm=0;
	Uart2_Init();		//串口初始化	
	TIME0_Init();			//定时器0初始化
	PS2_Init();			//PS2初始化
 	P0M1 &= 0x3F;	P0M0 &= 0x3F;   //设置P0.6，P0.7为准双向口
	P3M1 &= 0x7F;	P3M0 &= 0x7F;	  //设置P3.7为准双向口
	P2M1 &= 0xDF;	P2M0 &= 0xDF;	  //设置P2.5为准双向口 
  delay_ms(10);	//初始化后延时
	ENA=1;
	ENB=1;
  while (1)
	{ 
		
		PS2_control();
		IN1=0;
		IN2=0;
		IN3=0;
		IN4=0;//电机接收电流置零
		
		if(g_carstate==enTRIANGLE)
		{
			flag=1;//将遥控器发出的对应模式以flag的形式记录
		}
		else if(g_carstate==enCROSS)
		{
			flag=3;
		}
			else if(g_carstate==enCIRCLE)
		{
			flag=2;
		}
			else if(g_carstate==enSQUARE)
		{
			flag=0;
		}
		if(flag==1)
		{
			
		 BEEP_off();
			EA=1;
		 switch(g_carstate)     
		 {					 
				case enRUN:	
				{
					run();
				}
					break;	
			 case enBACK: 
			 {
				back();
			 }
				break;
			case enLEFT:
			{
				left();
			}
			
			break;		   
			case enRIGHT:
			{
				right();
			}
				break;	
			default: 
			{
			stop();
			}
			break;				    
	   }
	 }
		delay_ms(10);
	 if(flag==2)
	 {
		
		 BEEP_off();
		 EA=1;
		 ET0=0;//关闭定时器中断
	 }
	 
	 if(flag==3)
	 {
		 BEEP_off();
		 ET0=1;//关闭定时器中断
		 EA=1;

	 }
	if(flag==0)
	{
		
	 stop();
	}
  } 
}

