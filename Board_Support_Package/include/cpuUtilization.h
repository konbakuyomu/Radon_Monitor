/**
 * @file cpuUtilization.h
 * @brief CPU使用率统计功能头文件
 * @date 2025-03-07
 * @details 提供CPU使用率统计所需的接口函数和配置参数，
 *          用于实时监控系统CPU负载情况。该模块使用定时器
 *          产生高频率中断来精确计算系统CPU使用情况。
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* 头文件
 * -------------------------------------------------------------*/
#include "hc32_ll.h"

/* 宏定义
 * -------------------------------------------------------------*/
/**
 * @defgroup CPU_Timer_Configuration CPU使用率统计定时器配置
 * @brief CPU使用率统计相关的定时器配置参数
 * @{
 */
/** @brief CPU统计使用的定时器单元 */
#define CPU_TIMER_UNIT (CM_TMR0_2)
/** @brief CPU统计使用的定时器时钟 */
#define CPU_TIMER_CLOCK (FCG2_PERIPH_TMR0_2)
/** @brief CPU统计使用的定时器通道 */
#define CPU_TIMER_CHANNEL (TMR0_CH_B)
/** @brief CPU统计使用的定时器通道中断 */
#define CPU_TIMER_CHANNEL_INTERRUPT (TMR0_INT_CMP_B)
/** @brief CPU统计使用的定时器通道标志 */
#define CPU_TIMER_CHANNEL_FLAG (TMR0_FLAG_CMP_B)
/** @brief CPU统计使用的定时器中断源 */
#define CPU_TIMER_INTERRUPT_SOURCE (INT_SRC_TMR0_2_CMP_B)
/** @brief CPU统计使用的定时器中断号 */
#define CPU_TIMER_IRQ_NUMBER (INT000_IRQn)
/**
 * @}
 */

/* 全局变量
 * -------------------------------------------------------------*/
/**
 * @defgroup CPU_Global_Variables CPU使用率统计全局变量
 * @brief CPU使用率统计相关的全局变量定义
 * @{
 */
/**
 * @var g_processorRuntimeCounter
 * @brief CPU运行时间计数器
 * @details 由定时器中断递增，用于记录CPU总运行时间。此变量由
 *          定时器中断更新，用户代码不应直接修改此变量。
 */
extern volatile uint32_t g_processorRuntimeCounter;
/**
 * @}
 */

/* 函数声明
 * -------------------------------------------------------------*/
/**
 * @defgroup CPU_Functions CPU使用率统计功能函数
 * @brief CPU使用率统计相关的接口函数
 * @{
 */

/**
 * @brief 初始化CPU使用率统计
 * @details 初始化CPU使用率统计所需的定时器及中断。配置Timer0
 *          定时器产生高频中断（>10kHz），用于精确统计CPU运行时间。
 * @return 无
 * @note 此函数应在系统初始化阶段调用
 */
void configureCpuTimer(void);

/**
 * @brief 开始统计CPU使用率
 * @details 启动定时器开始收集CPU使用率数据。调用此函数后，
 *          g_processorRuntimeCounter计数器将开始递增，可用于计算CPU使用率。
 * @return 无
 * @note 应在所有任务启动后调用此函数
 */
void startCpuUsageStatistics(void);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif
