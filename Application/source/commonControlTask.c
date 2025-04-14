/**
 * @file commonControlTask.c
 * @brief 通用控制任务处理实现文件
 * @date 2025-04-11
 * @details 实现通用控制任务函数，处理LED和其他通用功能的控制
 */

/* 头文件
 * -------------------------------------------------------------*/
#include "commonControlProcessor.h"
#include "globalConfig.h"
#include "ledConfig.h"

/**
 * @addtogroup Common_Control_Task
 * @{
 */

/* 静态变量定义
 * -------------------------------------------------------------*/
/**
 * @var QueueHandle_t commonControlQueue
 * @brief 通用控制消息队列句柄，用于接收通用控制相关消息
 * @note 仅在当前文件内有效
 */
static QueueHandle_t commonControlQueue = NULL;

/**
 * @brief 通用控制消息队列的静态存储区
 */
static StaticQueue_t commonControlQueueBuffer;

/**
 * @brief 通用控制消息队列的静态存储区域
 * @note 大小为10个SystemMessage结构体的存储空间
 */
static uint8_t commonControlQueueStorage[10 * sizeof(MsgBusSystemMessage)];

/* 静态函数声明
 * -------------------------------------------------------------*/
/**
 * @brief 处理LED打开命令
 * @param [in] context 上下文参数（未使用）
 * @param [in] message 系统消息指针
 * @return 处理成功返回true
 */
static bool handleLedTurnOn(void* context, const void* message);

/**
 * @brief 处理LED关闭命令
 * @param [in] context 上下文参数（未使用）
 * @param [in] message 系统消息指针
 * @return 处理成功返回true
 */
static bool handleLedTurnOff(void* context, const void* message);

/**
 * @brief 处理LED切换命令
 * @param [in] context 上下文参数（未使用）
 * @param [in] message 系统消息指针
 * @return 处理成功返回true
 */
static bool handleLedToggle(void* context, const void* message);

/**
 * @brief 注册所有LED控制命令处理函数
 * @return 无返回值
 * @details 为不同LED和控制类型注册对应的处理函数
 */
static void registerLedHandlers(void);

/* 静态函数实现
 * -------------------------------------------------------------*/
/**
 * @brief 处理LED打开命令
 * @param [in] context 上下文参数（未使用）
 * @param [in] message 系统消息指针
 * @return 处理成功返回true
 */
static bool handleLedTurnOn(void* context, const void* message)
{
    // 防止未使用参数警告
    (void)context;

    const MsgBusSystemMessage* msg = (const MsgBusSystemMessage*)message;
    HAL_ledTurnOn(msg->payload.ledData.ledSelection);
    return true;
}

/**
 * @brief 处理LED关闭命令
 * @param [in] context 上下文参数（未使用）
 * @param [in] message 系统消息指针
 * @return 处理成功返回true
 */
static bool handleLedTurnOff(void* context, const void* message)
{
    // 防止未使用参数警告
    (void)context;

    const MsgBusSystemMessage* msg = (const MsgBusSystemMessage*)message;
    HAL_ledTurnOff(msg->payload.ledData.ledSelection);
    return true;
}

/**
 * @brief 处理LED切换命令
 * @param [in] context 上下文参数（未使用）
 * @param [in] message 系统消息指针
 * @return 处理成功返回true
 */
static bool handleLedToggle(void* context, const void* message)
{
    // 防止未使用参数警告
    (void)context;

    const MsgBusSystemMessage* msg = (const MsgBusSystemMessage*)message;
    HAL_ledToggle(msg->payload.ledData.ledSelection);
    return true;
}

/**
 * @brief 注册所有LED控制命令处理函数
 * @return 无返回值
 * @details 为不同LED和控制类型注册对应的处理函数
 */
