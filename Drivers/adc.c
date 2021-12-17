#include "adc.h"
#include "delay.h"
//#include "stm32f10x.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"



/*****************************************************************
* 2021/08/21 15:20
*
* 对灰度传感器的ADC配置函数
* 5V供电
* 将主控板的ADC1接到灰度传感器的out接口，读取其输出的模拟量
* 经ADC转换为数字量供函数触发
* 由手册可知，需将PC0复用为ADC1
*
*****************************************************************************
*
*          灰度分布编号
*               前                    
*    __________________________
*    |     1     |     2      | 
*    |           |            |        
*    |    PB6    |    PB10    | 
* 左 |___________|____________| 右
*    |     3     |     4      | 
*    |           |            |  
*    |    PB3    |    PB4     | 
*    |————————————————————————|                                
*                后            
*******************************************************************************/

void adc_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;//声明GPIO结构体
	ADC_InitTypeDef ADC_InitStruct;//声明ADC结构体

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);//GPIOC时钟使能
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);//ADC1时钟使能
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);  //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M

	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AIN;//模拟输入
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_0;
	GPIO_Init(GPIOC,&GPIO_InitStruct);//初始化结构体
	
	ADC_DeInit(ADC1);  ////复位ADC1,将外设 ADC1 的全部寄存器重设为缺省值
	
	ADC_InitStruct.ADC_ContinuousConvMode=DISABLE;//不连续转换模式
	ADC_InitStruct.ADC_DataAlign=ADC_DataAlign_Right;//右对齐模式
	ADC_InitStruct.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;//不开启外部触发转换
	ADC_InitStruct.ADC_Mode=ADC_Mode_Independent;//独立模式
	ADC_InitStruct.ADC_NbrOfChannel=1;//转换通道数目
	ADC_InitStruct.ADC_ScanConvMode=DISABLE;//不开启扫描
	ADC_Init(ADC1,&ADC_InitStruct);//初始化结构体
	
	ADC_Cmd( ADC1,ENABLE);//使能ADC1
	
	ADC_ResetCalibration(ADC1); //使能复位校准
	
	while(ADC_GetResetCalibrationStatus(ADC1));  //等待复位校准结束
	
	ADC_StartCalibration(ADC1); //开始ADC校准
	
	while(ADC_GetCalibrationStatus(ADC1));  //等待校准结束
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能
}


u16 get_adc(void)  //获取ADC1_CH1的值
{
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_1Cycles5 );	//ADC1,ADC通道,采样时间为239.5周期	  			    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束

	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
	
}


u16 get_adc_avr(void)  //对获取到的ADC1_CH1取平均值
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


u16 get_adc_avr_avr(void)  //对计算的平均值再去一次平均值以减小误差
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


