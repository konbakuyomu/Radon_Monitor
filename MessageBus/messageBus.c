/**
 * @file messageBus.c
 * @brief 消息总线系统实现 (纯C实现)
 * @date 2025-03-16
 * @details 实现基于FreeRTOS的线程安全消息传递系统功能。
 */

/* 头文件
 * -------------------------------------------------------------*/
#include "messageBus.h"
#include "globalConfig.h"
#include <stdbool.h>
#include <string.h>

/* 宏定义
 * -------------------------------------------------------------*/
/**
 * @defgroup MessageBus_Macros 消息总线相关宏
 * @brief 消息总线常量与配置
 * @{
 */
#define MAX_SUBSCRIBER_COUNT   8 /**< 每种消息类型的最大订阅者数量 */
#define MAX_MESSAGE_TYPE_COUNT MESSAGE_BUS_TYPE_MAX /**< 消息类型总数 */
/**
 * @}
 */

/* 类型定义
 * -------------------------------------------------------------*/
/**
 * @addtogroup MessageBus_Structs
 * @{
 */
/**
 * @struct SubscriberRecord
 * @brief 订阅者记录结构体
 * @details 用于保存每个订阅者的句柄、消息类型、队列和使用状态。
 */
typedef struct {
    MessageBusHandle handle; /**< 订阅句柄 */
    MessageBusType type;     /**< 订阅的消息类型 @ref MessageBusType */
    QueueHandle_t queue;     /**< FreeRTOS队列句柄 */
    bool inUse;              /**< 是否在使用中 */
} SubscriberRecord;
/**
 * @}
 */

/* 静态全局变量
 * -------------------------------------------------------------*/
/**
 * @defgroup MessageBus_StaticVars 文件内静态变量
 * @brief 仅在本文件内使用的静态全局变量
 * @{
 */
/**
 * @var static SubscriberRecord g_subscriberTable[]
 * @brief 订阅者数组，保存所有订阅信息，仅在本文件内有效
 */
static SubscriberRecord g_subscriberTable[MAX_SUBSCRIBER_COUNT * MAX_MESSAGE_TYPE_COUNT];
/**
 * @var static MessageBusHandle g_nextHandle
 * @brief 下一个可用句柄，仅在本文件内有效
 */
static MessageBusHandle g_nextHandle = 0;
/**
 * @var static bool g_messageBusInitialized
 * @brief 消息总线是否已初始化，仅在本文件内有效
 */
static bool g_messageBusInitialized = false;
/**
 * @}
 */

/* 静态函数声明
 * -------------------------------------------------------------*/
/**
 * @brief 查找空闲的订阅者记录（仅本文件内使用）
 * @return [out] SubscriberRecord* 找到的空闲记录；若无空闲条目，返回NULL
 */
static SubscriberRecord* findFreeSubscriberRecord(void);
/**
 * @brief 根据句柄查找订阅者记录（仅本文件内使用）
 * @param [in] handle 订阅句柄
 * @return [out] SubscriberRecord* 找到的记录；若未找到，返回NULL
 */
static SubscriberRecord* findSubscriberRecordByHandle(MessageBusHandle handle);
/**
 * @brief 向所有订阅者分发消息（仅本文件内使用）
 * @param [in] message 消息指针
 * @param [in] fromInterrupt 是否从中断中调用
 * @param [in] pHigherPriorityTaskWoken 任务切换标志（仅ISR模式有效）
 */
static void dispatchMessageToSubscribers(const MessageBusMessage* message,
                                         bool fromInterrupt,
                                         BaseType_t* pHigherPriorityTaskWoken);

/* 函数定义
 * -------------------------------------------------------------*/
/**
 * @addtogroup MessageBus_APIs
 * @{
 */
/**
 * @brief 初始化消息总线系统
 * @retval true 已经初始化或初始化成功
 * @retval false 初始化失败
 */
bool initializeMessageBus(void)
{
    if (g_messageBusInitialized) {
        return true; // 已经初始化
    }
    // 初始化所有订阅者条目
    memset(g_subscriberTable, 0, sizeof(g_subscriberTable));
    for (int i = 0; i < MAX_SUBSCRIBER_COUNT * MAX_MESSAGE_TYPE_COUNT; i++) {
        g_subscriberTable[i].inUse = false;
        g_subscriberTable[i].handle = MESSAGE_BUS_INVALID_HANDLE;
    }
    g_nextHandle = 0;
    g_messageBusInitialized = true;
    return true;
}

/**
 * @brief 订阅指定类型的消息
 * @param [in] type 消息类型 @ref MessageBusType
 * @param [in] queue FreeRTOS队列句柄，用于接收消息
 * @return [out] MessageBusHandle 订阅句柄，用于后续取消订阅；失败返回MESSAGE_BUS_INVALID_HANDLE
 */
