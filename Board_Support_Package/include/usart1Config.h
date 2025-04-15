/**
 * @file usart1Config.h
 * @brief USART1外设配置和初始化的头文件
 * @date 2025-02-11
 * @details 本文件定义了USART1相关的端口、引脚、DMA通道、中断源等配置，
 *          以及USART1初始化函数的声明。
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
 * @defgroup USART1_Peripheral_Definition USART1外设定义
 * @brief USART1外设相关的常量定义
 * @{
 */
/** @brief USART1控制单元 */
#define USART1_UNIT (CM_USART1)
/**
 * @}
 */

/**
 * @defgroup USART1_RX_DMA_Definition USART1接收DMA定义
 * @brief USART1接收DMA相关的常量定义
 * @{
 */
/** @brief USART1接收DMA单元 */
#define USART1_RX_DMA_UNIT (CM_DMA1)
/** @brief USART1接收DMA通道 */
#define USART1_RX_DMA_CH (DMA_CH0)
/** @brief USART1接收DMA触发选择器 */
#define USART1_RX_DMA_TRIG_SEL (AOS_DMA1_0)
/** @brief USART1接收DMA触发事件源 */
#define USART1_RX_DMA_TRIG_EVT_SRC (EVT_SRC_USART1_RI)
/** @brief USART1接收DMA传输完成中断 */
#define USART1_RX_DMA_TC_INT (DMA_INT_TC_CH0)
/** @brief USART1接收DMA传输完成标志 */
#define USART1_RX_DMA_TC_FLAG (DMA_FLAG_TC_CH0)
/** @brief USART1接收DMA传输完成中断号 */
#define USART1_RX_DMA_TC_IRQn (INT002_IRQn)
/** @brief USART1接收DMA传输完成中断源 */
#define USART1_RX_DMA_TC_INT_SRC (INT_SRC_DMA1_TC0)
/**
 * @}
 */

/**
 * @defgroup USART1_RX_GPIO_Definition USART1接收GPIO定义
 * @brief USART1接收GPIO相关的常量定义
 * @{
 */
/** @brief USART1接收端口 */
#define USART1_RX_PORT (GPIO_PORT_C)
/** @brief USART1接收引脚 */
#define USART1_RX_PIN (GPIO_PIN_04)
/** @brief USART1接收GPIO功能 */
#define USART1_RX_GPIO_FUNC (GPIO_FUNC_33)
/**
 * @}
 */

/**
 * @defgroup USART1_RX_IRQ_Definition USART1接收中断定义
 * @brief USART1接收中断相关的常量定义
 * @{
 */
/** @brief USART1接收错误中断号 */
#define USART1_RX_ERR_IRQn (INT003_IRQn)
/** @brief USART1接收错误中断源 */
#define USART1_RX_ERR_INT_SRC (INT_SRC_USART1_EI) // USART1接收错误中断
/** @brief USART1接收超时中断号 */
#define USART1_RX_TIMEOUT_IRQn (INT004_IRQn)
/** @brief USART1接收超时中断源 */
#define USART1_RX_TIMEOUT_INT_SRC (INT_SRC_USART1_RTO) // USART1接收超时中断
/** @brief USART1接收完成中断号 */
#define USART1_RX_FULL_IRQn (INT005_IRQn)
/** @brief USART1接收完成中断源 */
#define USART1_RX_FULL_INT_SRC (INT_SRC_USART1_RI) // USART1接收完成中断
/**
 * @}
 */

/**
 * @defgroup USART1_TX_DMA_Definition USART1发送DMA定义
 * @brief USART1发送DMA相关的常量定义
 * @{
 */
/** @brief USART1发送DMA单元 */
#define USART1_TX_DMA_UNIT (CM_DMA1)
/** @brief USART1发送DMA通道 */
#define USART1_TX_DMA_CH (DMA_CH1)
/** @brief USART1发送DMA触发选择器 */
#define USART1_TX_DMA_TRIG_SEL (AOS_DMA1_1)
/** @brief USART1发送DMA触发事件源 */
#define USART1_TX_DMA_TRIG_EVT_SRC (EVT_SRC_USART1_TI)
/** @brief USART1发送DMA传输完成中断 */
#define USART1_TX_DMA_TC_INT (DMA_INT_TC_CH1)
/** @brief USART1发送DMA传输完成标志 */
#define USART1_TX_DMA_TC_FLAG (DMA_FLAG_TC_CH1)
/** @brief USART1发送DMA传输完成中断号 */
#define USART1_TX_DMA_TC_IRQn (INT006_IRQn)
/** @brief USART1发送DMA传输完成中断源 */
#define USART1_TX_DMA_TC_INT_SRC (INT_SRC_DMA1_TC1)
/**
 * @}
 */

/**
 * @defgroup USART1_TX_GPIO_Definition USART1发送GPIO定义
 * @brief USART1发送GPIO相关的常量定义
 * @{
 */
/** @brief USART1发送端口 */
#define USART1_TX_PORT (GPIO_PORT_C)
/** @brief USART1发送引脚 */
#define USART1_TX_PIN (GPIO_PIN_05)
/** @brief USART1发送GPIO功能 */
#define USART1_TX_GPIO_FUNC (GPIO_FUNC_32)
/**
 * @}
 */

/**
 * @defgroup USART1_TX_IRQ_Definition USART1发送中断定义
 * @brief USART1发送中断相关的常量定义
 * @{
 */
/** @brief USART1发送完成中断号 */
#define USART1_TX_CPLT_IRQn (INT007_IRQn)
/** @brief USART1发送完成中断源 */
#define USART1_TX_CPLT_INT_SRC (INT_SRC_USART1_TCI) // USART1发送完成中断
/**
 * @}
 */

/**
 * @defgroup USART1_TMR0_Definition USART1定时器定义
 * @brief USART1定时器相关的常量定义
 * @{
 */
/** @brief USART1定时器单元 */
#define USART1_TMR0_UNIT (CM_TMR0_1)
/** @brief USART1定时器时钟 */
#define USART1_TMR0_CLK (FCG2_PERIPH_TMR0_1)
/** @brief USART1定时器通道 */
#define USART1_TMR0_CH (TMR0_CH_A)
/**
 * @}
 */

/* 函数声明
 * -------------------------------------------------------------*/

/**
 * @brief 使能USART1功能
 * @details 使能USART1的接收、接收中断、接收超时和接收超时中断功能
 * @return 无
 */
static inline void enableUsart1Functions(void)
{
    USART_FuncCmd(
        USART1_UNIT, (USART_RX | USART_INT_RX | USART_RX_TIMEOUT | USART_INT_RX_TIMEOUT), ENABLE);
}

/**
 * @brief 初始化USART1外设
 * @details 配置USART1的GPIO、DMA、中断和定时器，并使能USART1功能
 * @return 无
 */
void initializeUsart1(void);

/**
 * @brief 通过USART1发送数据
 * @param [in] data 指向要发送数据的指针
 * @param [in] length 要发送的数据长度（字节数）
 * @details 使用DMA方式通过USART1发送指定长度的数据
 * @return 无
 */
void sendDataViaUsart1(uint8_t* data, uint16_t length);

#ifdef __cplusplus
}
#endif
