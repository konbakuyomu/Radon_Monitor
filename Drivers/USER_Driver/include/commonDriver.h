/**
 * @file commonDriver.h
 * @brief 通用组件控制器头文件
 * @date 2025-04-15
 * @details 声明LED控制相关函数和类型，提供对LED的基础控制接口。
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* 头文件
 * -------------------------------------------------------------*/
#include <stddef.h>
#include <stdint.h>

/* 枚举与类型定义
 * -------------------------------------------------------------*/
/**
 * @defgroup LedControlCommand_Enum LED控制命令枚举
 * @brief LED控制命令类型
 * @{
 */
/**
 * @enum LedControlCommand
 * @brief LED控制命令
 * @var TURN_ON  打开LED
 * @var TURN_OFF 关闭LED
 * @var TOGGLE   切换LED状态
 */
typedef enum {
    TURN_ON,   /**< 打开LED */
    TURN_OFF,  /**< 关闭LED */
    TOGGLE     /**< 切换LED状态 */
} LedControlCommand;
/**
 * @}
 */

/* 函数声明
 * -------------------------------------------------------------*/
/**
 * @defgroup LedControl_API LED控制API
 * @brief LED控制相关对外API
 * @{
 */
/**
 * @brief 初始化LED控制器
 * @details 在使用其他LED控制函数前必须先调用此函数
 */
void initializeLedController(void);

/**
 * @brief 打开指定LED
 * @param [in] ledIdentifier LED标识符，支持单个或组合
 */
void turnOnLed(uint8_t ledIdentifier);

/**
 * @brief 关闭指定LED
 * @param [in] ledIdentifier LED标识符，支持单个或组合
 */
void turnOffLed(uint8_t ledIdentifier);

/**
 * @brief 切换指定LED状态
 * @param [in] ledIdentifier LED标识符，支持单个或组合
 */
void toggleLedState(uint8_t ledIdentifier);
/**
 * @}
 */

#ifdef __cplusplus
}
#endif
