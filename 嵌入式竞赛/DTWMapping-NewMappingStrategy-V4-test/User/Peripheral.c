/*
 * Peripheral.c
 *
 *  Created on: Jun 22, 2022
 *      Author: wysji
 */

#include "Func.h"


extern volatile uint16_t LaserFlag; // ȫ�ֱ���,�������ⷢ��or������
extern volatile uint16_t TIMProcessFlag;//ȫ�ֱ����������Ƿ�����ִ����һ�ζ�ʱ�����жϺ���
extern volatile u16 *p_AdcDmaData;

extern volatile u16 DataLaserOn;
extern volatile u16 DataLaserOff;
extern volatile u16 DifData;
extern volatile uint16_t DMAProcessFlag;
extern volatile u8 offset_p;
extern volatile u16 *DataStream;
extern volatile u8 KeyValue;


extern volatile u8 LcdScreen;
extern volatile u8 DtwRun;

u16 i =0      ;
u16 s   =0    ;
volatile u16 s_dif =0  ;
u8 j      =0  ;

u8 dma_cnt = 0;

/********************************************************************
* �� �� ��       : ADC_Function_Init
* ��������    : ��ʼ��ADC
* ��    ��          : ��
* ��    ��          : ��
********************************************************************/
void ADC_Function_Init(void)
{
    ADC_InitTypeDef ADC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE );
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);               //��ʼ��ADCʱ�ӣ�����ʱ��ΪPCLK2��8��Ƶ�����ʱ��Ϊ14MHz

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);          //����PA1��ΪAD�����

    ADC_DeInit(ADC1);
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;  //����ADģʽΪ����ģʽ��ֻʹ��ADC1
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;       //���ö�ͨ��ģʽ�����õ�ͨ��ģʽ
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;  //��������ת��ģʽ
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; //�������ⲿ����Դ�������������
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;  //�����Ҷ���
    ADC_InitStructure.ADC_NbrOfChannel = 1;     //Ҫת��ͨ������
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_DMACmd(ADC1, ENABLE); //����DMA
    ADC_Cmd(ADC1, ENABLE);          //ʹ��ADC
    Delay_Ms(10);
    ADC_BufferCmd(ADC1, DISABLE);   //disable buffer
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));

    ADC_BufferCmd(ADC1, ENABLE);   //enable buffer
}


void USART1_Init(uint32_t baudrate)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx;

    USART_Init(USART1, &USART_InitStructure);

//    DMA_Cmd(DMA1_Channel4, ENABLE); /* USART1 Tx */

    USART_Cmd(USART1, ENABLE);

}

void DMA1_Init( DMA_Channel_TypeDef* DMA_CHx, u32 ppadr,
        u32 memadr, u16 bufsize)
{

    DMA_InitTypeDef DMA_InitStructure = {0};
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);


    DMA_DeInit(DMA_CHx);
    DMA_InitStructure.DMA_PeripheralBaseAddr = ppadr;       //Դ��ַ
    DMA_InitStructure.DMA_MemoryBaseAddr = memadr;          //Ŀ�ĵ�ַ
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;      //DMA��������赽MEM
    DMA_InitStructure.DMA_BufferSize = bufsize;             //�����С
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;    //�����ַ������
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;             //MEM��ַ������
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //���贫������Ϊ���֣�16λ
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;         //MEM��������Ϊ���֣�16λ
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;           //��ͨģʽ
    DMA_InitStructure.DMA_Priority = DMA_Priority_High; //���ȼ�����Ϊ��
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;            //�ر�MEM��MEM����
    DMA_Init( DMA_CHx, &DMA_InitStructure );

}


void GPIOPB0_INIT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void GPIOPB1_INIT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

}


void TIM6_Init( u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;

    RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM6, ENABLE );

    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit( TIM6, &TIM_TimeBaseInitStructure);

    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
    TIM_ARRPreloadConfig( TIM6, ENABLE );

}

void TimInterrupt_Init(void)
{
   NVIC_InitTypeDef NVIC_InitStructure={0};
   NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn ;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //��ռ���ȼ�
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;        //�����ȼ�
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure);
}

