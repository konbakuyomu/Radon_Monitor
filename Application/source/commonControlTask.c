/**
 * @file commonControlTask.c
 * @brief 通用控制任务处理实现文件
 * @date 2025-04-15
 * @details 实现通用控制任务函数，处理LED和其他通用功能的控制
 */

/* 头文件
 * -------------------------------------------------------------*/
#include "globalConfig.h"
#include "commonControlProcessor.h"
#include "ledConfig.h"

/**
 * @addtogroup Common_Control_Task
 * @{
 */

/* 静态变量
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
static uint8_t commonControlQueueStorage[10 * sizeof(MessageBusMessage)];

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

    const MessageBusMessage* msg = (const MessageBusMessage*)message;
    HAL_ledTurnOn(msg->payload.led.ledIdentifier);
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

    const MessageBusMessage* msg = (const MessageBusMessage*)message;
    HAL_ledTurnOff(msg->payload.led.ledIdentifier);
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

    const MessageBusMessage* msg = (const MessageBusMessage*)message;
    HAL_ledToggle(msg->payload.led.ledIdentifier);
    return true;
}

/**
 * @brief 注册所有LED控制命令处理函数
 * @return 无返回值
 * @details 为不同LED和控制类型注册对应的处理函数
 */
static void registerLedHandlers(void)
{
    DeviceControlProcessor* processor = DeviceControlProcessor_getInstance();

    // 注册LED开启处理函数，支持不同的LED选择
    processor->commandProcessor.registerHandler(
        &processor->commandProcessor,
        CREATE_LED_CONTROL_COMMAND(LED_CONTROL_ACTION_TURN_ON, LED_STATUS),
        handleLedTurnOn,
        NULL);
    processor->commandProcessor.registerHandler(
        &processor->commandProcessor,
        CREATE_LED_CONTROL_COMMAND(LED_CONTROL_ACTION_TURN_ON, LED_NETWORK),
        handleLedTurnOn,
        NULL);
    processor->commandProcessor.registerHandler(
        &processor->commandProcessor,
        CREATE_LED_CONTROL_COMMAND(LED_CONTROL_ACTION_TURN_ON, LED_FAULT),
        handleLedTurnOn,
        NULL);
    processor->commandProcessor.registerHandler(
        &processor->commandProcessor,
        CREATE_LED_CONTROL_COMMAND(LED_CONTROL_ACTION_TURN_ON, LED_ALARM),
        handleLedTurnOn,
        NULL);
    processor->commandProcessor.registerHandler(
        &processor->commandProcessor,
        CREATE_LED_CONTROL_COMMAND(LED_CONTROL_ACTION_TURN_ON, LED_IS_ALL),
        handleLedTurnOn,
        NULL);

    // 注册LED关闭处理函数，支持不同的LED选择
    processor->commandProcessor.registerHandler(
        &processor->commandProcessor,
        CREATE_LED_CONTROL_COMMAND(LED_CONTROL_ACTION_TURN_OFF, LED_STATUS),
        handleLedTurnOff,
        NULL);
    processor->commandProcessor.registerHandler(
        &processor->commandProcessor,
        CREATE_LED_CONTROL_COMMAND(LED_CONTROL_ACTION_TURN_OFF, LED_NETWORK),
        handleLedTurnOff,
        NULL);
    processor->commandProcessor.registerHandler(
        &processor->commandProcessor,
        CREATE_LED_CONTROL_COMMAND(LED_CONTROL_ACTION_TURN_OFF, LED_FAULT),
        handleLedTurnOff,
        NULL);
    processor->commandProcessor.registerHandler(
        &processor->commandProcessor,
        CREATE_LED_CONTROL_COMMAND(LED_CONTROL_ACTION_TURN_OFF, LED_ALARM),
        handleLedTurnOff,
        NULL);
    processor->commandProcessor.registerHandler(
        &processor->commandProcessor,
        CREATE_LED_CONTROL_COMMAND(LED_CONTROL_ACTION_TURN_OFF, LED_IS_ALL),
        handleLedTurnOff,
        NULL);

    // 注册LED切换处理函数，支持不同的LED选择
    processor->commandProcessor.registerHandler(
        &processor->commandProcessor,
        CREATE_LED_CONTROL_COMMAND(LED_CONTROL_ACTION_TOGGLE, LED_STATUS),
        handleLedToggle,
        NULL);
    processor->commandProcessor.registerHandler(
        &processor->commandProcessor,
        CREATE_LED_CONTROL_COMMAND(LED_CONTROL_ACTION_TOGGLE, LED_NETWORK),
        handleLedToggle,
        NULL);
    processor->commandProcessor.registerHandler(
        &processor->commandProcessor,
        CREATE_LED_CONTROL_COMMAND(LED_CONTROL_ACTION_TOGGLE, LED_FAULT),
        handleLedToggle,
        NULL);
    processor->commandProcessor.registerHandler(
        &processor->commandProcessor,
        CREATE_LED_CONTROL_COMMAND(LED_CONTROL_ACTION_TOGGLE, LED_ALARM),
        handleLedToggle,
        NULL);
    processor->commandProcessor.registerHandler(
        &processor->commandProcessor,
        CREATE_LED_CONTROL_COMMAND(LED_CONTROL_ACTION_TOGGLE, LED_IS_ALL),
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
 *        FreeRTOS任务创建时传入的参数，本任务中未使用
 * @return 无返回值
 * @note 任务会一直运行，等待并处理消息队列中的消息
 * @details 任务创建消息队列，注册处理函数，然后循环等待和处理消息
 */
void commonControlTask(void* pvParameters)
{
    // 防止未使用参数警告
    (void)pvParameters;

    // 创建本地消息实例，用于等待消息总线中订阅的消息
    MessageBusMessage commonControlMessage;

    // 静态创建队列（仅在首次运行时创建）
    if (commonControlQueue == NULL) {
        commonControlQueue = xQueueCreateStatic(10,                          // 队列长度
                                                sizeof(MessageBusMessage), // 消息大小
                                                commonControlQueueStorage,   // 队列存储区
                                                &commonControlQueueBuffer);  // 队列缓冲区
    }

    // 初始化命令处理器
    DeviceControlProcessor* processor = DeviceControlProcessor_getInstance();

    // 注册LED控制命令处理函数
    registerLedHandlers();

    // 订阅所有相关消息
    subscribeMessage(MESSAGE_BUS_TYPE_LED_CONTROL, commonControlQueue);

    // 主任务循环
    for (;;) {
        // 等待消息总线中订阅的消息到来
        if (waitForMessage(commonControlQueue, &commonControlMessage)) {
            // 使用命令处理器执行命令
            bool handled = processor->commandProcessor.executeCommand(&processor->commandProcessor,
                                                                      &commonControlMessage);

            if (!handled) {
                // 处理未注册的消息类型（可选）
                // 本例中可以忽略，因为我们只订阅了已注册处理的消息类型
            }
        }
    }
}

/**
 * @}
 */
