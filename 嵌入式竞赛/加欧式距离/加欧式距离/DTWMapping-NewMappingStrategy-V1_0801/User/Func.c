/*
 * Func.c
 *
 *  Created on: May 15, 2022
 *      Author: wysji
 */

#include "Func.h"

extern volatile u16 DifData;
extern volatile uint16_t DMAProcessFlag;

extern volatile u16 *Buffer;
extern volatile u8 Flag;


extern volatile u8 SampleNum;             //用于粗采集的样本需求个数
extern volatile u8 SampleTrainNum;        //用于训练出模板的样本需求个数
extern volatile u16 Thresh;                 //用于判定有效信息起始的阈值数值
extern volatile u16 std_thresh;
extern volatile u8 TemplateWanted;

extern volatile u16 ThreshVolt;     //定义临界电压值
extern volatile float fac_Odist;
extern volatile float fac_dtw;
volatile u16 size;

// LCD画图，将ADC曲线在LCD上画出。 整体归一化在0-200这个范围
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
    // 如果此数据长度过长或不符合条件，则丢弃不绘制
    else {

        lcd_clear(BLACK);
        lcd_show_string(40, 80, 24, "Drop This One");
    }
}


// LCD 显示主菜单
// 搭配按键中断，可以实现菜单选择功能
void DrawMenu(u8 command1,u8 command2)
{
    u8 command = command1 *10 + command2;
    lcd_clear(BLACK);
    switch(command)
    {
    case 10:
        lcd_show_string(50, 20, 32, "Main Menu");
        lcd_show_string(80, 60, 16, "ParameterModify");
        lcd_show_string(80, 100, 16, "TemplateTraining");
        lcd_show_string(80, 140, 16, "ClearTemplate");
        lcd_show_string(80, 180, 16, "ChooseTemplate");
        lcd_show_string(80, 220, 16, "RunDtw");
        lcd_draw_circle(45, 68, 8);
        break;

    case 11:
        lcd_show_string(40, 80, 24, "SampleNum");
        lcd_show_string(100, 160, 32, "%d",SampleNum);
        break;
    case 12:
        lcd_show_string(20, 80, 24, "SampleTrainNum");
        lcd_show_string(100, 160, 32, "%d",SampleTrainNum);
        break;
    case 13:
        lcd_show_string(40, 80, 24, "Thresh");
        lcd_show_string(100, 160, 32, "%d",Thresh);
        break;
    case 16:
        lcd_show_string(40, 80, 24, "std_thresh");
        lcd_show_string(100, 160, 32, "%d",std_thresh);
        break;
    case 14:
        lcd_show_string(40, 80, 24, "fac_dtw");
        lcd_show_string(100, 160, 32, "%.2f",fac_dtw);
        break;
    case 15:
        lcd_show_string(40, 80, 24, "fac_Odist");
        lcd_show_string(100, 160, 32, "%.2f",fac_Odist);
        break;
    case 20:
        lcd_show_string(50, 20, 32, "Main Menu");
        lcd_show_string(80, 60, 16, "ParameterModify");
        lcd_show_string(80, 100, 16, "TemplateTraining");
        lcd_show_string(80, 140, 16, "ClearTemplate");
        lcd_show_string(80, 180, 16, "ChooseTemplate");
        lcd_show_string(80, 220, 16, "RunDtw");
        lcd_draw_circle(45, 108, 8);
        break;

    case 21:
        lcd_show_string(50, 20, 32, "Training");
        lcd_show_string(80, 86, 16, "Fast Mode");
        lcd_show_string(80, 152, 16, "Normal Mode");
        lcd_draw_circle(45, 94, 8);
        break;

    case 22:
        lcd_show_string(50, 20, 32, "Training");
        lcd_show_string(80, 86, 16, "Fast Mode");
        lcd_show_string(80, 152, 16, "Normal Mode");
        lcd_draw_circle(45, 160, 8);
        break;


    case 30:
        lcd_show_string(50, 20, 32, "Main Menu");
        lcd_show_string(80, 60, 16, "ParameterModify");
        lcd_show_string(80, 100, 16, "TemplateTraining");
        lcd_show_string(80, 140, 16, "ClearTemplate");
        lcd_show_string(80, 180, 16, "ChooseTemplate");
        lcd_show_string(80, 220, 16, "RunDtw");
        lcd_draw_circle(45, 148, 8);
        break;

    case 40:
        lcd_show_string(50, 20, 32, "Main Menu");
        lcd_show_string(80, 60, 16, "ParameterModify");
        lcd_show_string(80, 100, 16, "TemplateTraining");
        lcd_show_string(80, 140, 16, "ClearTemplate");
        lcd_show_string(80, 180, 16, "ChooseTemplate");
        lcd_show_string(80, 220, 16, "RunDtw");
        lcd_draw_circle(45, 188, 8);
        break;

    case 41:
        lcd_show_string(20, 80, 24, "TemplateWanted");
        lcd_show_string(100, 160, 32, "%d",TemplateWanted);
        break;

    case 50:
        lcd_show_string(50, 20, 32, "Main Menu");
        lcd_show_string(80, 60, 16, "ParameterModify");
        lcd_show_string(80, 100, 16, "TemplateTraining");
        lcd_show_string(80, 140, 16, "ClearTemplate");
        lcd_show_string(80, 180, 16, "ChooseTemplate");
        lcd_show_string(80, 220, 16, "RunDtw");
        lcd_draw_circle(45, 228, 8);
        break;

    default:
        break;
    }

}


