/**
 * @file commonControlProcessor.h
 * @brief 通用控制命令处理器头文件
 * @date 2025-04-11
 * @details 提供针对通用控制事件的命令处理器，包括LED控制等功能
 */

#pragma once

/* 头文件
 * -------------------------------------------------------------*/
#include "baseProcessor.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * @defgroup Common_Control_Processor 通用控制处理器模块
 * @brief 通用控制事件处理相关定义
 * @{
 */

/* 结构体
 * -------------------------------------------------------------*/
/**
 * @brief 通用控制命令处理器结构体（继承自基类）
 *        用于处理LED控制等通用事件消息
 */
typedef struct {
    CommandProcessorBaseC base; /**< 基类结构体，继承命令处理器基类功能 */
    // 可扩展其他成员
} CommonControlProcessorC;

/* 函数声明
 * -------------------------------------------------------------*/
/**
 * @brief 初始化通用控制命令处理器
 * @param [in,out] proc 指向处理器结构体的指针
 * @param [in] config 指向配置结构体的指针
 * @return 无返回值
 * @note 必须在使用前调用此函数进行初始化
 */
void CommonControlProcessorC_init(CommonControlProcessorC* proc,
                                  const CommandProcessorConfig* config);

/**
 * @brief 获取通用控制命令处理器单例（全局唯一实例）
 * @return 指向单例的指针
 */
CommonControlProcessorC* CommonControlProcessorC_getInstance(void);

/* 宏定义
 * -------------------------------------------------------------*/
/**
 * @brief 用于构建LED控制命令类型的宏定义
 * @details 将LED控制类型和LED选择组合成一个32位命令类型
 */
#define MAKE_LED_CONTROL_CMD(control, selection) (((uint32_t)(control) << 16) | (selection))

/**
 * @}
 */
