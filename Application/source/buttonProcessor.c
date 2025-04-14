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
#define BUTTON_MAX_COMMANDS 4

/**
 * @brief 用于构建按键命令类型的宏定义
 * @details 将按键ID和事件类型组合成一个32位命令类型
 */
#define MAKE_BUTTON_CMD(event, key) (((uint32_t)(event) << 16) | (key))

/* 类型定义
 * -------------------------------------------------------------*/
/**
 * @brief 按键事件消息结构
 */
typedef struct {
    uint8_t keyNumber;  /**< 按键编号 */
    btn_event_t event;  /**< 按键事件类型 */
} ButtonEventMessage;

/* 静态变量
 * -------------------------------------------------------------*/
/**
 * @brief Command Processor相关数组
 * @details 用于存储命令类型、处理函数和上下文
 */
static uint32_t buttonCmdTypes[BUTTON_MAX_COMMANDS];        /**< 支持按键命令 */
static CommandHandlerC buttonHandlers[BUTTON_MAX_COMMANDS]; /**< 对应的处理函数 */
static void* buttonContexts[BUTTON_MAX_COMMANDS];           /**< 对应的上下文 */

/**
 * @brief 按键任务的Command Processor单例对象
 */
static ButtonProcessorC instance_button;

/* 静态函数声明
 * -------------------------------------------------------------*/
/**
 * @brief 从按键事件构建命令类型
 * @param [in] self 处理器指针，此处未使用
 * @param [in] message 按键事件消息指针
 * @return 组合的命令类型
 * @details 从按键事件消息中提取按键编号和事件类型，并组合成命令类型
 */
static uint32_t Button_getCommandFromMessage(void* self, const void* message)
{
    const ButtonEventMessage* evt = (const ButtonEventMessage*)message;
    return MAKE_BUTTON_CMD(evt->event, evt->keyNumber);
}

/* 全局函数实现
 * -------------------------------------------------------------*/
/**
 * @brief 初始化按键命令处理器
 * @param [in,out] proc 指向处理器结构体的指针
 * @param [in] config 指向配置结构体的指针
 * @return 无返回值
 * @details
 *   - 调用基类初始化
 *   - 设置getCommandFromMessage为按键事件专用实现
 */
void ButtonProcessorC_init(ButtonProcessorC* proc, const CommandProcessorConfig* config)
{
    CommandProcessorBaseC_init(&proc->base, config);
    proc->base.getCommandFromMessage = Button_getCommandFromMessage;
}

/**
 * @brief 获取按键命令处理器单例
 * @return 指向单例的指针
 * @details
 *   - 单例模式，避免重复分配资源
 *   - 首次调用时自动初始化
 */
ButtonProcessorC* ButtonProcessorC_getInstance(void)
{
    static bool initialized = false;
    if (!initialized) {
        CommandProcessorConfig config = { .capacity = BUTTON_MAX_COMMANDS,
                                          .cmdTypes = buttonCmdTypes,
                                          .handlers = buttonHandlers,
                                          .contexts = buttonContexts };
        ButtonProcessorC_init(&instance_button, &config);
        initialized = true;
    }
    return &instance_button;
}

/**
 * @}
 */ 
