/**
 * @file keyConfig.h
 * @brief 按键配置和控制接口头文件
 * @date 2025-03-07
 * @details 提供按键初始化和配置的接口函数，支持多个按键的独立控制和定时器配置
 */

#ifndef KEYCONFIG_H
#define KEYCONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* 头文件
 * -------------------------------------------------------------*/
#include "hc32_ll.h"

/* 宏定义
 * -------------------------------------------------------------*/

/**
 * @defgroup KEY_TMRA_Definitions 按键定时器相关定义
 * @brief 按键扫描定时器配置相关宏定义
 * @{
 */
/** @brief 按键定时器单元 */
#define KEY_TMRA_UNIT (CM_TMRA_1)
/** @brief 按键定时器时钟 */
#define KEY_TMRA_CLK (FCG2_PERIPH_TMRA_1)
/** @brief 按键定时器中断 */
#define KEY_TMRA_INT_TYPE (TMRA_INT_OVF)
/** @brief 按键定时器标志 */
#define KEY_TMRA_INT_FLAG (TMRA_FLAG_OVF)
/** @brief 按键定时器中断源 */
#define KEY_TMRA_INT_SRC (INT_SRC_TMRA_1_OVF)
/** @brief 按键定时器IRQ编号 */
#define KEY_TMRA_IRQn (INT001_IRQn)
/** @brief 按键定时器时钟分频 */
#define KEY_TMRA_CLK_DIV (TMRA_CLK_DIV2)
/**
 * @}
 */

/**
 * @defgroup KEY_GPIO_Definitions 按键GPIO定义
 * @brief 按键对应的GPIO端口和引脚定义
 * @{
 */
/** @brief 上按键端口 */
#define KEY_UP_PORT (GPIO_PORT_C)
/** @brief 上按键引脚 */
#define KEY_UP_PIN (GPIO_PIN_07)

/** @brief 下按键端口 */
#define KEY_DOWN_PORT (GPIO_PORT_C)
/** @brief 下按键引脚 */
#define KEY_DOWN_PIN (GPIO_PIN_06)

/** @brief 右按键端口 */
#define KEY_RIGHT_PORT (GPIO_PORT_D)
/** @brief 右按键引脚 */
#define KEY_RIGHT_PIN (GPIO_PIN_15)

/** @brief 确认按键端口 */
#define KEY_CONFIRM_PORT (GPIO_PORT_D)
/** @brief 确认按键引脚 */
#define KEY_CONFIRM_PIN (GPIO_PIN_14)
/**
 * @}
 */

/* 枚举
 * -------------------------------------------------------------*/

/**
 * @enum KeyButton
 * @brief 按键编号枚举定义
 */
typedef enum {
    KEY_BUTTON_1 = 1,    /**< 上按键 */
    KEY_BUTTON_2 = 2,    /**< 下按键 */
    KEY_BUTTON_3 = 3,    /**< 右按键 */
    KEY_BUTTON_4 = 4,    /**< 确认按键 */
    KEY_BUTTON_ERROR = 5 /**< 错误按键 */
} KeyButton;

/* 函数声明
 * -------------------------------------------------------------*/

/**
 * @brief 配置按键扫描定时器
 * @details 初始化TMRA用于按键扫描，配置中断和回调函数
 * @return 无
 */
void configureKeyTimer(void);

/**
 * @brief 启动按键扫描定时器
 * @details 启动TMRA开始按键扫描
 * @return 无
 */
void startKeyTimer(void);

/**
 * @brief 初始化按键GPIO配置
 * @details 配置所有按键GPIO为输入模式
 * @return 无
 */
void initializeKeyGpio(void);

/**
 * @brief 初始化按键配置并注册按键读取函数
 * @details 将按键读取函数注册到按键处理模块，并初始化所有按键
 * @return 无
 */
void initializeKeyConfiguration(void);

#ifdef __cplusplus
}
#endif

#endif /* KEYCONFIG_H */