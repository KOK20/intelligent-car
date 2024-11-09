
#include "STC12C5A60S2.h"
#include  <intrins.h>
#include "stdio.h"
#include "string.h"
#include "ps2.h"

//����SPIͨ�ŵ�����
//��������޸Ķ�ӦIO��
sbit  DATA=P1^4;	   //�൱��DI/MOS
sbit  CMND=P1^3;       //�൱��DO/MIS
sbit  ATT=P1^2;        //�൱��Ƭѡ�ź�
sbit  CLK=P1^5;        //�൱��ʱ���ź�

//��ʼ��PS2ʱ������Ҫͨ���������ֱ����͵����������õ�
uchar code lx1[5]={0x01,0x43,0x00,0x01,0x00};						// ��������ģʽ
uchar code lx2[9]={0x01,0x44,0x00,0x01,0x03,0x00,0x00,0x00,0x00}; 	// ����ģʽ
uchar code lx3[9]={0x01,0x4f,0x00,0xff,0xff,0x03,0x00,0x00,0x00}; 	// �����ֽڳ���
uchar code lx4[9]={0x01,0x43,0x00,0x00,0x5A,0x5A,0x5A,0x5A,0x5A}; 	// �˳�����ģʽ

//��������������Ҫ���������ֱ�������
uchar code scan[9]={0x01,0x42,0x00,0x00,0x00,0x00,0x00,0x00,0x00};//{0x01,0x42,0x00,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A};	// ���Ͷ�ȡ
//�洢���ֱ������������ֻ�������
uchar  out[9];
//������������Ҫ��data[3]��data[4]���ֱ��İ���ֵ	    
uint FunBtn = 0xffff;
		   	
stPS2KeyValue stPS2key;
stPS2KeyValue * stPS2 = &stPS2key;

/**
* Function       delayms
* @author        Danny
* @date          2017.08.16
* @brief         ��ʱ����
* @param         xms����ʱʱ�䣨ms��
* @retval        void
* @par History   ��
*/	
void delayms(uint xms)	         
{
   uint i,j;   
   for(i=xms;i>0;i--)
      for(j=850;j>0;j--);
}

/**
* Function       delay
* @author        Danny
* @date          2017.08.16
* @brief         ��ʱ����
* @param         ��ʱһ��ָ������
* @retval        void
* @par History   ��
*/	

static void delay(uint n) 		  //��ʱ��׼ȷ
{
	uint i;
	for(i=0;i<=n;i++) 
	    _nop_();                 //��ָ����ʱһ��ָ������
		_nop_();	
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();	
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();	
		_nop_();				
}


/**
* Function       spi_write
* @author        Danny
* @date          2017.08.16
* @brief         ��SPI�������ֱ�����д��һ��������
* @param         command:Ҫд�뵽�ֱ�������
* @retval        void
* @par History   ��
*/
void spi_write(uchar command)			 
{
    uchar i;
    for(i=0; i<=7; i++)              //������һλһλ��д��SPI���� 
    {
		if(command&0x01)	         //ȡ��д������ݵ����ұߵ�һλ
			CMND=1;                  //����������ӻ���
			else
			CMND=0;
		command=command>>1;         //��Ҫд�����������һλ��
		_nop_();	                //����Ҫ����ʱ���������Ϊ����̫�������Ӧ��
		_nop_();
		CLK=0;                      //������д������
		delay(10*10);                   
		CLK=1;
		delay(1*10);
    }
	CMND=1;
}

/**
* Function       spi_read
* @author        Danny
* @date          2017.08.16
* @brief         ��SPI�����ж���һ��������
* @param         void
* @retval        void
* @par History   ��
*/
static uchar spi_read(uchar command)      
{
    uchar i,j=1;
	uchar res=0; 
    for(i=0; i<=7; i++)          
    {
		if(command&0x01)             //������һλһλ�Ĵ�SPI�����ж�ȡ����
			CMND=1;					 //����ӳ�
		else
			CMND=0;
		command = command >> 1;     //��Ҫ��������������һλ��
		_nop_();	                //����Ҫ����ʱ���������Ϊ����̫�������Ӧ��
		_nop_();
		CLK=0;
		delay(2);
		if(DATA)                   //���DATAΪ��Ļ���������������ӻ��루����ģ�鷢�ͳ����ݣ�Ҳ���Ǵ�SPI�����ж���һ�����ݣ�
		res = res + j;
		j=j<<1; 
		CLK=1;
		delay(5);		 
    }
    CMND=1;
    return res;	                   //res�Ƕ�����������,Ҳ�����ֱ�����������������
}
 
