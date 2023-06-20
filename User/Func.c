/*
 * Func.c
 *
 *  Created on: May 15, 2022
 *      Author: wysji
 */

#include "Func.h"

extern volatile u16 DifData;
extern volatile uint16_t DMAProcessFlag;
extern float fac_dtw;
extern float fac_Odist;
extern volatile u16 Thresh;
extern volatile u16 JudgeThresh;
extern volatile u8 TemplateWanted;
extern volatile u16 ThreshVolt;     //定义临界电压值
// 模板数据在FLASH内存储的位置
extern uint32_t AddrTemp[4];
extern uint32_t AddrTempSize[4];
extern uint32_t AddrTempPeak[4];
extern u16 D[max_L][2*max_w +1];
extern u32 DTW[max_L][2*max_w +1];

volatile u8 Flag =0;    // 标志位，GetTemplate函数使用


// LCD画图，将ADC曲线在LCD上画出。 整体归一化在0-200这个范围
/*******************************************************************************
* Function Name  : DrawCurve
* Description    : LCD画图，将ADC曲线在LCD上画出。 整体归一化在0-200这个范围
* Input          : *p_data: 数据指针
*                  SIZE: 数据点个数.
*
* Return         : None
*******************************************************************************/
void DrawCurve(u16 *p_data,u16 SIZE)
{
    if(SIZE != 0)
    {
        u16 k=0;
        u16 x1=0;
        u16 x2=0;
        u16 y1=0;
        u16 y2=0;

        u16 x_offset = 0;   //X轴起始位置
        u16 y_offset = 209; //y轴起始位置，实际位置在 (240 - y_offset) 处
        u16 y_max = 0;

        //先找到最大值，然后整体归一化
        for(k=0;k<SIZE;k++)
        {
            if(p_data[k] > y_max)
            {
                y_max = p_data[k];
            }

        }

        lcd_clear(BLACK);

        //如果点数少，将波形展宽尽可能占满LCD屏幕
        if(SIZE < 80)
        {
            for(k=1; k< SIZE; k++)
            {
                x1 = x_offset + 3*k -3;
                y1 = y_offset - (((float)p_data[k-1] /(float)y_max) * 199);

                x2 = x_offset + 3*k;
                y2 = y_offset - (((float)p_data[k] /(float)y_max) * 199);

                lcd_draw_line(x1,y1,x2,y2);

            }
        }
        else if(SIZE < 160) {
            for(k=1; k< SIZE; k++)
            {
                x1 = x_offset + 2*k -2;
                y1 = y_offset - (((float)p_data[k-1] /(float)y_max) * 199);

                x2 = x_offset + 2*k;
                y2 = y_offset - (((float)p_data[k] /(float)y_max) * 199);

                lcd_draw_line(x1,y1,x2,y2);

            }
        }
        else {
            for(k=1; k< SIZE; k++)
            {
                x1 = x_offset + k -1;
                y1 = y_offset - (((float)p_data[k-1] /(float)y_max) * 199);

                x2 = x_offset + k;
                y2 = y_offset - (((float)p_data[k] /(float)y_max) * 199);

                lcd_draw_line(x1,y1,x2,y2);

            }
        }



        lcd_show_string(0, 0, 16, "Template Size: %d",SIZE);
    }
    // 如果此数据长度过长或不符合条件，则丢弃不绘制，即size = 0的情况
    else {

        lcd_clear(BLACK);
        lcd_show_string(40, 80, 24, "Drop This One");
    }
}


