/**
 * @file messageBusProcessor.c
 * @brief 消息总线命令处理器实现文件
 * @date 2025-04-11
 * @details 实现针对消息总线消息的命令处理器功能，用于系统级消息的处理和分发
 */

/* 头文件
 * -------------------------------------------------------------*/
#include "messageBusProcessor.h"
#include <stdlib.h>
#include <string.h>

/**
 * @addtogroup Message_Bus_Processor
 * @{
 */

/* 宏定义
 * -------------------------------------------------------------*/
/**
 * @brief 消息总线命令处理器支持的最大命令数量
 */
#define MESSAGE_BUS_MAX_COMMANDS 3

/* 静态函数声明
 * -------------------------------------------------------------*/
/**
 * @brief 从SystemMessage获取命令类型的实现
 * @param [in] self 处理器指针，此处未使用
 * @param [in] message 指向SystemMessage结构体的指针
 * @return SystemMessage中的命令类型
 * @details 假设SystemMessage第一个字段为uint32_t message
 */
static uint32_t MessageBus_getCommandFromMessage(void* self, const void* message)
{
    // 直接取消息结构体的第一个字段（uint32_t message）
    return *((const uint32_t*)message);
}

/* 静态变量
 * -------------------------------------------------------------*/
/**
 * @brief 单例相关静态变量
 */
static MessageBusProcessorC instance_msgbus;                      /**< 单例对象 */
static uint32_t msgbus_cmdTypes[MESSAGE_BUS_MAX_COMMANDS];        /**< 命令类型数组 */
static CommandHandlerC msgbus_handlers[MESSAGE_BUS_MAX_COMMANDS]; /**< 处理函数数组 */
static void* msgbus_contexts[MESSAGE_BUS_MAX_COMMANDS];           /**< 上下文数组 */

/* 全局函数实现
 * -------------------------------------------------------------*/
/**
 * @brief 初始化消息总线命令处理器
 * @param [in,out] proc 指向处理器结构体的指针
 * @param [in] config 指向配置结构体的指针
 * @return 无返回值
 * @details
 *   - 调用基类初始化
 *   - 设置getCommandFromMessage为SystemMessage专用实现
 */
void MessageBusProcessorC_init(MessageBusProcessorC* proc, const CommandProcessorConfig* config)
{
    CommandProcessorBaseC_init(&proc->base, config);
    proc->base.getCommandFromMessage = MessageBus_getCommandFromMessage;
}

/**
 * @brief 获取消息总线命令处理器单例
 * @return 指向单例的指针
 * @details
 *   - 单例模式，避免重复分配资源
 *   - 首次调用时自动初始化
 */
MessageBusProcessorC* MessageBusProcessorC_getInstance(void)
{
    static bool initialized = false;
    if (!initialized) {
        CommandProcessorConfig config = { .capacity = MESSAGE_BUS_MAX_COMMANDS,
                                          .cmdTypes = msgbus_cmdTypes,
                                          .handlers = msgbus_handlers,
                                          .contexts = msgbus_contexts };
        MessageBusProcessorC_init(&instance_msgbus, &config);
        initialized = true;
    }
    return &instance_msgbus;
}

/**
 * @}
 */