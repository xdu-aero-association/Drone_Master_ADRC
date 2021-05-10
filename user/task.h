#ifndef __TASK_H
#define __TASK_H

#include "protocol.h"
#include "imu.h"
#include "mpu6050.h"
#include "adrc.h"
#include "adc.h"

#define MOTOR1        (TIM1->CCR3) 
#define MOTOR2        (TIM1->CCR2)
#define MOTOR3        (TIM1->CCR4)
#define MOTOR4        (TIM1->CCR1)

#define LOWSPEED      50  //����
#define NORMALSPEED   400  //ƽ��ʱ�����Ŵ�С
#define ERR_TIME      20   //û���յ���ȷң�����źŵĴ���
#define LOST_TIME     100  //��ʱ��û���յ���ȷң�����źŵĴ���
//LockMode
#define LOCKED        0    //����״̬���޲���
#define TOUNLOCK      1    //����״̬�ҳ��Խ���
#define UNLOCKED      2    //����״̬
#define LOCK_TIME     20   //����ʱ��,2��

extern AxisInt Gyro;
extern float roll,pitch,yaw;
extern short RCdata[];
extern ADRC_Param adrcX,adrcY;
extern float Kyaw,YawOut;

//��task.c��
void IMU_Processing(void);
void RC_Processing(void);
void RC_Monitor(void);
void RC_Data_Send(void);
void RC_Data_Send_10ms(void);
void HighSpeed_Data_Send(void);
//��control.c��
void Para_Init(void);
void Motor_Iner_loop(void);
void Motor_Outer_loop(void);

#endif