static void registerLedHandlers(void)
{
    CommonControlProcessorC* processor = CommonControlProcessorC_getInstance();

    // 注册LED开启处理函数，支持不同的LED选择
    processor->base.registerHandler(&processor->base,
                                    MAKE_LED_CONTROL_CMD(MSGBUS_LED_TURN_ON, LED_STATUS),
                                    handleLedTurnOn,
                                    NULL);
    processor->base.registerHandler(&processor->base,
                                    MAKE_LED_CONTROL_CMD(MSGBUS_LED_TURN_ON, LED_NETWORK),
                                    handleLedTurnOn,
                                    NULL);
    processor->base.registerHandler(&processor->base,
                                    MAKE_LED_CONTROL_CMD(MSGBUS_LED_TURN_ON, LED_FAULT),
                                    handleLedTurnOn,
                                    NULL);
    processor->base.registerHandler(&processor->base,
                                    MAKE_LED_CONTROL_CMD(MSGBUS_LED_TURN_ON, LED_ALARM),
                                    handleLedTurnOn,
                                    NULL);
    processor->base.registerHandler(&processor->base,
                                    MAKE_LED_CONTROL_CMD(MSGBUS_LED_TURN_ON, STATUS_LED_ALL),
                                    handleLedTurnOn,
                                    NULL);

    // 注册LED关闭处理函数，支持不同的LED选择
    processor->base.registerHandler(&processor->base,
                                    MAKE_LED_CONTROL_CMD(MSGBUS_LED_TURN_OFF, LED_STATUS),
                                    handleLedTurnOff,
                                    NULL);
    processor->base.registerHandler(&processor->base,
                                    MAKE_LED_CONTROL_CMD(MSGBUS_LED_TURN_OFF, LED_NETWORK),
                                    handleLedTurnOff,
                                    NULL);
    processor->base.registerHandler(&processor->base,
                                    MAKE_LED_CONTROL_CMD(MSGBUS_LED_TURN_OFF, LED_FAULT),
                                    handleLedTurnOff,
                                    NULL);
    processor->base.registerHandler(&processor->base,
                                    MAKE_LED_CONTROL_CMD(MSGBUS_LED_TURN_OFF, LED_ALARM),
                                    handleLedTurnOff,
                                    NULL);
    processor->base.registerHandler(&processor->base,
                                    MAKE_LED_CONTROL_CMD(MSGBUS_LED_TURN_OFF, STATUS_LED_ALL),
                                    handleLedTurnOff,
                                    NULL);

    // 注册LED切换处理函数，支持不同的LED选择
    processor->base.registerHandler(&processor->base,
                                    MAKE_LED_CONTROL_CMD(MSGBUS_LED_TOGGLE, LED_STATUS),
                                    handleLedToggle,
                                    NULL);
    processor->base.registerHandler(&processor->base,
                                    MAKE_LED_CONTROL_CMD(MSGBUS_LED_TOGGLE, LED_NETWORK),
                                    handleLedToggle,
                                    NULL);
    processor->base.registerHandler(&processor->base,
                                    MAKE_LED_CONTROL_CMD(MSGBUS_LED_TOGGLE, LED_FAULT),
                                    handleLedToggle,
                                    NULL);
    processor->base.registerHandler(&processor->base,
                                    MAKE_LED_CONTROL_CMD(MSGBUS_LED_TOGGLE, LED_ALARM),
                                    handleLedToggle,
                                    NULL);
    processor->base.registerHandler(&processor->base,
                                    MAKE_LED_CONTROL_CMD(MSGBUS_LED_TOGGLE, STATUS_LED_ALL),
                                    handleLedToggle,
                                    NULL);
}

/* 全局变量
 * -------------------------------------------------------------*/
/**
 * @var TaskHandle_t commonControlTaskHandle
 * @brief 通用控制任务句柄，用于控制和引用通用控制任务
 */
TaskHandle_t commonControlTaskHandle = NULL;

/* 全局函数实现
 * -------------------------------------------------------------*/
/**
 * @brief 通用控制任务函数，处理LED控制等通用功能
 * @param [in] pvParameters FreeRTOS任务参数
 *        This parameter is not used in this task
 * @return 无返回值
 * @note 任务会一直运行，等待并处理消息队列中的消息
 * @details 任务创建消息队列，注册处理函数，然后循环等待和处理消息
 */
void commonControlTask(void* pvParameters)
{
    // 防止未使用参数警告
    (void)pvParameters;

    // 创建本地消息实例，用于接收消息队列中的数据
    MsgBusSystemMessage commonControlMessage;

    // 静态创建队列（仅在首次运行时创建）
    if (commonControlQueue == NULL) {
        commonControlQueue = xQueueCreateStatic(10,                          // 队列长度
                                                sizeof(MsgBusSystemMessage), // 消息大小
                                                commonControlQueueStorage,   // 队列存储区
                                                &commonControlQueueBuffer);  // 队列缓冲区
    }

    // 订阅所有相关消息
    msgbus_subscribe(MSGBUS_MSG_LED_CONTROL, commonControlQueue);

    // 获取通用控制命令处理器单例并注册处理函数
    CommonControlProcessorC* processor = CommonControlProcessorC_getInstance();
    registerLedHandlers();

    for (;;) {
        // 等待消息
        if (msgbus_wait_for_message(commonControlQueue, &commonControlMessage)) {
            // 使用命令处理器执行命令
            processor->base.executeCommand(&processor->base, &commonControlMessage);
        }
    }
}

/**
 * @}
 */
