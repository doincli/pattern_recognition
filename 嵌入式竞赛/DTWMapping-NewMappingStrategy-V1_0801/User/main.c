
// Version�� V4 Version
// 20220801
// ʹ��DTWƥ�䡣 ·����������W = 10.
// ����ƥ����ԣ����ñȽ�ģʽ���б𣬷�����ֵ���޷�ʽ


#include <Func.h>


/* Global Variant */
volatile u8 SampleNum = 10;             //���ڴֲɼ��������������
volatile u8 SampleTrainNum = 8;        //����ѵ����ģ��������������
volatile u16 Thresh = 200;      //�����ж���Ч��Ϣ��ʼ����ֵ��ֵ
volatile u16 std_thresh = 1500; //�ж����ֵ��������ı�׼��ֵ���ڷ�ֵ���Ҵ��� max-1500 ��ֵ�ĵ㶼����������

volatile uint16_t LaserFlag = 0; // ȫ�ֱ���,�������ⷢ��or������
volatile uint16_t TIMProcessFlag = 0;//ȫ�ֱ����������Ƿ�����ִ����һ�ζ�ʱ�����жϺ���
volatile u16 *p_AdcDmaData = NULL;  // ���ڴ洢ADC�ɼ�������
volatile u16 DataLaserOn = 0;       //���������־λ
volatile u16 DataLaserOff = 0;      //��־λ
volatile u16 DifData = 0;           //Ԥ������һ�����ݵ�
volatile uint16_t DMAProcessFlag = 0;  //DMA��־λ��DMA������ɺ���1

volatile u16 ThreshVolt = 0;     //�����ٽ��ѹֵ
volatile u16 *Buffer = NULL;
volatile u8 Flag =0;    // GetTemplate����ʹ��
volatile u16 *DataStream = NULL;
volatile u8 offset_p = 0;   // ���ڶ�λ��ǰʱ��������ݵ���DataStream�����ڵ���ʼλ��

// ���ڿ���LCD��Ļ���˵�
volatile u8 KeyValue = 10;
volatile u8 LcdScreen =0;
volatile u8 LcdScreenSubOption = 0;
volatile u8 DtwRun = 0;
volatile u8 TemplateWanted = 0;

volatile u8 StateFlag = 1;
volatile u8 KeyReg = 0;
volatile u8 mallocFlag = 0;

 /* Local Variant */
static u8 offset_start = 0; //��λʵʱƥ�����ݵ�ͷλ��
static u32 dtw[4] = {0};


// ���ڼ���
u8 k=0;
u8 kk=0;
u8 i_dtw =0;
u8 j_dtw = 0;
u8 k_dtw = 0;

static u32 min_dtw = 0xFFFFFFFF;
static u8 TempPeakLeft = 0;
static u8 TempPeakRight = 0;

// ģ��������FLASH�ڴ洢��λ��
uint32_t AddrTemp[4] = {AddrTemplate3,AddrTemplate4,AddrTemplate6,AddrTemplate7 };
uint32_t AddrTempSize[4] = {AddrTemplate3_size,AddrTemplate4_size,
                                    AddrTemplate6_size,AddrTemplate7_size};
uint32_t AddrTempPeak[4] = {AddrTemplate3_Peak,AddrTemplate4_Peak,
                            AddrTemplate6_Peak,AddrTemplate7_Peak};



u8 DtwResultReg = 99;
volatile u8 DTWDataState = 0;
u8 cnt_state = 0;
volatile u32 BestDtwValue[4];

