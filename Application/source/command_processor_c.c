/**
 * @file command_processor_c.c
 * @brief 纯C命令处理器实现文件
 * @details 支持动态注册/注销命令处理函数，支持上下文参数，支持多协议/多实例，主循环无需修改即可扩展新命令类型。
 * @date 2025-04-11
 */

#include "command_processor_c.h" // 包含头文件，声明了所有类型和接口
#include <stdlib.h>              // 标准库，主要用于NULL等
#include <string.h>              // 提供memset等内存操作函数

/* ------------------ 基类方法实现 ------------------ */

/**
 * @brief 注册命令处理函数的实现
 * @param self 指向命令处理器基类的指针
 * @param cmdType 命令类型（如LED控制、串口命令等）
 * @param handler 处理函数指针
 * @param context 用户自定义上下文指针（回调时自动带回）
 * @details
 *   - 如果已注册该命令类型，则覆盖原有处理函数和上下文
 *   - 如果未注册且有空位，则新增一条映射
 *   - 超过容量则忽略
 */
static void registerHandler_impl(CommandProcessorBaseC* self, uint32_t cmdType, CommandHandlerC handler, void* context) {
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
 * @param self 指向命令处理器基类的指针
 * @param message 消息指针（如SystemMessage、UartMessage等）
 * @return true 找到并执行了handler，false 未注册
 * @details
 *   - 通过getCommandFromMessage获取消息类型
 *   - 查找已注册的handler并调用，带回上下文
 */
static bool executeCommand_impl(CommandProcessorBaseC* self, const void* message) {
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

/**
 * @brief 初始化命令处理器基类
 * @param base 指向基类结构体的指针
 * @param capacity 最大可注册命令数
 * @param cmdTypes 命令类型数组（外部分配内存，长度为capacity）
 * @param handlers 处理函数数组（外部分配内存，长度为capacity）
 * @param contexts 上下文数组（外部分配内存，长度为capacity）
 * @details
 *   - 必须在使用前初始化
 *   - cmdTypes/handlers/contexts内存由调用者管理
 *   - 注册/分发接口赋值为实现函数
 *   - getCommandFromMessage需由派生类赋值
 */
void CommandProcessorBaseC_init(CommandProcessorBaseC* base, uint16_t capacity,
                                uint32_t* cmdTypes, CommandHandlerC* handlers, void** contexts) {
    base->capacity = capacity;      // 最大可注册命令数
    base->count = 0;                // 当前已注册命令数
    base->cmdTypes = cmdTypes;      // 命令类型数组
    base->handlers = handlers;      // 处理函数数组
    base->contexts = contexts;      // 上下文数组
    base->registerHandler = registerHandler_impl; // 注册函数指针
    base->executeCommand = executeCommand_impl;   // 分发函数指针
    base->getCommandFromMessage = NULL;           // 虚函数，需派生类赋值
}

/* ------------------ 消息总线命令处理器实现 ------------------ */

/**
 * @brief 从SystemMessage获取命令类型的实现
 * @param self 未用
 * @param message 指向SystemMessage结构体的指针
 * @return 消息类型（假设SystemMessage第一个字段为uint32_t message）
 */
static uint32_t MessageBus_getCommandFromMessage(void* self, const void* message) {
    // 直接取消息结构体的第一个字段（uint32_t message）
    return *((const uint32_t*)message);
}

/* 单例相关静态变量 */
static MessageBusProcessorC instance_msgbus;         // 单例对象
static uint32_t msgbus_cmdTypes[16];                 // 命令类型数组
static CommandHandlerC msgbus_handlers[16];          // 处理函数数组
static void* msgbus_contexts[16];                    // 上下文数组

/**
 * @brief 初始化消息总线命令处理器
 * @param proc 指向处理器结构体的指针
 * @param capacity 最大可注册命令数
 * @param cmdTypes 命令类型数组
 * @param handlers 处理函数数组
 * @param contexts 上下文数组
 * @details
 *   - 调用基类初始化
 *   - 设置getCommandFromMessage为SystemMessage专用实现
 */
void MessageBusProcessorC_init(MessageBusProcessorC* proc, uint16_t capacity,
                               uint32_t* cmdTypes, CommandHandlerC* handlers, void** contexts) {
    CommandProcessorBaseC_init(&proc->base, capacity, cmdTypes, handlers, contexts);
    proc->base.getCommandFromMessage = MessageBus_getCommandFromMessage;
}

/**
 * @brief 获取消息总线命令处理器单例
 * @return 指向单例的指针
 * @details
 *   - 单例模式，避免重复分配资源
 *   - 首次调用时自动初始化
 */
MessageBusProcessorC* MessageBusProcessorC_getInstance(void) {
    static bool initialized = false;
    if (!initialized) {
        MessageBusProcessorC_init(&instance_msgbus, 16, msgbus_cmdTypes, msgbus_handlers, msgbus_contexts);
        initialized = true;
    }
    return &instance_msgbus;
}

/* ------------------ 串口协议命令处理器实现 ------------------ */

/**
 * @brief 从UartMessage获取命令类型的实现
 * @param self 未用
 * @param message 指向UartMessage结构体的指针
 * @return 命令类型（假设UartMessage第一个字段为uint32_t command）
 */
static uint32_t UartProtocol_getCommandFromMessage(void* self, const void* message) {
    // 直接取消息结构体的第一个字段（uint32_t command）
    return *((const uint32_t*)message);
}

/* 单例相关静态变量 */
static UartProtocolProcessorC instance_uart;         // 单例对象
static uint32_t uart_cmdTypes[16];                   // 命令类型数组
static CommandHandlerC uart_handlers[16];            // 处理函数数组
static void* uart_contexts[16];                      // 上下文数组

/**
 * @brief 初始化串口协议命令处理器
 * @param proc 指向处理器结构体的指针
 * @param capacity 最大可注册命令数
 * @param cmdTypes 命令类型数组
 * @param handlers 处理函数数组
 * @param contexts 上下文数组
 * @details
 *   - 调用基类初始化
 *   - 设置getCommandFromMessage为UartMessage专用实现
 */
void UartProtocolProcessorC_init(UartProtocolProcessorC* proc, uint16_t capacity,
                                 uint32_t* cmdTypes, CommandHandlerC* handlers, void** contexts) {
    CommandProcessorBaseC_init(&proc->base, capacity, cmdTypes, handlers, contexts);
    proc->base.getCommandFromMessage = UartProtocol_getCommandFromMessage;
}

/**
 * @brief 获取串口协议命令处理器单例
 * @return 指向单例的指针
 * @details
 *   - 单例模式，避免重复分配资源
 *   - 首次调用时自动初始化
 */
UartProtocolProcessorC* UartProtocolProcessorC_getInstance(void) {
    static bool initialized = false;
    if (!initialized) {
        UartProtocolProcessorC_init(&instance_uart, 16, uart_cmdTypes, uart_handlers, uart_contexts);
        initialized = true;
    }
    return &instance_uart;
}

/* ===================== 用法案例 =====================
 * 1. 定义消息结构体（如SystemMessage），第一个字段必须是uint32_t类型的消息类型
 *    typedef struct {
 *        uint32_t message; // 必须是第一个字段
 *        ... // 其他字段
 *    } SystemMessage;
 *
 * 2. 定义处理函数，签名为bool handler(void* context, const void* message)
 *    bool led_handler(void* context, const void* message) {
 *        const SystemMessage* msg = (const SystemMessage*)message;
 *        // 处理LED相关逻辑
 *        return true;
 *    }
 *
 * 3. 注册处理函数
 *    MessageBusProcessorC* proc = MessageBusProcessorC_getInstance();
 *    proc->base.registerHandler(&proc->base, MSG_TYPE_LED, led_handler, NULL);
 *
 * 4. 分发消息
 *    SystemMessage msg = { .message = MSG_TYPE_LED, ... };
 *    proc->base.executeCommand(&proc->base, &msg);
 *
 * 5. 支持多协议/多实例，串口协议同理
 * ================================================== */