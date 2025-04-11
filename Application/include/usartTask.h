/**
 * @file usartTask.hpp
 * @brief USART1任务处理头文件
 * @date 2023-07-20
 * @details 声明USART1相关的任务句柄和任务函数，用于处理串口通信
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
 * @var TaskHandle_t usart1TaskHandle
 * @brief USART1任务句柄，用于控制和引用USART1任务
 */
extern TaskHandle_t usart1TaskHandle;

/**
 * @brief USART1任务函数，处理串口1数据的收发
 * @param [in] pvParameters FreeRTOS任务参数
 *        This parameter is not used in this task
 * @return 无返回值
 * @note 任务会一直运行，等待并处理消息队列中的消息
 */
void usart1Task(void* pvParameters);

#ifdef __cplusplus
}
#endif