/*******************************************************************************
* Function Name  : DrawMenu
* Description    : LCD 显示主菜单，搭配按键中断，可以实现菜单选择功能
* Input          : command1: LcdScreen的键值
*                  command1: LcdScreenSubOption的键值
*
* Return         : None
*******************************************************************************/
void DrawMenu(u8 command1,u8 command2)
{
    u8 command = command1 *16 + command2;
    lcd_clear(BLACK);
    switch(command)
    {
    case 0x10:
        lcd_show_string(50, 20, 32, "Main Menu");
        lcd_show_string(80, 60, 16, "ParameterModify");
        lcd_show_string(80, 100, 16, "TemplateTraining");
        lcd_show_string(80, 140, 16, "Waveform");
        lcd_show_string(80, 180, 16, "ChooseTemplate");
        lcd_show_string(80, 220, 16, "RunDtw");
        lcd_draw_circle(45, 68, 8);
        break;

    case 0x11:
        lcd_show_string(40, 80, 24, "fac_dtw");
        lcd_show_string(100, 160, 32, "%d.%2.d",(u8)(fac_dtw),(u8)(100*fac_dtw));
        break;
    case 0x12:
        lcd_show_string(20, 80, 24, "fac_Odist");
        lcd_show_string(100, 160, 32,"%d.%2.d",(u8)(fac_Odist),(u8)(100*fac_Odist));
        break;
    case 0x13:
        lcd_show_string(40, 80, 24, "Thresh");
        lcd_show_string(100, 160, 32, "%d",Thresh);
        break;
    case 0x14:
        lcd_show_string(40, 80, 24, "JudgeThresh");
        lcd_show_string(100, 160, 32, "%d",JudgeThresh);
        break;
    case 0x15:
        lcd_show_string(50, 80, 32, "w of DTW");
        lcd_show_string(100, 140, 32, "%d",w);
        lcd_show_string(40, 200, 24, "Range: 5 to %d",max_w);
        break;
    case 0x20:
        lcd_show_string(50, 20, 32, "Main Menu");
        lcd_show_string(80, 60, 16, "ParameterModify");
        lcd_show_string(80, 100, 16, "TemplateTraining");
        lcd_show_string(80, 140, 16, "Waveform");
        lcd_show_string(80, 180, 16, "ChooseTemplate");
        lcd_show_string(80, 220, 16, "RunDtw");
        lcd_draw_circle(45, 108, 8);
        break;

    case 0x21:
        lcd_show_string(50, 20, 32, "Training");
        lcd_show_string(80, 86, 16, "Fast Mode");
        lcd_show_string(80, 152, 16, "Normal Mode");
        lcd_draw_circle(45, 94, 8);
        break;

    case 0x22:
        lcd_show_string(50, 20, 32, "Training");
        lcd_show_string(80, 86, 16, "Fast Mode");
        lcd_show_string(80, 152, 16, "Normal Mode");
        lcd_draw_circle(45, 160, 8);
        break;

    case 0x23:
        lcd_show_string(50, 20, 32, "Normal Mode");
        lcd_show_string(80, 80, 16, "Template-0");
        lcd_show_string(80, 120, 16, "Template-1");
        lcd_show_string(80, 160, 16, "Template-2");
        lcd_show_string(80, 200, 16, "Template-3");
        lcd_draw_circle(45, 88, 8);
        break;

    case 0x24:
        lcd_show_string(50, 20, 32, "Normal Mode");
        lcd_show_string(80, 80, 16, "Template-0");
        lcd_show_string(80, 120, 16, "Template-1");
        lcd_show_string(80, 160, 16, "Template-2");
        lcd_show_string(80, 200, 16, "Template-3");
        lcd_draw_circle(45, 128, 8);
        break;

    case 0x25:
        lcd_show_string(50, 20, 32, "Normal Mode");
        lcd_show_string(80, 80, 16, "Template-0");
        lcd_show_string(80, 120, 16, "Template-1");
        lcd_show_string(80, 160, 16, "Template-2");
        lcd_show_string(80, 200, 16, "Template-3");
        lcd_draw_circle(45, 168, 8);
        break;

    case 0x26:
        lcd_show_string(50, 20, 32, "Normal Mode");
        lcd_show_string(80, 80, 16, "Template-0");
        lcd_show_string(80, 120, 16, "Template-1");
        lcd_show_string(80, 160, 16, "Template-2");
        lcd_show_string(80, 200, 16, "Template-3");
        lcd_draw_circle(45, 208, 8);
        break;

    case 0x27:
        lcd_show_string(50, 20, 32, "Fast Mode");
        lcd_show_string(80, 80, 16, "Template-0");
        lcd_show_string(80, 120, 16, "Template-1");
        lcd_show_string(80, 160, 16, "Template-2");
        lcd_show_string(80, 200, 16, "Template-3");
        lcd_draw_circle(45, 88, 8);
        break;

    case 0x28:
        lcd_show_string(50, 20, 32, "Fast Mode");
        lcd_show_string(80, 80, 16, "Template-0");
        lcd_show_string(80, 120, 16, "Template-1");
        lcd_show_string(80, 160, 16, "Template-2");
        lcd_show_string(80, 200, 16, "Template-3");
        lcd_draw_circle(45, 128, 8);
        break;

    case 0x29:
        lcd_show_string(50, 20, 32, "Fast Mode");
        lcd_show_string(80, 80, 16, "Template-0");
        lcd_show_string(80, 120, 16, "Template-1");
        lcd_show_string(80, 160, 16, "Template-2");
        lcd_show_string(80, 200, 16, "Template-3");
        lcd_draw_circle(45, 168, 8);
        break;

    case 0x2a:
        lcd_show_string(50, 20, 32, "Fast Mode");
        lcd_show_string(80, 80, 16, "Template-0");
        lcd_show_string(80, 120, 16, "Template-1");
        lcd_show_string(80, 160, 16, "Template-2");
        lcd_show_string(80, 200, 16, "Template-3");
        lcd_draw_circle(45, 208, 8);
        break;


    case 0x30:
        lcd_show_string(50, 20, 32, "Main Menu");
        lcd_show_string(80, 60, 16, "ParameterModify");
        lcd_show_string(80, 100, 16, "TemplateTraining");
        lcd_show_string(80, 140, 16, "Waveform");
        lcd_show_string(80, 180, 16, "ChooseTemplate");
        lcd_show_string(80, 220, 16, "RunDtw");
        lcd_draw_circle(45, 148, 8);
        break;

    case 0x31:
        lcd_show_string(50, 20, 32, "Waveform");
        lcd_show_string(80, 80, 16, "Template-0");
        lcd_show_string(80, 120, 16, "Template-1");
        lcd_show_string(80, 160, 16, "Template-2");
        lcd_show_string(80, 200, 16, "Template-3");
        lcd_draw_circle(45, 88, 8);
        break;

    case 0x32:
        lcd_show_string(50, 20, 32, "Waveform");
        lcd_show_string(80, 80, 16, "Template-0");
        lcd_show_string(80, 120, 16, "Template-1");
        lcd_show_string(80, 160, 16, "Template-2");
        lcd_show_string(80, 200, 16, "Template-3");
        lcd_draw_circle(45, 128, 8);
        break;

    case 0x33:
        lcd_show_string(50, 20, 32, "Waveform");
        lcd_show_string(80, 80, 16, "Template-0");
        lcd_show_string(80, 120, 16, "Template-1");
        lcd_show_string(80, 160, 16, "Template-2");
        lcd_show_string(80, 200, 16, "Template-3");
        lcd_draw_circle(45, 168, 8);
        break;

    case 0x34:
        lcd_show_string(50, 20, 32, "Waveform");
        lcd_show_string(80, 80, 16, "Template-0");
        lcd_show_string(80, 120, 16, "Template-1");
        lcd_show_string(80, 160, 16, "Template-2");
        lcd_show_string(80, 200, 16, "Template-3");
        lcd_draw_circle(45, 208, 8);
        break;

    case 0x40:
        lcd_show_string(50, 20, 32, "Main Menu");
        lcd_show_string(80, 60, 16, "ParameterModify");
        lcd_show_string(80, 100, 16, "TemplateTraining");
        lcd_show_string(80, 140, 16, "Waveform");
        lcd_show_string(80, 180, 16, "ChooseTemplate");
        lcd_show_string(80, 220, 16, "RunDtw");
        lcd_draw_circle(45, 188, 8);
        break;

    case 0x41:
        lcd_show_string(20, 80, 24, "TemplateWanted");
        lcd_show_string(100, 160, 32, "%d",TemplateWanted);
        break;

    case 0x50:
        lcd_show_string(50, 20, 32, "Main Menu");
        lcd_show_string(80, 60, 16, "ParameterModify");
        lcd_show_string(80, 100, 16, "TemplateTraining");
        lcd_show_string(80, 140, 16, "Waveform");
        lcd_show_string(80, 180, 16, "ChooseTemplate");
        lcd_show_string(80, 220, 16, "RunDtw");
        lcd_draw_circle(45, 228, 8);
        break;

    default:
        break;
    }

}


