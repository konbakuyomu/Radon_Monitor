/**
 * @file usart1Config.h
 * @brief USART1外设配置和初始化的头文件
 * @date 2025-02-11
 * @details 本文件定义了USART1相关的端口、引脚、DMA通道、中断源等配置，
 *          以及USART1初始化函数的声明。
 */

#ifndef __USART1_CONFIG_H__
#define __USART1_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

/* 头文件
 * -------------------------------------------------------------*/
#include "hc32_ll.h"


/**
 * @defgroup USART1_Peripheral_Definition USART1外设定义
 * @{
 */
#define USART1_UNIT (CM_USART1)
/**
 * @}
 */

/**
 * @defgroup USART1_RX_DMA_Definition USART1接收DMA定义
 * @{
 */
#define USART1_RX_DMA_UNIT         (CM_DMA1)
#define USART1_RX_DMA_CH           (DMA_CH0)
#define USART1_RX_DMA_TRIG_SEL     (AOS_DMA1_0)
#define USART1_RX_DMA_TRIG_EVT_SRC (EVT_SRC_USART1_RI)
#define USART1_RX_DMA_TC_INT       (DMA_INT_TC_CH0)
#define USART1_RX_DMA_TC_FLAG      (DMA_FLAG_TC_CH0)
#define USART1_RX_DMA_TC_IRQn      (INT002_IRQn)
#define USART1_RX_DMA_TC_INT_SRC   (INT_SRC_DMA1_TC0)
/**
 * @}
 */

/**
 * @defgroup USART1_RX_GPIO_Definition USART1接收GPIO定义
 * @{
 */
#define USART1_RX_PORT      (GPIO_PORT_C)
#define USART1_RX_PIN       (GPIO_PIN_04)
#define USART1_RX_GPIO_FUNC (GPIO_FUNC_33)
/**
 * @}
 */

/**
 * @defgroup USART1_RX_IRQ_Definition USART1接收中断定义
 * @{
 */
#define USART1_RX_ERR_IRQn        (INT003_IRQn)
#define USART1_RX_ERR_INT_SRC     (INT_SRC_USART1_EI)  // USART1接收错误中断
#define USART1_RX_TIMEOUT_IRQn    (INT004_IRQn)
#define USART1_RX_TIMEOUT_INT_SRC (INT_SRC_USART1_RTO) // USART1接收超时中断
#define USART1_RX_FULL_IRQn       (INT005_IRQn)
#define USART1_RX_FULL_INT_SRC    (INT_SRC_USART1_RI)  // USART1接收完成中断
/**
 * @}
 */

/**
 * @defgroup USART1_TX_DMA_Definition USART1发送DMA定义
 * @{
 */
#define USART1_TX_DMA_UNIT         (CM_DMA1)
#define USART1_TX_DMA_CH           (DMA_CH1)
#define USART1_TX_DMA_TRIG_SEL     (AOS_DMA1_1)
#define USART1_TX_DMA_TRIG_EVT_SRC (EVT_SRC_USART1_TI)
#define USART1_TX_DMA_TC_INT       (DMA_INT_TC_CH1)
#define USART1_TX_DMA_TC_FLAG      (DMA_FLAG_TC_CH1)
#define USART1_TX_DMA_TC_IRQn      (INT006_IRQn)
#define USART1_TX_DMA_TC_INT_SRC   (INT_SRC_DMA1_TC1)
/**
 * @}
 */

/**
 * @defgroup USART1_TX_GPIO_Definition USART1发送GPIO定义
 * @{
 */
#define USART1_TX_PORT      (GPIO_PORT_C)
#define USART1_TX_PIN       (GPIO_PIN_05)
#define USART1_TX_GPIO_FUNC (GPIO_FUNC_32)
/**
 * @}
 */

/**
 * @defgroup USART1_TX_IRQ_Definition USART1发送中断定义
 * @{
 */
#define USART1_TX_CPLT_IRQn    (INT007_IRQn)
#define USART1_TX_CPLT_INT_SRC (INT_SRC_USART1_TCI) // USART1发送完成中断
/**
 * @}
 */

/**
 * @defgroup USART1_TMR0_Definition USART1定时器定义
 * @{
 */
#define USART1_TMR0_UNIT (CM_TMR0_1)
#define USART1_TMR0_CLK  (FCG2_PERIPH_TMR0_1)
#define USART1_TMR0_CH   (TMR0_CH_A)
/**
 * @}
 */

/* 函数声明
 * -------------------------------------------------------------*/

/**
 * @brief 使能USART1功能
 * @details 使能USART1的接收、接收中断、接收超时和接收超时中断功能
 */
static inline void USART1_FunctionEnable(void)
{
    USART_FuncCmd(
        USART1_UNIT,
        (USART_RX | USART_INT_RX | USART_RX_TIMEOUT | USART_INT_RX_TIMEOUT),
        ENABLE);
}

/**
 * @brief 初始化USART1外设
 * @details 配置USART1的GPIO、DMA、中断和定时器，并使能USART1功能
 */
void USART1_Init(void);

/**
 * @brief 通过USART1发送数据
 * @param [in] data 指向要发送数据的指针
 * @param [in] length 要发送的数据长度（字节数）
 * @details 使用DMA方式通过USART1发送指定长度的数据
 */
void USART1_Send(uint8_t* data, uint16_t length);

#ifdef __cplusplus
}
#endif

#endif /* __USART1_CONFIG_H__ */
