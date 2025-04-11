/**
 * @file globalConfig.h
 * @brief 全局变量和头文件包含的中转总集
 * @author konbakuyomu
 * @date 2025-02-11
 * @details 本文件作为项目的中央头文件，包含了所有常用的标准库、
 *          驱动库、FreeRTOS相关头文件，以及全局变量的声明。
 *          它起到了一个中转站的作用，方便其他源文件统一包含所需的头文件和全局变量。
 */

#ifndef __GLOBAL_CONFIG_H__
#define __GLOBAL_CONFIG_H__

/* C 语言兼容区域开始
 * xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx*/
#ifdef __cplusplus
extern "C" {
#endif

/* 头文件
 * -------------------------------------------------------------*/

// 标准库头文件
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// HC32F460驱动库头文件
#include "ev_hc32f460_lqfp100_v2_bsp.h"
#include "hc32_ll.h"

// FreeRTOS头文件
#include "FreeRTOS.h"
#include "event_groups.h"
#include "queue.h"
#include "semphr.h"
#include "stream_buffer.h"
#include "task.h"
#include "timers.h"

// qf_lib头文件
#include "beatTask.h"
#include "button.h"
#include "keyValueTransation.h"

// 板级支持包头文件
#include "beatTaskConfig.h"
#include "cpuUtilization.h"
#include "keyConfig.h"
#include "ledConfig.h"
#include "usart1Config.h"

// 硬件抽象层头文件
#include "halTypes.h"
#include "drvHal.h"
#include "hal.h"
#include "taskFactory.h"
#include "message_bus.h"
#include "usart1Driver.h"
#include "commonDriver.h"

// 应用层头文件
#include "usartTask.h"
#include "buttonControl.h"
#include "commonControlTask.h"

/* 枚举
 * -------------------------------------------------------------*/

/**
 * @defgroup Global_Enums 全局枚举定义
 * @brief 全局枚举的声明
 * @{
 */

/* 宏定义
 * -------------------------------------------------------------*/

/** @brief 初始化任务堆栈大小 */
#define INIT_TASK_STACK_SIZE configMINIMAL_STACK_SIZE
/** @brief 小型任务堆栈大小 */
#define SMALL_TASK_STACK_SIZE configMINIMAL_STACK_SIZE
/** @brief 中型任务堆栈大小 */
#define MEDIUM_TASK_STACK_SIZE configMINIMAL_STACK_SIZE * 2
/** @brief 大型任务堆栈大小 */
#define LARGE_TASK_STACK_SIZE configMINIMAL_STACK_SIZE * 4

/* 全局变量
 * -------------------------------------------------------------*/

/**
 * @defgroup Global_Variables 全局变量定义
 * @brief 全局变量的声明
 * @{
 */

/** @brief 初始化任务优先级 */
extern const uint8_t INIT_TASK_PRIORITY;
/** @brief 应用任务优先级 */
extern const uint8_t APP_TASK_PRIORITY;

/**
 * @}
 */

/* 结构体
 * -------------------------------------------------------------*/

/**
 * @typedef PortPinConfiguration
 * @brief 端口和引脚配置结构体
 */
typedef struct {
    uint8_t portNumber; /**< 端口号 */
    uint16_t pinNumber; /**< 引脚号 */
} PortPinConfiguration;

/* 函数声明
 * -------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif
/* C 语言兼容区域结束
 * xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx*/

/* C++ 语言兼容区域开始(这里头文件中已经有了C++和纯C的分区了，所以这里可以不用再分了)
 * xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx*/

// 软件应用层头文件
#include "commandProcessorBase.hpp"
#include "messageBusProcessor.hpp"
#include "uartProtocolProcessor.hpp"
#include "messageBus.hpp"

/* C++ 语言兼容区域结束
 * xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx*/

#endif /* __GLOBAL_CONFIG_H__ */
