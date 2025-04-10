#ifndef COMMAND_PROCESSOR_C_H
#define COMMAND_PROCESSOR_C_H

#include <stdbool.h>
#include <stdint.h>

/* ------------------ 通用命令处理器基类 ------------------ */

typedef bool (*CommandHandlerC)(void* context, const void* message);

typedef struct CommandProcessorBaseC {
    // 获取命令类型的函数指针（类似纯虚函数）
    uint32_t (*getCommandFromMessage)(void* self, const void* message);

    // 注册命令处理函数
    void (*registerHandler)(struct CommandProcessorBaseC* self,
                            uint32_t cmdType,
                            CommandHandlerC handler,
                            void* context);

    // 执行命令
    bool (*executeCommand)(struct CommandProcessorBaseC* self, const void* message);

    // 命令映射表容量
    uint16_t capacity;

    // 当前已注册命令数
    uint16_t count;

    // 命令类型数组
    uint32_t* cmdTypes;

    // 处理函数数组
    CommandHandlerC* handlers;

    // 用户上下文数组
    void** contexts;
} CommandProcessorBaseC;

/* 初始化基类 */
void CommandProcessorBaseC_init(CommandProcessorBaseC* base,
                                uint16_t capacity,
                                uint32_t* cmdTypes,
                                CommandHandlerC* handlers,
                                void** contexts);

/* ------------------ 消息总线命令处理器 ------------------ */

typedef struct {
    CommandProcessorBaseC base;
    // 其他成员
} MessageBusProcessorC;

/* 初始化消息总线处理器 */
void MessageBusProcessorC_init(MessageBusProcessorC* proc,
                               uint16_t capacity,
                               uint32_t* cmdTypes,
                               CommandHandlerC* handlers,
                               void** contexts);

/* 获取单例 */
MessageBusProcessorC* MessageBusProcessorC_getInstance(void);

/* ------------------ 串口协议命令处理器 ------------------ */

typedef struct {
    CommandProcessorBaseC base;
    // 其他成员
} UartProtocolProcessorC;

/* 初始化串口协议处理器 */
void UartProtocolProcessorC_init(UartProtocolProcessorC* proc,
                                 uint16_t capacity,
                                 uint32_t* cmdTypes,
                                 CommandHandlerC* handlers,
                                 void** contexts);

/* 获取单例 */
UartProtocolProcessorC* UartProtocolProcessorC_getInstance(void);

#endif // COMMAND_PROCESSOR_C_H