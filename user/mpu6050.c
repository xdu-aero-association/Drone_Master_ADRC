#include "mpu6050.h"

void Delay_ms(unsigned short time_ms)
{
	unsigned short i, j;
	for (i = 0; i < time_ms; i++)
	{
		for (j = 0; j < 10309; j++);
	}
}

//����MPU6050�������Ǵ����������̷�Χ
//fsr:0,��250dps;1,��500dps;2,��1000dps;3,��2000dps
//����ֵ:0,�ɹ�
#define MPU_Set_Gyro_Fsr(fsr)  (IIC_Write_Reg(0x1B, fsr << 3))  //Register 27 �C Gyroscope Configuration

//����MPU6050�ļ��ٶȴ����������̷�Χ
//fsr:0,��2g;1,��4g;2,��8g;3,��16g
//����ֵ:0,�ɹ�
#define MPU_Set_Accel_Fsr(fsr)  (IIC_Write_Reg(0x1C, fsr << 3))  //Register 28 �C Accelerometer Configuration

//����MPU6050�����ֵ�ͨ�˲���
//lpf:���ֵ�ͨ�˲�Ƶ��(Hz)
//����ֵ:0,�ɹ�
u8 MPU_Set_LPF(u16 lpf)
{
	u8 data = 0;
	if (lpf >= 188)data = 1;
	else if (lpf >= 98)data = 2;
	else if (lpf >= 42)data = 3;
	else if (lpf >= 20)data = 4;
	else if (lpf >= 10)data = 5;
	else data = 6;
	return IIC_Write_Reg(0x1A, data);  //Register 26 �C Configuration
}

//����MPU6050�Ĳ�����(�ٶ�Fs=1KHz)
//rate:4~1000(Hz)
//����ֵ:0,�ɹ�
u8 MPU_Set_Rate(u16 rate)
{
	u8 data,status=0;
	if (rate > 1000)rate = 1000;
	if (rate < 4)rate = 4;
	data = 1000 / rate - 1;
	status |= IIC_Write_Reg(0x19, data);  //Sample Rate Divider
	status |= MPU_Set_LPF(rate / 2);  //�Զ�����LPFΪ�����ʵ�һ��
	return status;
}
//�õ�������ֵ(ԭʼֵ)
//gx,gy,gz:������x,y,z���ԭʼ����(������)
//����ֵ:0,�ɹ�
//    ����,�������
u8 MPU_Get_Gyroscope(short *gx, short *gy, short *gz)
{
	u8 buf[6], res;
	res = IIC_Read_Len(0x43, 6, buf);  //Registers 67 to 72 �C Gyroscope Measurements
	if (res == 0)
	{
		*gx = ((u16)buf[0] << 8) | buf[1];
		*gy = ((u16)buf[2] << 8) | buf[3];
		*gz = ((u16)buf[4] << 8) | buf[5];
	}
	return res;;
}
//�õ����ٶ�ֵ(ԭʼֵ)
//gx,gy,gz:������x,y,z���ԭʼ����(������)
//����ֵ:0,�ɹ�
//    ����,�������
u8 MPU_Get_Accelerometer(short *ax, short *ay, short *az)
{
	u8 buf[6], res;
	res = IIC_Read_Len(0x3B, 6, buf);  //Registers 59 to 64 �C Accelerometer Measurements
	if (res == 0)
	{
		*ax = ((u16)buf[0] << 8) | buf[1];
		*ay = ((u16)buf[2] << 8) | buf[3];
		*az = ((u16)buf[4] << 8) | buf[5];
	}
	return res;;
}

//��ʼ��MPU6050
//����ֵ:0,�ɹ�
//����,�������
u8 MPU_Init(void)
{
	u8 res,status=0;
	status |= IIC_Write_Reg(0x6B,0x80);  //��λMPU6050
	Delay_ms(100);
	status |= IIC_Write_Reg(0x6B,0x00);  //����MPU6050 
	status |= IIC_Write_Reg(0x38,0x00);  //�ر������ж�
	status |= IIC_Write_Reg(0x6A,0x00);  //I2C��ģʽ�ر�
	status |= IIC_Write_Reg(0x23,0x00);  //�ر�FIFO
	status |= IIC_Write_Reg(0x6B,0x01);  //����CLKSEL,PLL X��Ϊ�ο�
	status |= IIC_Write_Reg(0x6C,0x00);  //���ٶ��������Ƕ�����
	status |= MPU_Set_Gyro_Fsr(2);  //�����Ǵ�����,��1000dps
	status |= MPU_Set_Accel_Fsr(0);  //���ٶȴ�����,��2g
	status |= MPU_Set_Rate(500);  //���ò�����500Hz
	status |= IIC_Read_Reg(0x75,&res);  //��ȡ����ID
	if (res != 0x68)//����ID��ȷ
		status |= 1;
	return status;
}
