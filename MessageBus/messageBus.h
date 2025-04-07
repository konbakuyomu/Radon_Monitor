/**
 * @file messageBus.h
 * @brief 消息总线系统头文件 (纯C实现)
 * @date 2025-03-16
 * @details 实现基于FreeRTOS的线程安全消息传递系统，
 *          支持发布-订阅模式，用于系统各组件间的解耦通信。
 */

#ifndef MESSAGE_BUS_H
#define MESSAGE_BUS_H

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

/* 类型定义
 * -------------------------------------------------------------*/
typedef int32_t MsgBusHandle_t; /**< 消息总线订阅句柄类型 */


#define MSGBUS_INVALID_HANDLE (-1) /**< 无效句柄 */
/* 枚举
 * -------------------------------------------------------------*/

/**
 * @enum MsgBusMessageType
 * @brief 消息类型枚举
 * @details 系统中所有支持的消息类型
 */
typedef enum {
    MSGBUS_MSG_NONE = 0,           /**< 无类型（无效消息） */
    MSGBUS_MSG_UART1_TRANSMIT = 1, /**< 串口1发送消息 */
    MSGBUS_MSG_UART1_RECEIVE = 2,  /**< 串口1接收消息 */
    MSGBUS_MSG_EEPROM_WRITE = 3,   /**< EEPROM写消息 */
    MSGBUS_MSG_LED_CONTROL = 4,    /**< LED控制消息 */
    MSGBUS_MSG_TEST_MESSAGE = 5,   /**< 测试消息 */
    MSGBUS_MSG_MAX_MESSAGE = 6     /**< 枚举边界标记 */
} MsgBusMessageType;

/**
 * @enum MsgBusLedControl
 * @brief LED控制枚举
 * @details 定义LED控制方式
 */
typedef enum {
    MSGBUS_LED_TURN_ON = 0,  /**< 打开LED */
    MSGBUS_LED_TURN_OFF = 1, /**< 关闭LED */
    MSGBUS_LED_TOGGLE = 2,   /**< 切换LED状态 */
    MSGBUS_LED_NONE = 3      /**< 无效控制 */
} MsgBusLedControl;

/* 常量定义
 * -------------------------------------------------------------*/
#define MSGBUS_MAX_USART_DATA_SIZE 50 /**< 串口数据最大大小 */

/* 结构体
 * -------------------------------------------------------------*/

/**
 * @struct MsgBusSystemMessage
 * @brief 消息结构体
 * @details 定义系统中传递的消息格式，包含消息类型和负载
 */
typedef struct {
    MsgBusMessageType message; /**< 消息类型 */
    union {
        /**
         * @brief 测试字节(实际上未使用)
         */
        uint8_t usartData;

        /**
         * @brief EEPROM数据结构
         */
        struct {
            uint32_t address; /**< EEPROM地址 */
            uint8_t data[16]; /**< 数据数组 */
            size_t len;       /**< 有效数据长度 */
        } eepromData;

        /**
         * @brief LED控制数据结构
         */
        struct {
            uint8_t ledSelection; /**< 选择需要控制的LED */
            MsgBusLedControl control;   /**< 控制方式 */
        } ledData;

        /**
         * @brief 测试数据结构
         */
        uint32_t testData;
    } payload;                    /**< 消息负载，根据消息类型解释 */
} MsgBusSystemMessage;

/* 函数声明
 * -------------------------------------------------------------*/

/**
 * @brief 初始化消息总线系统
 * @return 初始化是否成功
 */
bool msgbus_init(void);

/**
 * @brief 订阅指定类型的消息
 * @param [in] type 消息类型
 * @param [in] queue FreeRTOS队列句柄，用于接收消息
 * @return MsgBusHandle_t 订阅句柄，用于后续取消订阅；失败返回MSGBUS_INVALID_HANDLE
 */
MsgBusHandle_t msgbus_subscribe(MsgBusMessageType type, QueueHandle_t queue);

/**
 * @brief 取消订阅指定类型的消息
 * @param [in] handle 订阅时获得的句柄
 * @return bool 操作是否成功
 */
bool msgbus_unsubscribe(MsgBusHandle_t handle);

/**
 * @brief 发布消息到总线
 * @param [in] msg 要发布的消息
 */
void msgbus_publish(const MsgBusSystemMessage* msg);

/**
 * @brief 从ISR中发布消息到总线
 * @param [in] msg 要发布的消息
 * @param [in] pxHigherPriorityTaskWoken 是否需要任务切换
 */
void msgbus_publish_from_isr(const MsgBusSystemMessage* msg,
                             BaseType_t* pxHigherPriorityTaskWoken);

/**
 * @brief 等待消息
 * @param [in] queue 队列句柄
 * @param [out] msg 接收到的消息
 * @return bool 是否成功接收消息
 */
bool msgbus_wait_for_message(QueueHandle_t queue, MsgBusSystemMessage* msg);

#ifdef __cplusplus
}
#endif

#endif /* MESSAGE_BUS_H */ 