int main(void)
{

    //����������ڴ�
    u16 (*D)[2*w +1] = malloc(2 * 100 * (2*w +1) );
    u32 (*DTW)[2*w +1] = malloc(4 * 100 * (2*w +1) );
    for(k=0;k<200;k++)
    {
        for(kk=0;kk<2*w+1;kk++)
        {
            D[k][kk] = 0xFFFF;
            DTW[k][kk] = 0x0;
        }
    }

    for(k=0;k<4;k++)
    {
        BestDtwValue[k] = 0xFFFFFFFF;
    }

/*********************************************************/
/*****              �ڴ����                                                  ***********/
/*********************************************************/
/*********************************************************/
    DataStream = (u16* )malloc(256*2);   //Ԥ�����ڴ������������ݣ����Դ��256��u16������
    p_AdcDmaData = (u16* )malloc( 26*2);   //Ԥ�����ڴ��ADC�������ݣ�26*2�ֽ�,26��ADC����
    Buffer = (u16* )malloc( 20);         //Ԥ�����ڴ����������20���ֽ�
    Template *Template_Sample;   //����ṹ��ָ��Template_Sample
    Template_Sample = (Template* )malloc(4);
    Template_Sample->p_data = (u16* )malloc( 400);   //Ԥ�����ڴ����洢�ɼ�����ÿ��������ģ������,400���ֽ�,200��u16data

    TemplateData* TemplateDataSample[4];            //����ṹ��ָ�롣��ŵĶ�������ѵ���õĸ���ģ�������

    TemplateDataSample[0] = (TemplateData* )malloc(8);
    TemplateDataSample[0]->p_data = NULL;
    TemplateDataSample[0]->sz = 0;
    TemplateDataSample[0]->peak = 0;

    TemplateDataSample[1] = (TemplateData* )malloc(8);
    TemplateDataSample[1]->p_data = NULL;
    TemplateDataSample[1]->sz = 0;
    TemplateDataSample[1]->peak = 0;

    TemplateDataSample[2] = (TemplateData* )malloc(8);
    TemplateDataSample[2]->p_data = NULL;
    TemplateDataSample[2]->sz = 0;
    TemplateDataSample[2]->peak = 0;

    TemplateDataSample[3] = (TemplateData* )malloc(8);
    TemplateDataSample[3]->p_data = NULL;
    TemplateDataSample[3]->sz = 0;
    TemplateDataSample[3]->peak = 0;


/*********************************************************/
/*****              ��ʼ������                                               ***********/
/*********************************************************/
/*********************************************************/
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    Delay_Init();
    GPIOPB0_INIT();     //�����ź��������
    GPIOPB1_INIT();     //�����ź��������
    USART1_Init(115200);  //USART1��ʼ��

    ADC_Function_Init();  //ADC��ʼ��
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_239Cycles5 );//
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);     //�����������ADת��
    //����DMA1��ͨ����ͨ��1��ADC�������ݵ��ڴ�
    DMA1_Init( DMA1_Channel1, (u32)&ADC1->RDATAR, (u32)p_AdcDmaData, 26);
    DMA1Interrupt_Init();
    DMA_ITConfig( DMA1_Channel1, DMA_IT_TC, ENABLE); //����DMAͨ��1�ж�

    TimInterrupt_Init();    //�жϳ�ʼ��

    EXTI0_INT_INIT();       //�����жϳ�ʼ��
    EXTI1_INT_INIT();
    EXTI2_INT_INIT();
    EXTI4_INT_INIT();
    EXTI5_INT_INIT();

    //    LCD initial
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_RNG, ENABLE);
    RNG_Cmd(ENABLE);
    lcd_init();
    lcd_fill(0,0,239,239,BLACK);
    lcd_show_string(20, 80, 24, "Press Wake_Up");
    lcd_show_string(20, 160, 24, "To MainMenu");

    DMA1_Channel1->CNTR = 0; //
    DMA_Cmd( DMA1_Channel1, ENABLE );   //ʹ��DMAͨ��
    TIM6->CNT=0x00;             //��ʱ��������Ϊ0
    //����ʱ���ж�    - 1440��Ƶ������50,��������1KHzƵ�ʵķ����źţ�ռ�ձ�50%
    TIM6_Init( 50-1, 1440-1);
    TIM_Cmd(TIM6,ENABLE);       //ʹ�ܶ�ʱ��
/*********************************************************/
/*********************************************************/
/*********************************************************/
/*********************************************************/

//�Ȼ�ȡ����Ĺ��ź�ǿ��    ThreshVolt��
Delay_Ms(200);
ThreshVolt = GetThreshHoldVolt();


