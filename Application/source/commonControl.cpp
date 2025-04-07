/**
 * @file commonControl.cpp
 * @brief 通用控制任务处理实现文件
 * @date 2025-03-20
 * @details 实现通用控制任务函数，处理LED和其他通用功能的控制
 */

/* 头文件
 * -------------------------------------------------------------*/
#include "globalConfig.h"

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
static uint8_t commonControlQueueStorage[10 * sizeof(SystemMessage)];

/* 静态函数声明
 * -------------------------------------------------------------*/
static void handleLedControl(const SystemMessage& msg);

/* 静态函数实现
 * -------------------------------------------------------------*/
/**
 * @brief 处理LED控制命令
 * @param [in] msg 系统消息结构体
 *        This parameter contains LED control information
 * @details 根据接收到的消息参数控制LED的开启、关闭或切换状态
 */
static void handleLedControl(const SystemMessage& msg)
{
    // 处理LED控制逻辑
    // 可以根据msg中的参数进行不同操作
    switch (msg.payload.ledData.control) {
    case LedControl::TURN_ON:
        // 打开LED
        HAL_ledTurnOn(msg.payload.ledData.ledSelection);
        break;
    case LedControl::TURN_OFF:
        // 关闭LED
        HAL_ledTurnOff(msg.payload.ledData.ledSelection);
        break;
    case LedControl::TOGGLE:
        // 翻转LED状态
        HAL_ledToggle(msg.payload.ledData.ledSelection);
        break;
    default:
        // 默认关闭所有LED
        HAL_ledTurnOff(STATUS_LED_ALL);
        break;
    }
}

/* 函数实现
 * -------------------------------------------------------------*/
extern "C" {

/**
 * @var TaskHandle_t commonControlTaskHandle
 * @brief 通用控制任务句柄，用于控制和引用通用控制任务
 */
TaskHandle_t commonControlTaskHandle = NULL;

/**
 * @brief 通用控制任务函数，处理LED控制等通用功能
 * @param [in] pvParameters FreeRTOS任务参数
 *        This parameter is not used in this task
 * @return 无返回值
 * @note 任务会一直运行，等待并处理消息队列中的消息
 */
void commonControlTask(void* pvParameters)
{
    // 防止未使用参数警告
    (void)pvParameters;

    // 创建消息实例
    SystemMessage commonControlMessage;

    // 获取消息总线实例
    MessageBus& messageBus = MessageBus::getInstance();

    // // 获取命令处理器实例
    MessageBusProcessor& cmdProcessor = MessageBusProcessor::getInstance();

    // // 注册命令处理函数
    cmdProcessor.registerHandler(Message::LED_CONTROL, handleLedControl);

    // 静态创建队列（仅在首次运行时创建）
    if (commonControlQueue == NULL) {
        commonControlQueue
            = xQueueCreateStatic(10,                         // 队列长度
                                 sizeof(SystemMessage),      // 消息大小
                                 commonControlQueueStorage,  // 队列存储区
                                 &commonControlQueueBuffer); // 队列缓冲区
    }

    // 订阅所有相关消息
    messageBus.subscribe(Message::LED_CONTROL, commonControlQueue);

    for (;;) {
        // 等待消息
        if (messageBus.waitForMessage(commonControlQueue, commonControlMessage)) {
            // 执行命令处理
            if (!cmdProcessor.executeCommand(commonControlMessage)) {
                // 如果没有找到对应的处理函数，可以在这里添加默认处理逻辑
            }
        }
    }
}
}
