#include "mymath.h"
/**************�ļ�˵��**********************
���ó��Ⱥ����Ŀ����㷨
��⺯����ͻ�ĺ�����ǰ׺"M"
����Ŀ¼(������Ϊ�������ֵ��):
���Һ���      Msin(x)     [-pi/2,pi/2]    [-1,1]
���Һ���      Mcos(x)     [-pi/2,pi/2]    [-1,1]
�����к���1   Matan(x)    (-��,��)          (-pi/2,pi/2)
�����к���2   Matan2(y,x) (-��,��)          (-pi/2,pi/2)
�����Һ���    Masin(x)    [-1,1]          [-pi/2,pi/2]
�����Һ���    Macos(x)    [-1,1]          [0,pi]
ƽ��������    Msqrt       [0,��)           [0,��)
ƽ��������    Q_rsqrt     (0,��)           (0,��)
��Ȼָ������  Mexp        (-��,��)          (0,��)
��Ȼ��������  Mln         (0,��)           (-��,��)
����ֵ        ABS
����          SIGN
����          LIMIT
��Сֵ        MIN
�ϴ�ֵ        MAX
��ͨ�˲���    IIR_LowPassFilter
********************************************/

//�����к���.������(-��,��),ֵ��(-pi/2,pi/2)
float Matan(float rad)
{
	if (rad < 0)
		return -Matan(-rad);
	if (rad <= 0.25f)
	{
		float ans = rad*(0.47831170583472860f * rad - 1.1679191357523431f);
		return ans*(0.29922976727874773f * ans - 0.85630501107017942f);
	}
	if (rad <= 0.75f)
		return 0.463647609f + Matan((rad - 0.5f) / (1.0f + rad*0.5f));
	if (rad <= 2.0f)
		return 0.8760580506f + Matan((rad - 1.2f) / (1.0f + rad*1.2f));
	return 1.3258186637f + Matan((rad - 4.0f) / (1.0f + rad * 4.0f));
}

//360�㷴����.������(-��,��),ֵ��(-pi,pi]
float Matan2(float y,float x)
{
	if (x > 0)
		return Matan(y / x);
	if (x < 0)
		return (y >= 0) ? (Matan(y / x) + PI) : (Matan(y / x) - PI);
	return (y > 0) ? (PI / 2) : ((y < 0) ? (-PI / 2) : 0);
}

//����ƽ�����㷨
float Msqrt(float number)
{
	long i;
	float x, y;
	x = number * 0.5f;
	y = number;
	i = *(long *)&y;
	i = 0x5f3759df - (i >> 1);
	y = *(float *)&i;
	y = y * (1.5f - (x * y * y));
	y = y * (1.5f - (x * y * y));
	return number * y;
}

//����ƽ���������㷨
float Q_rsqrt(float number)
{
	long i;
	float x2, y;
	x2 = number * 0.5f;
	y  = number;
	i  = * ( long * ) &y;
	i  = 0x5f3759df - ( i >> 1 );
	y  = * ( float * ) &i;
	y  = y * ( 1.5f - ( x2 * y * y ) );
	y  = y * ( 1.5f - ( x2 * y * y ) );
	return y;
}

//�����Һ���.������[-1,1],ֵ��[-pi/2,pi/2]
float Masin(float x)
{
	if(x<0)
		return -Masin(-x);
	else if(x<1.0f)
		return Matan(x*Q_rsqrt(1.0f-x*x));
	else
	{
		x=PI/2.0f;
		return x;
	}
}

//���Һ���.������[-pi,pi],ֵ��[-1,1]
float Msin(float rad)
{
	float ans;
	ans = rad*(1.27323954f - 0.405284735f * ABS(rad));
	ans = ans*(0.775f + 0.225f*ABS(ans));
	return ans;
}

//ָ������
float Mexp(float x)
{
	long i = (long)(x * 12102203 + 1064872507);
	return *(float*)&i;
}

/***********************
����IIR��ͨ�˲���ֱ��II�ͽṹ
*@delay:��Ҫ�ݴ�3��״̬�����Ĵ洢�ռ�
*@DataIn:ÿ������������
����˲������������
**********************/
float IIR_LowPassFilter(float DataIn,float *delay)
{
	delay[0] = DataIn + 1.7f*delay[1] - 0.7325f*delay[2];
	float DataOut = (delay[0] + 2*delay[1] + delay[2]) * 0.008125f;
	delay[2] = delay[1];
	delay[1] = delay[0];
	return DataOut;
}

/***********************
һ��IIR��ͨ�˲�
*@delay:��Ҫ�ݴ�1��״̬�����Ĵ洢�ռ�
*@DataIn:ÿ������������
����˲������������
**********************/
float OneOrder_Filter(float DataIn,float delay)
{
	float DataOut = DataIn + 0.25f * delay;
	delay = DataOut;
	return DataOut * 0.75f;
}
