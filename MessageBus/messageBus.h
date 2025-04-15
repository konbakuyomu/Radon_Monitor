/**
 * @file messageBus.h
 * @brief 消息总线系统头文件 (纯C实现)
 * @date 2025-03-16
 * @details 实现基于FreeRTOS的线程安全消息传递系统，
 *          支持发布-订阅模式，用于系统各组件间的解耦通信。
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* 头文件
 * -------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "message_buffer.h"
#include "queue.h"
#include "semphr.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* 宏定义
 * -------------------------------------------------------------*/
/**
 * @defgroup MessageBus_Macros 消息总线相关宏
 * @brief 消息总线常量与配置
 * @{
 */
#define MESSAGE_BUS_INVALID_HANDLE (-1) /**< 无效句柄 */
#define MESSAGE_BUS_MAX_USART_DATA_SIZE 50 /**< 串口数据最大大小 */
/**
 * @}
 */

/* 类型定义
 * -------------------------------------------------------------*/
/**
 * @typedef MessageBusHandle
 * @brief 消息总线订阅句柄类型
 */
typedef int32_t MessageBusHandle;

/* 枚举
 * -------------------------------------------------------------*/
/**
 * @defgroup MessageBus_Enums 消息总线枚举类型
 * @brief 消息类型、LED控制等枚举
 * @{
 */
/**
 * @enum MessageBusType
 * @brief 消息类型枚举
 * @details 系统中所有支持的消息类型
 */
typedef enum {
    MESSAGE_BUS_TYPE_NONE = 0,           /**< 无类型（无效消息） */
    MESSAGE_BUS_TYPE_UART1_TRANSMIT = 1, /**< 串口1发送消息 */
    MESSAGE_BUS_TYPE_UART1_RECEIVE = 2,  /**< 串口1接收消息 */
    MESSAGE_BUS_TYPE_EEPROM_WRITE = 3,   /**< EEPROM写消息 */
    MESSAGE_BUS_TYPE_LED_CONTROL = 4,    /**< LED控制消息 */
    MESSAGE_BUS_TYPE_TEST_MESSAGE = 5,   /**< 测试消息 */
    MESSAGE_BUS_TYPE_MAX = 6             /**< 枚举边界标记 */
} MessageBusType;

/**
 * @enum LedControlAction
 * @brief LED控制动作枚举
 * @details 定义LED控制方式
 */
typedef enum {
    LED_CONTROL_ACTION_TURN_ON = 0,  /**< 打开LED */
    LED_CONTROL_ACTION_TURN_OFF = 1, /**< 关闭LED */
    LED_CONTROL_ACTION_TOGGLE = 2,   /**< 切换LED状态 */
    LED_CONTROL_ACTION_NONE = 3      /**< 无效控制 */
} LedControlAction;
/**
 * @}
 */

/* 结构体
 * -------------------------------------------------------------*/
/**
 * @defgroup MessageBus_Structs 消息总线结构体
 * @brief 消息结构体定义
 * @{
 */
/**
 * @struct MessageBusMessage
 * @brief 消息结构体
 * @details 定义系统中传递的消息格式，包含消息类型和负载
 */
typedef struct {
    MessageBusType type; /**< 消息类型 */
    union {
        uint8_t usartData; /**< 测试字节(实际上未使用) */
        struct {
            uint32_t address; /**< EEPROM地址 */
            uint8_t data[16]; /**< 数据数组 */
            size_t length;    /**< 有效数据长度 */
        } eeprom;
        struct {
            uint8_t ledIdentifier; /**< 选择需要控制的LED */
            LedControlAction action;   /**< 控制方式 */
        } led;
        uint32_t testValue; /**< 测试数据结构 */
    } payload;                    /**< 消息负载，根据消息类型解释 */
} MessageBusMessage;
/**
 * @}
 */

/* 函数声明
 * -------------------------------------------------------------*/
/**
 * @defgroup MessageBus_APIs 消息总线API
 * @brief 消息总线对外API
 * @{
 */
/**
 * @brief 初始化消息总线系统
 * @return 初始化是否成功
 */
bool initializeMessageBus(void);

/**
 * @brief 订阅指定类型的消息
 * @param [in] type 消息类型 @ref MessageBusType
 * @param [in] queue FreeRTOS队列句柄，用于接收消息
 * @return MessageBusHandle 订阅句柄，用于后续取消订阅；失败返回MESSAGE_BUS_INVALID_HANDLE
 */
MessageBusHandle subscribeMessage(MessageBusType type, QueueHandle_t queue);

/**
 * @brief 取消订阅指定类型的消息
 * @param [in] handle 订阅时获得的句柄
 * @return bool 操作是否成功
 */
bool unsubscribeMessage(MessageBusHandle handle);

/**
 * @brief 发布消息到总线
 * @param [in] message 要发布的消息
 */
void publishMessage(const MessageBusMessage* message);

/**
 * @brief 从ISR中发布消息到总线
 * @param [in] message 要发布的消息
 * @param [in] pHigherPriorityTaskWoken 是否需要任务切换
 */
void publishMessageFromInterrupt(const MessageBusMessage* message,
                                 BaseType_t* pHigherPriorityTaskWoken);

/**
 * @brief 等待消息
 * @param [in] queue 队列句柄
 * @param [out] message 接收到的消息
 * @return bool 是否成功接收消息
 */
bool waitForMessage(QueueHandle_t queue, MessageBusMessage* message);
/**
 * @}
 */

#ifdef __cplusplus
}
#endif
