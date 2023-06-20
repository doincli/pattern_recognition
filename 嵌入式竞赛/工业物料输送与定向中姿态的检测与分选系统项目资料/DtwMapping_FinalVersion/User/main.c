
// Version： DtwMapping_FinalVersion
//  2022/8/10
// 更改匹配策略，采用比较模式来判别，放弃阈值门限方式
// 优化菜单界面-删去cleartemplate功能，增加waveform展示波形功能
// 修改模板获取代码Gettemplate函数
// 修改模板训练算法，采用类粒子群算法，找出全局最优的模板
// DTW最后计算中包含了欧式距离的权重

#include <Func.h>


/* Global Variant */
volatile u16 Thresh = 300;      //用于判定有效信息起始的阈值数值
volatile u16 JudgeThresh = 400;
//参数
volatile float fac_dtw = 0.75;      // DTW的权重
volatile float fac_Odist = 0.25;    //欧式距离的权重
volatile u32 MedistanceValue[4];    //欧式距离变量
volatile u32 Finaldist[4];          //最终DTW表征值
volatile uint16_t LaserFlag = 0; // 全局变量,表征激光发光or不发光
volatile uint16_t TIMProcessFlag = 0;//全局变量，表征是否重新执行了一次定时器的中断函数
volatile u16 *p_AdcDmaData = NULL;  // 用于存储ADC采集的数据
volatile u16 DataLaserOn = 0;       //激光亮灭标志位
volatile u16 DataLaserOff = 0;      //标志位
volatile u16 DifData = 0;           //预处理后的一个数据点
volatile uint16_t DMAProcessFlag = 0;  //DMA标志位，DMA传送完成后置1
volatile u16 ThreshVolt = 0;     //定义临界电压值，用来判定是否为导轨光信号
volatile u16 *DataStream = NULL; //DataStream数据流，有效传感数据都被存储在这个数组内
volatile u8 offset_p = 0;   // 用于定位当前时间点下数据点在DataStream数组内的起始位置
volatile u8 DTWDataState = 0;   //表征DTW匹配时候的状态
volatile u32 BestDtwValue[4];

// 用于控制LCD屏幕主菜单
volatile u8 KeyValue = 10;  //键值
volatile u8 LcdScreen =0;
volatile u8 LcdScreenSubOption = 0;
volatile u8 DtwRun = 0;
volatile u8 TemplateWanted = 0;
volatile u8 StateFlag = 1;
volatile u8 KeyReg = 0;
// 用于控制LCD屏幕主菜单

volatile u8 mallocFlag = 0; //标志位
 /* Local Variant */
static u8 offset_start = 0; //定位实时匹配数据的头位置
static u32 dtw[4] = {0};
static u32 min_dtw = 0xFFFFFFFF;

// 用于计数
u8 k=0;
u8 kk=0;
u8 i_dtw =0;
u8 j_dtw = 0;
u8 k_dtw = 0;
u8 cnt_state = 0;
// 用于计数

// 模板数据在FLASH内存储的位置
uint32_t AddrTemp[4] = {AddrTemplate3,AddrTemplate4,AddrTemplate6,AddrTemplate7 };
uint32_t AddrTempSize[4] = {AddrTemplate3_size,AddrTemplate4_size,
                                    AddrTemplate6_size,AddrTemplate7_size};
uint32_t AddrTempPeak[4] = {AddrTemplate3_Peak,AddrTemplate4_Peak,
                            AddrTemplate6_Peak,AddrTemplate7_Peak};

//直接申明一个足够大的数组以供计算DTW时使用。
u16 D[max_L][2*max_w +1] = {0xFFFF};
u32 DTW[max_L][2*max_w +1] = {0xFFFFFFFF};
u8 DtwResultReg = 99;   //DTW运算最终结果，表征此时判定识别的样本是哪种姿态