/*******************************************************************************
* Function Name  : GetThreshHoldVolt
* Description    : 获取导轨光传感值
* Input          : 无
*
* Return         : 导轨光传感值
*******************************************************************************/
u16 GetThreshHoldVolt(void)
{
    u8 k =0;
    u32 sum =0;
    while(k<255) // 256个点平均数据
    {
        if(1 == DMAProcessFlag)
        {
        sum += DifData;
        DMAProcessFlag = 0;
        k++;
        }
    }

    return (sum>>8);

}


/*******************************************************************************
* Function Name  : GetTemplate
* Description    : 得到一个样本的有用信息
* Input          : Template_x: 结构体指针，用于缓存有效数据。最终一个样本的数据都会被保存在这里
*                   x: ThreshVolt
*                   y: Thresh，阈值参数
*
* Return         : None
*******************************************************************************/
void GetTemplate(Template* Template_x,u16 x, u16 y)
{
    u16 *p = Template_x->p_data;
    u16 size = 0;
    static u8 UnusefulFlag = 0; //标志位
    Flag =0;

    u16 Buffer[10] = {0};   //10个点的缓存区

    static u8 cnt = 0;
    volatile u8 p_buffer = 0;

    //第一个while循环用于探测有效信息的开始
    while(0 == Flag)
    {
        if(1 == DMAProcessFlag)
        {
            DMAProcessFlag =0;
            Buffer[p_buffer] = DifData;

            if(abs(DifData-x) > y)
            {
                cnt++;
                if(cnt >= 4)        //连续出现4个超出导轨光信号阈值的数据，判定样本正在通过。此时将之前缓存的数据也写入到Template_x中
                {
                    Flag = 1;
                    if(p_buffer>=8)
                    {
                        p_buffer = p_buffer - 8;
                        for(u8 i_buffer = 0; i_buffer< 9; i_buffer++)
                        {
                            p[size++] = Buffer[p_buffer++];
                        }
                    }
                    else
                    {
                        p_buffer = p_buffer + 2;
                        for(u8 i_buffer = 0; i_buffer< 9; i_buffer++)
                        {
                            p[size++] = Buffer[p_buffer++];
                            if(p_buffer >= 10)
                            {
                                p_buffer = 0;
                            }
                        }
                    }
                }

            }
            else
            {
                cnt = 0;
            }

            if(p_buffer >= 9)
            {
                p_buffer=0;
            }
            else {
                p_buffer++;
            }

        }
    }
    Flag = 0;

    //此while循环用于判定有效信息的结束，即样本已经通过
    while(0 == Flag)
    {
        if(1 == DMAProcessFlag)
        {
            DMAProcessFlag =0;
            if(abs(DifData-x) <= y)
            {
                p[size++] = DifData;
                UnusefulFlag++;
                if(UnusefulFlag >=30)   //当连续30个点都在导轨数据的范围内时，判定样本已经通过。注意此时已经多缓存了30个点
                {
                    Flag =1;
                }
            }
            else {
                p[size++] = DifData;
                UnusefulFlag = 0;
            }

            if(size>=200)   //限制有效数据的长度，不能超过200个，否则后续计算能力难以处理
            {

                Template_x->sz = 0;
                return ;
            }
        }
    }

    if(size >=50)   //数据长度太短也意味着可能发生了错误
    {
        Template_x->sz = size - 23 ;    //将多缓存的30个点中，减去后23个点，只保留最末段的7个点。
    }
    else {
        Template_x->sz = 0;
    }
    p =NULL;

}

// FLASH 保存模板数据。将数据保存到指定地址
void TemplateSave(u32 Addr, u16* data, u16 size_TemplateSave)
{
    FLASH_Unlock();
    FLASH_Status status = FLASH_COMPLETE;

    FLASH->CTLR |= CR_PG_Set;  //解锁flash写入

    for(u8 i =0; i< size_TemplateSave; i++)
    {

            *(__IO uint16_t *)Addr = data[i];
            status = FLASH_WaitForLastOperation(0x00005000);
            while(status != FLASH_COMPLETE)
            {
                status = FLASH_WaitForLastOperation(0x00005000);
            }
            Addr = Addr + 2 ;
    }

    FLASH->CTLR &= CR_PG_Reset; // 关闭flash写入
    FLASH_Lock();
}

/*******************************************************************************
* Function Name  : fasttraining
* Description    : 快速训练。其实就是只采一个样本，并将该样本作为模板
* Input          : Template_Sample: 结构体指针，用于缓存有效数据。最终一个样本的数据都会被保存在这里
*                   num_temp: 表示要训练的模板号
*
* Return         : None
*******************************************************************************/
void fasttraining(Template* Template_Sample,u8 num_temp)
{
    ThreshVolt = GetThreshHoldVolt();

    u8 flag_fasttraining =0;
    u16 peak =0;
    u32 Area = 0;
    u32 max_area =0;

    while(0 == flag_fasttraining)
    {
         lcd_clear(BLACK);
         lcd_show_string(50,20, 32, "Fast Mode");
         lcd_show_string(20,80, 16, "Train No.%d Template!",num_temp);
         lcd_show_string(20,140, 16, "Please place Sample");
         //采集样本信息
        GetTemplate(Template_Sample,ThreshVolt, Thresh);

        //如果该样本的数据点个数在200个以内
        if(Template_Sample->sz != 0)
        {
            flag_fasttraining = 1;
            peak = 5;
            max_area =0;
            //这段代码是为了在领域为5个单位的区间内，找到围场面积最大的一个区间，作为欧式距离匹配最重要的部分。目前代码不使用，但保留，且存储在FLASH中
            for(u8 i=5; i<Template_Sample->sz-1-5; i++)
            {
                Area = 0;
                for(u8 j= i-5; j< i+6;j++)
                {
                    Area+= Template_Sample->p_data[j];
                }
                if(Area > max_area)
                {
                    peak = i;
                    max_area = Area;
                }
            }
            //保存该次样本的数据长度
            DrawCurve(Template_Sample->p_data,Template_Sample->sz);
            Delay_Ms(500);

            //将数据写入FLASH地址
            TemplateSave(AddrTemp[num_temp],Template_Sample->p_data,
                    Template_Sample->sz);
            TemplateSave(AddrTempSize[num_temp],&(Template_Sample->sz),1);
            TemplateSave(AddrTempPeak[num_temp],&(peak),1);
        }
    }

}


