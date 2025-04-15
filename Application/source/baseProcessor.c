/**
 * @file baseProcessor.c
 * @brief 纯C命令处理器实现文件
 * @date 2025-04-15
 * @details 支持动态注册/注销命令处理函数，支持上下文参数，支持多协议/多实例，
 *          主循环无需修改即可扩展新命令类型。
 */

/* 头文件
 * -------------------------------------------------------------*/
#include "baseProcessor.h" // 包含头文件，声明了所有类型和接口
#include <stdlib.h>        // 标准库，主要用于NULL等
#include <string.h>        // 提供memset等内存操作函数

/**
 * @addtogroup Command_Processor
 * @{
 */

/* 静态函数声明
 * -------------------------------------------------------------*/

/**
 * @brief 注册命令处理函数的实现
 * @param [in,out] processor 指向命令处理器的指针
 * @param [in] commandType 命令类型（如LED控制、串口命令等）
 * @param [in] handler 处理函数指针
 * @param [in] context 用户自定义上下文指针（回调时自动带回）
 * @return 无返回值
 * @details
 *   - 如果已注册该命令类型，则覆盖原有处理函数和上下文
 *   - 如果未注册且有空位，则新增一条映射
 *   - 超过容量则忽略
 */
static void registerHandler_implementation(CommandProcessor* processor,
                                           uint32_t commandType,
                                           CommandHandler handler,
                                           void* context)
{
    // 遍历已注册的命令类型
    for (uint16_t i = 0; i < processor->count; ++i) {
        // 如果已注册该命令类型，则覆盖原有handler和context
        if (processor->commandTypes[i] == commandType) {
            processor->handlerFunctions[i] = handler;
            processor->contextPointers[i] = context;
            return;
        }
    }
    // 如果未注册且有空位，则新增一条映射
    if (processor->count < processor->capacity) {
        processor->commandTypes[processor->count] = commandType;
        processor->handlerFunctions[processor->count] = handler;
        processor->contextPointers[processor->count] = context;
        processor->count++;
    }
    // 超过容量则忽略（不做处理）
}

/**
 * @brief 执行命令（分发消息到对应handler）的实现
 * @param [in] processor 指向命令处理器的指针
 * @param [in] message 消息指针（如SystemMessage、UartMessage等）
 * @return 找到并执行了handler返回true，未注册返回false
 * @details
 *   - 通过getCommandFromMessage获取消息类型
 *   - 查找已注册的handler并调用，带回上下文
 */
static bool executeCommand_implementation(CommandProcessor* processor, const void* message)
{
    // 通过虚函数指针获取消息类型（如SystemMessage的message字段）
    uint32_t commandType = processor->getCommandTypeFromMessage(processor, message);
    // 遍历所有已注册命令
    for (uint16_t i = 0; i < processor->count; ++i) {
        // 找到匹配的命令类型
        if (processor->commandTypes[i] == commandType) {
            // 如果有处理函数，调用并传递上下文和消息
            if (processor->handlerFunctions[i]) {
                return processor->handlerFunctions[i](processor->contextPointers[i], message);
            }
        }
    }
    // 没有找到对应handler，返回false
    return false;
}

/* 全局函数实现
 * -------------------------------------------------------------*/

/**
 * @brief 初始化命令处理器
 * @param [in,out] processor 指向命令处理器结构体的指针
 * @param [in] configuration 指向配置结构体的指针
 * @return 无返回值
 * @details
 *   - 必须在使用前初始化
 *   - configuration中数组内存由调用者管理
 *   - 注册/分发接口赋值为实现函数
 *   - getCommandFromMessage需由派生类赋值
 */
void CommandProcessor_initialize(CommandProcessor* processor,
                                 const CommandProcessorConfiguration* configuration)
{
    processor->capacity = configuration->capacity;                 // 最大可注册命令数
    processor->count = 0;                                          // 当前已注册命令数
    processor->commandTypes = configuration->commandTypes;         // 命令类型数组
    processor->handlerFunctions = configuration->handlerFunctions; // 处理函数数组
    processor->contextPointers = configuration->contextPointers;   // 上下文数组
    processor->registerHandler = registerHandler_implementation;   // 注册函数指针
    processor->executeCommand = executeCommand_implementation;     // 分发函数指针
    processor->getCommandTypeFromMessage = NULL;                   // 虚函数，需派生类赋值
}

/**
 * @}
 */
