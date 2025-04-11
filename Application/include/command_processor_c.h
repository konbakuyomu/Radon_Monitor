#ifndef COMMAND_PROCESSOR_C_H
#define COMMAND_PROCESSOR_C_H

#include <stdbool.h>
#include <stdint.h>

/*
 * 纯C命令处理器模块头文件
 * 设计目标：支持动态注册/注销命令处理函数，支持上下文参数，支持多协议/多实例，主循环无需修改即可扩展新命令类型。
 * 适用于嵌入式消息分发、协议解析、事件驱动等场景。
 */

/* ------------------ 通用命令处理器基类 ------------------ */

/**
 * @brief 命令处理函数指针类型
 * @param context 用户自定义上下文指针（注册时传入，回调时自动带回）
 * @param message 消息指针（如SystemMessage、UartMessage等，具体类型由协议决定）
 * @return 处理成功返回true，失败或未处理返回false
 */
typedef bool (*CommandHandlerC)(void* context, const void* message);

/**
 * @brief 命令处理器基类结构体
 *        用于存储命令类型、处理函数、上下文等映射关系，并提供注册/分发接口
 */
typedef struct CommandProcessorBaseC {
    // 获取命令类型的函数指针（类似C++虚函数，派生类需赋值）
    uint32_t (*getCommandFromMessage)(void* self, const void* message);

    // 注册命令处理函数
    void (*registerHandler)(struct CommandProcessorBaseC* self,
                            uint32_t cmdType,
                            CommandHandlerC handler,
                            void* context);

    // 执行命令（分发消息到对应handler）
    bool (*executeCommand)(struct CommandProcessorBaseC* self, const void* message);

    // 命令映射表容量（最大可注册命令数）
    uint16_t capacity;

    // 当前已注册命令数
    uint16_t count;

    // 命令类型数组（每个元素对应一个已注册命令类型）
    uint32_t* cmdTypes;

    // 处理函数数组（每个元素对应一个已注册命令的处理函数）
    CommandHandlerC* handlers;

    // 用户上下文数组（每个元素对应一个已注册命令的上下文指针）
    void** contexts;
} CommandProcessorBaseC;

/**
 * @brief 初始化命令处理器基类
 * @param base 指向基类结构体的指针
 * @param capacity 最大可注册命令数
 * @param cmdTypes 命令类型数组（外部分配内存，长度为capacity）
 * @param handlers 处理函数数组（外部分配内存，长度为capacity）
 * @param contexts 上下文数组（外部分配内存，长度为capacity）
 * @note 必须在使用前初始化，且cmdTypes/handlers/contexts内存由调用者管理
 */
void CommandProcessorBaseC_init(CommandProcessorBaseC* base,
                                uint16_t capacity,
                                uint32_t* cmdTypes,
                                CommandHandlerC* handlers,
                                void** contexts);

/* ------------------ 消息总线命令处理器 ------------------ */

/**
 * @brief 消息总线命令处理器结构体（继承自基类）
 *        用于处理如SystemMessage等消息总线消息
 */
typedef struct {
    CommandProcessorBaseC base;
    // 可扩展其他成员
} MessageBusProcessorC;

/**
 * @brief 初始化消息总线命令处理器
 * @param proc 指向处理器结构体的指针
 * @param capacity 最大可注册命令数
 * @param cmdTypes 命令类型数组
 * @param handlers 处理函数数组
 * @param contexts 上下文数组
 */
void MessageBusProcessorC_init(MessageBusProcessorC* proc,
                               uint16_t capacity,
                               uint32_t* cmdTypes,
                               CommandHandlerC* handlers,
                               void** contexts);

/**
 * @brief 获取消息总线命令处理器单例（全局唯一实例）
 * @return 指向单例的指针
 * @note 推荐用单例，避免重复分配资源
 */
MessageBusProcessorC* MessageBusProcessorC_getInstance(void);

/* ------------------ 串口协议命令处理器 ------------------ */

/**
 * @brief 串口协议命令处理器结构体（继承自基类）
 *        用于处理如UartMessage等串口协议消息
 */
typedef struct {
    CommandProcessorBaseC base;
    // 可扩展其他成员
} UartProtocolProcessorC;

/**
 * @brief 初始化串口协议命令处理器
 * @param proc 指向处理器结构体的指针
 * @param capacity 最大可注册命令数
 * @param cmdTypes 命令类型数组
 * @param handlers 处理函数数组
 * @param contexts 上下文数组
 */
void UartProtocolProcessorC_init(UartProtocolProcessorC* proc,
                                 uint16_t capacity,
                                 uint32_t* cmdTypes,
                                 CommandHandlerC* handlers,
                                 void** contexts);

/**
 * @brief 获取串口协议命令处理器单例（全局唯一实例）
 * @return 指向单例的指针
 */
UartProtocolProcessorC* UartProtocolProcessorC_getInstance(void);

#endif // COMMAND_PROCESSOR_C_H