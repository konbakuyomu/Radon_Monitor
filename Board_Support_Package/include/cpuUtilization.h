/**
 * @file cpuUtilization.h
 * @brief CPU使用率统计功能头文件
 * @date 2025-03-07
 * @details 提供CPU使用率统计所需的接口函数和配置参数，
 *          用于实时监控系统CPU负载情况
 */

#ifndef CPU_UTILIZATION_H
#define CPU_UTILIZATION_H

#ifdef __cplusplus
extern "C" {
#endif

/* 头文件
 * -------------------------------------------------------------*/
#include "hc32_ll.h"

/* 宏定义
 * -------------------------------------------------------------*/
 
/**
 * @defgroup CPU_TMR0_Definitions CPU使用率统计定时器配置
 * @{
 */
/** @brief CPU统计使用的定时器单元 */
#define CPU_TMR0_UNIT    (CM_TMR0_2)
/** @brief CPU统计使用的定时器时钟 */
#define CPU_TMR0_CLK     (FCG2_PERIPH_TMR0_2)
/** @brief CPU统计使用的定时器通道 */
#define CPU_TMR0_CH      (TMR0_CH_B)
/** @brief CPU统计使用的定时器通道中断 */
#define CPU_TMR0_CH_INT  (TMR0_INT_CMP_B)
/** @brief CPU统计使用的定时器通道标志 */
#define CPU_TMR0_CH_FLAG (TMR0_FLAG_CMP_B)
/** @brief CPU统计使用的定时器中断源 */
#define CPU_TMR0_INT_SRC (INT_SRC_TMR0_2_CMP_B)
/** @brief CPU统计使用的定时器中断号 */
#define CPU_TMR0_IRQn    (INT000_IRQn)
/**
 * @}
 */

/* 全局变量
 * -------------------------------------------------------------*/
/**
 * @var CPU_RunTime
 * @brief CPU运行时间计数器
 */
extern volatile uint32_t CPU_RunTime;

/* 函数声明
 * -------------------------------------------------------------*/
/**
 * @brief 初始化CPU使用率统计
 * @details 初始化CPU使用率统计所需的定时器及中断
 * @return 无
 */
void configureCpuTimer(void);

/**
 * @brief 开始统计CPU使用率
 * @details 开始统计CPU使用率，启动定时器计数
 * @return 无
 */
void startCpuUsageStatistics(void);

#ifdef __cplusplus
}
#endif

#endif /* CPU_UTILIZATION_H */