/*******************************************************************************
* Function Name  : train
* Description    : 标准训练。选取若干个候选模板，然后用样本对他进行DTW匹配计算表征值，再给各个候选模板打分，此处是前5低的模板加1分。最后找到分数最高的那个
* Input          : Template_Sample: 结构体指针，用于缓存有效数据。最终一个样本的数据都会被保存在这里
*                   num_temp: 表示要训练的模板号
*
* Bug            : 该代码有BUG，运行完一次之后，可能是由于没有成功释放内存，导致如果再运行一次训练代码，会造成单片机死机。目前还没有发现问题的根源。
* Return         : None
*******************************************************************************/
void train(Template* Template_Sample,u8 num_temp)   // num_temp是模板标号
{
    ThreshVolt = GetThreshHoldVolt();
    u8 k=0;
    u16 PsoBuffer[200] = {0};           //存储有效信息的数组，作为匹配数组，通过窗口移动的方式去和候选模板匹配计算DTW表征值
    u8 PsoBestDtwCnt[PsoNum] = {0};     //用于存储每个候选模板得分分数，分数越高，代表该模板越好
    u16 peakpos[PsoNum] = {0};          //记录围场面积最大的那个区间的中点坐标。目前代码不使用，但保留，且存储在FLASH中
    u32 DtwOrder[PsoNum] = {0};         //用于存储DTW局部最优值的数组
    u8 p_start = 0;                     //指针偏移位

    u32 min_dtw = 0xFFFFFFFF;
    u32 LocalDtw = 0xFFFFFFFF;          //局部最优
    u32 GlobalDtw = 0xFFFFFFFF;         //全局最优
    u8 DtwBestNum = 99;

    //参数
    u32 MedistanceValue;
    u32 Finaldist;

    u8 i_dtw =0;
    u8 j_dtw = 0;
    u8 k_dtw = 0;

    u8 peak =0;
    u32 Area = 0;
    u32 max_area =0;

    //声明结构体变量，用来存储数据
    Template* PsoData[PsoNum];

    for(k=0; k<PsoNum;k++)      //分配内存空间
    {
        PsoData[k] = (Template* )malloc(4);
        PsoData[k]->p_data = NULL;
        PsoData[k]->sz = 0;

        PsoBestDtwCnt[PsoNum] = 0;
    }

    //比较数据缓存区，前50个有效数据都设置成导轨光强信号
    for(k=0;k<50;k++)
    {
        PsoBuffer[k] = ThreshVolt;
    }


    //收集多次模板数据并保存，将这些收集到的模板作为候选模板
    for(k=0; k< PsoNum; )
    {
        lcd_clear(BLACK);
        lcd_show_string(20,80, 16, "Train No.%d Template!",num_temp);
        lcd_show_string(20,140, 16, "Please place Sample");
        lcd_show_string(20,200, 16, "Remaining:%d",PsoNum - k);
        //获取模板
        GetTemplate(Template_Sample,ThreshVolt, Thresh);

        if(Template_Sample->sz != 0)
        {
            PsoData[k]->sz = Template_Sample->sz;
            PsoData[k]->p_data = (u16 *)malloc(2*(PsoData[k]->sz));
            if(PsoData[k]->p_data == NULL)  //检查是否分配内存失败
            {
                //LCD显示错误
                lcd_clear(BLACK);
                lcd_show_string(20,80, 16, "RAM Not Enough!");
                return;
            }
            //内存分配成功，将Template_Sample里存储的数据复制到PsoData数组里。
            //并且找到围场面积最大的一个区间，作为欧式距离匹配最重要的部分。目前代码不使用，但保留，且存储在FLASH中
            else
            {
                for(u8 cnt_t=0; cnt_t < PsoData[k]->sz; cnt_t++)
                {
                    PsoData[k]->p_data[cnt_t] = Template_Sample->p_data[cnt_t];
                }

                peak = 5;
                max_area =0;
                for(u8 i=5; i<Template_Sample->sz-1-5; i++)
                {
                    Area = 0;
                    for(u8 j= i-5; j< i+6;j++)
                    {
                        Area+= Template_Sample->p_data[j];
                    }
                    if(Area > max_area)
                    {
                        peak = i;
                        max_area = Area;
                    }
                }
                peakpos[k] = peak;


                //将读取的数据在LCD屏幕上显示出波形来
                DrawCurve(PsoData[k]->p_data,PsoData[k]->sz);
                Delay_Ms(WaveTime_Ms);

            }
            k++;
        }
    }

    //获取候选模板之后，继续通过若干个样本的数据来计算DTW表征值，以此来寻找最优的模板
    for(k=0; k< PsoTrainNum; k++)
    {
        lcd_clear(BLACK);
        lcd_show_string(20,80, 16, "Train No.%d Template!",num_temp);
        lcd_show_string(20,140, 16, "Please place Sample");
        lcd_show_string(20,200, 16, "Remaining:%d",PsoTrainNum - k);
        //获取模板
        GetTemplate(Template_Sample,ThreshVolt, Thresh);

        if(Template_Sample->sz != 0)
        {
            for(u8 kk=0;kk<Template_Sample->sz;kk++)
            {
                //将有效信息数据复制到匹配数组中，前50位作为冗余位，存储的是导轨光信号数据
                PsoBuffer[kk+50] = Template_Sample->p_data[kk];
            }

            //末尾再添加50个导轨光信号数据，提高冗余度
            for(u8 kk=0;kk<50;kk++)
            {
                PsoBuffer[50+Template_Sample->sz + kk] = ThreshVolt;
            }
        }

        GlobalDtw = 0xFFFFFFFF;
        //对每一个储存的局部模板进行匹配，找出最优的那个模板
        for(u8 kk=0; kk<PsoNum; kk++)
        {
            LocalDtw = 0xFFFFFFFF;
            p_start = 0;
            //DTW匹配范围
            while(p_start <= (Template_Sample->sz + 100 - PsoData[kk]->sz))
            {

                // 开始计算距离矩阵D
                for (i_dtw = 0; i_dtw < w; i_dtw++)
                {
                    k_dtw = 0;
                    for (j_dtw = (w  - i_dtw); j_dtw < 2 * w +1; j_dtw++)
                    {
                        D[i_dtw][j_dtw] = abs(PsoData[kk]->p_data[i_dtw] - PsoBuffer[k_dtw+p_start]);
                        k_dtw++;
                    }

                }
                // middle of D
                for (i_dtw = w; i_dtw < (PsoData[kk]->sz - w); i_dtw++)
                {

                    k_dtw = i_dtw - w;
                    for (j_dtw = 0; j_dtw < 2 * w + 1; j_dtw++)
                    {
                        D[i_dtw][j_dtw] = abs(PsoData[kk]->p_data[i_dtw] - PsoBuffer[k_dtw+p_start]);
                        k_dtw++;
                    }
                }

                // bottom of D
                for (i_dtw = (PsoData[kk]->sz - w); i_dtw < PsoData[kk]->sz; i_dtw++)
                {
                    k_dtw = i_dtw - w;

                    for (j_dtw = 0; j_dtw < (PsoData[kk]->sz - (i_dtw-w)); j_dtw++)
                    {
                        D[i_dtw][j_dtw] = abs(PsoData[kk]->p_data[i_dtw] - PsoBuffer[k_dtw+p_start]);
                        k_dtw++;
                    }

                }

                //根据距离矩阵D来计算DTW
                for (j_dtw = 0; j_dtw < w + 1; j_dtw++)
                {
                    DTW[0][j_dtw] = D[0][j_dtw];
                }

                for (j_dtw = w + 1; j_dtw < 2 * w + 1; j_dtw++)
                {

                    DTW[0][j_dtw] = D[0][j_dtw] + DTW[0][j_dtw - 1];
                }

                for (i_dtw = 1; i_dtw < PsoData[kk]->sz; i_dtw++)
                {
                        min_dtw = DTW[i_dtw - 1][0] < DTW[i_dtw - 1][1] ? DTW[i_dtw - 1][0] : DTW[i_dtw - 1][1];
                        DTW[i_dtw][0] = D[i_dtw][0] + min_dtw;
                        for (j_dtw = 1; j_dtw < 2 * w; j_dtw++)
                        {

                            min_dtw = DTW[i_dtw][j_dtw - 1];
                            if (DTW[i_dtw - 1][j_dtw] < min_dtw)
                            {
                                min_dtw = DTW[i_dtw - 1][j_dtw];
                            }
                            if(DTW[i_dtw - 1][j_dtw+1] < min_dtw)
                            {
                                min_dtw = DTW[i_dtw - 1][j_dtw+1];
                            }
                            DTW[i_dtw][j_dtw] = D[i_dtw][j_dtw] + min_dtw;
                        }

                        min_dtw = DTW[i_dtw][2*w-1] < DTW[i_dtw - 1][2*w] ? DTW[i_dtw][2 * w-1] : DTW[i_dtw - 1][2*w];
                        DTW[i_dtw][2 * w] = D[i_dtw][2 * w] + min_dtw;

                }

                MedistanceValue = 0;
                //计算欧式距离
                for(u8 cnt_k=0; cnt_k<PsoData[kk]->sz; cnt_k++)
                {
                    MedistanceValue += abs(PsoData[kk]->p_data[cnt_k] - PsoBuffer[cnt_k + p_start]);
                }
                //可调整参数    dtw 0.75   欧式距离0.25
                Finaldist = fac_dtw * (float)(DTW[PsoData[kk]->sz-1][w])/((float)(PsoData[kk]->sz))
                    + fac_Odist * ((float)MedistanceValue/(float)(PsoData[kk]->sz));

                if(Finaldist < LocalDtw )
                {
                    LocalDtw = Finaldist;
                }

                p_start++;
            }   //一个模板全部匹配完，找到该模板的局部最优DTW

            DtwOrder[kk] = LocalDtw;

        }   //所有模板全部匹配完之后，给DtwOrder排序，前5小的模板加分

        //通过排序，来找到前5个最优候选模板，并加分
        for(u8 n=0;n<5;n++)
        {
            GlobalDtw = DtwOrder[0];
            DtwBestNum = 0;
            for(u8 jj=0; jj<PsoNum;jj++)
            {
                if(DtwOrder[jj]< GlobalDtw)
                {
                    GlobalDtw = DtwOrder[jj];
                    DtwBestNum = jj;
                }
            }
            DtwOrder[DtwBestNum] = 0xFFFFFFFF;
            PsoBestDtwCnt[DtwBestNum]++;
        }

        lcd_clear(BLACK);
        lcd_show_string(20,80, 16, "Round-%d Mid DTW Value",k);
        lcd_show_string(100,140, 32, "%d",GlobalDtw);
        Delay_Ms(WaveTime_Ms);


    }//若干个样本之后.找到Cnt累计最多的那个模板，即把它作为模板使用

    u8 nn=0;
    u8 max_cnt = PsoBestDtwCnt[0];
    for(k=1;k<PsoNum;k++)
    {
        if(PsoBestDtwCnt[k] > max_cnt)
        {
            nn = k;
            max_cnt = PsoBestDtwCnt[k];
        }

    }

    lcd_clear(BLACK);
    lcd_show_string(40,140, 24, "Best Matching %d/%d",max_cnt,PsoNum);
    Delay_Ms(WaveTime_Ms);

    TemplateSave(AddrTemp[num_temp],PsoData[nn]->p_data,
                    PsoData[nn]->sz);
    TemplateSave(AddrTempSize[num_temp],&(PsoData[nn]->sz),1);
    TemplateSave(AddrTempPeak[num_temp],&(peakpos[nn]),1);

    for(k=0; k<PsoNum;k++)      //分配内存空间
    {
        free(PsoData[k]->p_data);
        PsoData[k]->p_data = NULL;
        free(PsoData[k]);
        PsoData[k] = NULL;
    }

}
