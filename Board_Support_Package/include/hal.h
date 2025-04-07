/**
 * @file hal.h
 * @brief 硬件抽象层头文件 (纯 C 版本)
 * @author konbakuyomu
 * @date 2025-03-07
 * @details 提供了硬件抽象层的接口定义，实现了对底层硬件的统一访问方式
 */

#ifndef HAL_H
#define HAL_H   

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
 * @param [in] initFunc 初始化函数
 * @param [in] delayFunc 延迟函数
 * @param [in] tickFunc 滴答计数函数
 * @return hal_status_t HAL_OK表示成功，其他表示失败
 */
hal_status_t HAL_registerCoreFunctions(void (*initFunc)(void),
                                       void (*delayFunc)(unsigned long),
                                       unsigned long (*tickFunc)(void));

/**
 * @brief 注册LED相关功能函数
 * @param [in] turnOnFunc 打开LED函数
 * @param [in] turnOffFunc 关闭LED函数
 * @param [in] toggleFunc 切换LED状态函数
 * @return hal_status_t HAL_OK表示成功，其他表示失败
 */
hal_status_t HAL_registerLedFunctions(hal_status_t (*turnOnFunc)(hal_led_t),
                                      hal_status_t (*turnOffFunc)(hal_led_t),
                                      hal_status_t (*toggleFunc)(hal_led_t));

/**
 * @brief 注册PWM相关功能函数
 * @param [in] startFunc 启动PWM函数
 * @param [in] stopFunc 停止PWM函数
 * @param [in] configureFunc 配置PWM函数
 * @param [in] setDirectionFunc 设置PWM方向函数
 * @param [in] getDirectionFunc 获取PWM方向函数
 * @return hal_status_t HAL_OK表示成功，其他表示失败
 */
hal_status_t HAL_registerPwmFunctions(hal_status_t (*startFunc)(uint32_t),
                                      hal_status_t (*stopFunc)(uint32_t),
                                      hal_status_t (*configureFunc)(const hal_pwm_config_t*),
                                      hal_status_t (*setDirectionFunc)(uint32_t, hal_pwm_direction_t),
                                      hal_status_t (*getDirectionFunc)(uint32_t, hal_pwm_direction_t*));

/**
 * @brief 注册UART相关功能函数
 * @param [in] sendFunc 发送UART数据函数
 * @param [in] receiveFunc 接收UART数据函数
 * @return hal_status_t HAL_OK表示成功，其他表示失败
 */
hal_status_t HAL_registerUartFunctions(hal_status_t (*sendFunc)(const hal_uart_data_t*),
                                      hal_status_t (*receiveFunc)(hal_uart_data_t*, uint32_t));

/**
 * @brief 注册CAN相关功能函数
 * @param [in] sendFunc 发送CAN消息函数
 * @return hal_status_t HAL_OK表示成功，其他表示失败
 */
hal_status_t HAL_registerCanFunctions(hal_status_t (*sendFunc)(const hal_can_message_t*));

/**
 * @brief 初始化HAL模块
 * @return hal_status_t HAL_OK表示成功，其他表示失败
 */
hal_status_t HAL_init(void);

/**
 * @brief 检查HAL是否已初始化
 * @return bool true表示已初始化，false表示未初始化
 */
bool HAL_isInitialized(void);

/**
 * @brief 获取HAL版本信息
 * @param [out] major 主版本号
 * @param [out] minor 次版本号
 */
void HAL_getVersion(uint8_t *major, uint8_t *minor);

/**
 * @brief 延迟指定毫秒数
 * @param [in] ms 延迟时间(毫秒)
 */
void HAL_delayMillis(unsigned long ms);

/**
 * @brief 获取当前系统滴答计数
 * @return 当前系统滴答计数值
 */
unsigned long HAL_getTick(void);

/* LED相关函数 */

/**
 * @brief 打开指定LED
 * @param [in] led LED标识符
 * @return hal_status_t HAL_OK表示成功，其他表示失败
 */
hal_status_t HAL_ledTurnOn(hal_led_t led);

/**
 * @brief 关闭指定LED
 * @param [in] led LED标识符
 * @return hal_status_t HAL_OK表示成功，其他表示失败
 */
hal_status_t HAL_ledTurnOff(hal_led_t led);

/**
 * @brief 切换指定LED状态
 * @param [in] led LED标识符
 * @return hal_status_t HAL_OK表示成功，其他表示失败
 */
hal_status_t HAL_ledToggle(hal_led_t led);

/* PWM相关函数 */

/**
 * @brief 启动指定的PWM
 * @param [in] channel PWM通道号
 * @return hal_status_t HAL_OK表示成功，其他表示失败
 */
hal_status_t HAL_pwmStart(uint32_t channel);

/**
 * @brief 停止指定的PWM
 * @param [in] channel PWM通道号
 * @return hal_status_t HAL_OK表示成功，其他表示失败
 */
hal_status_t HAL_pwmStop(uint32_t channel);

/**
 * @brief 配置PWM参数
 * @param [in] config PWM配置参数
 * @return hal_status_t HAL_OK表示成功，其他表示失败
 */
hal_status_t HAL_pwmConfigure(const hal_pwm_config_t *config);

/**
 * @brief 设置PWM方向
 * @param [in] channel PWM通道号
 * @param [in] direction PWM方向
 * @return hal_status_t HAL_OK表示成功，其他表示失败
 */
hal_status_t HAL_pwmSetDirection(uint32_t channel, hal_pwm_direction_t direction);

/**
 * @brief 获取PWM方向
 * @param [in] channel PWM通道号
 * @param [out] direction PWM方向
 * @return hal_status_t HAL_OK表示成功，其他表示失败
 */
hal_status_t HAL_pwmGetDirection(uint32_t channel, hal_pwm_direction_t *direction);

/* UART相关函数 */

/**
 * @brief 发送UART数据
 * @param [in] data UART数据配置
 * @return hal_status_t HAL_OK表示成功，其他表示失败
 */
hal_status_t HAL_uartSend(const hal_uart_data_t *data);

/**
 * @brief 接收UART数据
 * @param [in,out] data UART数据配置，接收到的数据存储在data->data中
 * @param [in] timeout 超时时间(毫秒)
 * @return hal_status_t HAL_OK表示成功，其他表示失败
 */
hal_status_t HAL_uartReceive(hal_uart_data_t *data, uint32_t timeout);

/* CAN相关函数 */

/**
 * @brief 发送CAN消息
 * @param [in] message CAN消息
 * @return hal_status_t HAL_OK表示成功，其他表示失败
 */
hal_status_t HAL_canSend(const hal_can_message_t *message);

#ifdef __cplusplus
}
#endif

#endif // HAL_H
