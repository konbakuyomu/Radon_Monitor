/**
 * @file messageBusProcessor.h
 * @brief 消息总线命令处理器头文件
 * @date 2025-04-11
 * @details 提供针对消息总线消息的命令处理器，用于系统级消息的处理和分发
 */

#pragma once

/* 头文件
 * -------------------------------------------------------------*/
#include "baseProcessor.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * @defgroup Message_Bus_Processor 消息总线处理器模块
 * @brief 消息总线事件处理相关定义
 * @{
 */

/* 结构体
 * -------------------------------------------------------------*/
/**
 * @brief 消息总线命令处理器结构体（继承自基类）
 *        用于处理如SystemMessage等消息总线消息
 */
typedef struct {
    CommandProcessor commandProcessor; /**< 命令处理器结构体，继承命令处理器功能 */
    // 可扩展其他成员
} SystemMessageDispatcher;

/* 函数声明
 * -------------------------------------------------------------*/
/**
 * @brief 初始化消息总线命令处理器
 * @param [in,out] processor 指向处理器结构体的指针
 * @param [in] configuration 指向配置结构体的指针
 * @return 无返回值
 * @note 必须在使用前调用此函数进行初始化
 */
void SystemMessageDispatcher_initialize(SystemMessageDispatcher* processor,
                                        const CommandProcessorConfiguration* configuration);

/**
 * @brief 获取消息总线命令处理器单例（全局唯一实例）
 * @return 指向单例的指针
 * @note 推荐用单例，避免重复分配资源
 */
SystemMessageDispatcher* SystemMessageDispatcher_getInstance(void);

/**
 * @}
 */