int main(void)
{

    for(k=0;k<max_L;k++)
    {
        for(kk=0;kk<2*max_w+1;kk++)
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
/*****              内存分配                                                  ***********/
/*********************************************************/
/*********************************************************/
    DataStream = (u16* )malloc(256*2);   //预分配内存存放数据流数据，可以存放256个u16型数据
    p_AdcDmaData = (u16* )malloc( 26*2);   //预分配内存给ADC采样数据，26*2字节,26个ADC数据
    Template *Template_Sample;   //定义结构体指针Template_Sample
    Template_Sample = (Template* )malloc(4);
    Template_Sample->p_data = (u16* )malloc( 400);   //预分配内存来存储采集到的每个样本的模板数据,400个字节,200个u16data

    TemplateData* TemplateDataSample[4];            //定义结构体指针。存放的都是最终训练好的各个模板的数据

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
/*****              初始化设置                                               ***********/
/*********************************************************/
/*********************************************************/
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    Delay_Init();
    GPIOPB0_INIT();     //方波信号输出引脚
    GPIOPB1_INIT();     //气阀信号输出引脚
    USART1_Init(115200);  //USART1初始化

    ADC_Function_Init();  //ADC初始化
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_239Cycles5 ); //ADC配置
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);     //软件触发启动AD转换
    //设置DMA1的通道，通道1从ADC接收数据到内存
    DMA1_Init( DMA1_Channel1, (u32)&ADC1->RDATAR, (u32)p_AdcDmaData, 26);
    DMA1Interrupt_Init();   //DMA传输中断，传输完毕后进入中断程序
    DMA_ITConfig( DMA1_Channel1, DMA_IT_TC, ENABLE); //配置DMA通道1中断

    TimInterrupt_Init();    //中断初始化

    //按键中断初始化
    EXTI0_INT_INIT();
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

    DMA1_Channel1->CNTR = 0; //DMA传输数据个数置0
    DMA_Cmd( DMA1_Channel1, ENABLE );   //使能DMA通道
    TIM6->CNT=0x00;             //定时器计数先重置为0
    //设置时钟中断    - 1440分频，计数50,这样就是1KHz频率的方波信号，占空比50%
    TIM6_Init( 50-1, 1440-1);
    TIM_Cmd(TIM6,ENABLE);       //使能定时器
/*********************************************************/
/*********************************************************/
/*********************************************************/
/*********************************************************/

//先获取导轨的光信号强度    ThreshVolt。
Delay_Ms(200);
ThreshVolt = GetThreshHoldVolt();


