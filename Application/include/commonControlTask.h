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

#include <stddef.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

/* 函数声明
 * -------------------------------------------------------------*/

/**
 * @var TaskHandle_t commonControlTaskHandle
 * @brief 通用控制任务句柄，用于控制和引用通用控制任务
 */
extern TaskHandle_t commonControlTaskHandle;

/**
 * @brief 通用控制任务函数，处理LED控制等通用功能
 * @param [in] pvParameters FreeRTOS任务参数
 *        This parameter is not used in this task
 * @return 无返回值
 * @note 任务会一直运行，等待并处理消息队列中的消息
 */
void commonControlTask(void* pvParameters);

#ifdef __cplusplus
}
#endif
