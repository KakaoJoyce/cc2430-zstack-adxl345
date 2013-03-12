#include <ioCC2430.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include "adxl345.h"
//#include "LCD3310.h"
#define SCL     P1_2   
#define SDA     P1_3

#define SDADirOut P1DIR|=0x08;      //xxxx1M01
#define SDADirIn  P1DIR&=~0x08;

#define TRUE 1
#define FALSE 0
#define	SlaveAddress 0xA6 //����������IIC�����еĴӵ�ַ,����ALT  ADDRESS��ַ���Ų�ͬ�޸�

INT8U BUFFER[14];
int  dis_data;

char wan,qian,bai,shi,ge;

void initUARTtest(void);
void UartTX_Send_String(char *Data,int len);
void conversion(void);

void Sendack(unsigned char h);
unsigned char I2C_Check_ack();

void I2C_Start_1(void);
void I2C_Stop_1(void);

void WriteI2CByte_1(INT8U b);
unsigned char ReadI2CByte_1(void);

unsigned char Single_Read_ADXL345(INT8U REG_Address);
void Single_Write_ADXL345(INT8U REG_Address,INT8U REG_data);

void Init_ADXL345();

void Delay_1u(unsigned int microSecs);
void Delay(INT16U n);

INT8U I2C_Check_ack(void);


void Multiple_Read_ADXL345(void);

void Delay(INT16U n)      //(5/32)*n   us
{
	INT16U d;
	for(d=0;d<n;d++);
	for(d=0;d<n;d++);
	for(d=0;d<n;d++);
	for(d=0;d<n;d++);
	for(d=0;d<n;d++);
}

void Delay_1u(unsigned int microSecs) {
  while(microSecs--)
  {
    /* 32 NOPs == 1 usecs */
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop");
  }
}

void conversion(void)   
{  
  BUFFER[13] = 0xff;
  INT16U temp_data;
  if ((BUFFER[8] & 0x80) == 0x80)
  {
    BUFFER[13] &= ~0x01;    
  }
  temp_data = ((BUFFER[8] & 0x0f) << 8) + BUFFER[7];    
  temp_data = (INT16U)(((float)temp_data /4096 ) * 100);
  BUFFER[7] = (INT8U)((temp_data) % 10 + '0');
  BUFFER[8] = (INT8U)((temp_data) / 10 + '0');
    if ((BUFFER[10] & 0x80) == 0x80)
  {
    BUFFER[13] &= ~0x02;    
  }
  temp_data = ((BUFFER[10] & 0x0f) << 8) + BUFFER[9];    
  temp_data = (INT16U)(((float)temp_data / 4096) * 100);
  BUFFER[9] = (INT8U)((temp_data) % 10 + '0');
  BUFFER[10] = (INT8U)((temp_data) / 10 + '0');
    if ((BUFFER[12] & 0x80) == 0x80)
  {
    BUFFER[13] &= ~0x04;    
  }
  temp_data = ((BUFFER[12] & 0x0f) << 8) + BUFFER[11];    
  temp_data = (INT16U)(((float)temp_data / 4096) * 100);
  BUFFER[11] = (INT8U)((temp_data) % 10 + '0');
  BUFFER[12] = (INT8U)((temp_data) / 10 + '0');
} 

void I2C_Start_1(void)
    {
        /*����I2C���ߵĺ�������SCLΪ�ߵ�ƽʱʹSDA����һ��������*/
        SDADirOut;
        Delay_1u(1); 
        SDA=1;
        SCL=1;
        Delay_1u(5);
        SDA=0;
        Delay_1u(5);
        SCL=0;
        Delay_1u(5);
    }

    void I2C_Stop_1(void)
    {
        /*��ֹI2C���ߣ���SCLΪ�ߵ�ƽʱʹSDA����һ��������*/
        SCL=0;
        Delay_1u(1);
        SDADirOut;
        Delay_1u(1);
        SDA=0;
        Delay_1u(5);
        SCL=1;
        Delay_1u(5);
        SDA=1;
        Delay_1u(5);
    }

void Sendack(unsigned char h){
        SCL=0;
        Delay_1u(5);
        SDADirOut;
        Delay(5);
        SDA=h&0x01;
        Delay_1u(5);
        SCL=1;
        Delay_1u(5);
        SCL=0;
        Delay_1u(5);
    }

INT8U I2C_Check_ack(void){
    SCL=0;
    Delay_1u(5);
    SDADirOut;
    SDA=1;
    SDADirIn;
    Delay_1u(5);          //�˴��Ƿ��б�ҪʹSDA�����ߣ�������
    SCL=1;
    Delay_1u(5);
    if(SDA==1)
    {
      SCL=0;
      return 0;  //er
    }
    SCL=0;
    return 1;
}

    


    void WriteI2CByte_1(unsigned char b)
    {
        /*��I2C����дһ���ֽ�*/
        unsigned char e=8;
        SDADirOut;
        while(e--){
          SCL=0;
          Delay_1u(5);
          if(b&0x80)SDA=1;
          else SDA=0;
          b<<=1;
          Delay_1u(3);
          SCL=1;
          Delay_1u(5);
        }
        SCL=0;
        I2C_Check_ack();
    }

