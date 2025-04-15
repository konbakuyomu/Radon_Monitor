/**
 * @file cpuUtilization.c
 * @brief CPU使用率统计功能实现
 * @date 2025-03-07
 * @details 实现CPU使用率统计的核心功能，包括定时器配置、中断处理和使用率计算。
 *          本模块使用定时器产生高频中断来精确计算系统CPU负载情况，为系统
 *          运行状态监控提供数据支持。
 */

#include "cpuUtilization.h"

/* 常量定义
 * -------------------------------------------------------------*/
/**
 * @defgroup CPU_Constants CPU使用率统计常量定义
 * @brief CPU使用率统计相关的常量
 * @{
 */
/**
 * @def TIMER_PERIOD_VALUE
 * @brief CPU统计使用的定时器比较值
 * @details Timer0 定时器的时钟源是 LRC 振荡器，频率为 32.768kHz。
 *          计算公式: Frequency = (Clock freq / div) / (Compare value + 1)
 *          当前频率 = (32768 / 1) / (2 + 1) = 10.922kHz > 10kHz。
 *          用于统计CPU使用率的高速定时器必须10倍于FreeRTOS的系统中断时间（默认是1ms）。
 *          因此定时器的中断时间至少在100us以内，等价于10kHZ频率。
 */
static const uint32_t TIMER_PERIOD_VALUE = 2UL;
/**
 * @}
 */

/* 全局变量
 * -------------------------------------------------------------*/
/**
 * @defgroup CPU_Global_Variables_Impl CPU使用率统计全局变量实现
 * @brief CPU使用率统计相关的全局变量实现
 * @{
 */
/**
 * @var g_processorRuntimeCounter
 * @brief CPU运行时间计数器，由定时器中断递增
 * @details 用于记录CPU总运行时间，每次定时器中断时递增，可用于计算
 *          系统的CPU使用率。
 */
volatile uint32_t g_processorRuntimeCounter = 0UL;
/**
 * @}
 */

/* 静态函数声明
 * -------------------------------------------------------------*/
/**
 * @defgroup CPU_Static_Functions CPU使用率统计静态函数
 * @brief 仅在本文件内部使用的静态函数
 * @{
 */
/**
 * @brief CPU统计定时器中断处理函数
 * @details 在定时器中断中递增g_processorRuntimeCounter计数器，用于CPU使用率计算
 */
static void processorTimerInterruptHandler(void);
/**
 * @}
 */

/* 函数实现
 * -------------------------------------------------------------*/
/**
 * @addtogroup CPU_Static_Functions
 * @{
 */
/**
 * @brief CPU统计定时器中断处理函数
 * @details 在定时器中断中递增g_processorRuntimeCounter计数器，用于计算CPU运行时间。
 *          该函数作为定时器中断的回调函数被调用。
 */
static void processorTimerInterruptHandler(void)
{
    if (TMR0_GetStatus(CPU_TIMER_UNIT, CPU_TIMER_CHANNEL_FLAG) == SET) {
        TMR0_ClearStatus(CPU_TIMER_UNIT, CPU_TIMER_CHANNEL_FLAG);
        g_processorRuntimeCounter++;
    }
}
/**
 * @}
 */

/**
 * @addtogroup CPU_Functions
 * @{
 */
/**
 * @brief 初始化CPU使用率统计
 * @details 初始化CPU使用率统计所需的定时器及中断配置。具体步骤包括：
 *          1. 启用Timer0时钟和LRC振荡器
 *          2. 配置Timer0为比较功能模式
 *          3. 设置定时器比较值，确保中断频率>10kHz
 *          4. 配置定时器中断并设置中断优先级
 */
void configureCpuTimer(void)
{
    stc_tmr0_init_t timerInitConfig;
    stc_irq_signin_config_t interruptSignConfig;

    /* 使能 timer0 时钟 */
    FCG_Fcg2PeriphClockCmd(CPU_TIMER_CLOCK, ENABLE);
    /* LRC 振荡器开启 */
    (void)CLK_LrcCmd(ENABLE);

    /* 配置 timer0 */
    (void)TMR0_StructInit(&timerInitConfig);
    timerInitConfig.u32ClockSrc = TMR0_CLK_SRC_LRC;
    timerInitConfig.u32ClockDiv = TMR0_CLK_DIV1;
    timerInitConfig.u32Func = TMR0_FUNC_CMP;
    timerInitConfig.u16CompareValue = TIMER_PERIOD_VALUE;
    (void)TMR0_Init(CPU_TIMER_UNIT, CPU_TIMER_CHANNEL, &timerInitConfig);

    /* 异步时钟源，写入TMR0寄存器需要等待三个异步时钟 */
    DDL_DelayMS(3U);
    TMR0_IntCmd(CPU_TIMER_UNIT, CPU_TIMER_CHANNEL_INTERRUPT, ENABLE);

    /* 配置中断 */
    interruptSignConfig.enIntSrc = CPU_TIMER_INTERRUPT_SOURCE;
    interruptSignConfig.enIRQn = CPU_TIMER_IRQ_NUMBER;
    interruptSignConfig.pfnCallback = &processorTimerInterruptHandler;
    (void)INTC_IrqSignIn(&interruptSignConfig);
    NVIC_ClearPendingIRQ(interruptSignConfig.enIRQn);
    NVIC_SetPriority(interruptSignConfig.enIRQn, DDL_IRQ_PRIO_DEFAULT);
    NVIC_EnableIRQ(interruptSignConfig.enIRQn);
}

/**
 * @brief 开始统计CPU使用率
 * @details 启动定时器开始收集CPU使用率数据。调用此函数后，
 *          g_processorRuntimeCounter计数器将开始递增，可用于计算CPU使用率。
 *          本函数应在系统初始化完成后调用。
 */
void startCpuUsageStatistics(void)
{
    /* 启动定时器 */
    TMR0_Start(CPU_TIMER_UNIT, CPU_TIMER_CHANNEL);
    /* 异步时钟源，写入TMR0寄存器需要等待异步时钟稳定 */
    DDL_DelayMS(1U);
}
/**
 * @}
 */