// 获得导轨的ADC数据
u16 GetThreshHoldVolt(void)
{
    u8 k =0;
    u16 sum =0;
    while(k<16) // 16个点平均数据
    {
        if(1 == DMAProcessFlag)
        {
        sum += DifData;
        DMAProcessFlag = 0;
        k++;
        }
    }

    return (sum>>4);

}

// 得到一个样本的有用信息
void GetTemplate(Template* Template_x,u16 x, u16 y)
{
    u16 *p = Template_x->p_data;
    size = 0;
    static u8 UnusefulFlag = 0;
    Flag =0;

    u8 k =0;
    while(0 == Flag)    //用于探测到第一个临界点，有效信息的开始
    {
        if(1 == DMAProcessFlag)
        {
            DMAProcessFlag =0;
            if(abs(DifData-x) <= y)
            {
                Buffer[k] = DifData;    //多存取10个数据点。在检测到数据起始后，多取前5位
                if(k == 9)
                {
                    k=0;
                }
                else {
                    k++;
                }
            }
            else
            {
                if(k>=5)
                {
                    for(u8 i=0; i<5;i++)
                    {
                        p[size++] =  Buffer[k-5+i];
                    }

                }
                else if(k ==4)
                {
                    p[size++] =  Buffer[9];
                    p[size++] =  Buffer[0];
                    p[size++] =  Buffer[1];
                    p[size++] =  Buffer[2];
                    p[size++] =  Buffer[3];
                }
                else if(k ==3)
                {
                    p[size++] =  Buffer[8];
                    p[size++] =  Buffer[9];
                    p[size++] =  Buffer[0];
                    p[size++] =  Buffer[1];
                    p[size++] =  Buffer[2];
                }
                else if(k ==2)
                {
                    p[size++] =  Buffer[7];
                    p[size++] =  Buffer[8];
                    p[size++] =  Buffer[9];
                    p[size++] =  Buffer[0];
                    p[size++] =  Buffer[1];
                }
                else if(k == 1)
                {

                    p[size++] =  Buffer[6];
                    p[size++] =  Buffer[7];
                    p[size++] =  Buffer[8];
                    p[size++] =  Buffer[9];
                    p[size++] =  Buffer[0];
                }
//                else if(k ==0)
                else
                {
                    p[size++] =  Buffer[5];
                    p[size++] =  Buffer[6];
                    p[size++] =  Buffer[7];
                    p[size++] =  Buffer[8];
                    p[size++] =  Buffer[9];

                }
                p[size++] =  DifData;
                Flag =1;
            }
        }
    }
    Flag = 0;

    while(0 == Flag)
    {
        if(1 == DMAProcessFlag)
        {
            DMAProcessFlag =0;
            if(abs(DifData-x) <= y)
            {
                p[size++] = DifData;
                UnusefulFlag++;
                if(UnusefulFlag >=30)   //当连续30个点都在导轨数据的范围内时，判定样本已经通过
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
        Template_x->sz = size - 23 ;
    }
    else {
        Template_x->sz = 0;
    }

    return;
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

//快速训练模式
void fasttraining(Template* Template_Sample)
{

    extern uint32_t AddrTemp[4];
    extern uint32_t AddrTempSize[4];
    extern uint32_t AddrTempPeak[4];

    u16 peak_pos = 0;
    u16 max_data =0;
    u8 j_1;
    u8 section_leftpos;
    u8 section_rightpos;


    for(u8 num_temp = 0; num_temp<4; )
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

            //保存该次样本的数据长度
            peak_pos = 0;    //记录峰值位置
             max_data = 0;      //记录最大值

             //该循环为了获取峰值位置
             for( j_1=0; j_1< Template_Sample->sz; j_1++)
             {
                 if(Template_Sample->p_data[j_1] > max_data)
                 {
                     peak_pos = j_1 ;
                     max_data = Template_Sample->p_data[j_1];
                 }
             }

              section_leftpos = peak_pos;
              section_rightpos = peak_pos;
              while((Template_Sample->p_data[section_leftpos] >= (max_data - std_thresh)) && max_data > std_thresh)
              {
                  section_leftpos --;
              }

              while((Template_Sample->p_data[section_rightpos] >= (max_data - std_thresh)) && max_data > std_thresh)
              {
                  section_rightpos ++;
              }
              //平均得到峰值邻域区间的中间点的坐标
              peak_pos = (section_leftpos + section_rightpos) >> 1;

//            TIM_Cmd(TIM6,DISABLE);       //定时器
            //保存该次样本的数据长度
            DrawCurve(Template_Sample->p_data,Template_Sample->sz);
            Delay_Ms(500);

            TemplateSave(AddrTemp[num_temp],Template_Sample->p_data,
                    Template_Sample->sz);
            TemplateSave(AddrTempSize[num_temp],&(Template_Sample->sz),1);
            TemplateSave(AddrTempPeak[num_temp],&(peak_pos),1);

//            TIM_Cmd(TIM6,ENABLE);       //使能定时器
            num_temp++;
        }
     }

}

// 训练模板。该函数包括粗采集，用于判断选择模板的长度以及峰值区间的位置。
//并根据此重新采集多组数据来均一化得到一个最终模板
void train(Template* Template_Sample)
{
    u8 jj=0;
    static u8* size_temp = NULL;
    size_temp = (u8 *)malloc(SampleNum);

    static u8* peak_pos = NULL;
    peak_pos = (u8 *)malloc(SampleNum);

    static u8* range = NULL;
    range = (u8 *)malloc(SampleNum);

    static u8* valid_peak_pos = NULL;
    valid_peak_pos = (u8 *)malloc(SampleNum);


    static u16 max_data = 0;
    static u8 j_1 = 0;
    static u8 ii =0;
    static u8 iii =0;
    static u8 rangemax = 0;
    static u8 TemplateSize = 0;
    static u16 TemplatePeakPos = 0;
    static u8 k=0;
    static u8 p_maxpos =0;
    static u8 section_leftpos = 0;
    static u8 section_rightpos = 0;
    static u8 section_midpos = 0;
    static u8 kk=0;
    static u8 dif=0;
    extern uint32_t AddrTemp[4];
    extern uint32_t AddrTempSize[4];
    extern uint32_t AddrTempPeak[4];

    static u16 lcd_data[150] = {0};
    static u16 lcd_cnt = 0;

    static TemplateData* TemplateDataSample[4];

    for(k=0; k<4;k++)
    {
        TemplateDataSample[k] = (TemplateData* )malloc(6);
        TemplateDataSample[k]->p_data = NULL;
        TemplateDataSample[k]->sz = 0;
    }

    for(u8 num_temp=0;num_temp<4;num_temp++)
    {

        //粗采集，目的是为了先采集若干个样本的数据，找到模板长度、峰值位置的众数等。最终为了得到TemplateSize，TemplatePeakPos。
         for( jj=0; jj<SampleNum; )
         {
             lcd_clear(BLACK);
             lcd_show_string(20,80, 16, "Train No.%d Template!",num_temp);
             lcd_show_string(20,140, 16, "Please place Sample");
             lcd_show_string(20,200, 16, "Remaining:%d",SampleNum - jj);
             //采集样本信息
            GetTemplate(Template_Sample,ThreshVolt, Thresh);

            //如果该样本的数据点个数在200个以内
            if(Template_Sample->sz != 0)
            {
 //               TIM_Cmd(TIM6,DISABLE);       //定时器
                //保存该次样本的数据长度
                size_temp[jj] = Template_Sample->sz;
                peak_pos[jj] =0;    //记录峰值位置
                max_data = 0;      //记录最大值

                 //该循环为了获取峰值位置
                 for( j_1=0; j_1< Template_Sample->sz; j_1++)
                 {
                     if(Template_Sample->p_data[j_1] > max_data)
                     {
                         peak_pos[jj] = j_1 ;
                         max_data = Template_Sample->p_data[j_1];
                     }
                 }

              section_leftpos = peak_pos[jj];
              section_rightpos = peak_pos[jj];
              while((Template_Sample->p_data[section_leftpos] >= (max_data - std_thresh)) && max_data > std_thresh)
              {
                  section_leftpos --;
              }

              while((Template_Sample->p_data[section_rightpos] >= (max_data - std_thresh)) && max_data > std_thresh)
              {
                  section_rightpos ++;
              }
              //平均得到峰值邻域区间的中间点的坐标
              peak_pos[jj] = (section_leftpos + section_rightpos) >> 1;

                 // 将本次样本数据通过串口发送给计算机方便判断样本可靠性
                for(u16 cnt =0; cnt<Template_Sample->sz; cnt++)
                {
                   /*****************************/
                   while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //等待上次发送 结束
                   USART1->DATAR = Template_Sample->p_data[cnt] ;
                   while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //等待上次发送 结束
                   USART1->DATAR = (Template_Sample->p_data[cnt] >>8);
                   /*****************************/
                }
                DrawCurve(Template_Sample->p_data,Template_Sample->sz);
                Delay_Ms(500);
//                TIM_Cmd(TIM6,ENABLE);       //使能定时器
                jj++;
            }

         }

    //

         lcd_clear(BLACK);
         lcd_show_string(20,40, 16, "Rough Training Done");
         lcd_show_string(20,80, 16, "Starting Fine Tune");
         lcd_show_string(20,120, 16, "Train No.%d Template!",num_temp);
         lcd_show_string(20,160, 16, "Please place Sample");
         lcd_show_string(20,200, 16, "Remaining:%d",SampleTrainNum);
    /**************************************************************/
    /*获取了若干个样本数据后，自适应固定模板的长度，以及峰值左右邻域在模板的位置****/
    /**************************************************************/
 //        TIM_Cmd(TIM6,DISABLE);       //失能定时器
         //在范围为4的区间内，找到若干个模板的长度的众数
         for(ii =0; ii<SampleNum; ii++)
         {
             for(iii=0;iii<SampleNum;iii++)
             {
                 if( abs(size_temp[ii] - size_temp[iii]) <=4 )
                 {
                     range[ii]++;
                 }
             }
         }
        rangemax = 0;
         for(ii = 0; ii<SampleNum;ii++)
         {
             if(range[ii] > rangemax)
             {
                 rangemax = range[ii];
                 TemplateSize = size_temp[ii];
             }
         }
    ///////////////////////////////////////////////////

             // 同理，找到峰值位置的众数
             for(ii = 0; ii<SampleNum; ii++)
             {
                 //舍弃掉数据点过多或过少的样本
                 if(abs(size_temp[ii] - TemplateSize) <=4)
                 {
                     valid_peak_pos[ii] = peak_pos[ii];
                 }
             }

             for(ii = 0; ii<SampleNum; ii++)
             {
                 range[ii] = 0;
                 if(valid_peak_pos[ii] != 0)
                 {
                     for(iii = 0; iii<SampleNum; iii++)
                     {
                         if(abs(valid_peak_pos[ii] - valid_peak_pos[iii]) < 4)
                         {
                             range[ii]++;
                         }
                     }
                 }
             }

             rangemax = 0;
             for(ii = 0; ii<SampleNum;ii++)
             {
                 if(range[ii] > rangemax)
                 {
                     rangemax = range[ii];
                     TemplatePeakPos = valid_peak_pos[ii]; // TemplatePeakPos:峰值区间中点位置坐标
                 }
             }

    /**************************************************************/
    /**************************************************************/
    /**************************************************************/


             //在固定模板长度及峰值邻域位置后，给模板数据划分内存空间
             TemplateDataSample[num_temp]->p_data = (u16* )malloc( TemplateSize* 2);
             TemplateDataSample[num_temp]->sz = TemplateSize;
             //先赋初值是为了后面做加和操作方便
         for( k=0;k< TemplateSize; k++)
          {
              TemplateDataSample[num_temp]->p_data[k] = 0;
          }

 //        TIM_Cmd(TIM6,ENABLE);       //使能定时器

         //重新以固定的模板长度采集若干个样本
for( jj=0; jj<SampleTrainNum;)
{
             GetTemplate(Template_Sample,ThreshVolt, Thresh);
 //            TIM_Cmd(TIM6,DISABLE);       //失能定时器

             //重置LCD需要采集的数据和计数

             lcd_cnt = 0;
             for(k=0; k< 120; k++)
             {
                 lcd_data[k] = 0;
             }


//样本数据点个数要接近
if( abs(Template_Sample->sz - TemplateSize) < 10)
{

      //获取峰值位置
      p_maxpos =0;
      max_data = 0;
      for( j_1=0; j_1< Template_Sample->sz; j_1++)
      {
          if(Template_Sample->p_data[j_1] > max_data)
          {
              p_maxpos = j_1 ;
              max_data = Template_Sample->p_data[j_1];
          }
      }

      //获取峰值位置左右邻域的坐标
      section_leftpos = p_maxpos;
      section_rightpos = p_maxpos;
      while(Template_Sample->p_data[section_leftpos] >= (max_data - std_thresh))
      {
          section_leftpos --;
      }


      while(Template_Sample->p_data[section_rightpos] >= (max_data - std_thresh))
      {
          section_rightpos ++;
      }
      //平均得到峰值邻域区间的中间点的坐标
      section_midpos = (section_leftpos + section_rightpos) >> 1;

      //判定在这个样本下如何选择有效的信息（固定最大值位置的前提下）
      //由于样本长度可能稍大或稍小标准模板长度，所以我们从section_midpos点开始，左右取固定个数点
      //多的点舍弃，少点的情况，我们补点，补点的值在理想情况下会接近 （ThreshVolt +- 200)
      if( abs(section_midpos - TemplatePeakPos) <= 5  )
     {
          kk =0;
          jj++;

          //分情况来获得一段有效数据
         if(Template_Sample->sz >= TemplateSize)
         {
              if(section_midpos >= TemplatePeakPos)
              {
                  for( k=(section_midpos - TemplatePeakPos); k< section_midpos +1; k++)
                  {
                      lcd_data[lcd_cnt++] = Template_Sample->p_data[k];
                      TemplateDataSample[num_temp]->p_data[kk++] +=   Template_Sample->p_data[k];
                      /*****************************///串口发送功能可以将数据传输给上位机更好地检查调试
                      while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //等待上次发送 结束
                      USART1->DATAR = Template_Sample->p_data[k];
                      while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //等待上次发送 结束
                      USART1->DATAR = Template_Sample->p_data[k] >>8;
                      /*****************************/
                  }

                  if((Template_Sample->sz - section_midpos) >= (TemplateSize - TemplatePeakPos))
                  {
                      for(k=section_midpos +1; k < TemplateSize+section_midpos - TemplatePeakPos; k++ )
                      {
                          lcd_data[lcd_cnt++] = Template_Sample->p_data[k];
                          TemplateDataSample[num_temp]->p_data[kk++] +=   Template_Sample->p_data[k];
                          /*****************************/
                          while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //等待上次发送 结束
                          USART1->DATAR = Template_Sample->p_data[k];
                          while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //等待上次发送 结束
                          USART1->DATAR = Template_Sample->p_data[k] >>8;
                          /*****************************/
                      }

                  }
                  else {
                     dif = (TemplateSize - TemplatePeakPos) - (Template_Sample->sz - section_midpos);
                     for(k=section_midpos +1; k < Template_Sample->sz ; k++ )
                      {
                          lcd_data[lcd_cnt++] = Template_Sample->p_data[k];
                          TemplateDataSample[num_temp]->p_data[kk++] +=   Template_Sample->p_data[k];
                          /*****************************/
                          while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //等待上次发送 结束
                          USART1->DATAR = Template_Sample->p_data[k];
                          while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //等待上次发送 结束
                          USART1->DATAR = Template_Sample->p_data[k] >>8;
                          /*****************************/
                      }

                     for(k= 0; k<dif;k++)
                     {
                         lcd_data[lcd_cnt++] = ThreshVolt;
                         TemplateDataSample[num_temp]->p_data[kk++] +=   ThreshVolt;
                         /*****************************/
                         while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //等待上次发送 结束
                         USART1->DATAR = ThreshVolt;
                         while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //等待上次发送 结束
                         USART1->DATAR = ThreshVolt >>8;
                         /*****************************/
                     }



                 }
              }

              // else: section_midpos < TemplatePeakPos
              else {
                  dif = TemplatePeakPos - section_midpos;
                  for(k = 0; k<dif; k++)
                  {
                      lcd_data[lcd_cnt++] = ThreshVolt;
                      TemplateDataSample[num_temp]->p_data[kk++] +=   ThreshVolt;
                      /*****************************/
                      while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //等待上次发送 结束
                      USART1->DATAR = ThreshVolt;
                      while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //等待上次发送 结束
                      USART1->DATAR = ThreshVolt >>8;
                      /*****************************/
                  }
                  for(k = 0; k< section_midpos+1; k++)
                  {
                      lcd_data[lcd_cnt++] = Template_Sample->p_data[k];
                      TemplateDataSample[num_temp]->p_data[kk++] +=   Template_Sample->p_data[k];
                      /*****************************/
                      while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //等待上次发送 结束
                      USART1->DATAR = Template_Sample->p_data[k];
                      while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //等待上次发送 结束
                      USART1->DATAR = Template_Sample->p_data[k] >>8;
                      /*****************************/
                  }

                  for(k = section_midpos+1; k< TemplateSize - dif; k++)
                  {
                      lcd_data[lcd_cnt++] = Template_Sample->p_data[k];
                      TemplateDataSample[num_temp]->p_data[kk++] +=   Template_Sample->p_data[k];
                      /*****************************/
                      while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //等待上次发送 结束
                      USART1->DATAR = Template_Sample->p_data[k];
                      while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //等待上次发送 结束
                      USART1->DATAR = Template_Sample->p_data[k] >>8;
                      /*****************************/
                  }
             }

         }

         // else: Template_Sample->sz < TemplateSize
         else
         {

             // left part of section_midpos
             if(section_midpos >= TemplatePeakPos)
             {
                 for( k=(section_midpos - TemplatePeakPos); k< section_midpos +1; k++)
                  {
                      lcd_data[lcd_cnt++] = Template_Sample->p_data[k];
                      TemplateDataSample[num_temp]->p_data[kk++] +=   Template_Sample->p_data[k];
                      /*****************************/
                      while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //等待上次发送 结束
                      USART1->DATAR = Template_Sample->p_data[k];
                      while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //等待上次发送 结束
                      USART1->DATAR = Template_Sample->p_data[k] >>8;
                      /*****************************/
                  }

             }
             // else: section_midpos < TemplatePeakPos
             else
             {
                 dif = TemplatePeakPos - section_midpos;

                 for(k= 0; k<dif;k++)
                     {
                         lcd_data[lcd_cnt++] = ThreshVolt;
                         TemplateDataSample[num_temp]->p_data[kk++] +=   ThreshVolt;
                         /*****************************/
                         while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //等待上次发送 结束
                         USART1->DATAR = ThreshVolt;
                         while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //等待上次发送 结束
                         USART1->DATAR = ThreshVolt >>8;
                         /*****************************/
                     }

                 for(k = 0; k< section_midpos+1; k++)
                  {
                      lcd_data[lcd_cnt++] = Template_Sample->p_data[k];
                      TemplateDataSample[num_temp]->p_data[kk++] +=   Template_Sample->p_data[k];
                      /*****************************/
                      while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //等待上次发送 结束
                      USART1->DATAR = Template_Sample->p_data[k];
                      while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //等待上次发送 结束
                      USART1->DATAR = Template_Sample->p_data[k] >>8;
                      /*****************************/
                  }


             }

             // right part of section_midpos
             if((Template_Sample->sz - section_midpos) >= (TemplateSize - TemplatePeakPos))
             {

                 for(k=section_midpos +1; k < TemplateSize+section_midpos - TemplatePeakPos; k++ )
                      {
                          lcd_data[lcd_cnt++] = Template_Sample->p_data[k];
                          TemplateDataSample[num_temp]->p_data[kk++] +=   Template_Sample->p_data[k];
                          /*****************************/
                          while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //等待上次发送 结束
                          USART1->DATAR = Template_Sample->p_data[k];
                          while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //等待上次发送 结束
                          USART1->DATAR = Template_Sample->p_data[k] >>8;
                          /*****************************/
                      }

             }
             // else: (Template_Sample->sz - section_midpos) < (TemplateSize - TemplatePeakPos)
             else
             {
                 dif = (TemplateSize - TemplatePeakPos) - (Template_Sample->sz - section_midpos);
                     for(k=section_midpos +1; k < Template_Sample->sz ; k++ )
                      {
                          lcd_data[lcd_cnt++] = Template_Sample->p_data[k];
                          TemplateDataSample[num_temp]->p_data[kk++] +=   Template_Sample->p_data[k];
                          /*****************************/
                          while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //等待上次发送 结束
                          USART1->DATAR = Template_Sample->p_data[k];
                          while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //等待上次发送 结束
                          USART1->DATAR = Template_Sample->p_data[k] >>8;
                          /*****************************/
                      }

                     for(k= 0; k<dif;k++)
                     {
                         lcd_data[lcd_cnt++] = ThreshVolt;
                         TemplateDataSample[num_temp]->p_data[kk++] +=   ThreshVolt;
                         /*****************************/
                         while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //等待上次发送 结束
                         USART1->DATAR = ThreshVolt;
                         while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //等待上次发送 结束
                         USART1->DATAR = ThreshVolt >>8;
                         /*****************************/
                     }
             }

         }

     }

}
//             TIM_Cmd(TIM6,ENABLE);       //使能定时器
             DrawCurve(lcd_data,lcd_cnt);
             lcd_show_string(20,210, 16, "Remaining:%d",SampleTrainNum - jj);

}


         //获取到若干个样本数据之后，做一个平均值，得到最后的用于匹配的模板。

//         TIM_Cmd(TIM6,DISABLE);       //失能定时器

         for( k=0;k< TemplateSize; k++)
          {
            TemplateDataSample[num_temp]->p_data[k] = TemplateDataSample[num_temp]->p_data[k]/SampleTrainNum;
          }

         //StdThreshhold用来表示DTW匹配的阈值，低于这个值，就表示匹配样本一致。
         //factor表示系数，需要根据实际测试调整。不同的姿态样本可以设置不同的系数
         //TemplateDataSample[num_temp]->StdThreshhold =  sum_temp;

         //将模板数据保存到固定flash地址。 重启之后就能直接把模板从flash读到ram里
         TemplateSave(AddrTemp[num_temp],TemplateDataSample[num_temp]->p_data,TemplateSize);
         TemplateSave(AddrTempSize[num_temp],&(TemplateDataSample[num_temp]->sz),1);
         TemplateSave(AddrTempPeak[num_temp],&TemplatePeakPos,1);



            for(u16 cnt =0; cnt<TemplateSize; cnt++)
            {
               /*****************************/
               while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //等待上次发送 结束
               USART1->DATAR = (u8)*(__IO uint16_t *)(AddrTemp[num_temp] + 2*cnt) ;
               while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //等待上次发送 结束
               USART1->DATAR = (u8)((*(__IO uint16_t *)(AddrTemp[num_temp] + 2*cnt)) >>8);
               /*****************************/
            }
            DrawCurve(TemplateDataSample[num_temp]->p_data,TemplateDataSample[num_temp]->sz);
            lcd_show_string(40, 20, 24, "Template-%d Curve",num_temp);
            Delay_Ms(1000);
            lcd_clear(BLACK);
            lcd_show_string(10, 120, 16, "Template-%d Training Done",num_temp);
            Delay_Ms(1000);

            free(TemplateDataSample[num_temp]->p_data);
            TemplateDataSample[num_temp]->p_data = NULL;
            free(TemplateDataSample[num_temp]);
            TemplateDataSample[num_temp] = NULL;

//            TIM_Cmd(TIM6,ENABLE);       //使能定时器
    }

    free(size_temp);
    size_temp = NULL;

    free(peak_pos);
    peak_pos = NULL;

    free(range);
    range = NULL;

    free(valid_peak_pos);
    valid_peak_pos = NULL;

}

