/*
 * Func.h
 *
 *  Created on: May 15, 2022
 *      Author: wysji
 */

#include "stdio.h"
#include "math.h"
#include "debug.h"
#include "stdlib.h"
#include <string.h>
#include "ch32v30x.h"
#include "lcd.h"
//#include <font.h>
#include "ch32v30x_rng.h"



/* Global define */
#define w 10         //搜索路径。由于代码中矩阵做了变形处理，因此该处搜索路径小于实际算法计算方法中的搜索路径值。
#define max_w 10     //最大DTW搜索路径，用于创建一个足够大的数组
#define max_L 100    //最长模板长度，用于创建一个足够大的数组
#define PeakDeep 5   //峰值区间赋权重深度

#define PsoNum 10   //类PSO算法候选模板个数
#define PsoTrainNum 10//类PSO算法中模板匹配样本的个数



#define WaveTime_Ms 750    //LCD显示波形的延时时间，毫秒单位

// Template Info Address 存储模板信息的FLASH地址，掉电不清除
#define AddrTemplateWrittenDone ((uint32_t)0x0802A000)
#define AddrTemplate3 ((uint32_t)0x0802C000)
#define AddrTemplate4 ((uint32_t)0x0802D000)
#define AddrTemplate6 ((uint32_t)0x0802E000)
#define AddrTemplate7 ((uint32_t)0x0802F000)

#define AddrTemplate3_size ((uint32_t)0x0802A002)
#define AddrTemplate4_size ((uint32_t)0x0802A004)
#define AddrTemplate6_size ((uint32_t)0x0802A006)
#define AddrTemplate7_size ((uint32_t)0x0802A008)

#define AddrTemplate3_Peak ((uint32_t)0x0802A010)
#define AddrTemplate4_Peak ((uint32_t)0x0802A014)
#define AddrTemplate6_Peak ((uint32_t)0x0802A018)
#define AddrTemplate7_Peak ((uint32_t)0x0802A01C)

// LCD control
#define  LCD_DATA        ((u32)0x60020000)
// LCD显示范围
#define Area_x_Start 90
#define Area_x_End 150
#define Area_y_Start 75
#define Area_y_End 165

// 光纤光亮灭
#define LaserOn 1
#define LaserOff 0

/* Flash Control Register bits */
#define CR_PG_Set                  ((uint32_t)0x00000001)
#define CR_PG_Reset                ((uint32_t)0x00001FFE)

/* Global Variant */


/* Global typedef */
typedef struct
{
    u16 *p_data;            //指向模板数据的指针，给他分配连续的内存地址
    u16 sz;                 //记录模板长度
    u16 peak;                //峰值区间中点坐标。目前代码不使用，但保留该值。

}TemplateData;

typedef struct
{
    u16 *p_data;       //指向样本数据的指针，给他分配连续的内存地址
    u16 sz;            //记录样本长度
}Template;


// LCD 主菜单的主选项宏定义
enum LcdChoice
{
    Drop = 0,
    ParameterMod = 1,
    TemplateTraining,   //2
    Waveform,           //3
    ChooseTemplate,     //4
    RunDtw,             //5
    StateBuffer = 10
};

// 按键选择功能宏定义。四个选项对应四个不同的按键，并由中断程序来将按键键值写入到按键寄存器KeyValue内
enum SwChoice
{
    Confirm = 0,
    Up,         //1
    Down,       //2
    Back,       //3
};



// 硬件函数
void ADC_Function_Init(void);
void GPIOPB0_INIT(void);
void GPIOPB1_INIT(void);
void DMA1_Init( DMA_Channel_TypeDef* DMA_CHx, u32 ppadr,
        u32 memadr, u16 bufsize);
void USART1_Init(uint32_t baudrate);
void TIM6_Init( u16 arr, u16 psc);
void TimInterrupt_Init(void);
void TIM6_IRQHandler(void)   __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA1Interrupt_Init(void);
void DMA1_Channel1_IRQHandler(void)   __attribute__((interrupt("WCH-Interrupt-fast")));
void EXTI0_INT_INIT(void);
void EXTI1_INT_INIT(void);
void EXTI2_INT_INIT(void);
void EXTI4_INT_INIT(void);
void EXTI5_INT_INIT(void);
void EXTI0_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void EXTI1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void EXTI2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void EXTI4_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void EXTI9_5_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

// 功能函数
u16 GetThreshHoldVolt(void);
void GetTemplate(Template* Template_x,u16 x, u16 y);
void TemplateSave(u32 Addr, u16* data, u16 size_TemplateSave);
void fasttraining(Template* Template_Sample,u8 num_temp);
void train(Template* Template_Sample,u8 num_temp);
void DrawCurve(u16 *p_data,u16 size);
void DrawMenu(u8 command1,u8 command2);


