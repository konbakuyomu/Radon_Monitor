/**
 * @file messageBus_example.c
 * @brief 消息总线使用示例
 * @date 2025-03-16
 */

#include "messageBus.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <stdio.h>

// 任务堆栈大小
#define TASK_STACK_SIZE 128
// 任务优先级
#define PUBLISHER_PRIORITY (tskIDLE_PRIORITY + 1)
#define SUBSCRIBER_PRIORITY (tskIDLE_PRIORITY + 2)

// 任务句柄
static TaskHandle_t publisherTask = NULL;
static TaskHandle_t subscriberTask = NULL;

// 队列句柄
static QueueHandle_t messageQueue = NULL;

// 订阅句柄
static MsgBusHandle_t subscriptionHandle = MSGBUS_INVALID_HANDLE;

// 任务函数声明
static void publisherTaskFunction(void *pvParameters);
static void subscriberTaskFunction(void *pvParameters);

/**
 * @brief 初始化示例
 */
void messageBus_example_init(void)
{
    // 初始化消息总线
    if (!msgbus_init()) {
        printf("Failed to initialize message bus\n");
        return;
    }
    
    // 创建消息队列
    messageQueue = xQueueCreate(5, sizeof(MsgBusSystemMessage));
    if (messageQueue == NULL) {
        printf("Failed to create message queue\n");
        return;
    }
    
    // 订阅测试消息
    subscriptionHandle = msgbus_subscribe(MSGBUS_MSG_TEST_MESSAGE, messageQueue);
    if (subscriptionHandle == MSGBUS_INVALID_HANDLE) {
        printf("Failed to subscribe to test message\n");
        vQueueDelete(messageQueue);
        return;
    }
    
    // 创建发布者任务
    BaseType_t result = xTaskCreate(
        publisherTaskFunction,
        "Publisher",
        TASK_STACK_SIZE,
        NULL,
        PUBLISHER_PRIORITY,
        &publisherTask
    );
    
    if (result != pdPASS) {
        printf("Failed to create publisher task\n");
        msgbus_unsubscribe(subscriptionHandle);
        vQueueDelete(messageQueue);
        return;
    }
    
    // 创建订阅者任务
    result = xTaskCreate(
        subscriberTaskFunction,
        "Subscriber",
        TASK_STACK_SIZE,
        NULL,
        SUBSCRIBER_PRIORITY,
        &subscriberTask
    );
    
    if (result != pdPASS) {
        printf("Failed to create subscriber task\n");
        vTaskDelete(publisherTask);
        msgbus_unsubscribe(subscriptionHandle);
        vQueueDelete(messageQueue);
        return;
    }
    
    printf("Message bus example initialized successfully\n");
}

/**
 * @brief 清理示例资源
 */
void messageBus_example_cleanup(void)
{
    // 删除任务
    if (publisherTask != NULL) {
        vTaskDelete(publisherTask);
    }
    
    if (subscriberTask != NULL) {
        vTaskDelete(subscriberTask);
    }
    
    // 取消订阅
    if (subscriptionHandle != MSGBUS_INVALID_HANDLE) {
        msgbus_unsubscribe(subscriptionHandle);
    }
    
    // 删除队列
    if (messageQueue != NULL) {
        vQueueDelete(messageQueue);
    }
    
    printf("Message bus example cleaned up\n");
}

/**
 * @brief 发布者任务
 */
static void publisherTaskFunction(void *pvParameters)
{
    uint32_t counter = 0;
    
    while (1) {
        // 创建测试消息
        MsgBusSystemMessage msg;
        msg.message = MSGBUS_MSG_TEST_MESSAGE;
        msg.payload.testData = counter++;
        
        // 发布消息
        msgbus_publish(&msg);
        
        printf("Published message with data: %lu\n", (unsigned long)msg.payload.testData);
        
        // 延时1秒
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/**
 * @brief 订阅者任务
 */
static void subscriberTaskFunction(void *pvParameters)
{
    MsgBusSystemMessage receivedMsg;
    
    while (1) {
        // 等待消息
        if (msgbus_wait_for_message(messageQueue, &receivedMsg)) {
            printf("Received message with data: %lu\n", 
                   (unsigned long)receivedMsg.payload.testData);
        } else {
            printf("Failed to receive message\n");
        }
    }
} 