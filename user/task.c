#include "task.h"
/**************�ļ�˵��**********************
���˷��������ƺ���֮��Ķ�ʱ�������ֱ�Ϊ��
Lock_And_Unlock();             ����������
RC_Prepare();                  �Խ��ջ����źŽ���Ԥ����
IMU_Processing();              ��̬������£�MPU6050����У׼
********************************************/

u8 ReqMsg[4]={0};  //��λ��ָ��
u8 ErrCnt=0;  //δ�յ�ң�����źŵĴ���
AxisInt Acc;  //������ٶ�У׼������
//���²���Ϊ��������ʹ��
AxisInt Gyro;  //������ٶ�У׼������
float roll,pitch,yaw;  //��������̬
short RCdata[4];  //ң������������
ADRC_Param adrcX,adrcY;  //�Կ��ſ���������
float Kyaw,YawOut;  //yaw�������������������
short HighSpeedCnt=0;  //���͸��������ڼ��ֹ������������

/***********************
��̬�������,MPU6050����У׼
*@period:2ms
**********************/
void IMU_Processing(void)
{
	static float IIRax[3],IIRay[3],IIRaz[3];
	static float IIRgx[3],IIRgy[3],IIRgz[3];
	AxisInt oacc,ogyro;
	MPU_Get_Accelerometer(&Acc.x,&Acc.y,&Acc.z);
	MPU_Get_Gyroscope(&Gyro.x,&Gyro.y,&Gyro.z);
	oacc=Acc;ogyro=Gyro;
	Acc.x=IIR_LowPassFilter(oacc.x,IIRax);
	Acc.y=IIR_LowPassFilter(oacc.y,IIRay);
	Acc.z=IIR_LowPassFilter(oacc.z,IIRaz);
	Gyro.x=IIR_LowPassFilter(ogyro.x,IIRgx);
	Gyro.y=IIR_LowPassFilter(ogyro.y,IIRgy);
	Gyro.z=IIR_LowPassFilter(ogyro.z,IIRgz);
	Acc_Correct(&Acc);
	Gyro_Correct(&Gyro);
	IMUupdate(Acc,Gyro,&roll,&pitch,&yaw);
	if(GlobalStat & ACC_CALI)
		if(!Acc_Calibrate(Acc))
			GlobalStat &=~ ACC_CALI;
	if(GlobalStat &GYRO_CALI)
		if(!Gyro_Calibrate(Gyro))
			GlobalStat &=~ GYRO_CALI;
}

/***********************
ʧ�ر���.����������������ʱ����
ԭ��1:�෭����75��(����̬ģʽ)
ԭ��2:����2��δ�յ�ң���ź�
ԭ��3:����10��δ�յ�ң���ź�
�������:
ԭ��3ֱ������,����:
���ڽ���������ֱ������,
���������̬����,���ű���Ϊ��������
**********************/
void Fail_Safe(char state)
{
	if(state==3)
		GlobalStat&=~MOTOR_LOCK;
	else if(RCdata[2]<NORMALSPEED-110)
		GlobalStat&=~MOTOR_LOCK;
	else
	{
		RCdata[0]=500;
		RCdata[1]=500;
		RCdata[2]=NORMALSPEED-100;
		RCdata[3]=500;
	}
}

/***********************
��ʱ����Ƿ��յ�ң�����ź��������ź�
*@period:100ms
**********************/
void RC_Monitor(void)
{
	ErrCnt++;
	if(ErrCnt>=ERR_TIME)
	{
		if(ErrCnt<LOST_TIME)
			Fail_Safe(2);
		else
		{
			Fail_Safe(3);
			ErrCnt--;
		}
	}
	if(STAT_PORT & STAT_Pin)
		LED3_PORT |= LED3_Pin;
	else
		LED3_PORT &=~ LED3_Pin;
}

