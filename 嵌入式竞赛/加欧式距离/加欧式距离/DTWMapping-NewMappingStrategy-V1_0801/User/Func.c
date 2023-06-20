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


extern volatile u8 SampleNum;             //���ڴֲɼ��������������
extern volatile u8 SampleTrainNum;        //����ѵ����ģ��������������
extern volatile u16 Thresh;                 //�����ж���Ч��Ϣ��ʼ����ֵ��ֵ
extern volatile u16 std_thresh;
extern volatile u8 TemplateWanted;

extern volatile u16 ThreshVolt;     //�����ٽ��ѹֵ
extern volatile float fac_Odist;
extern volatile float fac_dtw;
volatile u16 size;

// LCD��ͼ����ADC������LCD�ϻ����� �����һ����0-200�����Χ
void DrawCurve(u16 *p_data,u16 SIZE)
{
    if(SIZE != 0)
    {
        u16 k=0;
        u16 x1=0;
        u16 x2=0;
        u16 y1=0;
        u16 y2=0;

        u16 x_offset = 0;   //X����ʼλ��
        u16 y_offset = 209; //y����ʼλ�ã�ʵ��λ���� (240 - y_offset) ��
        u16 y_max = 0;

        //���ҵ����ֵ��Ȼ�������һ��
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
    // ��������ݳ��ȹ����򲻷�������������������
    else {

        lcd_clear(BLACK);
        lcd_show_string(40, 80, 24, "Drop This One");
    }
}


// LCD ��ʾ���˵�
// ���䰴���жϣ�����ʵ�ֲ˵�ѡ����
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


// ��õ����ADC����
u16 GetThreshHoldVolt(void)
{
    u8 k =0;
    u16 sum =0;
    while(k<16) // 16����ƽ������
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

// �õ�һ��������������Ϣ
void GetTemplate(Template* Template_x,u16 x, u16 y)
{
    u16 *p = Template_x->p_data;
    size = 0;
    static u8 UnusefulFlag = 0;
    Flag =0;

    u8 k =0;
    while(0 == Flag)    //����̽�⵽��һ���ٽ�㣬��Ч��Ϣ�Ŀ�ʼ
    {
        if(1 == DMAProcessFlag)
        {
            DMAProcessFlag =0;
            if(abs(DifData-x) <= y)
            {
                Buffer[k] = DifData;    //���ȡ10�����ݵ㡣�ڼ�⵽������ʼ�󣬶�ȡǰ5λ
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
                if(UnusefulFlag >=30)   //������30���㶼�ڵ������ݵķ�Χ��ʱ���ж������Ѿ�ͨ��
                {
                    Flag =1;
                }
            }
            else {
                p[size++] = DifData;
                UnusefulFlag = 0;
            }

            if(size>=200)   //������Ч���ݵĳ��ȣ����ܳ���200����������������������Դ���
            {

                Template_x->sz = 0;
                return ;
            }
        }
    }

    if(size >=50)   //���ݳ���̫��Ҳ��ζ�ſ��ܷ����˴���
    {
        Template_x->sz = size - 23 ;
    }
    else {
        Template_x->sz = 0;
    }

    return;
}

// FLASH ����ģ�����ݡ������ݱ��浽ָ����ַ
void TemplateSave(u32 Addr, u16* data, u16 size_TemplateSave)
{
    FLASH_Unlock();
    FLASH_Status status = FLASH_COMPLETE;

    FLASH->CTLR |= CR_PG_Set;  //����flashд��

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

    FLASH->CTLR &= CR_PG_Reset; // �ر�flashд��
    FLASH_Lock();
}

//����ѵ��ģʽ
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
         //�ɼ�������Ϣ
        GetTemplate(Template_Sample,ThreshVolt, Thresh);

        //��������������ݵ������200������
        if(Template_Sample->sz != 0)
        {

            //����ô����������ݳ���
            peak_pos = 0;    //��¼��ֵλ��
             max_data = 0;      //��¼���ֵ

             //��ѭ��Ϊ�˻�ȡ��ֵλ��
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
              //ƽ���õ���ֵ����������м�������
              peak_pos = (section_leftpos + section_rightpos) >> 1;

//            TIM_Cmd(TIM6,DISABLE);       //��ʱ��
            //����ô����������ݳ���
            DrawCurve(Template_Sample->p_data,Template_Sample->sz);
            Delay_Ms(500);

            TemplateSave(AddrTemp[num_temp],Template_Sample->p_data,
                    Template_Sample->sz);
            TemplateSave(AddrTempSize[num_temp],&(Template_Sample->sz),1);
            TemplateSave(AddrTempPeak[num_temp],&(peak_pos),1);

//            TIM_Cmd(TIM6,ENABLE);       //ʹ�ܶ�ʱ��
            num_temp++;
        }
     }

}

