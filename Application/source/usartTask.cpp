/**
 * @file usartTask.cpp
 * @brief USART1任务处理实现文件
 * @date 2023-07-20
 * @details 实现USART1任务函数，处理串口通信的发送和接收功能
 */

/* 头文件
 * -------------------------------------------------------------*/
#include "globalConfig.h"

/* 全局变量
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
static uint8_t uart1QueueStorage[10 * sizeof(SystemMessage)];

/**
 * @brief 处理UART1发送命令
 * @param [in] msg 系统消息结构体
 */
static void handleUart1Transmit(const MsgBusSystemMessage& msg)
{
    // // 获取USART1实例
    // USART1Driver& usart1Driver = USART1Driver::getInstance();
    // usart1Driver.send();
    USART1Driver_Send();
}

/**
 * @brief 处理UART1接收命令
 * @param [in] msg 系统消息结构体
 */
static void handleUart1Receive(const MsgBusSystemMessage& msg)
{
    UartData_t usart1Data = USART1Driver_Receive();

    // 这里做回显测试
    usart1Data.data[0] = 0xbf;
    usart1Data.data[1] = 0x7f;
    USART1Driver_SendTxDataMessage(usart1Data.data, usart1Data.size);
}

/**
 * @brief 处理UART1测试命令
 * @param [in] msg 系统消息结构体
 */
static void handleUart1Test(const MsgBusSystemMessage& msg)
{
    // 将消息总线发来的测试数据存储到usart1Driver的testData成员变量中
    USART1Driver_SaveTestData(msg.payload.testData);
}

/* 函数实现
 * -------------------------------------------------------------*/
extern "C" {

/**
 * @var TaskHandle_t usart1TaskHandle
 * @brief USART1任务句柄，用于控制和引用USART1任务
 */
TaskHandle_t usart1TaskHandle = nullptr;

/**
 * @brief USART1任务函数，处理串口1数据的收发
 * @param [in] pvParameters FreeRTOS任务参数
 *        FreeRTOS任务创建时传入的参数，本任务中未使用
 * @return 无返回值
 * @note 任务会一直运行，采用事件驱动模式等待并处理消息队列中的消息
 *       消息类型包括：UART1发送、UART1接收、测试消息
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
        uart1Queue = xQueueCreateStatic(10,                    // 队列长度
                                        sizeof(SystemMessage), // 消息大小
                                        uart1QueueStorage,     // 队列存储区
                                        &uart1QueueBuffer);    // 队列控制块
    }

    msgbus_subscribe(MSGBUS_MSG_UART1_TRANSMIT, uart1Queue);
    msgbus_subscribe(MSGBUS_MSG_UART1_RECEIVE, uart1Queue);
    msgbus_subscribe(MSGBUS_MSG_TEST_MESSAGE, uart1Queue);

    // // 向消息总线订阅本任务关心的消息类型
    // // 这些消息将被路由到uart1Queue队列
    // messageBus.subscribe(Message::UART1_TRANSMIT, uart1Queue); // UART发送消息
    // messageBus.subscribe(Message::UART1_RECEIVE, uart1Queue);  // UART接收消息
    // messageBus.subscribe(Message::TEST_MESSAGE, uart1Queue);   // 测试消息

    // 主任务循环
    for (;;) {
        // 阻塞等待消息队列中的新消息
        if (msgbus_wait_for_message(uart1Queue, &usart1TaskMessage)) {
            if (usart1TaskMessage.message == MSGBUS_MSG_UART1_TRANSMIT) {
                handleUart1Transmit(usart1TaskMessage);
            } else if (usart1TaskMessage.message == MSGBUS_MSG_UART1_RECEIVE) {
                handleUart1Receive(usart1TaskMessage);
            } else if (usart1TaskMessage.message == MSGBUS_MSG_TEST_MESSAGE) {
                handleUart1Test(usart1TaskMessage);
            }
        }
    }
}
}