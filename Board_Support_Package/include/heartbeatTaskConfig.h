/**
 * @file heartbeatTaskConfig.h
 * @brief 心跳定时器和定时任务配置头文件
 * @date 2025-04-15
 * @details 本文件定义了系统心跳定时器的配置参数和相关函数接口，
 *          用于提供系统定时任务调度的基础时钟和互斥控制。
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
 * @defgroup HEARTBEAT_TIMER_Configuration 心跳定时器配置参数
 * @brief 心跳定时器相关的配置参数和常量定义
 * @{
 */
#define HEARTBEAT_TIMER_UNIT      (CM_TMRA_2)          /**< 使用的TMRA单元 */
#define HEARTBEAT_TIMER_CLOCK     (FCG2_PERIPH_TMRA_2) /**< TMRA时钟外设 */
#define HEARTBEAT_TIMER_INT_TYPE  (TMRA_INT_OVF)       /**< TMRA中断类型 */
#define HEARTBEAT_TIMER_INT_FLAG  (TMRA_FLAG_OVF)      /**< TMRA中断标志 */
#define HEARTBEAT_TIMER_INT_SRC   (INT_SRC_TMRA_2_OVF) /**< TMRA中断源 */
#define HEARTBEAT_TIMER_IRQ_NUM   (INT008_IRQn)        /**< TMRA中断号 */
#define HEARTBEAT_TIMER_CLOCK_DIV (TMRA_CLK_DIV2)      /**< TMRA时钟分频 */
/**
 * @}
 */

/* 函数声明
 * -------------------------------------------------------------*/

/**
 * @brief 配置心跳定时器
 * @details 初始化TMRA定时器用于提供系统心跳，配置为1ms中断频率
 * @return 无
 */
void configureHeartbeatTimer(void);

/**
 * @brief 启动心跳定时器
 * @details 启动配置好的TMRA定时器，开始提供系统心跳，同时恢复心跳任务
 * @return 无
 */
void startHeartbeatTimer(void);

/**
 * @brief 注册定时任务互斥锁回调函数
 * @details 创建互斥锁和心跳任务，并将互斥锁回调函数注册到定时任务库
 * @return 无
 */
void registerHeartbeatTaskMutex(void);

#ifdef __cplusplus
}
#endif
