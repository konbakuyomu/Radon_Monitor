/**
 * @file usartTask.c
 * @brief USART1任务处理实现文件
 * @date 2025-04-15
 * @details 实现USART1任务函数，处理串口通信的发送和接收功能
 */

/* 头文件
 * -------------------------------------------------------------*/
#include "globalConfig.h"
#include "messageBusProcessor.h"

/**
 * @addtogroup USART_Task
 * @{
 */

/* 静态变量
 * -------------------------------------------------------------*/
/**
 * @brief UART1消息队列句柄，用于接收UART1相关消息
 * @note 仅在当前文件内有效
 */
static QueueHandle_t uart1Queue = NULL;

/**
 * @brief UART1消息队列的静态存储区
 */
static StaticQueue_t uart1QueueBuffer;

/**
 * @brief UART1消息队列的静态存储区域
 * @note 大小为10个Message结构体的存储空间
 */
static uint8_t uart1QueueStorage[10 * sizeof(MessageBusMessage)];

/* 静态函数声明
 * -------------------------------------------------------------*/
/**
 * @brief 处理UART1发送命令
 * @param [in] context 上下文指针（本例中未使用）
 * @param [in] message 消息指针
 * @return 处理是否成功
 */
static bool handleUart1Transmit(void* context, const void* message);

/**
 * @brief 处理UART1接收命令
 * @param [in] context 上下文指针（本例中未使用）
 * @param [in] message 消息指针
 * @return 处理是否成功
 */
static bool handleUart1Receive(void* context, const void* message);

/**
 * @brief 处理UART1测试命令
 * @param [in] context 上下文指针（本例中未使用）
 * @param [in] message 消息指针
 * @return 处理是否成功
 */
static bool handleUart1Test(void* context, const void* message);

/* 静态函数实现
 * -------------------------------------------------------------*/
/**
 * @brief 处理UART1发送命令
 * @param [in] context 上下文指针（本例中未使用）
 * @param [in] message 消息指针
 * @return 处理是否成功
 */
static bool handleUart1Transmit(void* context, const void* message)
{
    (void)context; // 未使用参数
    const MessageBusMessage* msg = (const MessageBusMessage*)message;

    sendUsart1Data();
    return true;
}

/**
 * @brief 处理UART1接收命令
 * @param [in] context 上下文指针（本例中未使用）
 * @param [in] message 消息指针
 * @return 处理是否成功
 */
static bool handleUart1Receive(void* context, const void* message)
{
    (void)context; // 未使用参数
    const MessageBusMessage* msg = (const MessageBusMessage*)message;

    UartData usart1Data = receiveUsart1Data();

    // 这里做回显测试
    usart1Data.data[0] = 0xbf;
    usart1Data.data[1] = msg->payload.usartData;
    addUsart1TransmitData(usart1Data.data, usart1Data.size);
    return true;
}

/**
 * @brief 处理UART1测试命令
 * @param [in] context 上下文指针（本例中未使用）
 * @param [in] message 消息指针
 * @return 处理是否成功
 */
static bool handleUart1Test(void* context, const void* message)
{
    (void)context; // 未使用参数
    const MessageBusMessage* msg = (const MessageBusMessage*)message;

    // 将消息总线发来的测试数据存储到usart1Driver的testData成员变量中
    saveUsart1TestData(msg->payload.testValue);

    // 获取testData并拆成4字节小端序
    // uint32_t testDataValue = getUsart1TestData();
    // uint8_t testDataBytes[4];
    // testDataBytes[0] = (uint8_t)(testDataValue & 0xFF);
    // testDataBytes[1] = (uint8_t)((testDataValue >> 8) & 0xFF);
    // testDataBytes[2] = (uint8_t)((testDataValue >> 16) & 0xFF);
    // testDataBytes[3] = (uint8_t)((testDataValue >> 24) & 0xFF);
    // addUsart1TransmitData(testDataBytes, sizeof(testDataBytes));

    return true;
}

/* 全局变量
 * -------------------------------------------------------------*/
/**
 * @var TaskHandle_t usart1TaskHandle
 * @brief USART1任务句柄，用于控制和引用USART1任务
 */
TaskHandle_t usart1TaskHandle = NULL;

/* 全局函数实现
 * -------------------------------------------------------------*/
/**
 * @brief USART1任务函数，处理串口1数据的收发
 * @param [in] pvParameters FreeRTOS任务参数
 *        FreeRTOS任务创建时传入的参数，本任务中未使用
 * @return 无返回值
 * @note 任务会一直运行，采用事件驱动模式等待并处理消息队列中的消息
 *       消息类型包括：UART1发送、UART1接收、测试消息
 * @details 任务初始化消息队列、注册消息处理函数并订阅相关消息类型，
 *          然后进入循环等待并处理接收到的消息
 */
void usart1Task(void* pvParameters)
{
    // 显式忽略未使用参数，避免编译器警告
    (void)pvParameters;

    // 创建本地消息实例，用于等待消息总线中订阅的消息
    MessageBusMessage usart1TaskMessage;

    // 静态创建FreeRTOS消息队列（仅在首次运行时创建）
    // 队列长度10，消息大小为SystemMessage结构体大小
    // 使用预分配的存储空间uart1QueueStorage和队列控制块uart1QueueBuffer
    if (uart1Queue == NULL) {
        uart1Queue = xQueueCreateStatic(10,                        // 队列长度
                                        sizeof(MessageBusMessage), // 消息大小
                                        uart1QueueStorage,         // 队列存储区
                                        &uart1QueueBuffer);        // 队列控制块
    }

    // 初始化命令处理器
    SystemMessageDispatcher* processor = SystemMessageDispatcher_getInstance();

    // 注册串口发送、接收、测试命令处理函数
    processor->commandProcessor.registerHandler(
        &processor->commandProcessor, MESSAGE_BUS_TYPE_UART1_TRANSMIT, handleUart1Transmit, NULL);
    processor->commandProcessor.registerHandler(
        &processor->commandProcessor, MESSAGE_BUS_TYPE_UART1_RECEIVE, handleUart1Receive, NULL);
    processor->commandProcessor.registerHandler(
        &processor->commandProcessor, MESSAGE_BUS_TYPE_TEST_MESSAGE, handleUart1Test, NULL);

    // 订阅消息
    subscribeMessage(MESSAGE_BUS_TYPE_UART1_TRANSMIT, uart1Queue);
    subscribeMessage(MESSAGE_BUS_TYPE_UART1_RECEIVE, uart1Queue);
    subscribeMessage(MESSAGE_BUS_TYPE_TEST_MESSAGE, uart1Queue);

    // 主任务循环
    for (;;) {
        // 等待消息总线中订阅的消息到来
        if (waitForMessage(uart1Queue, &usart1TaskMessage)) {
            // 使用命令处理器执行命令
            bool handled = processor->commandProcessor.executeCommand(&processor->commandProcessor,
                                                                      &usart1TaskMessage);

            if (!handled) {
                // 处理未注册的消息类型（可选）
                // 本例中可以忽略，因为我们只订阅了已注册处理的消息类型
            }
        }
    }
}

/**
 * @}
 */
