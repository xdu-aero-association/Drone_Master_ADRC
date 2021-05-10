#ifndef __NIMING_H
#define __NIMING_H

#include "usart.h"

#define SENDBUF_SIZE   128
//FcnWord
#define P_STAT         0x01  //״̬
#define P_ATTI         0x02  //��̬
#define P_SENSOR       0x04  //������
#define P_CTRL         0x08  //ң��
#define P_MOTOR        0x10  //����
#define P_QUATERNION   0x20  //��Ԫ��
#define P_ROL_CTRL     0xA1  //ROL����
#define P_PIT_CTRL     0xA2  //PIT����
#define P_YAW_CTRL     0xA3  //YAW����
#define P_CHART1       0XB1  //������ʾ1
#define P_CHART2       0XB2  //������ʾ2
#define P_REQ1         0xC1  //������1
#define P_REQ2         0xC2  //������2
#define P_REQ3         0xC3  //������3
#define P_REQ4         0xC4  //������4
//GlobalStat
#define MOTOR_LOCK     0x01  //�ѽ���
#define ACC_CALI       0x02  //׼��У׼���ٶȼ�
#define GYRO_CALI      0x04  //׼��У׼������
#define IDTFY_MODE     0x10  //������ʶģʽ
#define SPEED_MODE     0x20  //�ٶ�ģʽ
#define TX_BUSY        0x40  //����������ͨ��DMA����
#define FAIL_SAFE      0x80  //���й���
//stat
#define REQ_MODE_SEL   0xC0
#define REQ_MODE_ATTI  0x40
#define REQ_MODE_SPEED 0x80
#define REQ_MODE_IDTFY 0xC0
//ReqMsg1
#define REQ_STAT       0x01
#define REQ_ATTI       0x02
#define REQ_SENSOR     0x04
#define REQ_RC         0x08
#define REQ_MOTOR      0x10
#define REQ_QUATERNION 0x20
//ReqMsg2
#define REQ_ROL_CTRL   0x01
#define REQ_PIT_CTRL   0x02
#define REQ_YAW_CTRL   0x04
#define REQ_ACC_CALI   0x40
#define REQ_GYRO_CALI  0x80

//���ݲ�ֺ궨�壬�ڷ��ʹ���1�ֽڵ���������ʱ������int16��float�ȣ���Ҫ�����ݲ�ֳɵ����ֽڽ��з���
#define BYTE0(dwTemp)       ( *( (char *)(&dwTemp)    ) )
#define BYTE1(dwTemp)       ( *( (char *)(&dwTemp) + 1) )
#define BYTE2(dwTemp)       ( *( (char *)(&dwTemp) + 2) )
#define BYTE3(dwTemp)       ( *( (char *)(&dwTemp) + 3) )

extern u8 FcnWord;  //��task.c����
extern u8 LenWord;  //��task.c����
extern u8 RxTemp[12];  //��task.c����
extern u8 GlobalStat;  //��control.c,task.c����
extern u8 RcvCnt;

void Protocol_Init(void);
void Total_Send(void);
void XDAA_Send_S16_Data(s16 *data,u8 len,u8 fcn);
void XDAA_Send_U8_Data(u8 *data,u8 len,u8 fcn);
void XDAA_Send_HighSpeed_Data(short x,short y);

#endif
