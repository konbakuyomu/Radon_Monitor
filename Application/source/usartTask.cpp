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
static void handleUart1Transmit(const SystemMessage& msg)
{
    // 获取USART1实例
    USART1Driver& usart1Driver = USART1Driver::getInstance();
    usart1Driver.send();
}

/**
 * @brief 处理UART1接收命令
 * @param [in] msg 系统消息结构体
 */
static void handleUart1Receive(const SystemMessage& msg)
{
    // 获取USART1实例
    USART1Driver& usart1Driver = USART1Driver::getInstance();

    // 获取协议解析器实例
    UartProtocolProcessor& uartProtocolProcessor
        = UartProtocolProcessor::getInstance();

    // 从 procBuffer 消息缓冲区中读取串口数据
    UartData usart1Data = usart1Driver.receive();

    // 解析串口数据
    uartProtocolProcessor.parseAndExecute(usart1Data.data(), usart1Data.size());

    // 这里做回显测试
    // usart1Data.data()[0] = 0xbf;
    // usart1Data.data()[1] = 0x7f;
    // usart1Driver.sendTxDataMessage(usart1Data);
}

/**
 * @brief 处理UART1测试命令
 * @param [in] msg 系统消息结构体
 */
static void handleUart1Test(const SystemMessage& msg)
{
    // 获取USART1实例
    USART1Driver& usart1Driver = USART1Driver::getInstance();

    // 将消息总线发来的测试数据存储到usart1Driver的testData成员变量中
    usart1Driver.saveTestData(msg.payload.testData);
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
    SystemMessage usart1TaskMessage;

    // 获取全局消息总线单例实例
    // 消息总线用于管理不同组件间的通信
    MessageBus& messageBus = MessageBus::getInstance();

    // 获取命令处理器单例实例
    // 负责将消息路由到对应的处理函数
    MessageBusProcessor& cmdProcessor = MessageBusProcessor::getInstance();

    // 注册消息类型与处理函数的映射关系
    // UART1发送消息 -> handleUart1Transmit函数
    // UART1接收消息 -> handleUart1Receive函数
    // 测试消息 -> handleUart1Test函数
    cmdProcessor.registerHandler(Message::UART1_TRANSMIT, handleUart1Transmit);
    cmdProcessor.registerHandler(Message::UART1_RECEIVE, handleUart1Receive);
    cmdProcessor.registerHandler(Message::TEST_MESSAGE, handleUart1Test);

    // 静态创建FreeRTOS消息队列（仅在首次运行时创建）
    // 队列长度10，消息大小为SystemMessage结构体大小
    // 使用预分配的存储空间uart1QueueStorage和队列控制块uart1QueueBuffer
    if (uart1Queue == NULL) {
        uart1Queue = xQueueCreateStatic(10,                    // 队列长度
                                        sizeof(SystemMessage), // 消息大小
                                        uart1QueueStorage,     // 队列存储区
                                        &uart1QueueBuffer);    // 队列控制块
    }

    // 向消息总线订阅本任务关心的消息类型
    // 这些消息将被路由到uart1Queue队列
    messageBus.subscribe(Message::UART1_TRANSMIT, uart1Queue); // UART发送消息
    messageBus.subscribe(Message::UART1_RECEIVE, uart1Queue);  // UART接收消息
    messageBus.subscribe(Message::TEST_MESSAGE, uart1Queue);   // 测试消息

    // 主任务循环
    for (;;) {
        // 阻塞等待消息队列中的新消息
        if (messageBus.waitForMessage(uart1Queue, usart1TaskMessage)) {
            // 执行消息对应的处理函数
            if (!cmdProcessor.executeCommand(usart1TaskMessage)) {
                // 处理函数未找到时的默认逻辑
                // 当前为空实现，可根据需要添加日志或错误处理
            }
        }
    }
}
}