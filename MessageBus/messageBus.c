/**
 * @file messageBus.c
 * @brief 消息总线系统实现 (纯C实现)
 * @date 2025-03-16
 * @details 实现基于FreeRTOS的线程安全消息传递系统功能
 */

/* 头文件
 * -------------------------------------------------------------*/
#include "messageBus.h"
#include "globalConfig.h"
#include <stdbool.h>
#include <string.h>

/* 常量定义
 * -------------------------------------------------------------*/
#define MAX_SUBSCRIBERS 8 /**< 每种消息类型的最大订阅者数量 */
#define MAX_TYPES       MSGBUS_MSG_MAX_MESSAGE /**< 消息类型总数 */

/* 类型定义
 * -------------------------------------------------------------*/

/**
 * @struct SubscriberEntry
 * @brief 订阅者记录结构体
 */
typedef struct {
    MsgBusHandle_t handle;     /**< 订阅句柄 */
    MsgBusMessageType msgType; /**< 订阅的消息类型 */
    QueueHandle_t queue;       /**< FreeRTOS队列句柄 */
    bool inUse;                /**< 是否在使用中 */
} SubscriberEntry;

/* 静态变量
 * -------------------------------------------------------------*/
static SubscriberEntry s_subscribers[MAX_SUBSCRIBERS * MAX_TYPES]; /**< 订阅者数组 */
static MsgBusHandle_t s_nextHandle = 0; /**< 下一个可用句柄 */
static bool s_initialized = false;      /**< 是否已初始化 */

/* 私有函数声明
 * -------------------------------------------------------------*/
static SubscriberEntry* findFreeEntry(void);
static SubscriberEntry* findEntryByHandle(MsgBusHandle_t handle);
static void sendMessageToSubscribers(const MsgBusSystemMessage* msg,
                                     bool fromISR,
                                     BaseType_t* pxHigherPriorityTaskWoken);

/* 函数定义
 * -------------------------------------------------------------*/

/**
 * @brief 初始化消息总线系统
 * @return 初始化是否成功
 */
bool msgbus_init(void)
{
    if (s_initialized) {
        return true; // 已经初始化
    }

    // 初始化所有订阅者条目
    memset(s_subscribers, 0, sizeof(s_subscribers));
    for (int i = 0; i < MAX_SUBSCRIBERS * MAX_TYPES; i++) {
        s_subscribers[i].inUse = false;
        s_subscribers[i].handle = MSGBUS_INVALID_HANDLE;
    }

    s_nextHandle = 0;
    s_initialized = true;
    return true;
}

/**
 * @brief 订阅指定类型的消息
 * @param [in] type 消息类型
 * @param [in] queue FreeRTOS队列句柄，用于接收消息
 * @return MsgBusHandle_t 订阅句柄，用于后续取消订阅；失败返回MSGBUS_INVALID_HANDLE
 */
MsgBusHandle_t msgbus_subscribe(MsgBusMessageType type, QueueHandle_t queue)
{
    // 参数检查
    if (type == MSGBUS_MSG_NONE || type >= MSGBUS_MSG_MAX_MESSAGE || !queue) {
        return MSGBUS_INVALID_HANDLE;
    }

    // 确保系统已初始化
    if (!s_initialized && !msgbus_init()) {
        return MSGBUS_INVALID_HANDLE;
    }

    MsgBusHandle_t newHandle;
    SubscriberEntry* entry;

    // 进入临界区，防止并发访问
    taskENTER_CRITICAL();

    // 寻找空闲条目
    entry = findFreeEntry();
    if (entry == NULL) {
        taskEXIT_CRITICAL();
        return MSGBUS_INVALID_HANDLE; // 没有可用条目
    }

    // 分配新句柄
    newHandle = s_nextHandle++;
    if (s_nextHandle < 0) { // 句柄溢出
        s_nextHandle = 0;
    }

    // 初始化条目
    entry->handle = newHandle;
    entry->msgType = type;
    entry->queue = queue;
    entry->inUse = true;

    taskEXIT_CRITICAL();

    return newHandle;
}

/**
 * @brief 取消订阅指定类型的消息
 * @param [in] handle 订阅时获得的句柄
 * @return bool 操作是否成功
 */