void TIM6_IRQHandler(void)
{
    if(1 == LaserFlag)
    {
        GPIO_WriteBit(GPIOB, GPIO_Pin_0, 1);
        LaserFlag = LaserOff;
    }

    else {
        GPIO_WriteBit(GPIOB, GPIO_Pin_0, 0);    //������
        LaserFlag = LaserOn;
    }

    DMA1_Channel1->CNTR = 26; //����DMAͨ��1�������
    TIMProcessFlag = 1;       //�ñ�־λ
    TIM_ClearFlag(TIM6, TIM_FLAG_Update);//�����־λ
}

void DMA1Interrupt_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure={0};
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //��ռ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;        //�����ȼ�
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

}

// ADCת�����ɸ��жϺ���Ԥ��������
void DMA1_Channel1_IRQHandler(void)
{
    if(1 == LaserFlag)
    {
        s = 0;
        for(i=18; i<26;i++)  // ��һ��ADCת��������ȡ�ض�λ�ò�����ֵ����
        {
            s += p_AdcDmaData[i];
        }
        DataLaserOn = s >>3;
        dma_cnt = dma_cnt | 0x01;
    }
    else {
        s = 0;
        for(i=18; i<26;i++)
        {
            s += p_AdcDmaData[i];
        }
        DataLaserOff = s >>3;
        dma_cnt = dma_cnt | 0x10;
    }

    if(dma_cnt == 0x11)
    {
        s_dif += DataLaserOn - DataLaserOff;    //����ֵõ���Ч�⴫������
        j++;
        if(j == 4)          //ÿ4����Ч�����������ƽ�������õ�һ�����������ݣ���Ϊ��������
        {
            j = 0;
            DifData = (s_dif>>2);
            s_dif =0;
            *(u16 *)(DataStream + offset_p) = DifData;  //д��DataStream��
            offset_p ++;                 //�����ݱ�־λǰ������������256ʱ����ض�����Ϊ0
            DMAProcessFlag = 1;          //�ñ�־λ
        }
        dma_cnt = 0x00;
    }
    DMA_ClearFlag(DMA1_FLAG_TC1);
}


// �������ú���
void EXTI0_INT_INIT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    EXTI_InitTypeDef EXTI_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* GPIOA ----> EXTI_Line0 */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void EXTI1_INT_INIT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    EXTI_InitTypeDef EXTI_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOE, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    /* GPIOE ----> EXTI_Line1 */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource1);
    EXTI_InitStructure.EXTI_Line = EXTI_Line1;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void EXTI2_INT_INIT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    EXTI_InitTypeDef EXTI_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOE, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    /* GPIOE ----> EXTI_Line2 */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource2);
    EXTI_InitStructure.EXTI_Line = EXTI_Line2;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void EXTI4_INT_INIT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    EXTI_InitTypeDef EXTI_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOE, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    /* GPIOE ----> EXTI_Line4 */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource4);
    EXTI_InitStructure.EXTI_Line = EXTI_Line4;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void EXTI5_INT_INIT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    EXTI_InitTypeDef EXTI_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOE, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    /* GPIOE ----> EXTI_Line5 */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource5);
    EXTI_InitStructure.EXTI_Line = EXTI_Line5;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

//�����жϳ���
//���ж���Wake_Up����������������˵�
void EXTI0_IRQHandler(void)
{
    DtwRun = 0;
    LcdScreen = ParameterMod;
    EXTI_ClearFlag(EXTI_Line0); // ���жϱ�־λΪ��

}

void EXTI1_IRQHandler(void)
{
    KeyValue = Up;
    EXTI_ClearFlag(EXTI_Line1); // ���жϱ�־λΪ��
}

void EXTI2_IRQHandler(void)
{
    KeyValue = Down;
    EXTI_ClearFlag(EXTI_Line2); // ���жϱ�־λΪ��
}

void EXTI4_IRQHandler(void)
{
    KeyValue = Back;
    EXTI_ClearFlag(EXTI_Line4); // ���жϱ�־λΪ��
}

void EXTI9_5_IRQHandler(void)
{
    KeyValue = Confirm;
    EXTI_ClearFlag(EXTI_Line5); // ���жϱ�־λΪ��
}
