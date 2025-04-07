/**
 * @file CommonDriver.cpp
 * @brief 通用组件控制器实现文件
 * @date 2025-03-23
 * @details 实现通用组件控制器类，提供对LED、PWM等基础组件的控制功能
 */

/* 头文件
 * -------------------------------------------------------------*/
#include "globalConfig.h"

/* CommonDriver类实现
 * -------------------------------------------------------------*/

/**
 * @brief 获取通用驱动器单例
 * @return CommonDriver& 通用驱动器单例引用
 */
CommonDriver& CommonDriver::getInstance()
{
    static CommonDriver instance;
    return instance;
}

/**
 * @brief 打开指定LED
 * @param [in] ledSelection LED选择器
 */
void CommonDriver::ledTurnOn(uint8_t ledSelection)
{
    sendLedControlMessage(ledSelection, LedControl::TURN_ON);
}

/**
 * @brief 关闭指定LED
 * @param [in] ledSelection LED选择器
 */
void CommonDriver::ledTurnOff(uint8_t ledSelection)
{
    sendLedControlMessage(ledSelection, LedControl::TURN_OFF);
}

/**
 * @brief 切换指定LED状态
 * @param [in] ledSelection LED选择器
 */
void CommonDriver::ledToggle(uint8_t ledSelection)
{
    sendLedControlMessage(ledSelection, LedControl::TOGGLE);
}

/**
 * @brief 发送LED控制消息
 * @param [in] ledSelection LED选择器
 * @param [in] control 控制命令
 */
void CommonDriver::sendLedControlMessage(uint8_t ledSelection, LedControl control)
{
    // 初始化消息
    SystemMessage ledMsg;
    ledMsg.message = Message::LED_CONTROL;
    ledMsg.payload.ledData.ledSelection = ledSelection;
    ledMsg.payload.ledData.control = control;

    // 获取消息总线并发布消息
    MessageBus::getInstance().publish(ledMsg);
}
