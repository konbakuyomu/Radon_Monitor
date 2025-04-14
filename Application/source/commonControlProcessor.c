/**
 * @file commonControlProcessor.c
 * @brief 通用控制命令处理器实现文件
 * @date 2025-04-11
 * @details 实现通用控制命令处理器的相关功能，包括从消息解析命令类型，初始化处理器和获取单例等
 */

/* 头文件
 * -------------------------------------------------------------*/
#include "commonControlProcessor.h"
#include "globalConfig.h"

/**
 * @addtogroup Common_Control_Processor
 * @{
 */

/* 宏定义
 * -------------------------------------------------------------*/
/**
 * @brief 通用控制命令处理器支持的最大命令数量
 */
#define COMMON_CONTROL_MAX_COMMANDS 15

/* 静态变量
 * -------------------------------------------------------------*/
/**
 * @brief Command Processor相关数组
 * @details 用于存储命令类型、处理函数和上下文
 */
static uint32_t commonCmdTypes[COMMON_CONTROL_MAX_COMMANDS];        /**< 支持通用控制命令 */
static CommandHandlerC commonHandlers[COMMON_CONTROL_MAX_COMMANDS]; /**< 对应的处理函数 */
static void* commonContexts[COMMON_CONTROL_MAX_COMMANDS];           /**< 对应的上下文 */

/**
 * @brief 通用控制任务的Command Processor单例对象
 */
static CommonControlProcessorC instance_common_control;

/* 静态函数声明
 * -------------------------------------------------------------*/
/**
 * @brief 从LED控制消息构建命令类型
 * @param [in] self 处理器指针，此处未使用
 * @param [in] message 系统消息指针
 * @return 组合的命令类型
 * @details 从系统消息中提取LED控制类型和LED选择，并组合成命令类型
 */
static uint32_t CommonControl_getCommandFromMessage(void* self, const void* message)
{
    const MsgBusSystemMessage* msg = (const MsgBusSystemMessage*)message;

    // 检查消息类型是否为LED控制
    if (msg->message == MSGBUS_MSG_LED_CONTROL) {
        return MAKE_LED_CONTROL_CMD(msg->payload.ledData.control,
                                    msg->payload.ledData.ledSelection);
    }

    // 不支持的消息类型返回0
    return 0;
}

/* 全局函数实现
 * -------------------------------------------------------------*/
/**
 * @brief 初始化通用控制命令处理器
 * @param [in,out] proc 指向处理器结构体的指针
 * @param [in] config 指向配置结构体的指针
 * @return 无返回值
 * @details
 *   - 调用基类初始化
 *   - 设置getCommandFromMessage为系统消息专用实现
 */
void CommonControlProcessorC_init(CommonControlProcessorC* proc,
                                  const CommandProcessorConfig* config)
{
    CommandProcessorBaseC_init(&proc->base, config);
    proc->base.getCommandFromMessage = CommonControl_getCommandFromMessage;
}

/**
 * @brief 获取通用控制命令处理器单例
 * @return 指向单例的指针
 * @details
 *   - 单例模式，避免重复分配资源
 *   - 首次调用时自动初始化
 */
CommonControlProcessorC* CommonControlProcessorC_getInstance(void)
{
    static bool initialized = false;
    if (!initialized) {
        CommandProcessorConfig config = { .capacity = COMMON_CONTROL_MAX_COMMANDS,
                                          .cmdTypes = commonCmdTypes,
                                          .handlers = commonHandlers,
                                          .contexts = commonContexts };
        CommonControlProcessorC_init(&instance_common_control, &config);
        initialized = true;
    }
    return &instance_common_control;
}

/**
 * @}
 */