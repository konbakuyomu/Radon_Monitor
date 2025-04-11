/**
 * @file commonDriver.h
 * @brief 通用组件控制器头文件
 * @date 2025-03-20
 * @details 声明通用组件控制函数，提供对LED、PWM等基础组件的控制接口
 */

#ifndef __COMMON_DRIVER_H__
#define __COMMON_DRIVER_H__

#ifdef __cplusplus
extern "C" {
#endif

/* 头文件
 * -------------------------------------------------------------*/
#include <stddef.h>
#include <stdint.h>

/* 定义和枚举
 * -------------------------------------------------------------*/
/**
 * @enum LedControl
 * @brief LED控制命令枚举
 */
typedef enum {
    LED_CONTROL_TURN_ON,  /**< 打开LED */
    LED_CONTROL_TURN_OFF, /**< 关闭LED */
    LED_CONTROL_TOGGLE    /**< 切换LED状态 */
} LedControl_t;

/* 函数声明
 * -------------------------------------------------------------*/

/**
 * @brief 初始化通用驱动器
 * @details 在使用其他函数前必须先调用此函数
 */
void CommonDriver_Init(void);

/**
 * LED控制函数
 * --------------------------------------*/

/**
 * @brief 打开指定LED
 * @param [in] ledSelection LED选择器
 */
void CommonDriver_LedTurnOn(uint8_t ledSelection);

/**
 * @brief 关闭指定LED
 * @param [in] ledSelection LED选择器
 */
void CommonDriver_LedTurnOff(uint8_t ledSelection);

/**
 * @brief 切换指定LED状态
 * @param [in] ledSelection LED选择器
 */
void CommonDriver_LedToggle(uint8_t ledSelection);

#ifdef __cplusplus
}
#endif

#endif /* __COMMON_DRIVER_H__ */ 