MessageBusHandle subscribeMessage(MessageBusType type, QueueHandle_t queue)
{
    if (type == MESSAGE_BUS_TYPE_NONE || type >= MESSAGE_BUS_TYPE_MAX || !queue) {
        return MESSAGE_BUS_INVALID_HANDLE;
    }
    if (!g_messageBusInitialized && !initializeMessageBus()) {
        return MESSAGE_BUS_INVALID_HANDLE;
    }
    MessageBusHandle newHandle;
    SubscriberRecord* record;
    taskENTER_CRITICAL();
    record = findFreeSubscriberRecord();
    if (record == NULL) {
        taskEXIT_CRITICAL();
        return MESSAGE_BUS_INVALID_HANDLE;
    }
    newHandle = g_nextHandle++;
    if (g_nextHandle < 0) {
        g_nextHandle = 0;
    }
    record->handle = newHandle;
    record->type = type;
    record->queue = queue;
    record->inUse = true;
    taskEXIT_CRITICAL();
    return newHandle;
}

/**
 * @brief 取消订阅指定类型的消息
 * @param [in] handle 订阅时获得的句柄
 * @retval true 操作成功
 * @retval false 操作失败
 */
bool unsubscribeMessage(MessageBusHandle handle)
{
    if (handle == MESSAGE_BUS_INVALID_HANDLE || !g_messageBusInitialized) {
        return false;
    }
    bool success = false;
    taskENTER_CRITICAL();
    SubscriberRecord* record = findSubscriberRecordByHandle(handle);
    if (record != NULL && record->inUse) {
        record->inUse = false;
        record->handle = MESSAGE_BUS_INVALID_HANDLE;
        record->queue = NULL;
        success = true;
    }
    taskEXIT_CRITICAL();
    return success;
}

/**
 * @brief 发布消息到总线
 * @param [in] message 要发布的消息
 */
void publishMessage(const MessageBusMessage* message)
{
    if (!g_messageBusInitialized || !message || message->type == MESSAGE_BUS_TYPE_NONE
        || message->type >= MESSAGE_BUS_TYPE_MAX) {
        return;
    }
    dispatchMessageToSubscribers(message, false, NULL);
}

/**
 * @brief 从ISR中发布消息到总线
 * @param [in] message 要发布的消息
 * @param [in] pHigherPriorityTaskWoken 是否需要任务切换
 */
void publishMessageFromInterrupt(const MessageBusMessage* message,
                                 BaseType_t* pHigherPriorityTaskWoken)
{
    if (!g_messageBusInitialized || !message || message->type == MESSAGE_BUS_TYPE_NONE
        || message->type >= MESSAGE_BUS_TYPE_MAX) {
        return;
    }
    dispatchMessageToSubscribers(message, true, pHigherPriorityTaskWoken);
}

/**
 * @brief 等待消息
 * @param [in] queue 队列句柄
 * @param [out] message 接收到的消息
 * @retval true 成功接收消息
 * @retval false 接收失败
 */
bool waitForMessage(QueueHandle_t queue, MessageBusMessage* message)
{
    if (!g_messageBusInitialized || !queue || !message) {
        return false;
    }
    return xQueueReceive(queue, message, portMAX_DELAY) == pdPASS;
}
/**
 * @}
 */

/* 静态函数定义
 * -------------------------------------------------------------*/
/**
 * @brief 查找空闲的订阅者记录
 * @return [out] SubscriberRecord* 找到的空闲记录；若无空闲条目，返回NULL
 */
static SubscriberRecord* findFreeSubscriberRecord(void)
{
    for (int i = 0; i < MAX_SUBSCRIBER_COUNT * MAX_MESSAGE_TYPE_COUNT; i++) {
        if (!g_subscriberTable[i].inUse) {
            return &g_subscriberTable[i];
        }
    }
    return NULL;
}

/**
 * @brief 根据句柄查找订阅者记录
 * @param [in] handle 订阅句柄
 * @return [out] SubscriberRecord* 找到的记录；若未找到，返回NULL
 */
static SubscriberRecord* findSubscriberRecordByHandle(MessageBusHandle handle)
{
    for (int i = 0; i < MAX_SUBSCRIBER_COUNT * MAX_MESSAGE_TYPE_COUNT; i++) {
        if (g_subscriberTable[i].inUse && g_subscriberTable[i].handle == handle) {
            return &g_subscriberTable[i];
        }
    }
    return NULL;
}

/**
 * @brief 向所有订阅者分发消息
 * @param [in] message 消息指针
 * @param [in] fromInterrupt 是否从中断中调用
 * @param [in] pHigherPriorityTaskWoken 任务切换标志（仅ISR模式有效）
 */
static void dispatchMessageToSubscribers(const MessageBusMessage* message,
                                         bool fromInterrupt,
                                         BaseType_t* pHigherPriorityTaskWoken)
{
    MessageBusType type = message->type;
    if (fromInterrupt) {
        UBaseType_t savedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();
        for (int i = 0; i < MAX_SUBSCRIBER_COUNT * MAX_MESSAGE_TYPE_COUNT; i++) {
            if (g_subscriberTable[i].inUse && g_subscriberTable[i].type == type) {
                xQueueSendFromISR(g_subscriberTable[i].queue, message, pHigherPriorityTaskWoken);
            }
        }
        taskEXIT_CRITICAL_FROM_ISR(savedInterruptStatus);
    } else {
        taskENTER_CRITICAL();
        for (int i = 0; i < MAX_SUBSCRIBER_COUNT * MAX_MESSAGE_TYPE_COUNT; i++) {
            if (g_subscriberTable[i].inUse && g_subscriberTable[i].type == type) {
                xQueueSend(g_subscriberTable[i].queue, message, 0);
            }
        }
        taskEXIT_CRITICAL();
    }
}