//第一层死循环，把所有功能全部包含在内，并且永不跳出循环
while(1)
{
    // 先是LCD的主循环. 用有限状态机来实现
    while(LcdScreen)
    {
        //先显示主菜单
        DrawMenu(LcdScreen,LcdScreenSubOption);
        //先循环检测键值，等到获取了键值再进行下一步处理
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
        // ParameterMod主选项
        if(LcdScreen == ParameterMod)
        {
            // 子选项
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
                        if(fac_dtw <0.99)fac_dtw+=0.01;
                        break;
                    case Down:
                        if(fac_dtw >0.5)fac_dtw-=0.01;
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
                        if(fac_Odist < 0.99)  fac_Odist+=0.01;
                        break;
                    case Down:
                        if(fac_Odist > 0.1)fac_Odist-=0.01;
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
                        Delay_Ms(1000);
                        break;
                    case Up:
                        if(JudgeThresh < 500) JudgeThresh+=10;
                        break;
                    case Down:
                        if(JudgeThresh > 300) JudgeThresh-=10;
                        break;
                    case Back:
                        LcdScreenSubOption--;
                        break;
                    }
                    break;
                case 5:
                    switch(KeyReg)
                    {
                    case Confirm:
                        LcdScreenSubOption = 0;
                        lcd_clear(BLACK);
                        lcd_show_string(20, 80, 24, "Modify Done");
                        Delay_Ms(2000);
                        break;
                    case Up:
                        //if(w <max_w) w+=1;
                        break;
                    case Down:
                        //if(w >5) w -=1;
                        break;
                    case Back:
                        LcdScreenSubOption--;
                        break;
                    }
                    break;
                }
            }
        }
        // TemplateTraining 主选项
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
                        LcdScreen = Waveform;
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
                        LcdScreenSubOption = 7;
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

            else if(LcdScreenSubOption == 7)
            {
                switch(KeyReg)
                {
                case Confirm:
                    fasttraining(Template_Sample,LcdScreenSubOption-7); // 快速训练模式
                    //最后要往Flash的一个固定地址里面写一个标志位，开机检查这个标志位是否为挂起状态，
                    //这样来判断模板数据是否保存在Flash里。
                    FLASH_Unlock();
                    FLASH_ProgramHalfWord(AddrTemplateWrittenDone, 0xF |
                            *(__IO uint16_t *)AddrTemplateWrittenDone);
                    FLASH_Lock();
                    LcdScreenSubOption = 8;
                    break;
                case Up:
                    LcdScreenSubOption = 10;
                    break;
                case Down:
                    LcdScreenSubOption = 8;
                    break;
                case Back:
                    LcdScreenSubOption = 1;
                    break;
                }
            }

            else if(LcdScreenSubOption == 8)
            {
                switch(KeyReg)
                {
                case Confirm:
                    fasttraining(Template_Sample,LcdScreenSubOption-7); // 快速训练模式
                    //最后要往Flash的一个固定地址里面写一个标志位，开机检查这个标志位是否为挂起状态，这样来判断模板数据是否保存在
                    //Flash里。
                    FLASH_Unlock();
                    FLASH_ProgramHalfWord(AddrTemplateWrittenDone, 0xF0 |
                            *(__IO uint16_t *)AddrTemplateWrittenDone);
                    FLASH_Lock();
                    LcdScreenSubOption = 9;
                    break;
                case Up:
                    LcdScreenSubOption = 7;
                    break;
                case Down:
                    LcdScreenSubOption = 9;
                    break;
                case Back:
                    LcdScreenSubOption = 1;
                    break;
                }
            }
            else if(LcdScreenSubOption == 9)
            {
                switch(KeyReg)
                {
                case Confirm:
                    fasttraining(Template_Sample,LcdScreenSubOption-7); // 快速训练模式
                    //最后要往Flash的一个固定地址里面写一个标志位，开机检查这个标志位是否为挂起状态，这样来判断模板数据是否保存在
                    //Flash里。
                    FLASH_Unlock();
                    FLASH_ProgramHalfWord(AddrTemplateWrittenDone, 0xF00 |
                            *(__IO uint16_t *)AddrTemplateWrittenDone);
                    FLASH_Lock();
                    LcdScreenSubOption = 10;
                    break;
                case Up:
                    LcdScreenSubOption = 8;
                    break;
                case Down:
                    LcdScreenSubOption = 10;
                    break;
                case Back:
                    LcdScreenSubOption = 1;
                    break;
                }
            }
            else if(LcdScreenSubOption == 10)
            {
                switch(KeyReg)
                {
                case Confirm:
                    fasttraining(Template_Sample,LcdScreenSubOption-7); // 快速训练模式
                    //最后要往Flash的一个固定地址里面写一个标志位，开机检查这个标志位是否为挂起状态，这样来判断模板数据是否保存在
                    //Flash里。
                    FLASH_Unlock();
                    FLASH_ProgramHalfWord(AddrTemplateWrittenDone, 0xF000 |
                            *(__IO uint16_t *)AddrTemplateWrittenDone);
                    FLASH_Lock();
                    LcdScreenSubOption = 7;
                    break;
                case Up:
                    LcdScreenSubOption = 9;
                    break;
                case Down:
                    LcdScreenSubOption = 7;
                    break;
                case Back:
                    LcdScreenSubOption = 1;
                    break;
                }
            }

            else if(LcdScreenSubOption == 2)
            {
                switch(KeyReg)
                {
                case Confirm:
                    LcdScreenSubOption = 3;
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
            else if(LcdScreenSubOption == 3)
            {
                switch(KeyReg)
                {
                case Confirm:
                    lcd_clear(BLACK);
                    lcd_show_string(40, 80, 24, "Template-%d Training",(LcdScreenSubOption-3));
                    Delay_Ms(1000);
                    train(Template_Sample,LcdScreenSubOption-3);
                    //最后要往Flash的一个固定地址里面写一个标志位，开机检查这个标志位是否为挂起状态，这样来判断模板数据是否保存在
                    //Flash里。
                    FLASH_Unlock();
                    FLASH_ProgramHalfWord(AddrTemplateWrittenDone, 0xF |
                            *(__IO uint16_t *)AddrTemplateWrittenDone);
                    FLASH_Lock();
                    LcdScreenSubOption = 4;
                    break;
                case Up:
                    LcdScreenSubOption = 6;
                    break;
                case Down:
                    LcdScreenSubOption = 4;
                    break;
                case Back:
                    LcdScreenSubOption = 2;
                    break;
                }

            }
            else if(LcdScreenSubOption == 4)
            {
                switch(KeyReg)
                {
                case Confirm:
                    lcd_clear(BLACK);
                    lcd_show_string(40, 80, 24, "Template-%d Training",(LcdScreenSubOption-3));
                    Delay_Ms(1000);
                    train(Template_Sample,LcdScreenSubOption-3);
                    //最后要往Flash的一个固定地址里面写一个标志位，开机检查这个标志位是否为挂起状态，这样来判断模板数据是否保存在
                    //Flash里。
                    FLASH_Unlock();
                    FLASH_ProgramHalfWord(AddrTemplateWrittenDone, 0xF0 |
                            *(__IO uint16_t *)AddrTemplateWrittenDone);
                    FLASH_Lock();
                    LcdScreenSubOption = 5;
                    break;
                case Up:
                    LcdScreenSubOption = 3;
                    break;
                case Down:
                    LcdScreenSubOption = 5;
                    break;
                case Back:
                    LcdScreenSubOption = 2;
                    break;
                }
            }
            else if(LcdScreenSubOption == 5)
            {
                switch(KeyReg)
                {
                case Confirm:
                    lcd_clear(BLACK);
                    lcd_show_string(40, 80, 24, "Template-%d Training",(LcdScreenSubOption-3));
                    Delay_Ms(1000);
                    train(Template_Sample,LcdScreenSubOption-3);
                    //最后要往Flash的一个固定地址里面写一个标志位，开机检查这个标志位是否为挂起状态，这样来判断模板数据是否保存在
                    //Flash里。
                    FLASH_Unlock();
                    FLASH_ProgramHalfWord(AddrTemplateWrittenDone, 0xF00 |
                            *(__IO uint16_t *)AddrTemplateWrittenDone);
                    FLASH_Lock();
                    LcdScreenSubOption = 6;
                    break;
                case Up:
                    LcdScreenSubOption = 4;
                    break;
                case Down:
                    LcdScreenSubOption = 6;
                    break;
                case Back:
                    LcdScreenSubOption = 2;
                    break;
                }
            }
            else if(LcdScreenSubOption == 6)
            {
                switch(KeyReg)
                {
                case Confirm:
                    lcd_clear(BLACK);
                    lcd_show_string(40, 80, 24, "Template-%d Training",(LcdScreenSubOption-3));
                    Delay_Ms(1000);
                    train(Template_Sample,LcdScreenSubOption-3);
                    //最后要往Flash的一个固定地址里面写一个标志位，开机检查这个标志位是否为挂起状态，这样来判断模板数据是否保存在
                    //Flash里。
                    FLASH_Unlock();
                    FLASH_ProgramHalfWord(AddrTemplateWrittenDone, 0xF000 |
                            *(__IO uint16_t *)AddrTemplateWrittenDone);
                    FLASH_Lock();
                    LcdScreenSubOption = 2;
                    break;
                case Up:
                    LcdScreenSubOption = 5;
                    break;
                case Down:
                    LcdScreenSubOption = 3;
                    break;
                case Back:
                    LcdScreenSubOption = 2;
                    break;
                }
            }

        }
        // Waveform 主选项
        else if(LcdScreen == Waveform)
        {
            switch(LcdScreenSubOption)
            {
                case 0:
                    switch(KeyReg)
                    {
                    case Confirm:
                        LcdScreenSubOption = 1;
                        break;
                    case Up:
                        LcdScreen = 2;
                        break;
                    case Down:
                        LcdScreen = 4;
                        break;
                    case Back:
                        break;
                    }
                    break;

                case 1:
                    switch(KeyReg)
                    {
                    case Confirm:
                        //首先检查标志位是否一致，判断FLASH中是否已经存储着数据
                        if(0x000F == ((*(__IO uint16_t *)AddrTemplateWrittenDone) & 0x000F) )
                        {
                            DrawCurve((u16 *)AddrTemp[LcdScreenSubOption-1],
                                    *((__IO uint16_t *)AddrTempSize[LcdScreenSubOption-1]));
                            Delay_Ms(WaveTime_Ms);

                        }
                        //如果没有数据，即还没有训练得到模板，则LCD显示下列信息
                        else {
                            lcd_clear(BLACK);
                            lcd_show_string(20, 80, 24, "No Template Data!");
                            lcd_show_string(20, 160, 24, "Do Training First!");
                            Delay_Ms(1000);
                        }
                        break;
                    case Up:
                        LcdScreenSubOption = 4;
                        break;
                    case Down:
                        LcdScreenSubOption = 2;
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
                        if(0x00F0 == ((*(__IO uint16_t *)AddrTemplateWrittenDone) & 0x00F0))
                        {
                            DrawCurve((u16 *)AddrTemp[LcdScreenSubOption-1],
                                    *((__IO uint16_t *)AddrTempSize[LcdScreenSubOption-1]));
                            Delay_Ms(WaveTime_Ms);

                        }
                        else {
                            lcd_clear(BLACK);
                            lcd_show_string(20, 80, 24, "No Template Data!");
                            lcd_show_string(20, 160, 24, "Do Training First!");
                            Delay_Ms(1000);
                        }
                        break;
                    case Up:
                        LcdScreenSubOption = 1;
                        break;
                    case Down:
                        LcdScreenSubOption = 3;
                        break;
                    case Back:
                        LcdScreenSubOption = 0;
                        break;
                    }
                    break;

                case 3:
                    switch(KeyReg)
                    {
                    case Confirm:
                        if(0x0F00 == ((*(__IO uint16_t *)AddrTemplateWrittenDone) & 0x0F00) )
                        {
                            DrawCurve((u16 *)AddrTemp[LcdScreenSubOption-1],
                                    *((__IO uint16_t *)AddrTempSize[LcdScreenSubOption-1]));
                            Delay_Ms(WaveTime_Ms);
                        }
                        else {
                            lcd_clear(BLACK);
                            lcd_show_string(20, 80, 24, "No Template Data!");
                            lcd_show_string(20, 160, 24, "Do Training First!");
                            Delay_Ms(1000);
                        }
                        break;
                    case Up:
                        LcdScreenSubOption = 2;
                        break;
                    case Down:
                        LcdScreenSubOption = 4;
                        break;
                    case Back:
                        LcdScreenSubOption = 0;
                        break;
                    }
                    break;

                case 4:
                    switch(KeyReg)
                    {
                    case Confirm:
                        if(0xF000 == ((*(__IO uint16_t *)AddrTemplateWrittenDone) & 0xF000) )
                        {
                            DrawCurve((u16 *)AddrTemp[LcdScreenSubOption-1],
                                    *((__IO uint16_t *)AddrTempSize[LcdScreenSubOption-1]));
                            Delay_Ms(WaveTime_Ms);

                        }
                        else {
                            lcd_clear(BLACK);
                            lcd_show_string(20, 80, 24, "No Template Data!");
                            lcd_show_string(20, 160, 24, "Do Training First!");
                            Delay_Ms(1000);
                        }
                        break;
                    case Up:
                        LcdScreenSubOption = 3;
                        break;
                    case Down:
                        LcdScreenSubOption = 1;
                        break;
                    case Back:
                        LcdScreenSubOption = 0;
                        break;
                    }
                    break;

                default:
                    LcdScreenSubOption = 0;
                    break;

            }
        }

        // ChooseTemplate 主选项
        else if(LcdScreen == ChooseTemplate)
        {
            // 子选项
            if(LcdScreenSubOption == 0)
            {
                switch(KeyReg)
                {
                case Confirm:
                    LcdScreenSubOption = 1;
                    break;
                case Up:
                    LcdScreen = Waveform;
                    break;
                case Down:
                    LcdScreen = RunDtw;
                    break;
                case Back:
                   // LcdScreen = RunDtw;
                    break;
                }
            }
            else
            {
                switch(KeyReg)
                {
                    case Confirm:
                        LcdScreen = RunDtw;
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
                        LcdScreen = ChooseTemplate;
                        LcdScreenSubOption = 0;
                        break;
                }

            }

        }
        // RunDtw 主选项
        else if(LcdScreen == RunDtw)
        {
            switch(KeyReg)
            {
                case Confirm:
                    if(*(__IO uint16_t *)AddrTemplateWrittenDone != 0xFFFF)
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
                            //在获取模板长度后，给模板数据划分内存空间
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
                        //重新采集导轨光信号
                        ThreshVolt = GetThreshHoldVolt();
                        mallocFlag = 1;
                        LcdScreenSubOption = 0;
                        LcdScreen = Drop;
                        DtwRun = 1;
                        lcd_clear(BLACK);
                        lcd_show_string(40, 80, 24, "Running DTW");
                        Delay_Ms(2000);
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

    //DTW匹配的主循环
    while(DtwRun)
    {
        //检查ADC数据处理完成标志位
        if(DMAProcessFlag)
        {
            //处于状态0时刻，即还未检测到物料样本时的状态
            if(DTWDataState == 0)
            {
                //检查获取得光传感信号值是否在导轨光强度值范围内
                if(abs(DifData-ThreshVolt) > Thresh)
                {
                    cnt_state++;    //如果是，计数加1

                    if(cnt_state > 6)
                    {
                        DTWDataState = 1;   //当连续检测到6个超出导轨光强度值的
                        cnt_state = 0;      //则认为物料样本正在通过光纤下方，
                    }                       //进入状态1
                }
                else {
                    cnt_state = 0;  //如果不是，则重置计数为0
                }
            }
            else{   //不处于状态0时，即状态1时。状态2只发生在状态1跳转到状态2时刻，并且运行一遍后立马重置为状态0
                if(abs(DifData-ThreshVolt) <= Thresh) //检测光传感信号值是否在导轨光强度值范围内
                {
                    cnt_state++;
                    if(cnt_state > 15)  //如果连续检测到15个点的光传感信号值都在导轨光强度值范围内，
                    {                   //则认为物料样本已经通过光纤，既可以进入状态2，进行姿态判断
                        DTWDataState =2;
                        cnt_state = 0;
                    }
                }
                else {                  //重置计数为0
                    cnt_state = 0;
                }
            }

            //状态1时刻。此状态下，每获取一个数据点，都会进行一次匹配计算，得到Finaldist这个参数值，它是
            //DTW值和欧式距离两者做加权求和得到的一个表征值，值越小，表明此时物料样本姿态与模板的匹配程度
            //越好，识别为这个模板的可信度越高
            if(1 == DTWDataState)
            {
                for( k=0;k<4;k++)
                {
                    dtw[k] = 0;
                    if(offset_p > TemplateDataSample[k]->sz-1)
                    {
                        offset_start = offset_p-TemplateDataSample[k]->sz;
                    }
                    else {
                        offset_start = (256 - TemplateDataSample[k]->sz + offset_p);
                    }
                    // 开始计算距离矩阵D
                    for (i_dtw = 0; i_dtw < w; i_dtw++)
                    {
                        k_dtw = 0;
                        for (j_dtw = (w  - i_dtw); j_dtw < 2 * w +1; j_dtw++)
                        {
                            D[i_dtw][j_dtw] = abs(TemplateDataSample[k]->p_data[i_dtw] -
                                    DataStream[k_dtw+offset_start]);
                            k_dtw++;
                        }
                    }
                    // middle of D
                    for (i_dtw = w; i_dtw < (TemplateDataSample[k]->sz - w); i_dtw++)
                    {
                        k_dtw = i_dtw - w;
                        for (j_dtw = 0; j_dtw < 2 * w + 1; j_dtw++)
                        {
                            D[i_dtw][j_dtw] = abs(TemplateDataSample[k]->p_data[i_dtw] -
                                    DataStream[k_dtw+offset_start]);
                            k_dtw++;
                        }
                    }
                    // bottom of D
                    for (i_dtw = (TemplateDataSample[k]->sz - w);
                            i_dtw < TemplateDataSample[k]->sz; i_dtw++)
                    {
                        k_dtw = i_dtw - w;
                        for (j_dtw = 0; j_dtw < (TemplateDataSample[k]->sz - (i_dtw-w));
                                j_dtw++)
                        {
                            D[i_dtw][j_dtw] = abs(TemplateDataSample[k]->p_data[i_dtw] -
                                    DataStream[k_dtw+offset_start]);
                            k_dtw++;
                        }
                    }
                    // 根据距离矩阵D来计算DTW
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
                            min_dtw = DTW[i_dtw][2*w-1] < DTW[i_dtw - 1][2*w] ?
                                    DTW[i_dtw][2 * w-1] : DTW[i_dtw - 1][2*w];
                            DTW[i_dtw][2 * w] = D[i_dtw][2 * w] + min_dtw;
                    }
                    dtw[k] = (DTW[TemplateDataSample[k]->sz-1][w])/TemplateDataSample[k]->sz;
                    //计算欧式距离
                    MedistanceValue[k] = 0;
                    for(kk=0; kk<TemplateDataSample[k]->sz; kk++)
                    {
                        MedistanceValue[k] += abs(DataStream[offset_start + kk] -
                                TemplateDataSample[k]->p_data[kk]);
                    }
                    //可调整参数    dtw 0.75   欧式距离0.25
                    Finaldist[k] = fac_dtw * dtw[k]  + fac_Odist *
                            (MedistanceValue[k]/TemplateDataSample[k]->sz);
                    if(Finaldist[k] < BestDtwValue[k]) //BestDtwValue首先置为最大值
                    {
                        BestDtwValue[k] = Finaldist[k]; //在每次计算时，都要去和目前的最优值比较，
                    }                                   //从而得到全局最优值
                }
            }
            // 如果在状态2。此时物料样本通过光纤，已经采集到足够多的数据，可以进行比较判别，来判断刚通过
            //的物料样本属于哪个姿态
            else if(2 == DTWDataState) //这时候有效数据段结束，比较判别来识别模板
            {
                DtwResultReg = 0;
                for(k=1;k<4;k++)    //该循环是为了找到最小的DTW表征值BestDtwValue，并且识别
                {                   //是与哪个模板匹配得出的，从而判断该物料样本属于哪个姿态。
                    if(BestDtwValue[k] < BestDtwValue[0])
                    {
                        BestDtwValue[0] = BestDtwValue[k];
                        DtwResultReg = k;
                    }
                }
                //设置一个阈值，如果DTW表征值大于该值，那么认为本次识别的可信度很低，容易发生误识别的错误
                if(BestDtwValue[0] >JudgeThresh)
                {
                    lcd_show_num(104,150, 9, 1, 32);
                    GPIO_WriteBit(GPIOB, GPIO_Pin_1, 1);//不是想要的模板，吹气吹掉
                    Delay_Ms(30);//
                    GPIO_WriteBit(GPIOB, GPIO_Pin_1, 0);
                }
                //如果此时可信度较高，则判断是否是我们要保留的姿态，不是的话通过吹气吹掉物料
                else if ( DtwResultReg != TemplateWanted){
                    lcd_show_num(104,150, DtwResultReg, 1, 32);
                    GPIO_WriteBit(GPIOB, GPIO_Pin_1, 1);//不是想要的模板，吹气吹掉
                    Delay_Ms(25);
                    GPIO_WriteBit(GPIOB, GPIO_Pin_1, 0);
                }
                else {
                    lcd_show_num(104,150, DtwResultReg, 1, 32);
                     }
                //重置BestDtwValue参数值
                for(k=0;k<4;k++)
                {
                    BestDtwValue[k] = 0xFFFFFFFF;
                }
                //重置状态为0
                DTWDataState = 0;
                }

            DMAProcessFlag = 0; //重置标志位为0
        }
    }

}
    return 0;
}