/**
* Function       PS2_Init
* @author        Danny
* @date          2017.08.16
* @brief         PS2��ʼ��
* @param         void
* @retval        void
* @par History   ��
*/	
void PS2_Init()
{
    uchar i =0;
    //out�����ʼ��
	for(i=0; i<9; i++)  	
	{
    	out[i]=0x00;	
	}  
	//Ƭѡ�ź�����,���ӻ���ͨ�ŵĹ�������Ҫ��Ƭѡ�����͵�
	ATT=0;
	delay(7);
	for(i=0;i<9;i++)	
	{
		out[i]=spi_read(scan[i]);
		delay(7);
	} 
	ATT=1;
	delayms(15);
 
   	//Ƭѡ�ź�����,���ӻ���ͨ�ŵĹ�������Ҫ��Ƭѡ�����͵�
	//ͨ���������ֱ���д�������������ֱ�
	ATT=0;
	delay(7);
	for(i=0; i<5; i++)  
	{
		spi_write(lx1[i]);		   //��������ģʽ
		delay(7);	
	} 
    ATT=1;
	delayms(8); 

    //Ƭѡ�ź�����,���ӻ���ͨ�ŵĹ�������Ҫ��Ƭѡ�����͵�
	//ͨ���������ֱ���д�������������ֱ�
	ATT=0;
	delay(7);
	for(i=0; i<9; i++)	 		  //����ģʽ
	{
		spi_write(lx2[i]);
		delay(7);																
	} 
	ATT=1;
	delayms(8); 

    //Ƭѡ�ź�����,���ӻ���ͨ�ŵĹ�������Ҫ��Ƭѡ�����͵�
	//ͨ���������ֱ���д�������������ֱ�
	ATT=0;
	delay(7);	   
	for(i=0; i<9; i++)	
	{							  //�����ֽڳ���
		spi_write(lx3[i]);
		delay(7);	
	} 
	ATT=1;
	delayms(8); 
	
	//Ƭѡ�ź�����,���ӻ���ͨ�ŵĹ�������Ҫ��Ƭѡ�����͵�
	//ͨ���������ֱ���д�������������ֱ�
	ATT=0;
	delay(7);	
	for(i=0; i<9; i++)			  //�˳�����ģʽ
	{
		spi_write(lx4[i]);
		delay(7);	
	} 
	ATT=1;
	delayms(8); 
		
}

/**
* Function       ReadPS2KeyValue
* @author        Danny
* @date          2017.08.16
* @brief         PS2�������
* @param         void
* @retval        void
* @par History   ��
*/	
stPS2KeyValue* ReadPS2KeyValue(void)
{
    
    //������������Ҫ��data[3]��data[4]���ֱ��İ���ֵ
	//ֻ�������������õ��Ǿֲ�����
	uint funbtn = 0xffff;
	uchar i =0;
	ATT=0;
	for(i=0;i<9;i++)	//����ɨ�谴��
	{
		out[i]=spi_read(scan[i]);	
	} 
	ATT=1;
	//�Ѷ�����data[3]��data[4]������funbtn��ȥ
	funbtn = ((uint)out[4] << 8) | out[3];
	//�Ѵ��ֱ���ⰴ�����µ�������ȡ��������ȫ�ֱ���
	stPS2->FunBtn = funbtn;
	//�����5,6,7,8��ֱ������ǵ�ҡ������
	stPS2->X1 = out[7];
	stPS2->Y1 = out[8];
	stPS2->X2 = out[5];
	stPS2->Y2 = out[6];
	return stPS2;
}

