#include "task.h"
/**************�ļ�˵��**********************
Para_Init();
Motor_Iner_loop();
Motor_Outer_loop();
��task.c����ͷ�ļ�task.h
--------------��������װ�����---------------
X������,����󷽵�����Ϊ1��ʼ˳ʱ����,1���������Ϊ��ʱ��
********************************************/

short PwmOut[4];  //�������,��ֵ������ʱ��,���PWM
/**********************
���Ʋ�����ʼ��
**********************/
void Para_Init(void)
{
	MOTOR1 = PwmOut[0]; MOTOR2 = PwmOut[1]; MOTOR3 = PwmOut[2]; MOTOR4 = PwmOut[3];
	adrcX.KpOut = 2.0f; adrcX.B = 1000.0f; adrcX.KpIn = 1.0f; adrcX.KdIn = 0.1f;
	adrcY.KpOut = 2.0f; adrcY.B = 1000.0f; adrcY.KpIn = 1.0f; adrcY.KdIn = 0.1f;
	ADRC_ParamClear(&adrcX);ADRC_ParamClear(&adrcY);
	Kyaw = 2.0f;
}

/**********************
����ڻ��Կ��ſ���
**********************/
void Motor_Iner_loop(void)
{
	if(!(GlobalStat & MOTOR_LOCK))
	{
		MOTOR1 = 0;
		MOTOR2 = 0;
		MOTOR3 = 0;
		MOTOR4 = 0;
		return;
	}
	if(RCdata[2]<=LOWSPEED)
	{
		MOTOR1 = LOWSPEED;
		MOTOR2 = LOWSPEED;
		MOTOR3 = LOWSPEED;
		MOTOR4 = LOWSPEED;
		ADRC_ParamClear(&adrcX);
		ADRC_ParamClear(&adrcY);
		return;
	}
	ADRC_LESO(&adrcX, GyroToDeg(Gyro.x));
	ADRC_LESO(&adrcY, GyroToDeg(Gyro.y));
	adrcX.u = adrcX.KpIn * (adrcX.AttOut - adrcX.SpeEst) - adrcX.KdIn * adrcX.AccEst - adrcX.w / adrcX.B;
	adrcY.u = adrcY.KpIn * (adrcY.AttOut - adrcY.SpeEst) - adrcY.KdIn * adrcY.AccEst - adrcY.w / adrcY.B;
	PwmOut[0] = RCdata[2] + DegToPwm(-adrcX.u - adrcY.u - YawOut);
	PwmOut[1] = RCdata[2] + DegToPwm(-adrcX.u + adrcY.u + YawOut);
	PwmOut[2] = RCdata[2] + DegToPwm(+adrcX.u + adrcY.u - YawOut);
	PwmOut[3] = RCdata[2] + DegToPwm(+adrcX.u - adrcY.u + YawOut);
	MOTOR1 = LIMIT(PwmOut[0], LOWSPEED, 1000);
	MOTOR2 = LIMIT(PwmOut[1], LOWSPEED, 1000);
	MOTOR3 = LIMIT(PwmOut[2], LOWSPEED, 1000);
	MOTOR4 = LIMIT(PwmOut[3], LOWSPEED, 1000);
}

/**********************
����⻷��������
**********************/
void Motor_Outer_loop(void)
{
	if(GlobalStat & SPEED_MODE)
	{
		adrcX.AttOut = PwmToDegAdd(RCdata[0]);
		adrcY.AttOut = -PwmToDegAdd(RCdata[1]);
	}
	else
	{
		adrcX.AttOut = adrcX.KpOut * (PwmToDegAdd(RCdata[0]) - roll);
		adrcY.AttOut = adrcY.KpOut * (-PwmToDegAdd(RCdata[1]) - pitch);
	}
	YawOut = Kyaw * (PwmToDegAdd(RCdata[3]) - GyroToDeg(Gyro.z));
}