// ѵ��ģ�塣�ú��������ֲɼ��������ж�ѡ��ģ��ĳ����Լ���ֵ�����λ�á�
//�����ݴ����²ɼ�������������һ���õ�һ������ģ��
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

        //�ֲɼ���Ŀ����Ϊ���Ȳɼ����ɸ����������ݣ��ҵ�ģ�峤�ȡ���ֵλ�õ������ȡ�����Ϊ�˵õ�TemplateSize��TemplatePeakPos��
         for( jj=0; jj<SampleNum; )
         {
             lcd_clear(BLACK);
             lcd_show_string(20,80, 16, "Train No.%d Template!",num_temp);
             lcd_show_string(20,140, 16, "Please place Sample");
             lcd_show_string(20,200, 16, "Remaining:%d",SampleNum - jj);
             //�ɼ�������Ϣ
            GetTemplate(Template_Sample,ThreshVolt, Thresh);

            //��������������ݵ������200������
            if(Template_Sample->sz != 0)
            {
 //               TIM_Cmd(TIM6,DISABLE);       //��ʱ��
                //����ô����������ݳ���
                size_temp[jj] = Template_Sample->sz;
                peak_pos[jj] =0;    //��¼��ֵλ��
                max_data = 0;      //��¼���ֵ

                 //��ѭ��Ϊ�˻�ȡ��ֵλ��
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
              //ƽ���õ���ֵ����������м�������
              peak_pos[jj] = (section_leftpos + section_rightpos) >> 1;

                 // ��������������ͨ�����ڷ��͸�����������ж������ɿ���
                for(u16 cnt =0; cnt<Template_Sample->sz; cnt++)
                {
                   /*****************************/
                   while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //�ȴ��ϴη��� ����
                   USART1->DATAR = Template_Sample->p_data[cnt] ;
                   while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //�ȴ��ϴη��� ����
                   USART1->DATAR = (Template_Sample->p_data[cnt] >>8);
                   /*****************************/
                }
                DrawCurve(Template_Sample->p_data,Template_Sample->sz);
                Delay_Ms(500);
//                TIM_Cmd(TIM6,ENABLE);       //ʹ�ܶ�ʱ��
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
    /*��ȡ�����ɸ��������ݺ�����Ӧ�̶�ģ��ĳ��ȣ��Լ���ֵ����������ģ���λ��****/
    /**************************************************************/
 //        TIM_Cmd(TIM6,DISABLE);       //ʧ�ܶ�ʱ��
         //�ڷ�ΧΪ4�������ڣ��ҵ����ɸ�ģ��ĳ��ȵ�����
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

             // ͬ���ҵ���ֵλ�õ�����
             for(ii = 0; ii<SampleNum; ii++)
             {
                 //���������ݵ�������ٵ�����
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
                     TemplatePeakPos = valid_peak_pos[ii]; // TemplatePeakPos:��ֵ�����е�λ������
                 }
             }

    /**************************************************************/
    /**************************************************************/
    /**************************************************************/


             //�ڹ̶�ģ�峤�ȼ���ֵ����λ�ú󣬸�ģ�����ݻ����ڴ�ռ�
             TemplateDataSample[num_temp]->p_data = (u16* )malloc( TemplateSize* 2);
             TemplateDataSample[num_temp]->sz = TemplateSize;
             //�ȸ���ֵ��Ϊ�˺������ӺͲ�������
         for( k=0;k< TemplateSize; k++)
          {
              TemplateDataSample[num_temp]->p_data[k] = 0;
          }

 //        TIM_Cmd(TIM6,ENABLE);       //ʹ�ܶ�ʱ��

         //�����Թ̶���ģ�峤�Ȳɼ����ɸ�����
for( jj=0; jj<SampleTrainNum;)
{
             GetTemplate(Template_Sample,ThreshVolt, Thresh);
 //            TIM_Cmd(TIM6,DISABLE);       //ʧ�ܶ�ʱ��

             //����LCD��Ҫ�ɼ������ݺͼ���

             lcd_cnt = 0;
             for(k=0; k< 120; k++)
             {
                 lcd_data[k] = 0;
             }


//�������ݵ����Ҫ�ӽ�
if( abs(Template_Sample->sz - TemplateSize) < 10)
{

      //��ȡ��ֵλ��
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

      //��ȡ��ֵλ���������������
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
      //ƽ���õ���ֵ����������м�������
      section_midpos = (section_leftpos + section_rightpos) >> 1;

      //�ж���������������ѡ����Ч����Ϣ���̶����ֵλ�õ�ǰ���£�
      //�����������ȿ����Դ����С��׼ģ�峤�ȣ��������Ǵ�section_midpos�㿪ʼ������ȡ�̶�������
      //��ĵ��������ٵ����������ǲ��㣬�����ֵ����������»�ӽ� ��ThreshVolt +- 200)
      if( abs(section_midpos - TemplatePeakPos) <= 5  )
     {
          kk =0;
          jj++;

          //����������һ����Ч����
         if(Template_Sample->sz >= TemplateSize)
         {
              if(section_midpos >= TemplatePeakPos)
              {
                  for( k=(section_midpos - TemplatePeakPos); k< section_midpos +1; k++)
                  {
                      lcd_data[lcd_cnt++] = Template_Sample->p_data[k];
                      TemplateDataSample[num_temp]->p_data[kk++] +=   Template_Sample->p_data[k];
                      /*****************************///���ڷ��͹��ܿ��Խ����ݴ������λ�����õؼ�����
                      while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //�ȴ��ϴη��� ����
                      USART1->DATAR = Template_Sample->p_data[k];
                      while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //�ȴ��ϴη��� ����
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
                          while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //�ȴ��ϴη��� ����
                          USART1->DATAR = Template_Sample->p_data[k];
                          while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //�ȴ��ϴη��� ����
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
                          while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //�ȴ��ϴη��� ����
                          USART1->DATAR = Template_Sample->p_data[k];
                          while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //�ȴ��ϴη��� ����
                          USART1->DATAR = Template_Sample->p_data[k] >>8;
                          /*****************************/
                      }

                     for(k= 0; k<dif;k++)
                     {
                         lcd_data[lcd_cnt++] = ThreshVolt;
                         TemplateDataSample[num_temp]->p_data[kk++] +=   ThreshVolt;
                         /*****************************/
                         while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //�ȴ��ϴη��� ����
                         USART1->DATAR = ThreshVolt;
                         while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //�ȴ��ϴη��� ����
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
                      while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //�ȴ��ϴη��� ����
                      USART1->DATAR = ThreshVolt;
                      while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //�ȴ��ϴη��� ����
                      USART1->DATAR = ThreshVolt >>8;
                      /*****************************/
                  }
                  for(k = 0; k< section_midpos+1; k++)
                  {
                      lcd_data[lcd_cnt++] = Template_Sample->p_data[k];
                      TemplateDataSample[num_temp]->p_data[kk++] +=   Template_Sample->p_data[k];
                      /*****************************/
                      while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //�ȴ��ϴη��� ����
                      USART1->DATAR = Template_Sample->p_data[k];
                      while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //�ȴ��ϴη��� ����
                      USART1->DATAR = Template_Sample->p_data[k] >>8;
                      /*****************************/
                  }

                  for(k = section_midpos+1; k< TemplateSize - dif; k++)
                  {
                      lcd_data[lcd_cnt++] = Template_Sample->p_data[k];
                      TemplateDataSample[num_temp]->p_data[kk++] +=   Template_Sample->p_data[k];
                      /*****************************/
                      while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //�ȴ��ϴη��� ����
                      USART1->DATAR = Template_Sample->p_data[k];
                      while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //�ȴ��ϴη��� ����
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
                      while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //�ȴ��ϴη��� ����
                      USART1->DATAR = Template_Sample->p_data[k];
                      while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //�ȴ��ϴη��� ����
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
                         while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //�ȴ��ϴη��� ����
                         USART1->DATAR = ThreshVolt;
                         while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //�ȴ��ϴη��� ����
                         USART1->DATAR = ThreshVolt >>8;
                         /*****************************/
                     }

                 for(k = 0; k< section_midpos+1; k++)
                  {
                      lcd_data[lcd_cnt++] = Template_Sample->p_data[k];
                      TemplateDataSample[num_temp]->p_data[kk++] +=   Template_Sample->p_data[k];
                      /*****************************/
                      while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //�ȴ��ϴη��� ����
                      USART1->DATAR = Template_Sample->p_data[k];
                      while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //�ȴ��ϴη��� ����
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
                          while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //�ȴ��ϴη��� ����
                          USART1->DATAR = Template_Sample->p_data[k];
                          while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //�ȴ��ϴη��� ����
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
                          while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //�ȴ��ϴη��� ����
                          USART1->DATAR = Template_Sample->p_data[k];
                          while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //�ȴ��ϴη��� ����
                          USART1->DATAR = Template_Sample->p_data[k] >>8;
                          /*****************************/
                      }

                     for(k= 0; k<dif;k++)
                     {
                         lcd_data[lcd_cnt++] = ThreshVolt;
                         TemplateDataSample[num_temp]->p_data[kk++] +=   ThreshVolt;
                         /*****************************/
                         while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //�ȴ��ϴη��� ����
                         USART1->DATAR = ThreshVolt;
                         while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //�ȴ��ϴη��� ����
                         USART1->DATAR = ThreshVolt >>8;
                         /*****************************/
                     }
             }

         }

     }

}
//             TIM_Cmd(TIM6,ENABLE);       //ʹ�ܶ�ʱ��
             DrawCurve(lcd_data,lcd_cnt);
             lcd_show_string(20,210, 16, "Remaining:%d",SampleTrainNum - jj);

}


         //��ȡ�����ɸ���������֮����һ��ƽ��ֵ���õ���������ƥ���ģ�塣

//         TIM_Cmd(TIM6,DISABLE);       //ʧ�ܶ�ʱ��

         for( k=0;k< TemplateSize; k++)
          {
            TemplateDataSample[num_temp]->p_data[k] = TemplateDataSample[num_temp]->p_data[k]/SampleTrainNum;
          }

         //StdThreshhold������ʾDTWƥ�����ֵ���������ֵ���ͱ�ʾƥ������һ�¡�
         //factor��ʾϵ������Ҫ����ʵ�ʲ��Ե�������ͬ����̬�����������ò�ͬ��ϵ��
         //TemplateDataSample[num_temp]->StdThreshhold =  sum_temp;

         //��ģ�����ݱ��浽�̶�flash��ַ�� ����֮�����ֱ�Ӱ�ģ���flash����ram��
         TemplateSave(AddrTemp[num_temp],TemplateDataSample[num_temp]->p_data,TemplateSize);
         TemplateSave(AddrTempSize[num_temp],&(TemplateDataSample[num_temp]->sz),1);
         TemplateSave(AddrTempPeak[num_temp],&TemplatePeakPos,1);



            for(u16 cnt =0; cnt<TemplateSize; cnt++)
            {
               /*****************************/
               while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //�ȴ��ϴη��� ����
               USART1->DATAR = (u8)*(__IO uint16_t *)(AddrTemp[num_temp] + 2*cnt) ;
               while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //�ȴ��ϴη��� ����
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

//            TIM_Cmd(TIM6,ENABLE);       //ʹ�ܶ�ʱ��
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

