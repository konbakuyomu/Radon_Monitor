/**
 * @file halTypes.h
 * @brief 硬件抽象层类型定义
 * @date 2025-03-07
 * @details 定义了硬件抽象层使用的各种数据类型，包括状态码、LED、PWM、UART和CAN相关类型
 */

#pragma once

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
/**
 * @defgroup HAL_Version 硬件抽象层版本定义
 * @brief 硬件抽象层版本号常量
 * @{
 */
/** @brief HAL主版本号 */
#define HAL_VERSION_MAJOR 1
/** @brief HAL次版本号 */
#define HAL_VERSION_MINOR 0
/**
 * @}
 */

/* 返回码定义
 * -------------------------------------------------------------*/
/**
 * @typedef HardwareStatus
 * @brief HAL层操作返回状态码
 */
typedef enum {
    HAL_OK = 0,          /**< 操作成功 */
    HAL_ERROR = -1,      /**< 一般错误 */
    HAL_BUSY = -2,       /**< 设备忙 */
    HAL_TIMEOUT = -3,    /**< 操作超时 */
    HAL_INVALID_ARG = -4 /**< 参数无效 */
} HardwareStatus;

/* LED相关类型
 * -------------------------------------------------------------*/
/**
 * @typedef LedIdentifier
 * @brief LED标识符类型
 * @details 用于表示LED的标识符，由ledConfig.h中定义的LED常量赋值
 */
typedef uint8_t LedIdentifier;

/* PWM相关类型
 * -------------------------------------------------------------*/
/**
 * @typedef PwmDirection
 * @brief PWM方向枚举
 */
typedef enum {
    PWM_FORWARD = 0, /**< 正向PWM */
    PWM_REVERSE = 1  /**< 反向PWM */
} PwmDirection;

/**
 * @typedef PwmConfiguration
 * @brief PWM配置结构体
 */
typedef struct {
    uint32_t channel;       /**< 通道号 */
    uint32_t frequencyHz;   /**< 频率(Hz) */
    float dutyCyclePercent; /**< 占空比(0-100) */
} PwmConfiguration;

/* UART相关类型
 * -------------------------------------------------------------*/
/**
 * @typedef UartChannel
 * @brief UART通道枚举
 */
typedef enum {
    UART_CHANNEL_1 = 0, /**< UART通道1 */
    UART_CHANNEL_2 = 1, /**< UART通道2 */
    UART_CHANNEL_3 = 2, /**< UART通道3 */
    UART_CHANNEL_4 = 3  /**< UART通道4 */
} UartChannel;

/**
 * @typedef UartTransferData
 * @brief UART数据传输结构体
 */
typedef struct {
    uint8_t* data;       /**< 数据指针 */
    size_t length;       /**< 数据长度 */
    UartChannel channel; /**< 通道号 */
} UartTransferData;

/* CAN相关类型
 * -------------------------------------------------------------*/
/**
 * @typedef CanMessage
 * @brief CAN消息结构体
 */
typedef struct {
    uint32_t id;   /**< 消息ID */
    uint8_t* data; /**< 数据指针 */
    size_t length; /**< 数据长度 */
} CanMessage;

#ifdef __cplusplus
}
#endif
