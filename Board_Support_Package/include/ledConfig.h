/**
 * @file ledConfig.h
 * @brief LED配置和控制接口头文件
 * @author konbakuyomu
 * @date 2025-03-07
 * @details 提供LED初始化和控制的接口函数，支持多个LED的独立或组合控制
 */

#ifndef LED_CONFIG_H
#define LED_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* 头文件
 * -------------------------------------------------------------*/
#include "ev_hc32f460_lqfp100_v2.h"

/* 宏定义
 * -------------------------------------------------------------*/

/**
 * @defgroup LED_Selection LED选择定义
 * @brief LED选择标识符，用于指定要操作的LED
 * @{
 */
/** @brief 状态LED标识符 */
extern const uint8_t LED_STATUS;
/** @brief 网络LED标识符 */
extern const uint8_t LED_NETWORK;
/** @brief 故障LED标识符 */
extern const uint8_t LED_FAULT;
/** @brief 报警LED标识符 */
extern const uint8_t LED_ALARM;
/** @brief 所有LED的组合标识符 */
extern const uint8_t STATUS_LED_ALL;
/**
 * @}
 */

/* 函数声明
 * -------------------------------------------------------------*/

/**
 * @brief 初始化所有LED
 * @details 配置LED引脚为输出模式，并将所有LED初始状态设置为关闭
 * @return 无
 */
void ledConfigInit(void);

/**
 * @brief 打开指定的LED
 * @param [in] ledSelection 要打开的LED选择
 *                         - LED_STATUS: 状态LED
 *                         - LED_NETWORK: 网络LED
 *                         - LED_FAULT: 故障LED
 *                         - LED_ALARM: 报警LED
 *                         - STATUS_LED_ALL: 所有LED
 * @return 无
 */
void ledTurnOn(uint8_t ledSelection);

/**
 * @brief 关闭指定的LED
 * @param [in] ledSelection 要关闭的LED选择
 *                         - LED_STATUS: 状态LED
 *                         - LED_NETWORK: 网络LED
 *                         - LED_FAULT: 故障LED
 *                         - LED_ALARM: 报警LED
 *                         - STATUS_LED_ALL: 所有LED
 * @return 无
 */
void ledTurnOff(uint8_t ledSelection);

/**
 * @brief 切换指定的LED状态
 * @param [in] ledSelection 要切换的LED选择
 *                         - LED_STATUS: 状态LED
 *                         - LED_NETWORK: 网络LED
 *                         - LED_FAULT: 故障LED
 *                         - LED_ALARM: 报警LED
 *                         - STATUS_LED_ALL: 所有LED
 * @return 无
 */
void ledToggle(uint8_t ledSelection);

#ifdef __cplusplus
}
#endif

#endif /* LED_CONFIG_H */