bool msgbus_unsubscribe(MsgBusHandle_t handle)
{
    if (handle == MSGBUS_INVALID_HANDLE || !s_initialized) {
        return false;
    }

    bool success = false;

    // 进入临界区，防止并发访问
    taskENTER_CRITICAL();

    // 查找句柄对应的条目
    SubscriberEntry* entry = findEntryByHandle(handle);
    if (entry != NULL && entry->inUse) {
        entry->inUse = false;
        entry->handle = MSGBUS_INVALID_HANDLE;
        entry->queue = NULL;
        success = true;
    }

    taskEXIT_CRITICAL();

    return success;
}

/**
 * @brief 发布消息到总线
 * @param [in] msg 要发布的消息
 */
void msgbus_publish(const MsgBusSystemMessage* msg)
{
    if (!s_initialized || !msg || msg->message == MSGBUS_MSG_NONE
        || msg->message >= MSGBUS_MSG_MAX_MESSAGE) {
        return;
    }

    sendMessageToSubscribers(msg, false, NULL);
}

/**
 * @brief 从ISR中发布消息到总线
 * @param [in] msg 要发布的消息
 * @param [in] pxHigherPriorityTaskWoken 是否需要任务切换
 */
void msgbus_publish_from_isr(const MsgBusSystemMessage* msg,
                             BaseType_t* pxHigherPriorityTaskWoken)
{
    if (!s_initialized || !msg || msg->message == MSGBUS_MSG_NONE
        || msg->message >= MSGBUS_MSG_MAX_MESSAGE) {
        return;
    }

    sendMessageToSubscribers(msg, true, pxHigherPriorityTaskWoken);
}

/**
 * @brief 等待消息
 * @param [in] queue 队列句柄
 * @param [out] msg 接收到的消息
 * @return bool 是否成功接收消息
 */
bool msgbus_wait_for_message(QueueHandle_t queue, MsgBusSystemMessage* msg)
{
    if (!s_initialized || !queue || !msg) {
        return false;
    }

    return xQueueReceive(queue, msg, portMAX_DELAY) == pdPASS;
}

/* 私有函数定义
 * -------------------------------------------------------------*/

/**
 * @brief 查找空闲的订阅者条目
 * @return SubscriberEntry* 找到的空闲条目；若无空闲条目，返回NULL
 */
static SubscriberEntry* findFreeEntry(void)
{
    for (int i = 0; i < MAX_SUBSCRIBERS * MAX_TYPES; i++) {
        if (!s_subscribers[i].inUse) {
            return &s_subscribers[i];
        }
    }
    return NULL; // 无可用条目
}

/**
 * @brief 根据句柄查找订阅者条目
 * @param [in] handle 订阅句柄
 * @return SubscriberEntry* 找到的条目；若未找到，返回NULL
 */
static SubscriberEntry* findEntryByHandle(MsgBusHandle_t handle)
{
    for (int i = 0; i < MAX_SUBSCRIBERS * MAX_TYPES; i++) {
        if (s_subscribers[i].inUse && s_subscribers[i].handle == handle) {
            return &s_subscribers[i];
        }
    }
    return NULL; // 未找到
}

/**
 * @brief 向所有订阅者发送消息
 * @param [in] msg 消息指针
 * @param [in] fromISR 是否从ISR中调用
 * @param [in] pxHigherPriorityTaskWoken 任务切换标志（仅ISR模式有效）
 */
static void sendMessageToSubscribers(const MsgBusSystemMessage* msg,
                                     bool fromISR,
                                     BaseType_t* pxHigherPriorityTaskWoken)
{
    MsgBusMessageType msgType = msg->message;

    if (fromISR) {
        UBaseType_t savedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();

        for (int i = 0; i < MAX_SUBSCRIBERS * MAX_TYPES; i++) {
            if (s_subscribers[i].inUse && s_subscribers[i].msgType == msgType) {
                xQueueSendFromISR(
                    s_subscribers[i].queue, msg, pxHigherPriorityTaskWoken);
            }
        }

        taskEXIT_CRITICAL_FROM_ISR(savedInterruptStatus);
    } else {
        taskENTER_CRITICAL();

        for (int i = 0; i < MAX_SUBSCRIBERS * MAX_TYPES; i++) {
            if (s_subscribers[i].inUse && s_subscribers[i].msgType == msgType) {
                xQueueSend(s_subscribers[i].queue, msg, 0);
            }
        }

        taskEXIT_CRITICAL();
    }
}