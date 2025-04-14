/**
 * @file buttonProcessor.h
 * @brief 按钮命令处理器头文件
 * @date 2025-04-11
 * @details 提供针对按钮事件的命令处理器，负责处理和分发按钮相关的事件消息
 */

#pragma once

/* 头文件
 * -------------------------------------------------------------*/
#include "baseProcessor.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * @defgroup buttonProcessor 按钮处理器模块
 * @brief 按钮事件处理相关定义
 * @{
 */

/* 结构体
 * -------------------------------------------------------------*/
/**
 * @brief 按键命令处理器结构体（继承自基类）
 *        用于处理按键事件消息
 */
typedef struct {
    CommandProcessorBaseC base;  /**< 基类结构体，继承命令处理器基类功能 */
    // 可扩展其他成员
} ButtonProcessorC;

/* 函数声明
 * -------------------------------------------------------------*/
/**
 * @brief 初始化按键命令处理器
 * @param [in,out] proc 指向处理器结构体的指针
 * @param [in] config 指向配置结构体的指针
 * @return 无返回值
 * @note 必须在使用前调用此函数进行初始化
 */
void ButtonProcessorC_init(ButtonProcessorC* proc, const CommandProcessorConfig* config);

/**
 * @brief 获取按键命令处理器单例（全局唯一实例）
 * @return 指向单例的指针
 */
ButtonProcessorC* ButtonProcessorC_getInstance(void);

/* 宏定义
 * -------------------------------------------------------------*/
/**
 * @brief 用于构建按键命令类型的宏定义
 * @details 将按键ID和事件类型组合成一个32位命令类型
 */
#define MAKE_BUTTON_CMD(event, key) (((uint32_t)(event) << 16) | (key))

/**
 * @}
 */
