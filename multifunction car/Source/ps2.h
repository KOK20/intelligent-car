#ifndef __PS2_H__
#define __PS2_H__

#define  uchar unsigned char
#define  uint  unsigned int

#define PSB_SELECT      0x0001
#define PSB_L3          0x0002
#define PSB_R3          0x0004
#define PSB_START       0x0008
#define PSB_PAD_UP      0x0010
#define PSB_PAD_RIGHT   0x0020
#define PSB_PAD_DOWN    0x0040
#define PSB_PAD_LEFT    0x0080
#define PSB_L2          0x0100
#define PSB_R2          0x0200
#define PSB_L1          0x0400
#define PSB_R1          0x0800
#define PSB_GREEN       0x1000
#define PSB_RED         0x2000
#define PSB_BLUE        0x4000
#define PSB_PINK        0x8000
#define PSB_TRIANGLE    0x1000
#define PSB_CIRCLE      0x2000
#define PSB_CROSS       0x4000
#define PSB_SQUARE      0x8000

typedef struct{

	uint FunBtn;
	uchar X1;
	uchar Y1;
	uchar X2;
	uchar Y2;

}stPS2KeyValue;


void PS2_Init(void);

stPS2KeyValue* ReadPS2KeyValue(void);
void SendBuf(uchar * ch) ;

void delayms(uint xms);	 //��ʱ��׼ȷ



#endif