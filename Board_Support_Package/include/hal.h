/**
 * @file hal.h
 * @brief 硬件抽象层头文件 (纯 C 版本)
 * @author konbakuyomu
 * @date 2025-03-07
 * @details 提供了硬件抽象层的接口定义，实现了对底层硬件的统一访问方式
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* 头文件
 * -------------------------------------------------------------*/
#include "halTypes.h"

/* 函数声明
 * -------------------------------------------------------------*/

/**
 * @brief 注册核心功能函数
 * @param [in] initializeFunction 初始化函数
 * @param [in] delayFunction 延迟函数
 * @param [in] getTickFunction 滴答计数函数
 * @return HardwareStatus
 *         - HAL_OK: 注册成功
 *         - HAL_INVALID_ARG: 参数无效
 * @details 注册系统核心功能，包括初始化、延时和获取系统时间等
 */
HardwareStatus HAL_registerCoreFunctions(void (*initializeFunction)(void),
                                         void (*delayFunction)(unsigned long),
                                         unsigned long (*getTickFunction)(void));

/**
 * @brief 注册LED相关功能函数
 * @param [in] turnOnFunction 打开LED函数
 * @param [in] turnOffFunction 关闭LED函数
 * @param [in] toggleFunction 切换LED状态函数
 * @return HardwareStatus
 *         - HAL_OK: 注册成功
 *         - HAL_INVALID_ARG: 参数无效
 * @details 注册LED控制相关功能，包括开、关和切换状态
 */
HardwareStatus HAL_registerLedFunctions(HardwareStatus (*turnOnFunction)(LedIdentifier),
                                        HardwareStatus (*turnOffFunction)(LedIdentifier),
                                        HardwareStatus (*toggleFunction)(LedIdentifier));

/**
 * @brief 注册PWM相关功能函数
 * @param [in] startFunction 启动PWM函数
 * @param [in] stopFunction 停止PWM函数
 * @param [in] configureFunction 配置PWM函数
 * @param [in] setDirectionFunction 设置PWM方向函数
 * @param [in] getDirectionFunction 获取PWM方向函数
 * @return HardwareStatus
 *         - HAL_OK: 注册成功
 *         - HAL_INVALID_ARG: 参数无效
 * @details 注册PWM控制相关功能，包括启动、停止、配置和方向控制
 */
HardwareStatus
    HAL_registerPwmFunctions(HardwareStatus (*startFunction)(uint32_t),
                             HardwareStatus (*stopFunction)(uint32_t),
                             HardwareStatus (*configureFunction)(const PwmConfiguration*),
                             HardwareStatus (*setDirectionFunction)(uint32_t, PwmDirection),
                             HardwareStatus (*getDirectionFunction)(uint32_t, PwmDirection*));

/**
 * @brief 注册UART相关功能函数
 * @param [in] sendFunction 发送UART数据函数
 * @param [in] receiveFunction 接收UART数据函数
 * @return HardwareStatus
 *         - HAL_OK: 注册成功
 *         - HAL_INVALID_ARG: 参数无效
 * @details 注册UART通信相关功能，包括数据发送和接收
 */
HardwareStatus HAL_registerUartFunctions(HardwareStatus (*sendFunction)(const UartTransferData*),
                                         HardwareStatus (*receiveFunction)(UartTransferData*,
                                                                           uint32_t));

/**
 * @brief 注册CAN相关功能函数
 * @param [in] sendFunction 发送CAN消息函数
 * @return HardwareStatus
 *         - HAL_OK: 注册成功
 *         - HAL_INVALID_ARG: 参数无效
 * @details 注册CAN通信相关功能，包括消息发送
 */
HardwareStatus HAL_registerCanFunctions(HardwareStatus (*sendFunction)(const CanMessage*));

/**
 * @brief 初始化HAL模块
 * @return HardwareStatus
 *         - HAL_OK: 初始化成功
 *         - HAL_ERROR: 初始化失败
 * @details 初始化硬件抽象层，准备系统硬件使用
 */
HardwareStatus HAL_initialize(void);

/**
 * @brief 检查HAL是否已初始化
 * @return bool
 *         - true: 已初始化
 *         - false: 未初始化
 * @details 检查HAL模块是否已经完成初始化
 */
bool HAL_isInitialized(void);

/**
 * @brief 获取HAL版本信息
 * @param [out] majorVersion 主版本号
 * @param [out] minorVersion 次版本号
 * @details 获取当前HAL的版本信息
 */
void HAL_getVersion(uint8_t* majorVersion, uint8_t* minorVersion);

/**
 * @brief 延迟指定毫秒数
 * @param [in] milliseconds 延迟时间(毫秒)
 * @details 实现系统的毫秒级延时
 */
