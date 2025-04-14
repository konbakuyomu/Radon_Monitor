/**
 * @file uartProcessor.h
 * @brief 串口协议命令处理器头文件
 * @date 2025-04-11
 * @details 提供针对串口协议消息的命令处理器，负责处理来自串口的数据包和命令
 */

#pragma once

/* 头文件
 * -------------------------------------------------------------*/
#include "baseProcessor.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * @defgroup UART_Processor 串口处理器模块
 * @brief 串口协议处理相关定义
 * @{
 */

/* 结构体
 * -------------------------------------------------------------*/
/**
 * @brief 串口协议命令处理器结构体（继承自基类）
 *        用于处理如UartMessage等串口协议消息
 */
typedef struct {
    CommandProcessorBaseC base; /**< 基类结构体，继承命令处理器基类功能 */
    // 可扩展其他成员
} UartProtocolProcessorC;

/* 函数声明
 * -------------------------------------------------------------*/
/**
 * @brief 初始化串口协议命令处理器
 * @param [in,out] proc 指向处理器结构体的指针
 * @param [in] config 指向配置结构体的指针
 * @return 无返回值
 * @note 必须在使用前调用此函数进行初始化
 */
void UartProtocolProcessorC_init(UartProtocolProcessorC* proc,
                                 const CommandProcessorConfig* config);

/**
 * @brief 获取串口协议命令处理器单例（全局唯一实例）
 * @return 指向单例的指针
 */
UartProtocolProcessorC* UartProtocolProcessorC_getInstance(void);

/**
 * @}
 */
