#ifndef __ADRC_H
#define __ADRC_H

#include "mymath.h"

#define T       0.002f
//�Կ��ſ������Ŀ��Ʋ���������״̬����
//�Կ��ſ���������ƽ�Ϊ���,ʹ�õĲ������ܻ�Ƶ���Ķ�
typedef struct
{
	//���Ʋ���
	float KpOut;     //�⻷��̬��������
	float KpIn;      //�ڻ����ٶȱ�������
	float KdIn;      //�ڻ����ٶ�΢�ֿ���
	float B;         //�Ŷ���������
	//״̬����
	float AttOut;    //�⻷���
	float SpeEst;    //���ٶȹ���
	float AccEst;    //�Ǽ��ٶȹ���
	float w;         //���Ŷ�
	float u;         //�������������
	float x1;        //����΢������������ź�
	float x2;        //����΢�������΢���ź�
}ADRC_Param;

void ADRC_LESO(ADRC_Param *adrc,float y);
void ADRC_ParamClear(ADRC_Param *adrc);
void ADRC_TD(ADRC_Param* adrc);

#endif