void HAL_delayMilliseconds(unsigned long milliseconds);

/**
 * @brief 获取当前系统滴答计数
 * @return 当前系统滴答计数值
 * @details 获取系统启动以来的滴答计数，用于时间计算
 */
unsigned long HAL_getTickCount(void);

/* LED相关函数
 * -------------------------------------------------------------*/

/**
 * @brief 打开指定LED
 * @param [in] ledId LED标识符
 * @return HardwareStatus
 *         - HAL_OK: 操作成功
 *         - HAL_ERROR: 操作失败
 * @details 控制指定的LED点亮
 */
HardwareStatus HAL_ledTurnOn(LedIdentifier ledId);

/**
 * @brief 关闭指定LED
 * @param [in] ledId LED标识符
 * @return HardwareStatus
 *         - HAL_OK: 操作成功
 *         - HAL_ERROR: 操作失败
 * @details 控制指定的LED熄灭
 */
HardwareStatus HAL_ledTurnOff(LedIdentifier ledId);

/**
 * @brief 切换指定LED状态
 * @param [in] ledId LED标识符
 * @return HardwareStatus
 *         - HAL_OK: 操作成功
 *         - HAL_ERROR: 操作失败
 * @details 将指定LED的状态在开和关之间切换
 */
HardwareStatus HAL_ledToggle(LedIdentifier ledId);

/* PWM相关函数
 * -------------------------------------------------------------*/

/**
 * @brief 启动指定的PWM
 * @param [in] channelNumber PWM通道号
 * @return HardwareStatus
 *         - HAL_OK: 操作成功
 *         - HAL_ERROR: 操作失败
 * @details 启动指定通道的PWM输出
 */
HardwareStatus HAL_pwmStart(uint32_t channelNumber);

/**
 * @brief 停止指定的PWM
 * @param [in] channelNumber PWM通道号
 * @return HardwareStatus
 *         - HAL_OK: 操作成功
 *         - HAL_ERROR: 操作失败
 * @details 停止指定通道的PWM输出
 */
HardwareStatus HAL_pwmStop(uint32_t channelNumber);

/**
 * @brief 配置PWM参数
 * @param [in] configuration PWM配置参数
 * @return HardwareStatus
 *         - HAL_OK: 操作成功
 *         - HAL_ERROR: 操作失败
 * @details 根据配置参数设置PWM通道的工作模式
 */
HardwareStatus HAL_pwmConfigure(const PwmConfiguration* configuration);

/**
 * @brief 设置PWM方向
 * @param [in] channelNumber PWM通道号
 * @param [in] direction PWM方向
 * @return HardwareStatus
 *         - HAL_OK: 操作成功
 *         - HAL_ERROR: 操作失败
 * @details 设置指定PWM通道的输出方向
 */
HardwareStatus HAL_pwmSetDirection(uint32_t channelNumber, PwmDirection direction);

/**
 * @brief 获取PWM方向
 * @param [in] channelNumber PWM通道号
 * @param [out] direction PWM方向
 * @return HardwareStatus
 *         - HAL_OK: 操作成功
 *         - HAL_ERROR: 操作失败
 * @details 获取指定PWM通道的当前输出方向
 */
HardwareStatus HAL_pwmGetDirection(uint32_t channelNumber, PwmDirection* direction);

/* UART相关函数
 * -------------------------------------------------------------*/

/**
 * @brief 发送UART数据
 * @param [in] transferData UART数据配置
 * @return HardwareStatus
 *         - HAL_OK: 发送成功
 *         - HAL_ERROR: 发送失败
 * @details 通过指定的UART通道发送数据
 */
HardwareStatus HAL_uartSend(const UartTransferData* transferData);

/**
 * @brief 接收UART数据
 * @param [in,out] transferData UART数据配置，接收到的数据存储在transferData->data中
 * @param [in] timeoutMilliseconds 超时时间(毫秒)
 * @return HardwareStatus
 *         - HAL_OK: 接收成功
 *         - HAL_ERROR: 接收失败
 *         - HAL_TIMEOUT: 接收超时
 * @details 从指定的UART通道接收数据，支持超时机制
 */
HardwareStatus HAL_uartReceive(UartTransferData* transferData, uint32_t timeoutMilliseconds);

/* CAN相关函数
 * -------------------------------------------------------------*/

/**
 * @brief 发送CAN消息
 * @param [in] message CAN消息
 * @return HardwareStatus
 *         - HAL_OK: 发送成功
 *         - HAL_ERROR: 发送失败
 * @details 发送CAN总线消息
 */
HardwareStatus HAL_canSend(const CanMessage* message);

#ifdef __cplusplus
}
#endif
