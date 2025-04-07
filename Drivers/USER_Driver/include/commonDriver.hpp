/**
 * @file CommonDriver.hpp
 * @brief 通用组件控制器头文件
 * @date 2025-03-20
 * @details 声明通用组件控制器类，提供对LED、PWM等基础组件的控制接口
 */

#pragma once

#ifdef __cplusplus
/* 头文件
 * -------------------------------------------------------------*/
#include <cstddef>
#include <cstdint>

/* 前向声明
 * -------------------------------------------------------------*/
// 前向声明，避免循环引用
enum class LedControl;

/* 通用驱动器类
 * -------------------------------------------------------------*/

/**
 * @class CommonDriver
 * @brief 通用驱动器类，封装对基础组件的控制
 * @details 提供简单统一的接口控制如LED、PWM等基础组件，内部通过消息总线发送消息实现
 */
class CommonDriver
{
public:
    /**
     * @brief 获取通用驱动器单例
     * @return CommonDriver& 通用驱动器单例引用
     */
    static CommonDriver& getInstance();

    /**
     * LED控制函数
     * --------------------------------------*/

    /**
     * @brief 打开指定LED
     * @param [in] ledSelection LED选择器
     */
    void ledTurnOn(uint8_t ledSelection);

    /**
     * @brief 关闭指定LED
     * @param [in] ledSelection LED选择器
     */
    void ledTurnOff(uint8_t ledSelection);

    /**
     * @brief 切换指定LED状态
     * @param [in] ledSelection LED选择器
     */
    void ledToggle(uint8_t ledSelection);

private:
    /**
     * @brief 构造函数(私有)
     */
    CommonDriver() = default;

    /**
     * @brief 析构函数(私有)
     */
    ~CommonDriver() = default;

    /**
     * @brief 发送LED控制消息
     * @param [in] ledSelection LED选择器
     * @param [in] control 控制命令
     */
    void sendLedControlMessage(uint8_t ledSelection, LedControl control);
};
#endif
