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
#include "usart1Driver.h"

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
    CommandProcessor base; /**< 基类结构体，继承命令处理器基类功能 */
    // 可扩展其他成员
} UartProtocolProcessor;

/* UART 帧结构体
 * -------------------------------------------------------------*/
typedef struct {
    uint8_t address;        /**< 设备地址 */
    uint8_t functionCode;   /**< 功能码 */
    const uint8_t* data;    /**< 数据指针 */
    size_t dataLength;      /**< 数据长度 */
} UartFrame;

/* 函数声明
 * -------------------------------------------------------------*/
/**
 * @brief 初始化串口协议命令处理器
 * @param [in,out] proc 指向处理器结构体的指针
 * @param [in] config 指向配置结构体的指针
 * @return 无返回值
 * @note 必须在使用前调用此函数进行初始化
 */
void UartProtocolProcessor_initialize(UartProtocolProcessor* processor,
                                        const CommandProcessorConfiguration* configuration);

/**
 * @brief 获取串口协议命令处理器单例（全局唯一实例）
 * @return 指向单例的指针
 */
UartProtocolProcessor* UartProtocolProcessor_getInstance(void);

/* UART 帧解析与构建函数原型
 * -------------------------------------------------------------*/
/**
 * @brief 解析原始串口数据为UART帧
 * @param [in] raw 原始UART数据结构
 * @param [out] frame 解析后的帧结构
 * @return true解析成功,false失败
 */
bool parseUartFrame(const UartData* raw, UartFrame* frame);

/**
 * @brief 根据UART帧构建要发送的字节流
 * @param [in] frame 要发送的帧结构
 * @param [out] buffer 输出字节缓冲区
 * @return 输出缓冲区总长度
 */
size_t buildUartFrame(const UartFrame* frame, uint8_t* buffer);

/**
 * @brief 计算CRC16-IBM校验码
 * @param [in] data 输入字节数组
 * @param [in] len 长度
 * @return CRC16校验值
 */
uint16_t calcCRC16(const uint8_t* data, size_t len);

/**
 * @}
 */
