/**
 * @file commonDriver.c
 * @brief 通用组件控制器实现文件
 * @date 2025-04-11
 * @details 实现通用组件控制函数，提供对LED、PWM等基础组件的控制功能
 */

/* 头文件
 * -------------------------------------------------------------*/
#include "commonDriver.h"
#include "globalConfig.h"

/* 静态变量
 * -------------------------------------------------------------*/
static uint8_t common_driver_initialized = 0;

/* 私有函数声明
 * -------------------------------------------------------------*/
/**
 * @brief 发送LED控制消息
 * @param [in] ledSelection LED选择器
 * @param [in] control 控制命令
 */
static void CommonDriver_SendLedControlMessage(uint8_t ledSelection,
                                               LedControl_t control);

/* 公共函数实现
 * -------------------------------------------------------------*/

/**
 * @brief 初始化通用驱动器
 * @details 在使用其他函数前必须先调用此函数
 */
void CommonDriver_Init(void)
{
    // 初始化操作...
    common_driver_initialized = 1;
}

/**
 * @brief 打开指定LED
 * @param [in] ledSelection LED选择器
 */
void CommonDriver_LedTurnOn(uint8_t ledSelection)
{
    CommonDriver_SendLedControlMessage(ledSelection, LED_CONTROL_TURN_ON);
}

/**
 * @brief 关闭指定LED
 * @param [in] ledSelection LED选择器
 */
void CommonDriver_LedTurnOff(uint8_t ledSelection)
{
    CommonDriver_SendLedControlMessage(ledSelection, LED_CONTROL_TURN_OFF);
}

/**
 * @brief 切换指定LED状态
 * @param [in] ledSelection LED选择器
 */
void CommonDriver_LedToggle(uint8_t ledSelection)
{
    CommonDriver_SendLedControlMessage(ledSelection, LED_CONTROL_TOGGLE);
}

/* 私有函数实现
 * -------------------------------------------------------------*/

/**
 * @brief 发送LED控制消息
 * @param [in] ledSelection LED选择器
 * @param [in] control 控制命令
 */
static void CommonDriver_SendLedControlMessage(uint8_t ledSelection,
                                               LedControl_t control)
{
    // 验证LED选择是否有效
    // 有效的LED选择: LED_STATUS(0x01), LED_NETWORK(0x02), LED_FAULT(0x04),
    // LED_ALARM(0x08)或它们的组合
    if ((ledSelection & STATUS_LED_ALL) == 0
        || (ledSelection & ~STATUS_LED_ALL) != 0) {
        // ledSelection不包含任何有效的LED或包含无效的位
        return;
    }

    // 初始化消息
    MsgBusSystemMessage ledMsg;
    ledMsg.message = MSGBUS_MSG_LED_CONTROL;
    ledMsg.payload.ledData.ledSelection = ledSelection;

    // 转换LedControl_t枚举到MsgBusLedControl枚举
    switch (control) {
    case LED_CONTROL_TURN_ON:
        ledMsg.payload.ledData.control = MSGBUS_LED_TURN_ON;
        break;
    case LED_CONTROL_TURN_OFF:
        ledMsg.payload.ledData.control = MSGBUS_LED_TURN_OFF;
        break;
    case LED_CONTROL_TOGGLE:
        ledMsg.payload.ledData.control = MSGBUS_LED_TOGGLE;
        break;
    default:
        ledMsg.payload.ledData.control = MSGBUS_LED_NONE;
        break;
    }

    // 发布消息
    msgbus_publish(&ledMsg);
}