/**
 * @file buttonProcessor.c
 * @brief 按键命令处理器实现文件
 * @date 2025-03-15
 * @details 实现按键命令处理器的相关功能，包括从消息解析命令类型，初始化处理器和获取单例等
 */

/* 头文件
 * -------------------------------------------------------------*/
#include "buttonProcessor.h"
#include "globalConfig.h"

/**
 * @addtogroup buttonProcessor
 * @{
 */

/* 宏定义
 * -------------------------------------------------------------*/
/**
 * @brief 按键命令处理器支持的最大命令数量
 */
#define BUTTON_MAX_COMMAND_COUNT 4

/* 静态变量
 * -------------------------------------------------------------*/
/**
 * @brief Command Processor相关数组
 * @details 用于存储命令类型、处理函数和上下文
 */
static uint32_t buttonCommandTypes[BUTTON_MAX_COMMAND_COUNT];           /**< 支持按键命令 */
static CommandHandler buttonHandlerFunctions[BUTTON_MAX_COMMAND_COUNT]; /**< 对应的处理函数 */
static void* buttonContextPointers[BUTTON_MAX_COMMAND_COUNT];           /**< 对应的上下文 */

/**
 * @brief 按键任务的Command Processor单例对象
 */
static ButtonCommandProcessor buttonProcessorInstance;

/* 静态函数声明
 * -------------------------------------------------------------*/
/**
 * @brief 从按键事件构建命令类型
 * @param [in] processor 处理器指针，此处未使用
 * @param [in] eventMessage 按键事件消息指针
 * @return 组合的命令类型
 * @details 从按键事件消息中提取按键编号和事件类型，并组合成命令类型
 */
static uint32_t ButtonCommand_getTypeFromEventMessage(void* processor, const void* eventMessage)
{
    const ButtonEventMessage* buttonEvent = (const ButtonEventMessage*)eventMessage;
    return CREATE_BUTTON_COMMAND(buttonEvent->eventType, buttonEvent->buttonIdentifier);
}

/* 全局函数实现
 * -------------------------------------------------------------*/
/**
 * @brief 初始化按键命令处理器
 * @param [in,out] processor 指向处理器结构体的指针
 * @param [in] configuration 指向配置结构体的指针
 * @return 无返回值
 * @details
 *   - 调用基类初始化
 *   - 设置getCommandTypeFromMessage为按键事件专用实现
 */
void ButtonCommandProcessor_initialize(ButtonCommandProcessor* processor,
                                       const CommandProcessorConfiguration* configuration)
{
    CommandProcessor_initialize(&processor->commandProcessor, configuration);
    processor->commandProcessor.getCommandTypeFromMessage = ButtonCommand_getTypeFromEventMessage;
}

/**
 * @brief 获取按键命令处理器单例
 * @return 指向单例的指针
 * @details
 *   - 单例模式，避免重复分配资源
 *   - 首次调用时自动初始化
 */
ButtonCommandProcessor* ButtonCommandProcessor_getInstance(void)
{
    static bool isInitialized = false;
    if (!isInitialized) {
        CommandProcessorConfiguration configuration = { .capacity = BUTTON_MAX_COMMAND_COUNT,
                                                        .commandTypes = buttonCommandTypes,
                                                        .handlerFunctions = buttonHandlerFunctions,
                                                        .contextPointers = buttonContextPointers };
        ButtonCommandProcessor_initialize(&buttonProcessorInstance, &configuration);
        isInitialized = true;
    }
    return &buttonProcessorInstance;
}

/**
 * @}
 */
