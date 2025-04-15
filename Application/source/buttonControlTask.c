/**
 * @file buttonControlTask.c
 * @brief 按键控制任务处理实现文件
 * @date 2025-04-15
 * @details 实现按键任务函数，可以识别不同的按键动作
 */

/* 头文件
 * -------------------------------------------------------------*/
#include "buttonControlTask.h"
#include "buttonProcessor.h"
#include "globalConfig.h"

/**
 * @addtogroup Button_Control_Task
 * @{
 */

/* 静态函数声明
 * -------------------------------------------------------------*/
/**
 * @brief 处理按钮1点击事件
 * @param [in] context 上下文指针（未使用）
 * @param [in] message 消息指针（按键事件）
 * @return 处理是否成功
 * @details 切换状态LED的开关状态
 */
static bool handleButton1Click(void* context, const void* message)
{
    (void)context;
    (void)message;

    toggleLedState(LED_STATUS);
    return true;
}

/**
 * @brief 处理按钮2点击事件
 * @param [in] context 上下文指针（未使用）
 * @param [in] message 消息指针（按键事件）
 * @return 处理是否成功
 * @details 切换网络LED的开关状态
 */
static bool handleButton2Click(void* context, const void* message)
{
    (void)context;
    (void)message;

    toggleLedState(LED_NETWORK);
    return true;
}

/**
 * @brief 处理按钮3点击事件
 * @param [in] context 上下文指针（未使用）
 * @param [in] message 消息指针（按键事件）
 * @return 处理是否成功
 * @details 切换故障LED的开关状态
 */
static bool handleButton3Click(void* context, const void* message)
{
    (void)context;
    (void)message;

    toggleLedState(LED_FAULT);
    return true;
}

/**
 * @brief 处理按钮4点击事件
 * @param [in] context 上下文指针（未使用）
 * @param [in] message 消息指针（按键事件）
 * @return 处理是否成功
 * @details 切换报警LED的开关状态
 */
static bool handleButton4Click(void* context, const void* message)
{
    (void)context;
    (void)message;

    toggleLedState(LED_ALARM);
    return true;
}

/* 全局变量
 * -------------------------------------------------------------*/
/**
 * @var TaskHandle_t buttonTaskHandle
 * @brief 按键任务句柄，用于控制和引用按键处理任务
 */
TaskHandle_t buttonTaskHandle = NULL;

/* 全局函数实现
 * -------------------------------------------------------------*/
/**
 * @brief 按键处理任务函数，处理按键输入和LED控制
 * @param [in] pvParameters FreeRTOS任务参数
 *        FreeRTOS任务创建时传入的参数，本任务中未使用
 * @return 无返回值
 * @note 任务会一直运行，等待并处理按键输入事件
 * @details 初始化按键处理器并注册回调函数，然后循环检测和处理按键事件
 */
void buttonTask(void* pvParameters)
{
    // 防止未使用参数警告
    (void)pvParameters;

    // 创建按键事件消息实例，用于等待按键事件
    ButtonEventMessage buttonMsg;

    // 初始化命令处理器
    ButtonCommandProcessor* processor = ButtonCommandProcessor_getInstance();

    // 注册按键处理函数
    processor->commandProcessor.registerHandler(&processor->commandProcessor,
                                                CREATE_BUTTON_COMMAND(buttonClick, KEY_BUTTON_1),
                                                handleButton1Click,
                                                NULL);
    processor->commandProcessor.registerHandler(&processor->commandProcessor,
                                                CREATE_BUTTON_COMMAND(buttonClick, KEY_BUTTON_2),
                                                handleButton2Click,
                                                NULL);
    processor->commandProcessor.registerHandler(&processor->commandProcessor,
                                                CREATE_BUTTON_COMMAND(buttonClick, KEY_BUTTON_3),
                                                handleButton3Click,
                                                NULL);
    processor->commandProcessor.registerHandler(&processor->commandProcessor,
                                                CREATE_BUTTON_COMMAND(buttonClick, KEY_BUTTON_4),
                                                handleButton4Click,
                                                NULL);

    for (;;) {
        // 检查是否有按键事件
        if (buttonAvailableCount() != 0) {
            // 读取按键事件
            buttonReadEvent(&buttonMsg.buttonIdentifier, &buttonMsg.eventType);

            // 使用命令处理器执行命令
            bool handled = processor->commandProcessor.executeCommand(&processor->commandProcessor,
                                                                      &buttonMsg);

            if (!handled) {
                // 未处理的按键事件（可以添加默认处理或日志）
            }
        }

        HAL_delayMilliseconds(10);
    }
}

/**
 * @}
 */
