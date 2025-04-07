/**
 * @file buttonControl.hpp
 * @brief 按键控制任务处理头文件
 * @date 2025-03-15
 * @details 声明按键控制相关的任务句柄和任务函数，用于处理按键输入和LED控制
 */

#pragma once

/* 头文件
 * -------------------------------------------------------------*/
#ifdef __cplusplus
#include <cstddef>
#include <cstdint>
#else
#include <stddef.h>
#include <stdint.h>
#endif

#include "FreeRTOS.h"
#include "task.h"

/* 函数声明
 * -------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @var TaskHandle_t buttonTaskHandle
 * @brief 按键任务句柄，用于控制和引用按键处理任务
 */
extern TaskHandle_t buttonTaskHandle;

/**
 * @brief 按键处理任务函数，处理按键输入和LED控制
 * @param [in] pvParameters FreeRTOS任务参数
 *        This parameter is not used in this task
 * @return 无返回值
 * @note 任务会一直运行，等待并处理按键输入事件
 */
void buttonTask(void* pvParameters);

#ifdef __cplusplus
}
#endif
