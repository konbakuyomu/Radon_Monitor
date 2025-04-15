/**
 * @file commonControlTask.h
 * @brief 通用控制任务处理头文件
 * @date 2025-04-11
 * @details 声明通用控制相关的任务句柄和任务函数，用于处理LED控制等通用功能
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* 头文件
 * -------------------------------------------------------------*/

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include <stddef.h>
#include <stdint.h>

/**
 * @defgroup Common_Control_Task 通用控制任务模块
 * @brief 通用控制任务相关定义
 * @{
 */

/* 全局变量
 * -------------------------------------------------------------*/
/**
 * @var TaskHandle_t commonControlTaskHandle
 * @brief 通用控制任务句柄，用于控制和引用通用控制任务
 * @note 通过此句柄可以控制任务状态，如挂起、恢复等
 */
extern TaskHandle_t commonControlTaskHandle;

/* 函数声明
 * -------------------------------------------------------------*/
/**
 * @brief 通用控制任务函数，处理LED控制等通用功能
 * @param [in] pvParameters FreeRTOS任务参数
 *        This parameter is not used in this task
 * @return 无返回值
 * @note 任务会一直运行，等待并处理消息队列中的消息
 * @details 该任务负责:
 *          - 监听消息队列
 *          - 处理LED控制等通用指令
 */
void commonControlTask(void* pvParameters);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif
