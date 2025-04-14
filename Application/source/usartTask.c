/**
 * @file usartTask.c
 * @brief USART1任务处理实现文件
 * @date 2025-04-11
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
static uint8_t uart1QueueStorage[10 * sizeof(MsgBusSystemMessage)];

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
    const MsgBusSystemMessage* msg = (const MsgBusSystemMessage*)message;

    USART1Driver_Send();
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
    const MsgBusSystemMessage* msg = (const MsgBusSystemMessage*)message;

    UartData_t usart1Data = USART1Driver_Receive();

    // 这里做回显测试
    usart1Data.data[0] = 0xbf;
    usart1Data.data[1] = msg->payload.usartData;
    USART1Driver_SendTxDataMessage(usart1Data.data, usart1Data.size);
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
    const MsgBusSystemMessage* msg = (const MsgBusSystemMessage*)message;

    // 将消息总线发来的测试数据存储到usart1Driver的testData成员变量中
    USART1Driver_SaveTestData(msg->payload.testData);
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

    // 创建本地消息实例，用于接收消息队列中的数据
    MsgBusSystemMessage usart1TaskMessage;

    // 静态创建FreeRTOS消息队列（仅在首次运行时创建）
    // 队列长度10，消息大小为SystemMessage结构体大小
    // 使用预分配的存储空间uart1QueueStorage和队列控制块uart1QueueBuffer
    if (uart1Queue == NULL) {
        uart1Queue = xQueueCreateStatic(10,                          // 队列长度
                                        sizeof(MsgBusSystemMessage), // 消息大小
                                        uart1QueueStorage,           // 队列存储区
                                        &uart1QueueBuffer);          // 队列控制块
    }

    // 初始化Command Processor
    MessageBusProcessorC* processor = MessageBusProcessorC_getInstance();

    // 注册消息处理函数
    processor->base.registerHandler(
        &processor->base, MSGBUS_MSG_UART1_TRANSMIT, handleUart1Transmit, NULL);
    processor->base.registerHandler(
        &processor->base, MSGBUS_MSG_UART1_RECEIVE, handleUart1Receive, NULL);
    processor->base.registerHandler(
        &processor->base, MSGBUS_MSG_TEST_MESSAGE, handleUart1Test, NULL);

    // 订阅消息
    msgbus_subscribe(MSGBUS_MSG_UART1_TRANSMIT, uart1Queue);
    msgbus_subscribe(MSGBUS_MSG_UART1_RECEIVE, uart1Queue);
    msgbus_subscribe(MSGBUS_MSG_TEST_MESSAGE, uart1Queue);

    // 主任务循环
    for (;;) {
        // 阻塞等待消息队列中的新消息
        if (msgbus_wait_for_message(uart1Queue, &usart1TaskMessage)) {
            // 使用Command Processor处理消息
            bool handled = processor->base.executeCommand(&processor->base, &usart1TaskMessage);

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
