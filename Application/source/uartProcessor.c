/**
 * @file uartProcessor.c
 * @brief 串口协议命令处理器实现文件
 * @date 2025-04-11
 * @details 实现针对串口协议消息的命令处理器功能，负责处理来自串口的数据包和命令
 */

/* 头文件
 * -------------------------------------------------------------*/
#include "uartProcessor.h"
#include <stdlib.h>
#include <string.h>

/**
 * @addtogroup UART_Processor
 * @{
 */

/* 宏定义
 * -------------------------------------------------------------*/
/**
 * @brief 串口协议命令处理器支持的最大命令数量
 */
#define UART_PROTOCOL_MAX_COMMANDS 16

/* 静态函数声明
 * -------------------------------------------------------------*/
/**
 * @brief 从UartMessage获取命令类型的实现
 * @param [in] self 处理器指针，此处未使用
 * @param [in] message 指向UartMessage结构体的指针
 * @return UartMessage中的命令类型
 * @details 假设UartMessage第一个字段为uint32_t command
 */
static uint32_t UartProtocol_getCommandFromMessage(void* self, const void* message)
{
    // 直接取消息结构体的第一个字段（uint32_t command）
    return *((const uint32_t*)message);
}

/* 静态变量
 * -------------------------------------------------------------*/
/**
 * @brief 单例相关静态变量
 */
static UartProtocolProcessorC instance_uart;                      /**< 单例对象 */
static uint32_t uart_cmdTypes[UART_PROTOCOL_MAX_COMMANDS];        /**< 命令类型数组 */
static CommandHandlerC uart_handlers[UART_PROTOCOL_MAX_COMMANDS]; /**< 处理函数数组 */
static void* uart_contexts[UART_PROTOCOL_MAX_COMMANDS];           /**< 上下文数组 */

/* 全局函数实现
 * -------------------------------------------------------------*/
/**
 * @brief 初始化串口协议命令处理器
 * @param [in,out] proc 指向处理器结构体的指针
 * @param [in] config 指向配置结构体的指针
 * @return 无返回值
 * @details
 *   - 调用基类初始化
 *   - 设置getCommandFromMessage为UartMessage专用实现
 */
void UartProtocolProcessorC_init(UartProtocolProcessorC* proc, const CommandProcessorConfig* config)
{
    CommandProcessorBaseC_init(&proc->base, config);
    proc->base.getCommandFromMessage = UartProtocol_getCommandFromMessage;
}

/**
 * @brief 获取串口协议命令处理器单例
 * @return 指向单例的指针
 * @details
 *   - 单例模式，避免重复分配资源
 *   - 首次调用时自动初始化
 */
UartProtocolProcessorC* UartProtocolProcessorC_getInstance(void)
{
    static bool initialized = false;
    if (!initialized) {
        CommandProcessorConfig config = { .capacity = UART_PROTOCOL_MAX_COMMANDS,
                                          .cmdTypes = uart_cmdTypes,
                                          .handlers = uart_handlers,
                                          .contexts = uart_contexts };
        UartProtocolProcessorC_init(&instance_uart, &config);
        initialized = true;
    }
    return &instance_uart;
}

/**
 * @}
 */
