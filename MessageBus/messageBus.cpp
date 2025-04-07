/**
 * @file messageBus.cpp
 * @brief 消息总线系统实现
 * @date 2025-03-16
 * @details 实现基于FreeRTOS的线程安全消息传递系统功能
 */

/* 头文件
 * -------------------------------------------------------------*/
#include "globalConfig.h"

/* 函数定义
 * -------------------------------------------------------------*/

/**
 * @brief 获取消息总线单例实例
 * @return MessageBus& 消息总线单例引用
 */
MessageBus& MessageBus::getInstance()
{
    static MessageBus instance; // C++11保证的线程安全初始化
    return instance;
}

/**
 * @brief 消息总线构造函数
 * @details 创建互斥量用于保护共享资源
 */
MessageBus::MessageBus() noexcept
{
    // 数组在声明时已经零初始化
}

/**
 * @brief 消息总线析构函数
 * @details 释放互斥量资源
 */
MessageBus::~MessageBus()
{
    // 不需要 vSemaphoreDelete()
}

/**
 * @brief 订阅指定类型的消息
 * @param [in] type 消息类型
 * @param [in] queue FreeRTOS队列句柄
 * @return bool 订阅成功返回true，否则返回false
 */
bool MessageBus::subscribe(Message type, QueueHandle_t queue) noexcept
{
    if (type == Message::NONE || type >= Message::MAX_MESSAGE || !queue) {
        return false;
    }

    const int typeIndex = static_cast<int>(type);
    bool success = false;

    // 现代C++: 替代了原来的双重循环查找空位的代码
    // 原代码:
    // 1. 先遍历数组检查是否已存在
    // 2. 再遍历数组寻找空位置(nullptr)插入
    // unordered_set自动处理重复和插入，O(1)时间复杂度
    // 进入临界区，防止其他任务并发修改 subscribers
    taskENTER_CRITICAL();
    auto result = subscribers[typeIndex].insert(queue);
    success = result.second || subscribers[typeIndex].count(queue) > 0;
    taskEXIT_CRITICAL();

    return success;
}

/**
 * @brief 取消订阅指定类型的消息
 * @param [in] type 消息类型
 * @param [in] queue 之前订阅时使用的队列句柄
 */
void MessageBus::unsubscribe(Message type, QueueHandle_t queue) noexcept
{
    if (type == Message::NONE || type >= Message::MAX_MESSAGE || !queue) {
        return;
    }

    const int typeIndex = static_cast<int>(type);

    // 现代C++: 替代了原来遍历数组查找元素的代码
    // 原代码:
    // for (int i = 0; i < MAX_SUBSCRIBERS; ++i) {
    //     if (subscribers[typeIndex][i] == queue) {
    //         subscribers[typeIndex][i] = nullptr;
    //         break;
    //     }
    // }
    // unordered_set.erase直接O(1)删除元素
    // 进入临界区，防止其他任务并发修改 subscribers
    taskENTER_CRITICAL();
    subscribers[typeIndex].erase(queue);
    taskEXIT_CRITICAL();
}

/**
 * @brief 发布消息到总线
 * @param [in] msg 要发布的消息
 * @details 将消息发送到所有订阅该类型的队列
 */
void MessageBus::publish(const SystemMessage& msg) noexcept
{
    if (msg.message == Message::NONE || msg.message >= Message::MAX_MESSAGE) {
        return;
    }

    const int typeIndex = static_cast<int>(msg.message);

    // 现代C++: 替代了原来遍历固定大小数组的代码
    // 原代码:
    // for (int i = 0; i < MAX_SUBSCRIBERS; ++i) {
    //     auto queue = subscribers[typeIndex][i];
    //     if (queue) {
    //         xQueueSend(queue, &msg, 0);
    //     }
    // }
    // 使用范围for循环自动遍历所有有效元素，更简洁高效
    // 进入临界区，防止其他任务并发修改 subscribers
    taskENTER_CRITICAL();
    for (const auto& queue : subscribers[typeIndex]) {
        if (queue) { // 仍然保留nullptr检查以保证安全
            xQueueSend(queue, &msg, 0);
        }
    }
    taskEXIT_CRITICAL();
}

/**
 * @brief 从ISR中发布消息到总线
 * @param [in] msg 要发布的消息
 * @param [in] pxHigherPriorityTaskWoken 是否需要任务切换
 */
void MessageBus::publishFromISR(const SystemMessage& msg,
                                BaseType_t* pxHigherPriorityTaskWoken) noexcept
{
    // 如果消息类型无效，直接返回
    if (msg.message == Message::NONE || msg.message >= Message::MAX_MESSAGE) {
        return;
    }

    // 获取消息类型索引
    const int typeIndex = static_cast<int>(msg.message);

    // 在 ISR 环境下，推荐使用 taskENTER_CRITICAL_FROM_ISR() /
    // taskEXIT_CRITICAL_FROM_ISR() 这一对 API。
    // 这两个宏会保存并恢复中断状态，使得临界区可以正确嵌套调用。
    UBaseType_t savedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();
    for (const auto& queue : subscribers[typeIndex]) {
        if (queue) {
            xQueueSendFromISR(queue, &msg, pxHigherPriorityTaskWoken);
        }
    }
    taskEXIT_CRITICAL_FROM_ISR(savedInterruptStatus);
}

/**
 * @brief 等待消息
 * @param [out] msg 接收到的消息
 * @return bool 是否成功接收消息
 */
bool MessageBus::waitForMessage(xQueueHandle queue, SystemMessage& msg) noexcept
{
    if (!queue) {
        return false;
        HAL_delayMillis(1000);
    }

    return xQueueReceive(queue, &msg, portMAX_DELAY) == pdPASS;
}
