/**
 * @file baseProcessor.c
 * @brief 纯C命令处理器实现文件
 * @date 2025-04-11
 * @details 支持动态注册/注销命令处理函数，支持上下文参数，支持多协议/多实例，
 *          主循环无需修改即可扩展新命令类型。
 */

/* 头文件
 * -------------------------------------------------------------*/
#include "baseProcessor.h" // 包含头文件，声明了所有类型和接口
#include <stdlib.h>              // 标准库，主要用于NULL等
#include <string.h>              // 提供memset等内存操作函数

/**
 * @addtogroup Command_Processor
 * @{
 */

/* 静态函数声明
 * -------------------------------------------------------------*/

/**
 * @brief 注册命令处理函数的实现
 * @param [in,out] self 指向命令处理器基类的指针
 * @param [in] cmdType 命令类型（如LED控制、串口命令等）
 * @param [in] handler 处理函数指针
 * @param [in] context 用户自定义上下文指针（回调时自动带回）
 * @return 无返回值
 * @details
 *   - 如果已注册该命令类型，则覆盖原有处理函数和上下文
 *   - 如果未注册且有空位，则新增一条映射
 *   - 超过容量则忽略
 */
static void registerHandler_impl(CommandProcessorBaseC* self,
                                 uint32_t cmdType,
                                 CommandHandlerC handler,
                                 void* context)
{
    // 遍历已注册的命令类型
    for (uint16_t i = 0; i < self->count; ++i) {
        // 如果已注册该命令类型，则覆盖原有handler和context
        if (self->cmdTypes[i] == cmdType) {
            self->handlers[i] = handler;
            self->contexts[i] = context;
            return;
        }
    }
    // 如果未注册且有空位，则新增一条映射
    if (self->count < self->capacity) {
        self->cmdTypes[self->count] = cmdType;
        self->handlers[self->count] = handler;
        self->contexts[self->count] = context;
        self->count++;
    }
    // 超过容量则忽略（不做处理）
}

/**
 * @brief 执行命令（分发消息到对应handler）的实现
 * @param [in] self 指向命令处理器基类的指针
 * @param [in] message 消息指针（如SystemMessage、UartMessage等）
 * @return 找到并执行了handler返回true，未注册返回false
 * @details
 *   - 通过getCommandFromMessage获取消息类型
 *   - 查找已注册的handler并调用，带回上下文
 */
static bool executeCommand_impl(CommandProcessorBaseC* self, const void* message)
{
    // 通过虚函数指针获取消息类型（如SystemMessage的message字段）
    uint32_t cmd = self->getCommandFromMessage(self, message);
    // 遍历所有已注册命令
    for (uint16_t i = 0; i < self->count; ++i) {
        // 找到匹配的命令类型
        if (self->cmdTypes[i] == cmd) {
            // 如果有处理函数，调用并传递上下文和消息
            if (self->handlers[i]) {
                return self->handlers[i](self->contexts[i], message);
            }
        }
    }
    // 没有找到对应handler，返回false
    return false;
}

/* 全局函数实现
 * -------------------------------------------------------------*/

/**
 * @brief 初始化命令处理器基类
 * @param [in,out] base 指向基类结构体的指针
 * @param [in] config 指向配置结构体的指针
 * @return 无返回值
 * @details
 *   - 必须在使用前初始化
 *   - config中数组内存由调用者管理
 *   - 注册/分发接口赋值为实现函数
 *   - getCommandFromMessage需由派生类赋值
 */
void CommandProcessorBaseC_init(CommandProcessorBaseC* base, const CommandProcessorConfig* config)
{
    base->capacity = config->capacity;            // 最大可注册命令数
    base->count = 0;                              // 当前已注册命令数
    base->cmdTypes = config->cmdTypes;            // 命令类型数组
    base->handlers = config->handlers;            // 处理函数数组
    base->contexts = config->contexts;            // 上下文数组
    base->registerHandler = registerHandler_impl; // 注册函数指针
    base->executeCommand = executeCommand_impl;   // 分发函数指针
    base->getCommandFromMessage = NULL;           // 虚函数，需派生类赋值
}

/**
 * @}
 */
