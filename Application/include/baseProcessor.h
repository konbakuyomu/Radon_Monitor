/**
 * @file baseProcessor.h
 * @brief 纯C命令处理器模块头文件
 * @date 2025-04-11
 * @details 提供命令处理器基类和相关接口，设计目标：支持动态注册/注销命令处理函数，
 *          支持上下文参数，支持多协议/多实例，主循环无需修改即可扩展新命令类型。
 *          适用于嵌入式消息分发、协议解析、事件驱动等场景。
 */

#pragma once

/* 头文件
 * -------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>

/**
 * @defgroup Command_Processor 命令处理器模块
 * @brief 命令处理器通用基类和接口定义
 * @{
 */

/* 类型定义
 * -------------------------------------------------------------*/
/**
 * @brief 命令处理函数指针类型
 * @param [in] context 用户自定义上下文指针（注册时传入，回调时自动带回）
 * @param [in] message 消息指针（如SystemMessage、UartMessage等，具体类型由协议决定）
 * @return 处理成功返回true，失败或未处理返回false
 */
typedef bool (*CommandHandlerC)(void* context, const void* message);

/* 结构体
 * -------------------------------------------------------------*/
/**
 * @brief 命令处理器配置结构体
 */
typedef struct {
    uint16_t capacity;  /**< 最大可注册命令数 */
    uint32_t* cmdTypes; /**< 命令类型数组（外部分配内存，长度为capacity） */
    CommandHandlerC* handlers; /**< 处理函数数组（外部分配内存，长度为capacity） */
    void** contexts;           /**< 上下文数组（外部分配内存，长度为capacity） */
} CommandProcessorConfig;

/**
 * @brief 命令处理器基类结构体
 *        用于存储命令类型、处理函数、上下文等映射关系，并提供注册/分发接口
 */
typedef struct CommandProcessorBaseC {
    /**
     * @brief 获取命令类型的函数指针（类似C++虚函数，派生类需赋值）
     * @param [in] self 指向自身的指针
     * @param [in] message 消息指针
     * @return 从消息中解析出的命令类型
     */
    uint32_t (*getCommandFromMessage)(void* self, const void* message);

    /**
     * @brief 注册命令处理函数
     * @param [in,out] self 指向自身的指针
     * @param [in] cmdType 命令类型
     * @param [in] handler 处理函数指针
     * @param [in] context 用户上下文指针
     * @return 无返回值
     */
    void (*registerHandler)(struct CommandProcessorBaseC* self,
                            uint32_t cmdType,
                            CommandHandlerC handler,
                            void* context);

    /**
     * @brief 执行命令（分发消息到对应handler）
     * @param [in] self 指向自身的指针
     * @param [in] message 消息指针
     * @return 处理成功返回true，失败或未处理返回false
     */
    bool (*executeCommand)(struct CommandProcessorBaseC* self, const void* message);

    uint16_t capacity;  /**< 命令映射表容量（最大可注册命令数） */
    uint16_t count;     /**< 当前已注册命令数 */
    uint32_t* cmdTypes; /**< 命令类型数组（每个元素对应一个已注册命令类型） */
    CommandHandlerC* handlers; /**< 处理函数数组（每个元素对应一个已注册命令的处理函数） */
    void** contexts; /**< 用户上下文数组（每个元素对应一个已注册命令的上下文指针） */
} CommandProcessorBaseC;

/* 函数声明
 * -------------------------------------------------------------*/
/**
 * @brief 初始化命令处理器基类
 * @param [in,out] base 指向基类结构体的指针
 * @param [in] config 指向配置结构体的指针
 * @return 无返回值
 * @note 必须在使用前初始化，且config中的数组内存由调用者管理
 */
void CommandProcessorBaseC_init(CommandProcessorBaseC* base, const CommandProcessorConfig* config);

/**
 * @note 特定领域的命令处理器实现已移至对应的头文件:
 * - 消息总线命令处理器: messageBusProcessor.h
 * - 串口协议命令处理器: uartProcessor.h
 * - 按键命令处理器: buttonProcessor.h
 */

/**
 * @}
 */
