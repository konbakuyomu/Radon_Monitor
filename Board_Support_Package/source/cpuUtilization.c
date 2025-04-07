/**
 * @file cpuUtilization.c
 * @brief CPU使用率统计功能实现
 * @date 2025-03-07
 * @details 实现CPU使用率统计的核心功能，包括定时器配置、中断处理和使用率计算
 */

#include "cpuUtilization.h"

/* 常量定义
 * -------------------------------------------------------------*/

/**
 * @brief CPU统计使用的定时器比较值
 * @details Timer0 定时器的时钟源是 LRC 振荡器，频率为 32.768kHz
 *          Frequency = (Clock freq / div) / (Compare value + 1)
 *          当前频率 = (32768 / 1) / (2 + 1) = 10.922kHz > 10kHz
 *          用于统计CPU使用率的高速定时器必须10倍于FreeRTOS的系统中断时间（默认是1ms）。
 *          因此定时器的中断时间至少在100us以内，等价于10kHZ频率
 */
static const uint32_t CPU_TMR0_PERIOD_VALUE = 2UL;

/* 全局变量
 * -------------------------------------------------------------*/
/**
 * @var CPU_RunTime
 * @brief CPU运行时间计数器，由定时器中断递增
 */
volatile uint32_t CPU_RunTime = 0UL;

/* 静态函数声明
 * -------------------------------------------------------------*/
/**
 * @brief CPU统计定时器中断处理函数
 * @details 在定时器中断中递增CPU_RunTime计数器
 * @param 无
 * @return 无
 */
static void CPU_TMR0_IRQHandler(void);

/* 函数实现
 * -------------------------------------------------------------*/
/**
 * @brief CPU统计定时器中断处理函数
 * @details 在定时器中断中递增CPU_RunTime计数器，用于计算CPU运行时间
 */
static void CPU_TMR0_IRQHandler(void)
{
    if (TMR0_GetStatus(CPU_TMR0_UNIT, CPU_TMR0_CH_FLAG) == SET) {
        TMR0_ClearStatus(CPU_TMR0_UNIT, CPU_TMR0_CH_FLAG);
        CPU_RunTime++;
    }
}

/**
 * @brief 初始化CPU使用率统计
 * @details 初始化CPU使用率统计所需的定时器及中断配置
 */
void configureCpuTimer(void)
{
    stc_tmr0_init_t stcTmr0Init;
    stc_irq_signin_config_t stcIrqSignConfig;

    /* 使能 timer0 时钟 */
    FCG_Fcg2PeriphClockCmd(CPU_TMR0_CLK, ENABLE);
    /* LRC 振荡器开启 */
    (void)CLK_LrcCmd(ENABLE);

    /* 配置 timer0 */
    (void)TMR0_StructInit(&stcTmr0Init);
    stcTmr0Init.u32ClockSrc = TMR0_CLK_SRC_LRC;
    stcTmr0Init.u32ClockDiv = TMR0_CLK_DIV1;
    stcTmr0Init.u32Func = TMR0_FUNC_CMP;
    stcTmr0Init.u16CompareValue = CPU_TMR0_PERIOD_VALUE;
    (void)TMR0_Init(CPU_TMR0_UNIT, CPU_TMR0_CH, &stcTmr0Init);

    /* 异步时钟源，写入TMR0寄存器需要等待三个异步时钟。 */
    DDL_DelayMS(3U);
    TMR0_IntCmd(CPU_TMR0_UNIT, CPU_TMR0_CH_INT, ENABLE);

    /* 配置中断 */
    stcIrqSignConfig.enIntSrc = CPU_TMR0_INT_SRC;
    stcIrqSignConfig.enIRQn = CPU_TMR0_IRQn;
    stcIrqSignConfig.pfnCallback = &CPU_TMR0_IRQHandler;
    (void)INTC_IrqSignIn(&stcIrqSignConfig);
    NVIC_ClearPendingIRQ(stcIrqSignConfig.enIRQn);
    NVIC_SetPriority(stcIrqSignConfig.enIRQn, DDL_IRQ_PRIO_DEFAULT);
    NVIC_EnableIRQ(stcIrqSignConfig.enIRQn);
}

/**
 * @brief 开始统计CPU使用率
 * @details 启动定时器开始收集CPU使用率数据
 */
void startCpuUsageStatistics(void)
{
    /* 启动定时器 */
    TMR0_Start(CPU_TMR0_UNIT, CPU_TMR0_CH);
    /* 异步时钟源，写入TMR0寄存器需要等待6个异步时钟（例程写法） */
    DDL_DelayMS(1U);
}