/***********************
���յ���ң�����źŽ��д���.�յ�һ֡����ִ��һ��
*@period:100ms(Not strict)
**********************/
void RC_Processing(void)
{
	switch(FcnWord)
	{
	case P_STAT:
		if((RxTemp[0]&MOTOR_LOCK)&&(RCdata[2]<=LOWSPEED))
			GlobalStat|=MOTOR_LOCK;
		else
			GlobalStat&=~MOTOR_LOCK;
		switch(RxTemp[0] & REQ_MODE_SEL)
		{
			case REQ_MODE_SPEED:GlobalStat|=SPEED_MODE;break;
			case REQ_MODE_ATTI:GlobalStat&=~SPEED_MODE;break;
			default:break;
		}
	case P_CTRL:
		RCdata[0]=(RxTemp[0]<<8) | RxTemp[1];
		RCdata[1]=(RxTemp[2]<<8) | RxTemp[3];
		RCdata[2]=(RxTemp[4]<<8) | RxTemp[5];
		RCdata[3]=(RxTemp[6]<<8) | RxTemp[7];
		break;
	case P_REQ1:
		ReqMsg[0]=RxTemp[0];
		break;
	case P_REQ2:
		ReqMsg[1]=RxTemp[0];
		if(ReqMsg[1] & REQ_ACC_CALI)
			GlobalStat|=ACC_CALI;
		if(ReqMsg[1] & REQ_GYRO_CALI)
			GlobalStat|=GYRO_CALI;
		break;
	case P_REQ3:
		ReqMsg[2]=RxTemp[0];
		break;
	case P_REQ4:
		ReqMsg[3]=RxTemp[0];
		break;
	case P_ROL_CTRL:
		adrcX.KpIn=(RxTemp[0]*256.0f+RxTemp[1])/1000.0f;
		adrcX.KdIn=(RxTemp[2]*256.0f+RxTemp[3])/1000.0f;
		adrcX.B=(RxTemp[4]*256.0f+RxTemp[5]);
		adrcX.KpOut=(RxTemp[6]*256.0f+RxTemp[7])/1000.0f;
		break;
	case P_PIT_CTRL:
		adrcY.KpIn=(RxTemp[0]*256.0f+RxTemp[1])/1000.0f;
		adrcY.KdIn=(RxTemp[2]*256.0f+RxTemp[3])/1000.0f;
		adrcY.B=(RxTemp[4]*256.0f+RxTemp[5]);
		adrcY.KpOut=(RxTemp[6]*256.0f+RxTemp[7])/1000.0f;
		break;
	case P_YAW_CTRL:
		Kyaw=(RxTemp[0]*256.0f+RxTemp[1])/1000.0f;
		break;
	default:break;
	}
	if((ABS(roll)>75)||(ABS(pitch)>75))
		if(!(GlobalStat & SPEED_MODE))
			Fail_Safe(1);
}