unsigned char ReadI2CByte_1(void)
    {
        /*��I2C���߶�һ���ֽ�*/
    unsigned char i=8;
    unsigned char c=0;
    SCL=0;
    SDADirOut;
    SDA=1;
    Delay_1u(3);
    SDADirIn;
    while(i--){
      c<<=1;
      SCL=0;
      Delay_1u(5);
      SCL=1;
      Delay_1u(5);
      if(SDA==1)c|=0x01;
      else c&=0xfe;
    }
    SCL=0;
    SDADirOut;
    return c; 
    }
//******���ֽ�д��******************************************* 

void Single_Write_ADXL345(INT8U REG_Address,INT8U REG_data) 
{ 
    I2C_Start_1();                  //��ʼ�ź� 
    WriteI2CByte_1(SlaveAddress);   //�����豸��ַ+д�ź� 
    WriteI2CByte_1(REG_Address);    //�ڲ��Ĵ�����ַ����ο�����pdf22ҳ  
    WriteI2CByte_1(REG_data);       //�ڲ��Ĵ������ݣ���ο�����pdf22ҳ  
    I2C_Stop_1();                   //����ֹͣ�ź� 
} 

//********���ֽڶ�ȡ***************************************** 
unsigned char Single_Read_ADXL345(INT8U REG_Address) 
{  INT8U REG_data;
    I2C_Start_1();                          //��ʼ�ź� 
    WriteI2CByte_1(SlaveAddress);           //�����豸��ַ+д�ź� 
    WriteI2CByte_1(REG_Address);                   //���ʹ洢��Ԫ��ַ����0��ʼ	
    I2C_Start_1();                          //��ʼ�ź� 
    WriteI2CByte_1(SlaveAddress+1);         //�����豸��ַ+���ź� 
    REG_data=ReadI2CByte_1();              //�����Ĵ������� 
    Sendack(1);   
    I2C_Stop_1();                           //ֹͣ�ź� 
    return REG_data;  
} 

void Init_ADXL345() 
{ 
  P1DIR = 0x0C;
   Single_Write_ADXL345(0x31,0x0B);   //������Χ,����16g��13λģʽ 
   Single_Write_ADXL345(0x2C,0x0F);   //�����趨Ϊ12.5 �ο�pdf13ҳ 
   Single_Write_ADXL345(0x2D,0x08);   //ѡ���Դģʽ   �ο�pdf24ҳ 
   Single_Write_ADXL345(0x2E,0x80);   //ʹ�� DATA_READY �ж� 
   Single_Write_ADXL345(0x1E,0x00);   //X ƫ���� ���ݲ��Դ�������״̬д��pdf29ҳ 
   Single_Write_ADXL345(0x1F,0x00);   //Y ƫ���� ���ݲ��Դ�������״̬д��pdf29ҳ 
   Single_Write_ADXL345(0x20,0x05);   //Z ƫ���� ���ݲ��Դ�������״̬д��pdf29ҳ 
} 

//********************************************************* 
// 
//��������ADXL345�ڲ����ٶ����ݣ���ַ��Χ0x32~0x37 
// 
//********************************************************* 
void Multiple_Read_ADXL345(void) 
{   INT8U f; 
    I2C_Start_1();                          //��ʼ�ź� 
    WriteI2CByte_1(SlaveAddress);           //�����豸��ַ+д�ź� 
    WriteI2CByte_1(0x32);                   //���ʹ洢��Ԫ��ַ����0x32��ʼ	
    I2C_Start_1();                          //��ʼ�ź� 
    WriteI2CByte_1(SlaveAddress+1);         //�����豸��ַ+���ź� 
 for (f=0; f<6; f++)                      //������ȡ6����ַ���ݣ��洢��BUF 
    { 
        BUFFER[7+f] = ReadI2CByte_1();          //BUF[0]�洢0x32��ַ�е����� 
      if (f == 5) 
        { 
          Sendack(1);                //���һ��������Ҫ��NOACK 
        } 
        else 
        { 
          Sendack(0);                //��ӦACK 
       } 
   } 
    I2C_Stop_1();                          //ֹͣ�ź� 
//    Delay(500);
//    Delay(500);
//    Delay(500);
//    Delay(500);
} 

void displayXYZ(INT8U *pData){
  int  dis_data;
  uint16 temp;
  dis_data=(pData[7]<<8)+pData[6];
    temp=(uint16)((float)dis_data*3.9);  //�������ݺ���ʾ,�鿼ADXL345�������ŵ�4ҳ
	if(dis_data<0){
	dis_data=-dis_data;
        HalLcdWriteStringValue("X: -",temp,10,0);
	}
	else
    {
        HalLcdWriteStringValue("X:  ",temp,10,0);
    }
    dis_data=(pData[9]<<8)+pData[8];  //�ϳ�����   
	    temp=(uint16)((float)dis_data*3.9);  //�������ݺ���ʾ,�鿼ADXL345�������ŵ�4ҳ
	if(dis_data<0){
	dis_data=-dis_data;
        HalLcdWriteStringValue("Y: -",temp,10,1);
	}
	else
    {
        HalLcdWriteStringValue("Y:  ",temp,10,1);
    }
    dis_data=(pData[11]<<8)+pData[10];    //�ϳ�����  
    temp=(uint16)((float)dis_data*3.9);  //�������ݺ���ʾ,�鿼ADXL345�������ŵ�4ҳ
	if(dis_data<0){
	dis_data=-dis_data;
        HalLcdWriteStringValue("Z: -",temp,10,2);
	}
	else
    {
        HalLcdWriteStringValue("Z:  ",temp,10,2);
    }
}
