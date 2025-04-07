#ifndef BEAT_TIMER_CONFIG_H
#define BEAT_TIMER_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* 头文件
 * -------------------------------------------------------------*/
#include "hc32_ll.h"

/* 宏定义
 * -------------------------------------------------------------*/
/**
 * @defgroup BEAT_TMRA_Config 心跳定时器配置参数
 * @brief 心跳定时器相关的配置参数和常量定义
 * @{
 */
#define BEAT_TMRA_UNIT     (CM_TMRA_2)          /**< 使用的TMRA单元 */
#define BEAT_TMRA_CLK      (FCG2_PERIPH_TMRA_2) /**< TMRA时钟外设 */
#define BEAT_TMRA_INT_TYPE (TMRA_INT_OVF)       /**< TMRA中断类型 */
#define BEAT_TMRA_INT_FLAG (TMRA_FLAG_OVF)      /**< TMRA中断标志 */
#define BEAT_TMRA_INT_SRC  (INT_SRC_TMRA_2_OVF) /**< TMRA中断源 */
#define BEAT_TMRA_IRQn     (INT008_IRQn)        /**< TMRA中断号 */
#define BEAT_TMRA_CLK_DIV  (TMRA_CLK_DIV2)      /**< TMRA时钟分频 */
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
void configureBeatTimer(void);

/**
 * @brief 启动心跳定时器
 * @details 启动配置好的TMRA定时器，开始提供系统心跳，同时恢复心跳任务
 * @return 无
 */
void startBeatTimer(void);

/**
 * @brief 注册定时任务互斥锁回调函数
 * @details 创建互斥锁和心跳任务，并将互斥锁回调函数注册到btask库
 * @return 无
 */
void registerBeatTaskMutex(void);

#ifdef __cplusplus
}
#endif

#endif /* BEAT_TIMER_CONFIG_H */