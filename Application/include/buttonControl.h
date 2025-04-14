/**
 * @file buttonControl.h
 * @brief 按键控制任务处理头文件
 * @date 2025-03-15
 * @details 声明按键控制相关的任务句柄和任务函数，用于处理按键输入和LED控制
 *          本文件定义了按键任务相关的接口和全局变量
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* 头文件
 * -------------------------------------------------------------*/
#include <stddef.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"

/**
 * @defgroup Button_Task 按键任务模块
 * @brief 按键任务相关定义
 * @{
 */

/**
 * @var TaskHandle_t buttonTaskHandle
 * @brief 按键任务句柄，用于控制和引用按键处理任务
 * @note 通过此句柄可以控制任务状态，如挂起、恢复等
 */
extern TaskHandle_t buttonTaskHandle;

/* 函数声明
 * -------------------------------------------------------------*/

/**
 * @brief 按键处理任务函数
 * @param [in] pvParameters FreeRTOS任务参数
 *        This parameter is not used in this task
 * @return 无返回值
 * @note 任务会一直运行，等待并处理按键输入事件
 * @details 该任务负责:
 *          - 初始化按键处理器
 *          - 监听按键事件
 *          - 调用注册的处理函数
 */
void buttonTask(void* pvParameters);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif
