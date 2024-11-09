#include  "delay.h"
#include "ps2.h"
#include "stdio.h"
#include "15W4KxxS4.h"
#include "uart.h"
#include "beep.h"
/**********��������***********/
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
 /*********��������**********/
volatile uint8 Flag1=FALSE;
uint8 uart2temp,sig;
int g_carstate = 0;
int pwm_control=0;
int flag=0;
/************��������********************/
void TIME0_Init(void);//��ʱ������
/************ö�ٰ���״̬****************/
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
/**************��ʱ������**************/
void TIME0_Init(void)
{
		
		
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TMOD |= 0x01;		//���ö�ʱ��ģʽ
  TL0 = (65526-1000/1.085)/256;		//���ö�ʱ��ʼֵ
	TH0 = 65526-1000/1.085;		//���ö�ʱ����ֵ
	TR0= 1;		    //����T0����
	TF0=0;
	ET0= 1;		    //����T0�жϣ���ʱ���жϿ�
	EA = 1;	        //�����ж�
}
/********** ����2��ʼ������************/
void Uart2_Init(void)
{	
	P_SW2|=S2_S;   //ѡ��P46 P47Ϊ����2
		
	S2CON = 0x50;		//8λ����,�ɱ䲨���ʣ��������н�����	
	AUXR |= 0x04;		//��ʱ��2ʱ��ΪFosc,��1T
	T2L = 0xE0;		  //�趨��ʱ��ֵ
	T2H = 0xFE;     //�趨��ʱ��ֵ
	AUXR |= 0x10;   //������ʱ��2
	IE2 |= 0x01;    //����2�жϴ�
}
/**************�˶�״̬�����趨*********/
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
/**********����2�жϷ�����********************/
void Uart2() interrupt UART2_VECTOR using 1
{
	IE2 &= 0xFE; 			               // ����2�жϹر�
	Flag1=TRUE;                       //���յ�����,���ձ�ʶ����Ч

	led1=0;                           //����2ָʾ���������
		
	if (S2CON & S2RI)                //���н��յ�ֹͣλ���м�ʱ��ʱ����λ��1
  {
      S2CON &= ~S2RI;              //���S2CON�Ĵ�����ӦS2RIλ����λ����������㣩
			uart2temp = S2BUF;       
   }
	 switch(uart2temp)
    {
			 case 0x01:   //�յ���1��
       run();
			 delay_ms(300);
			 stop();
            break;
        
        case 0x02:    //�յ���2��
       back();
				 delay_ms(300);
				stop();
            break;
        
        case 3:    //�յ���3��
       left();
				 delay_ms(300);
				stop();
            break;
        
        case 4:    //�յ���4��
       right();
				 delay_ms(300);
				stop();
            break;
        
        default:
            P2=0XFF;
            break;
    }
  if (S2CON & S2TI)                //��ֹͣλ��ʼ����ʱ����λ��1
   {
      S2CON &= ~S2TI;   			     //���S2CON�Ĵ�����ӦS2TIλ����λ����������㣩
   }
	 IE2 |= 0x01;                    // ����2�жϴ�	
}
/**************��ʱ��0�ж�***************/
void timer0()interrupt 1 
{
    
   TL0 = (65526-1000/1.085)/256;		//���ö�ʱ��ʼֵ
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
//******************ps2��������***************//
 void PS2_control()
{
        //PS2������������
        stPS2KeyValue * PS2;
    	
		PS2 = ReadPS2KeyValue();	    //����ֱ�����֮���õ�ֵ
		//��data[3]��data[4]����������ϳ�16λ������
		//��data[4]data[3]:��select����ʱ
		//FunBtn��0x1111 1111 1111 1110
		if((PS2->FunBtn & PSB_SELECT) == 0)
		{
		 
		}
		
		if((PS2->FunBtn & PSB_START) == 0)
		{
		  
		}
		 
		if((PS2->FunBtn & PSB_L3) == 0)
		{
			
			 g_carstate = enSTOP;      //��С������Ϊֹͣ״̬
		}

		if((PS2->FunBtn & PSB_R3) == 0)
		{
			
			 g_carstate = enSTOP;      //��С������Ϊֹͣ״̬
		}

		if((PS2->FunBtn & PSB_PAD_UP) == 0)
		{
			
			 g_carstate = enRUN;       //��С������Ϊǰ��״̬
		}

		else if((PS2->FunBtn & PSB_PAD_RIGHT) == 0)
		{
			 
			 g_carstate = enRIGHT;     //��С������Ϊ״̬��ת
		}
		else if((PS2->FunBtn & PSB_PAD_DOWN) == 0)
		{
			
			g_carstate = enBACK;       //��С������Ϊ����״̬
		}
		else if((PS2->FunBtn & PSB_PAD_LEFT) == 0)
		{
			 
			 g_carstate = enLEFT;       //��С������Ϊ��ת״̬
		}
	   else if((PS2->FunBtn & PSB_TRIANGLE) == 0) // ң��������ģʽ
		{
			  BEEP_on();
		delay_ms(10);
			g_carstate= enTRIANGLE;
			 
		} 
		 
		 else if((PS2->FunBtn & PSB_CIRCLE) == 0)	// ����ģʽ
		{
			BEEP_on();
		 delay_ms(10);
			  g_carstate= enCIRCLE;
		}  
	
		else if((PS2->FunBtn & PSB_CROSS) == 0) // �Զ�����ģʽ
		{
			delay_ms(10);
	     g_carstate= enCROSS;
		BEEP_on();
		 
		} 
		 
		else if((PS2->FunBtn & PSB_SQUARE) == 0)	  //ֹͣ
		{
			g_carstate= enSQUARE;
		}  
		else if((PS2->FunBtn & PSB_R2) == 0)
		{
			g_carstate= enR2;
		}
		else                           //ȥ������һֱִ�У��Ӵ˶���ſ�ֹͣ
		{
		  g_carstate = enSTOP;        //��С������Ϊֹͣ״̬
		}
		
 	
}
/**********����2�������ݺ���*****************/
void SendDataByUart2(uint8 dat)
{
    S2BUF = dat;                 //д���ݵ�UART���ݼĴ���
		while(!(S2CON&S2TI));        //��ֹͣλû�з���ʱ��S2TIΪ0��һֱ�ȴ�
    S2CON&=~S2TI;                //���S2CON�Ĵ�����ӦS2TIλ����λ����������㣩
}
/************ ����2���յ����ݺ��ͳ�ȥ****************/
void UART2_Tx_Puts(void)
{
  if(Flag1)      //��������ͨ�����ڱ����յ�
	{
		
	
		IE2 &= 0xFE; 			                     // ����2�жϹر�	
		SendDataByUart2(uart2temp);            //�����ַ� 
    SendDataByUart2(0x0D);                 //���ͻ��з�
    SendDataByUart2(0x0A);                 //���ͻ��з�						
		IE2 |= 0x01;                           // ����2�жϴ�													
		Flag1=FALSE;                            //������ձ�ʶ��
  }
}
 /***********������***************/
int main()
{	
	int i=0;
	int pwm=0;
	Uart2_Init();		//���ڳ�ʼ��	
	TIME0_Init();			//��ʱ��0��ʼ��
	PS2_Init();			//PS2��ʼ��
 	P0M1 &= 0x3F;	P0M0 &= 0x3F;   //����P0.6��P0.7Ϊ׼˫���
	P3M1 &= 0x7F;	P3M0 &= 0x7F;	  //����P3.7Ϊ׼˫���
	P2M1 &= 0xDF;	P2M0 &= 0xDF;	  //����P2.5Ϊ׼˫��� 
  delay_ms(10);	//��ʼ������ʱ
	ENA=1;
	ENB=1;
  while (1)
	{ 
		
		PS2_control();
		IN1=0;
		IN2=0;
		IN3=0;
		IN4=0;//������յ�������
		
		if(g_carstate==enTRIANGLE)
		{
			flag=1;//��ң���������Ķ�Ӧģʽ��flag����ʽ��¼
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
		 ET0=0;//�رն�ʱ���ж�
	 }
	 
	 if(flag==3)
	 {
		 BEEP_off();
		 ET0=1;//�رն�ʱ���ж�
		 EA=1;

	 }
	if(flag==0)
	{
		
	 stop();
	}
  } 
}

