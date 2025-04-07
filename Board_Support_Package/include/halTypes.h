/**
 * @file halTypes.h
 * @brief 硬件抽象层类型定义
 * @date 2025-03-07
 * @details 定义了硬件抽象层使用的各种数据类型，包括状态码、LED、PWM、UART和CAN相关类型
 */

#ifndef HAL_TYPES_H
#define HAL_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/* 头文件
 * -------------------------------------------------------------*/
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* 版本控制
 * -------------------------------------------------------------*/
#define HAL_VERSION_MAJOR 1
#define HAL_VERSION_MINOR 0

/* 返回码定义
 * -------------------------------------------------------------*/
/**
 * @typedef hal_status_t
 * @brief HAL层操作返回状态码
 */
typedef enum {
    HAL_OK = 0,          /**< 操作成功 */
    HAL_ERROR = -1,      /**< 一般错误 */
    HAL_BUSY = -2,       /**< 设备忙 */
    HAL_TIMEOUT = -3,    /**< 操作超时 */
    HAL_INVALID_ARG = -4 /**< 参数无效 */
} hal_status_t;

/* LED相关类型
 * -------------------------------------------------------------*/
/**
 * @typedef hal_led_t
 * @brief LED标识符类型
 */
typedef uint8_t hal_led_t;

/* PWM相关类型
 * -------------------------------------------------------------*/
/**
 * @typedef hal_pwm_direction_t
 * @brief PWM方向枚举
 */
typedef enum {
    PWM_FORWARD = 0, /**< 正向PWM */
    PWM_REVERSE = 1  /**< 反向PWM */
} hal_pwm_direction_t;

/**
 * @typedef hal_pwm_config_t
 * @brief PWM配置结构体
 */
typedef struct {
    uint32_t channel;   /**< 通道号 */
    uint32_t frequency; /**< 频率(Hz) */
    float dutyCycle;    /**< 占空比(0-100) */
} hal_pwm_config_t;

/* UART相关类型
 * -------------------------------------------------------------*/
/**
 * @typedef hal_uart_channel_t
 * @brief UART通道枚举
 */
typedef enum {
    UART_CHANNEL_1 = 0, /**< UART通道1 */
    UART_CHANNEL_2 = 1, /**< UART通道2 */
    UART_CHANNEL_3 = 2, /**< UART通道3 */
    UART_CHANNEL_4 = 3  /**< UART通道4 */
} hal_uart_channel_t;

/**
 * @typedef hal_uart_data_t
 * @brief UART数据传输结构体
 */
typedef struct {
    uint8_t* data;              /**< 数据指针 */
    size_t length;              /**< 数据长度 */
    hal_uart_channel_t channel; /**< 通道号 */
} hal_uart_data_t;

/* CAN相关类型
 * -------------------------------------------------------------*/
/**
 * @typedef hal_can_message_t
 * @brief CAN消息结构体
 */
typedef struct {
    uint32_t id;   /**< 消息ID */
    uint8_t* data; /**< 数据指针 */
    size_t length; /**< 数据长度 */
} hal_can_message_t;

#ifdef __cplusplus
}
#endif

#endif /* HAL_TYPES_H */
