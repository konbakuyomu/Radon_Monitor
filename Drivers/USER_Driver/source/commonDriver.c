/**
 * @file commonDriver.c
 * @brief LED控制器实现文件
 * @date 2025-04-15
 * @details 实现LED控制相关函数。
 */

/* 头文件
 * -------------------------------------------------------------*/
#include "commonDriver.h"
#include "globalConfig.h"

/* 静态变量
 * -------------------------------------------------------------*/
/**
 * @var static uint8_t g_ledControllerInitialized
 * @brief LED控制器初始化标志，仅在本文件内有效
 */
static uint8_t g_ledControllerInitialized = 0;

/* 私有函数声明
 * -------------------------------------------------------------*/
/**
 * @brief 发送LED控制消息（仅在本文件内使用）
 * @param [in] ledIdentifier LED标识符
 * @param [in] command 控制命令 @ref LedControlCommand
 */
static void sendLedControlMessage(uint8_t ledIdentifier, LedControlCommand command);

/* 公共函数实现
 * -------------------------------------------------------------*/
/**
 * @addtogroup LedControl_API
 * @{
 */
/**
 * @brief 初始化LED控制器
 * @details 在使用其他LED控制函数前必须先调用此函数
 */
void initializeLedController(void)
{
    g_ledControllerInitialized = 1;
}

/**
 * @brief 打开指定LED
 * @param [in] ledIdentifier LED标识符，支持单个或组合
 */
void turnOnLed(uint8_t ledIdentifier)
{
    sendLedControlMessage(ledIdentifier, TURN_ON);
}

/**
 * @brief 关闭指定LED
 * @param [in] ledIdentifier LED标识符，支持单个或组合
 */
void turnOffLed(uint8_t ledIdentifier)
{
    sendLedControlMessage(ledIdentifier, TURN_OFF);
}

/**
 * @brief 切换指定LED状态
 * @param [in] ledIdentifier LED标识符，支持单个或组合
 */
void toggleLedState(uint8_t ledIdentifier)
{
    sendLedControlMessage(ledIdentifier, TOGGLE);
}
/**
 * @}
 */

/* 私有函数实现
 * -------------------------------------------------------------*/
/**
 * @brief 发送LED控制消息（仅在本文件内使用）
 * @param [in] ledIdentifier LED标识符
 * @param [in] command 控制命令 @ref LedControlCommand
 */
static void sendLedControlMessage(uint8_t ledIdentifier, LedControlCommand command)
{
    // 验证LED选择是否有效
    if ((ledIdentifier & LED_IS_ALL) == 0 || (ledIdentifier & ~LED_IS_ALL) != 0) {
        // ledIdentifier不包含任何有效的LED或包含无效的位
        return;
    }

    MessageBusMessage ledMessage;
    ledMessage.type = MESSAGE_BUS_TYPE_LED_CONTROL;
    ledMessage.payload.led.ledIdentifier = ledIdentifier;

    switch (command) {
    case TURN_ON:
        ledMessage.payload.led.action = LED_CONTROL_ACTION_TURN_ON;
        break;
    case TURN_OFF:
        ledMessage.payload.led.action = LED_CONTROL_ACTION_TURN_OFF;
        break;
    case TOGGLE:
        ledMessage.payload.led.action = LED_CONTROL_ACTION_TOGGLE;
        break;
    default:
        ledMessage.payload.led.action = LED_CONTROL_ACTION_NONE;
        break;
    }

    publishMessage(&ledMessage);
}
