/**
 * @file buttonControl.c
 * @brief 按键控制任务处理实现文件
 * @date 2025-03-15
 * @details 实现按键任务函数，处理按键输入和LED控制功能
 */

/* 头文件
 * -------------------------------------------------------------*/
#include "buttonControl.h"
#include "buttonProcessor.h"
#include "globalConfig.h"

/**
 * @addtogroup Button_Task
 * @{
 */

/* 类型定义
 * -------------------------------------------------------------*/
/**
 * @brief 用于构建按键命令类型的宏定义
 * @details 将按键ID和事件类型组合成一个32位命令类型
 */
#define MAKE_BUTTON_CMD(event, key) (((uint32_t)(event) << 16) | (key))

/**
 * @brief 按键事件消息结构
 */
typedef struct {
    uint8_t keyNumber; /**< 按键编号 */
    btn_event_t event; /**< 按键事件类型 */
} ButtonEventMessage;

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

    CommonDriver_LedToggle(LED_STATUS);
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

    CommonDriver_LedToggle(LED_NETWORK);
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

    CommonDriver_LedToggle(LED_FAULT);
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

    CommonDriver_LedToggle(LED_ALARM);
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
 *        This parameter is not used in this task
 * @return 无返回值
 * @note 任务会一直运行，等待并处理按键输入事件
 * @details 初始化按键处理器并注册回调函数，然后循环检测和处理按键事件
 */
void buttonTask(void* pvParameters)
{
    // 防止未使用参数警告
    (void)pvParameters;

    // 按键事件消息
    ButtonEventMessage buttonMsg;

    // 获取按键处理器单例
    ButtonProcessorC* processor = ButtonProcessorC_getInstance();

    // 注册按键处理函数
    processor->base.registerHandler(
        &processor->base, MAKE_BUTTON_CMD(btn_click, KEY_BUTTON_1), handleButton1Click, NULL);
    processor->base.registerHandler(
        &processor->base, MAKE_BUTTON_CMD(btn_click, KEY_BUTTON_2), handleButton2Click, NULL);
    processor->base.registerHandler(
        &processor->base, MAKE_BUTTON_CMD(btn_click, KEY_BUTTON_3), handleButton3Click, NULL);
    processor->base.registerHandler(
        &processor->base, MAKE_BUTTON_CMD(btn_click, KEY_BUTTON_4), handleButton4Click, NULL);

    for (;;) {
        // 检查是否有按键事件
        if (btn_available() != 0) {
            // 读取按键事件
            btn_read_event(&buttonMsg.keyNumber, &buttonMsg.event);

            // 使用Command Processor处理按键事件
            bool handled = processor->base.executeCommand(&processor->base, &buttonMsg);

            if (!handled) {
                // 未处理的按键事件（可以添加默认处理或日志）
            }
        }

        HAL_delayMillis(10);
    }
}

/**
 * @}
 */