//��һ����ѭ���������й���ȫ���������ڣ�������������ѭ��
while(1)
{
    // ����LCD����ѭ��. ������״̬����ʵ��
    while(LcdScreen)
    {
        //����ʾ���˵�
        DrawMenu(LcdScreen,LcdScreenSubOption);
        //��ѭ������ֵ���ȵ���ȡ�˼�ֵ�ٽ�����һ������
        StateFlag = 1;
        while(StateFlag)
        {
            if(KeyValue == Confirm || KeyValue == Up || KeyValue == Down || KeyValue == Back )
            {
                KeyReg = KeyValue;
                KeyValue = 10;
                StateFlag = 0;
            }
        }
        // ParameterMod��ѡ��
        if(LcdScreen == ParameterMod)
        {
            // ��ѡ��
            if(LcdScreenSubOption == 0)
            {
                switch(KeyReg)
                {
                case Confirm:
                    LcdScreenSubOption = 1;
                    break;
                case Up:
                    LcdScreen = RunDtw;
                    break;
                case Down:
                    LcdScreen = TemplateTraining;
                    break;
                default:
                    break;
                }
            }
            else if(LcdScreenSubOption != 0)
            {
                switch(LcdScreenSubOption)
                {
                case 1:
                    switch(KeyReg)
                    {
                    case Confirm:
                        LcdScreenSubOption = 2;
                        break;
                    case Up:
                        SampleNum++;
                        break;
                    case Down:
                        SampleNum--;
                        break;
                    case Back:
                        LcdScreenSubOption = 0;
                        break;
                    }
                    break;
                case 2:
                    switch(KeyReg)
                    {
                    case Confirm:
                        LcdScreenSubOption = 3;
                        break;
                    case Up:
                        SampleTrainNum++;
                        break;
                    case Down:
                        SampleTrainNum--;
                        break;
                    case Back:
                        LcdScreenSubOption--;
                        break;
                    }
                    break;
                case 3:
                    switch(KeyReg)
                    {
                    case Confirm:
                        LcdScreenSubOption = 4;
                        break;
                    case Up:
                        if(Thresh <500)Thresh+=10;
                        break;
                    case Down:
                        if(Thresh >100)Thresh-=10;
                        break;
                    case Back:
                        LcdScreenSubOption--;
                        break;
                    }
                    break;
                case 4:
                    switch(KeyReg)
                    {
                    case Confirm:
                        LcdScreenSubOption = 0;
                        lcd_clear(BLACK);
                        lcd_show_string(20, 80, 24, "Modify Done");
                        Delay_Ms(2000);
                        break;
                    case Up:
                        if(std_thresh <2000)std_thresh+=50;
                        break;
                    case Down:
                        if(std_thresh >1000)std_thresh-=50;
                        break;
                    case Back:
                        LcdScreenSubOption--;
                        break;
                    }
                    break;
                }
            }
        }
        // TemplateTraining ��ѡ��
        else if(LcdScreen == TemplateTraining)
        {
            if(LcdScreenSubOption == 0)
            {
                switch(KeyReg)
                {
                    case Confirm:
                        LcdScreenSubOption = 1;
                        break;
                    case Up:
                        LcdScreen = ParameterMod;
                        break;
                    case Down:
                        LcdScreen = ClearTemplate;
                        break;
                    case Back:
                        LcdScreen = ParameterMod;
                        break;
                    default: break;
                }
            }
            else if(LcdScreenSubOption == 1)
            {
                switch(KeyReg)
                {
                    case Confirm:
                        fasttraining(Template_Sample); // ����ѵ��ģʽ
                        //���Ҫ��Flash��һ���̶���ַ����дһ����־λ��������������־λ�Ƿ�Ϊ����״̬���������ж�ģ�������Ƿ񱣴���
                        //Flash�
                        FLASH_Unlock();
                        FLASH_ProgramHalfWord(AddrTemplateWrittenDone, 0xABCD);
                        FLASH_Lock();
                        LcdScreen = RunDtw;
                        LcdScreenSubOption = 0;
                        break;
                    case Up:
                        LcdScreenSubOption = 2;
                        break;
                    case Down:
                        LcdScreenSubOption = 2;
                        break;
                    case Back:
                        LcdScreenSubOption = 0;
                        break;
                    default: break;
                }
            }
            else if(LcdScreenSubOption == 2)
            {
                switch(KeyReg)
                {
                case Confirm:
                    lcd_clear(BLACK);
                    lcd_show_string(40, 80, 24, "TemplateTraining");
                    Delay_Ms(1000);
                    //ѭ����¼3��4��6��7 �ĸ���̬��ģ�����ݣ����ֱ�洢����Ӧ�ĵ�ַ����
                    //**************************************************//
                    //**************************************************//
                    //**************************************************//
                    train(Template_Sample);

                    //���Ҫ��Flash��һ���̶���ַ����дһ����־λ��������������־λ�Ƿ�Ϊ����״̬���������ж�ģ�������Ƿ񱣴���
                    //Flash�
                    FLASH_Unlock();
                    FLASH_ProgramHalfWord(AddrTemplateWrittenDone, 0xABCD);
                    FLASH_Lock();

                    lcd_clear(BLACK);
                    lcd_show_string(40, 80, 24, "Training Done");
                    Delay_Ms(2000);
                    LcdScreen = RunDtw;
                    LcdScreenSubOption = 0;
                    break;
                case Up:
                    LcdScreenSubOption = 1;
                    break;
                case Down:
                    LcdScreenSubOption = 1;
                    break;
                case Back:
                    LcdScreenSubOption = 0;
                    break;
                }
            }

        }
        // ClearTemplate ��ѡ��
        else if(LcdScreen == ClearTemplate)
        {
            switch(KeyReg)
            {
            // ���ȷ�����ģ�����ݣ���FLASHд���־λ���㣬ģ���ڴ�ռ��ͷ�
            case Confirm:
                FLASH_Unlock();
                FLASH_ProgramHalfWord(AddrTemplateWrittenDone, 0x0000);
                FLASH_Lock();

                for(k=0;k<4;k++)
                {
                    free(TemplateDataSample[k]->p_data);
                    TemplateDataSample[k]->p_data = NULL;
                }
                lcd_clear(BLACK);
                lcd_show_string(20, 80, 24, "ClearTemplate Done");
                Delay_Ms(2000);
                LcdScreen = ParameterMod;
                break;

            case Up:
                LcdScreen = TemplateTraining;
                break;
            case Down:
                LcdScreen = ChooseTemplate;
                break;
            case Back:
                LcdScreen = ParameterMod;
                break;
            }

        }
        // ChooseTemplate ��ѡ��
        else if(LcdScreen == ChooseTemplate)
        {
            // ��ѡ��
            if(LcdScreenSubOption == 0)
            {
                switch(KeyReg)
                {
                case Confirm:
                    LcdScreenSubOption = 1;
                    break;
                case Up:
                    LcdScreen = ClearTemplate;
                    break;
                case Down:
                    LcdScreen = RunDtw;
                    break;
                case Back:
                    LcdScreen = ParameterMod;
                    break;
                }
            }
            else
            {
                switch(KeyReg)
                {
                    case Confirm:
                        LcdScreen = ParameterMod;
                        LcdScreenSubOption = 0;
                        lcd_clear(BLACK);
                        lcd_show_string(40, 80, 24, "Template Chosen");
                        lcd_show_string(0, 160, 24, "Template-%d Selected",TemplateWanted);
                        Delay_Ms(2000);
                        break;

                    case Up:
                        if(TemplateWanted <3)
                        {
                            TemplateWanted++;
                        }
                        else
                        {
                            TemplateWanted = 0;
                        }
                        break;
                    case Down:
                        if(TemplateWanted >0)
                        {
                            TemplateWanted--;
                        }
                        else {
                            TemplateWanted = 3;
                        }
                        break;
                    case Back:
                        LcdScreen = ParameterMod;
                        LcdScreenSubOption = 0;
                        break;
                }

            }

        }
        // RunDtw ��ѡ��
        else if(LcdScreen == RunDtw)
        {
            switch(KeyReg)
            {
                case Confirm:
                    if(*(__IO uint16_t *)AddrTemplateWrittenDone != 0xABCD)
                    {
                        lcd_clear(BLACK);
                        lcd_show_string(20, 80, 24, "No Template Data!");
                        lcd_show_string(20, 160, 24, "Do Training First!");
                        Delay_Ms(2000);
                        LcdScreen = TemplateTraining;
                        LcdScreenSubOption = 0;
                        break;
                    }
                    else {

                    if(mallocFlag)
                    {
                        for(k=0;k<4;k++)
                        {
                            free(TemplateDataSample[k]->p_data);
                            TemplateDataSample[k]->p_data = NULL;
                        }
                        mallocFlag = 0;
                    }
                    for(k=0; k<4;k++)
                    {
                        TemplateDataSample[k]->sz = *(__IO uint16_t *)AddrTempSize[k];
                        TemplateDataSample[k]->peak = *(__IO uint16_t *)AddrTempPeak[k];
                        //�ڻ�ȡģ�峤�Ⱥ󣬸�ģ�����ݻ����ڴ�ռ�
                        TemplateDataSample[k]->p_data = (u16* )malloc( (TemplateDataSample[k]->sz)* 2);
                        if(TemplateDataSample[k]->p_data == NULL)
                        {
                            lcd_clear(BLACK);
                            lcd_show_string(40, 80, 24, "NOT enough RAM");
                            Delay_Ms(3000);
                            return 0;
                        }
                        memcpy(TemplateDataSample[k]->p_data,(u32 *)AddrTemp[k],(TemplateDataSample[k]->sz)*2 );
                    }
                    //���²ɼ�������ź�
                    ThreshVolt = GetThreshHoldVolt();
                    mallocFlag = 1;
                    LcdScreenSubOption = 0;
                    LcdScreen = Drop;
                    DtwRun = 1;
                    lcd_clear(BLACK);
                    lcd_show_string(40, 80, 24, "Running DTW");
                }break;

                case Up:
                    LcdScreen = ChooseTemplate;
                    break;
                case Down:
                    LcdScreen = ParameterMod;
                    break;
                case Back:
                    break;
                default: break;
                }

            }
        }

    //ƥ�����ѭ��
    while(DtwRun)
    {

        if(DMAProcessFlag)
        {
            if(DTWDataState == 0)
            {
                if(abs(DifData-ThreshVolt) > Thresh)
                {
                    cnt_state++;

                    if(cnt_state > 10)
                    {
                        DTWDataState = 1;
                        cnt_state = 0;
                    }
                }
                else {
                    cnt_state = 0;
                }

            }
            else{
                if(abs(DifData-ThreshVolt) <= Thresh)
                {
                    cnt_state++;
                    if(cnt_state > 20)
                    {
                        DTWDataState =2;
                        cnt_state = 0;
                    }

                }
                else {
                    cnt_state = 0;
                }
            }

            if(1 == DTWDataState)
            {
                for( k=0;k<4;k++)
                {
                    dtw[k] = 0;
                    TempPeakLeft = TemplateDataSample[k]->peak- PeakDeep;
                    TempPeakRight = TemplateDataSample[k]->peak + PeakDeep;
                    if(offset_p > TemplateDataSample[k]->sz-1)
                    {
                        offset_start = offset_p-TemplateDataSample[k]->sz;
                    }
                    else {
                        offset_start = (256 - TemplateDataSample[k]->sz + offset_p);
                    }
                    // ��ʼ����������D
                    for (i_dtw = 0; i_dtw < w; i_dtw++)
                    {
                        k_dtw = 0;
                        for (j_dtw = (w  - i_dtw); j_dtw < 2 * w +1; j_dtw++)
                        {
                            D[i_dtw][j_dtw] = abs(TemplateDataSample[k]->p_data[i_dtw] - DataStream[k_dtw+offset_start]);
                            k_dtw++;
                        }

                    }
                    // middle of D
                    for (i_dtw = w; i_dtw < (TemplateDataSample[k]->sz - w); i_dtw++)
                    {

                        k_dtw = i_dtw - w;
                        for (j_dtw = 0; j_dtw < 2 * w + 1; j_dtw++)
                        {
                            D[i_dtw][j_dtw] = abs(TemplateDataSample[k]->p_data[i_dtw] - DataStream[k_dtw+offset_start]);
                            k_dtw++;
                        }
                    }

                    // bottom of D
                    for (i_dtw = (TemplateDataSample[k]->sz - w); i_dtw < TemplateDataSample[k]->sz; i_dtw++)
                    {
                        k_dtw = i_dtw - w;

                        for (j_dtw = 0; j_dtw < (TemplateDataSample[k]->sz - (i_dtw-w)); j_dtw++)
                        {

                            D[i_dtw][j_dtw] = abs(TemplateDataSample[k]->p_data[i_dtw] - DataStream[k_dtw+offset_start]);
                            k_dtw++;
                        }

                    }
                    // ���ݾ������D������DTW
                    for (j_dtw = 0; j_dtw < w + 1; j_dtw++)
                    {
                        DTW[0][j_dtw] = D[0][j_dtw];
                    }

                    for (j_dtw = w + 1; j_dtw < 2 * w + 1; j_dtw++)
                    {

                        DTW[0][j_dtw] = D[0][j_dtw] + DTW[0][j_dtw - 1];
                    }

                    for (i_dtw = 1; i_dtw < TemplateDataSample[k]->sz; i_dtw++)
                    {
                        if(i_dtw>= TempPeakLeft && i_dtw<= TempPeakRight)
                        {
                            min_dtw = DTW[i_dtw - 1][0] < DTW[i_dtw - 1][1] ? DTW[i_dtw - 1][0] : DTW[i_dtw - 1][1];
                            DTW[i_dtw][0] = 1.5*D[i_dtw][0] + min_dtw;
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
                                DTW[i_dtw][j_dtw] = 1.5*D[i_dtw][j_dtw] + min_dtw;
                            }

                            min_dtw = DTW[i_dtw][2*w-1] < DTW[i_dtw - 1][2*w] ? DTW[i_dtw][2 * w-1] : DTW[i_dtw - 1][2*w];
                            DTW[i_dtw][2 * w] = 1.5*D[i_dtw][2 * w] + min_dtw;
                        }
                        else {
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

                    }
                    dtw[k] = (DTW[TemplateDataSample[k]->sz-1][w])/TemplateDataSample[k]->sz;

                    if(dtw[k] < BestDtwValue[k])
                    {
                        BestDtwValue[k] = dtw[k];
                    }
                }
                // 4��ģ���DTWֵ������ϣ�������ֵ�Ƚ�

            }
            else if(2 == DTWDataState) //��ʱ����Ч���ݶν������Ƚ��б���ʶ��ģ��
            {
                //ͼ�λ���ʾ����ɸѡ����
                lcd_clear(BLACK);
                lcd_show_string(50,20, 32, "Template");
                lcd_show_string(20,160, 16, "Temp.0 DTW: %d",BestDtwValue[0]);
                lcd_show_string(20,180, 16, "Temp.1 DTW: %d",BestDtwValue[1]);
                lcd_show_string(20,200, 16, "Temp.2 DTW: %d",BestDtwValue[2]);
                lcd_show_string(20,220, 16, "Temp.3 DTW: %d",BestDtwValue[3]);

                DtwResultReg = 0;
                for(k=1;k<4;k++)
                {
                    if(BestDtwValue[k] < BestDtwValue[0])
                    {
                        BestDtwValue[0] = BestDtwValue[k];
                        DtwResultReg = k;
                    }
                }

                lcd_show_num(100,100, DtwResultReg, 1, 32);

                if(DtwResultReg != TemplateWanted)
                {
                    GPIO_WriteBit(GPIOB, GPIO_Pin_1, 1);//������Ҫ��ģ�壬��������
                    Delay_Ms(10);
                }
                GPIO_WriteBit(GPIOB, GPIO_Pin_1, 0);
                //end

                for(k=0;k<4;k++)
                {
                    BestDtwValue[k] = 0xFFFFFFFF;
                }
                DTWDataState = 0;
            }

            DMAProcessFlag = 0;
        }
    }

}
    return 0;
}

