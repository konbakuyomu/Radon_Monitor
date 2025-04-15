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
#define SYSTEM_MESSAGE_MAX_COMMANDS 3

/* 静态函数声明
 * -------------------------------------------------------------*/
/**
 * @brief 从SystemMessage获取命令类型的实现
 * @param [in] processor 处理器指针，此处未使用
 * @param [in] message 指向SystemMessage结构体的指针
 * @return SystemMessage中的命令类型
 * @details 假设SystemMessage第一个字段为uint32_t message
 */
static uint32_t SystemMessage_extractCommandType(void* processor, const void* message)
{
    // 直接取消息结构体的第一个字段（uint32_t message）
    return *((const uint32_t*)message);
}

/* 静态变量
 * -------------------------------------------------------------*/
/**
 * @brief 单例相关静态变量
 */
static SystemMessageDispatcher systemMessageDispatcherInstance;         /**< 单例对象 */
static uint32_t systemMessageCommandTypes[SYSTEM_MESSAGE_MAX_COMMANDS]; /**< 命令类型数组 */
static CommandHandler
    systemMessageHandlerFunctions[SYSTEM_MESSAGE_MAX_COMMANDS];         /**< 处理函数数组 */
static void* systemMessageContextPointers[SYSTEM_MESSAGE_MAX_COMMANDS]; /**< 上下文数组 */

/* 全局函数实现
 * -------------------------------------------------------------*/
/**
 * @brief 初始化消息总线命令处理器
 * @param [in,out] processor 指向处理器结构体的指针
 * @param [in] configuration 指向配置结构体的指针
 * @return 无返回值
 * @details
 *   - 调用基类初始化
 *   - 设置getCommandTypeFromMessage为SystemMessage专用实现
 */
void SystemMessageDispatcher_initialize(SystemMessageDispatcher* processor,
                                        const CommandProcessorConfiguration* configuration)
{
    CommandProcessor_initialize(&processor->commandProcessor, configuration);
    processor->commandProcessor.getCommandTypeFromMessage = SystemMessage_extractCommandType;
}

/**
 * @brief 获取消息总线命令处理器单例
 * @return 指向单例的指针
 * @details
 *   - 单例模式，避免重复分配资源
 *   - 首次调用时自动初始化
 */
SystemMessageDispatcher* SystemMessageDispatcher_getInstance(void)
{
    static bool isInitialized = false;
    if (!isInitialized) {
        CommandProcessorConfiguration configuration
            = { .capacity = SYSTEM_MESSAGE_MAX_COMMANDS,
                .commandTypes = systemMessageCommandTypes,
                .handlerFunctions = systemMessageHandlerFunctions,
                .contextPointers = systemMessageContextPointers };
        SystemMessageDispatcher_initialize(&systemMessageDispatcherInstance, &configuration);
        isInitialized = true;
    }
    return &systemMessageDispatcherInstance;
}

/**
 * @}
 */