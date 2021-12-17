#include "adc.h"
#include "delay.h"
//#include "stm32f10x.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"



/*****************************************************************
* 2021/08/21 15:20
*
* �ԻҶȴ�������ADC���ú���
* 5V����
* �����ذ��ADC1�ӵ��Ҷȴ�������out�ӿڣ���ȡ�������ģ����
* ��ADCת��Ϊ����������������
* ���ֲ��֪���轫PC0����ΪADC1
*
*****************************************************************************
*
*          �Ҷȷֲ����
*               ǰ                    
*    __________________________
*    |     1     |     2      | 
*    |           |            |        
*    |    PB6    |    PB10    | 
* �� |___________|____________| ��
*    |     3     |     4      | 
*    |           |            |  
*    |    PB3    |    PB4     | 
*    |������������������������������������������������|                                
*                ��            
*******************************************************************************/

void adc_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;//����GPIO�ṹ��
	ADC_InitTypeDef ADC_InitStruct;//����ADC�ṹ��

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);//GPIOCʱ��ʹ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);//ADC1ʱ��ʹ��
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);  //����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M

	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AIN;//ģ������
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_0;
	GPIO_Init(GPIOC,&GPIO_InitStruct);//��ʼ���ṹ��
	
	ADC_DeInit(ADC1);  ////��λADC1,������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ
	
	ADC_InitStruct.ADC_ContinuousConvMode=DISABLE;//������ת��ģʽ
	ADC_InitStruct.ADC_DataAlign=ADC_DataAlign_Right;//�Ҷ���ģʽ
	ADC_InitStruct.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;//�������ⲿ����ת��
	ADC_InitStruct.ADC_Mode=ADC_Mode_Independent;//����ģʽ
	ADC_InitStruct.ADC_NbrOfChannel=1;//ת��ͨ����Ŀ
	ADC_InitStruct.ADC_ScanConvMode=DISABLE;//������ɨ��
	ADC_Init(ADC1,&ADC_InitStruct);//��ʼ���ṹ��
	
	ADC_Cmd( ADC1,ENABLE);//ʹ��ADC1
	
	ADC_ResetCalibration(ADC1); //ʹ�ܸ�λУ׼
	
	while(ADC_GetResetCalibrationStatus(ADC1));  //�ȴ���λУ׼����
	
	ADC_StartCalibration(ADC1); //��ʼADCУ׼
	
	while(ADC_GetCalibrationStatus(ADC1));  //�ȴ�У׼����
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������
}


u16 get_adc(void)  //��ȡADC1_CH1��ֵ
{
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_1Cycles5 );	//ADC1,ADCͨ��,����ʱ��Ϊ239.5����	  			    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������

	return ADC_GetConversionValue(ADC1);	//�������һ��ADC1�������ת�����
	
}


u16 get_adc_avr(void)  //�Ի�ȡ����ADC1_CH1ȡƽ��ֵ
{
	u32 avr_value=0;
	u8 avr_t;
	for(avr_t=0;avr_t<10;avr_t++)
	{
		avr_value += get_adc();
		delay_ms(5);
		
	}
	return avr_value / 10;
}


u16 get_adc_avr_avr(void)  //�Լ����ƽ��ֵ��ȥһ��ƽ��ֵ�Լ�С���
{
	u32 avr_avr_value=0;
	u8 avr_avr_t;
	for(avr_avr_t=0;avr_avr_t<10;avr_avr_t++)
	{
		avr_avr_value += get_adc_avr();
		delay_ms(5);
	}
	return avr_avr_value/10;
	
}