void RC_Data_Send(void)
{
	ErrCnt=0;
	if(HighSpeedCnt) return;
	s16 sdata[6];
	//��λ������1
	if(ReqMsg[0] & REQ_STAT)
	{
		u16 voltage=Get_Battery_Voltage();
		u8 udata[3]={GlobalStat,BYTE1(voltage),BYTE0(voltage)};
		XDAA_Send_U8_Data(udata,3,P_STAT);
		ReqMsg[0] &=~ REQ_STAT;
	}
	if(ReqMsg[0] & REQ_ATTI)
	{
		sdata[0]=(s16)(roll*100);
		sdata[1]=(s16)(pitch*100);
		sdata[2]=(s16)(yaw*100);
		XDAA_Send_S16_Data(sdata,3,P_ATTI);
		ReqMsg[0] &=~ REQ_ATTI;
	}
	if(ReqMsg[0] & REQ_SENSOR)
	{
		sdata[0]=Acc.x;sdata[1]=Acc.y;sdata[2]=Acc.z;
		sdata[3]=Gyro.x;sdata[4]=Gyro.y;sdata[5]=Gyro.z;
		XDAA_Send_S16_Data(sdata,6,P_SENSOR);
		ReqMsg[0] &=~ REQ_SENSOR;
	}
	if(ReqMsg[0] & REQ_RC)
	{
		XDAA_Send_S16_Data(RCdata,4,P_CTRL);
		ReqMsg[0] &=~ REQ_RC;
	}
	if(ReqMsg[0] & REQ_MOTOR)
	{
		sdata[0]=MOTOR1;
		sdata[1]=MOTOR2;
		sdata[2]=MOTOR3;
		sdata[3]=MOTOR4;
		XDAA_Send_S16_Data(sdata,4,P_MOTOR);
		ReqMsg[0] &=~ REQ_MOTOR;
	}
	if(ReqMsg[0] & REQ_QUATERNION)
	{
		float x=DegToRad(roll/2.0f);
		float y=DegToRad(pitch/2.0f);
		float z=DegToRad(yaw/2.0f);
		float q0=Mcos(x)*Mcos(y)*Mcos(z)+Msin(x)*Msin(y)*Msin(z);
		float q1=Msin(x)*Mcos(y)*Mcos(z)-Mcos(x)*Msin(y)*Msin(z);
		float q2=Mcos(x)*Msin(y)*Mcos(z)+Msin(x)*Mcos(y)*Msin(z);
		float q3=Mcos(x)*Mcos(y)*Msin(z)-Msin(x)*Msin(y)*Mcos(z);
		sdata[0]=(s16)(q0*10000);
		sdata[1]=(s16)(q1*10000);
		sdata[2]=(s16)(q2*10000);
		sdata[3]=(s16)(q3*10000);
		XDAA_Send_S16_Data(sdata,4,P_QUATERNION);
		ReqMsg[0] &=~ REQ_QUATERNION;
	}
	//��λ������2
	if(ReqMsg[1] & REQ_ROL_CTRL)
	{
		sdata[0]=(s16)(adrcX.KpIn*1000);
		sdata[1]=(s16)(adrcX.KdIn*1000);
		sdata[2]=(s16)(adrcX.B);
		sdata[3]=(s16)(adrcX.KpOut*1000);
		XDAA_Send_S16_Data(sdata,4,P_ROL_CTRL);
		ReqMsg[1] &=~ REQ_ROL_CTRL;
	}
	if(ReqMsg[1] & REQ_PIT_CTRL)
	{
		sdata[0]=(s16)(adrcY.KpIn*1000);
		sdata[1]=(s16)(adrcY.KdIn*1000);
		sdata[2]=(s16)(adrcY.B);
		sdata[3]=(s16)(adrcY.KpOut*1000);
		XDAA_Send_S16_Data(sdata,4,P_PIT_CTRL);
		ReqMsg[1] &=~ REQ_PIT_CTRL;
	}
	if(ReqMsg[1] & REQ_YAW_CTRL)
	{
		sdata[0]=(s16)(Kyaw*1000);
		sdata[1]=0;
		sdata[2]=0;
		sdata[3]=0;
		XDAA_Send_S16_Data(sdata,4,P_YAW_CTRL);
		ReqMsg[1] &=~ REQ_YAW_CTRL;
	}
}

void RC_Data_Send_10ms(void)
{
	if(HighSpeedCnt) return;
	s16 sdata[4];
	static u8 cnt1=0,cnt2=0;
	if(ReqMsg[2] & 0x0F)
	{
		cnt1=20;
		ReqMsg[2] &=~ 0x0F;
	}
	if(cnt1)
	{
		sdata[0]=DegToGyro(adrcY.SpeEst);
		sdata[1]=Gyro.y;
		sdata[2]=(s16)(adrcY.u*100);
		sdata[3]=(s16)adrcY.w;
		XDAA_Send_S16_Data(sdata,4,P_CHART1);
		cnt1--;
	}
	if(ReqMsg[2] & 0xF0)
	{
		cnt2=20;
		ReqMsg[2] &=~ 0xF0;
	}
	if(cnt2)
	{
		sdata[0]=(s16)(adrcY.AccEst*100);
		sdata[1]=(s16)(adrcY.AttOut*100);
		sdata[2]=0;
		sdata[3]=0;
		XDAA_Send_S16_Data(sdata,4,P_CHART2);
		cnt2--;
	}
}

void HighSpeed_Data_Send(void)
{
	if(ReqMsg[3])
	{
		HighSpeedCnt=5000;
		ReqMsg[3]=0;
	}
	while(HighSpeedCnt)
	{
		XDAA_Send_HighSpeed_Data(DegToGyro(adrcY.u),Gyro.y);
		HighSpeedCnt--;
	}
}
