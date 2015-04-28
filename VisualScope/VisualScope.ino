#include <Arduino.h>
#include "ITG3200.h"
#include <Wire.h>

ITG3200 gyro;

float OutData[4]={0};
float angle_m;
float gyro_m;


//============================================================================
//函数名称：CRC_CHECK
//函数返回：
//参数说明：
//功能概要：计算CRC校验码
//============================================================================
unsigned short CRC_CHECK(unsigned char *Buf, unsigned char CRC_CNT)
{
    unsigned short CRC_Temp;
    unsigned char i,j;
    CRC_Temp = 0xffff;

    for (i=0;i<CRC_CNT; i++)
    {      
        CRC_Temp ^= Buf[i];
        for (j=0;j<8;j++)
        {
            if (CRC_Temp & 0x01)
                CRC_Temp = (CRC_Temp >>1 ) ^ 0xa001;
            else
                CRC_Temp = CRC_Temp >> 1;
        }
    }
    return(CRC_Temp);
}


//============================================================================
//函数名称：OutPut_Data
//函数返回：
//参数说明：
//功能概要：输出CH通道的数据到上位机
//============================================================================
void OutPut_Data(void)
{
  int temp[4] = {0};
  unsigned int temp1[4] = {0};
  unsigned char databuf[10] = {0};
  uint8_t i;
  unsigned short CRC16 = 0;
  for(i=0;i<4;i++)
   {
    
    temp[i]  = (int)OutData[i];
    temp1[i] = (unsigned int)temp[i];
    
   }
   
  for(i=0;i<4;i++) 
  {
    databuf[i*2]   = (unsigned char)(temp1[i]%256);
    databuf[i*2+1] = (unsigned char)(temp1[i]/256);
  }
  
  CRC16 = CRC_CHECK(databuf,8);
  databuf[8] = CRC16%256;
  databuf[9] = CRC16/256;
  
  for(i=0;i<10;i++)
  {
     //uart_send1(UART1,databuf[i]);
     Serial.write(databuf[i]);
  }

}


//============================================================================
//函数名称：Data_acquisition
//函数返回：
//参数说明：
//功能概要：数据采集上位机监测
//============================================================================
void Data_acquisition(float temp1,float temp2,float temp3,float temp4)
{
       OutData[0] = temp1;
       OutData[1] = temp2;
       OutData[2] = temp3; 
       OutData[3] = temp4;
       OutPut_Data();
}

void setup() {
    gyro.init();
    gyro.zeroCalibrate(200,10);
    
    Serial.begin(9600);
    Serial.println("init ok!");
    
}

void loop() { 
    int16_t x,y,z;
    gyro.getXYZ(&x,&y,&z);
    
    Data_acquisition(x, y, z, 0);
    OutPut_Data();
}
