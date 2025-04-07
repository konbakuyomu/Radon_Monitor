/**
 * @file usartDriver.cpp
 * @brief 串口驱动接口实现 (使用MessageBuffer重构)
 * @date 2025-03-18
 * @details 实现通信接口基类和USART1驱动类的所有方法，
 *          提供基于MessageBuffer的串口通信功能，同时实现
 *          C接口以支持与BSP层的交互。
 */

/* 头文件
 * -------------------------------------------------------------*/
#include "globalConfig.h"
#include <algorithm> // 为std::min和std::copy添加

/* 静态成员定义
 * -------------------------------------------------------------*/

/**
 * @defgroup USART1Driver_Static_Members USART1Driver静态成员定义
 * @{
 */
// 定义静态成员
std::array<uint8_t, 50> USART1Driver::sendBuffer = { 0 };
std::array<uint8_t, 50> USART1Driver::dmaBuffer = { 0 };
std::array<uint8_t, 50> USART1Driver::procBuffer = { 0 };
SemaphoreHandle_t USART1Driver::txCompleteSemaphore = nullptr;
StaticSemaphore_t USART1Driver::txCompleteSemaphoreBuffer;

// 消息缓冲区相关静态成员定义
MessageBufferHandle_t USART1Driver::txMessageBuffer = nullptr;
MessageBufferHandle_t USART1Driver::rxMessageBuffer = nullptr;
StaticMessageBuffer_t USART1Driver::txMessageBufferStorage;
StaticMessageBuffer_t USART1Driver::rxMessageBufferStorage;
uint8_t USART1Driver::txMessageBufferMemory[MAX_SEND_BUFFER_SIZE];
uint8_t USART1Driver::rxMessageBufferMemory[MAX_RECEIVE_BUFFER_SIZE];

/**
 * @}
 */

/* CommInterface基类实现
 * -------------------------------------------------------------*/

/**
 * @brief CommInterface构造函数
 * @details 初始化消息缓冲区句柄为空
 */
CommInterface::CommInterface()
{
    // 不再需要初始化成员变量，因为移除了它们
}

/**
 * @brief CommInterface析构函数
 * @details 基类虚析构函数，资源清理在子类中实现
 */
CommInterface::~CommInterface()
{
    // 基类析构函数，资源清理在子类中实现
}

/* USART1Driver实现
 * -------------------------------------------------------------*/

/**
 * @brief 获取USART1驱动单例
 * @return USART1Driver& 单例引用
 * @details 使用C++11静态局部变量的线程安全初始化特性
 */
USART1Driver& USART1Driver::getInstance() noexcept
{
    static USART1Driver instance;
    return instance;
}

/**
 * @brief USART1驱动构造函数
 * @details 创建消息缓冲区并初始化缓冲区
 */
USART1Driver::USART1Driver() noexcept
    : CommInterface()
{
    // 静态创建消息缓冲区（如果尚未创建）
    if (txMessageBuffer == nullptr) {
        txMessageBuffer = xMessageBufferCreateStatic(
            MAX_SEND_BUFFER_SIZE, txMessageBufferMemory, &txMessageBufferStorage);
    }

    if (rxMessageBuffer == nullptr) {
        rxMessageBuffer = xMessageBufferCreateStatic(
            MAX_RECEIVE_BUFFER_SIZE, rxMessageBufferMemory, &rxMessageBufferStorage);
    }

    // 创建二进制信号量（如果尚未创建）
    if (txCompleteSemaphore == nullptr) {
        txCompleteSemaphore
            = xSemaphoreCreateBinaryStatic(&txCompleteSemaphoreBuffer);
    }

    // 初始化消息
    usart1DriverMessage.message = Message::NONE;
    usart1DriverMessage.payload.usartData = pdFALSE;

    // 初始化测试数据
    testData = 0;
}

/**
 * @brief USART1驱动析构函数
 * @details 清理消息缓冲区资源
 */
USART1Driver::~USART1Driver() noexcept
{
    // 由于使用静态创建，并且是单例，不再需要在这里删除消息缓冲区
    // 如果程序需要清理资源，可以在其他地方提供专门的函数
}

/**
 * @brief 发送数据
 * @return size_t 成功发送的字节数
 * @details 从消息缓冲区读取数据并通过串口发送
 */
size_t USART1Driver::send()
{
    size_t bytesSent = 0;

    // 从消息缓冲区读取数据到发送缓冲区
    bytesSent = xMessageBufferReceive(
        txMessageBuffer, sendBuffer.data(), MAX_SEND_BUFFER_SIZE, portMAX_DELAY);

    if (bytesSent > 0) {
        // 假设上一次发送完成后信号量被释放了，但还没被消费（例如上次发送后没有正确等待完成）。
        // 如果不先清空这个信号量，当前函数会在第二次xSemaphoreTake时立即返回，误以为当前的发送已经完成了
        xSemaphoreTake(txCompleteSemaphore, 0);

        // 开始串口发送
        hal_uart_data_t uartData;
        uartData.data = sendBuffer.data();
        uartData.length = bytesSent;
        uartData.channel = UART_CHANNEL_1;
        HAL_uartSend(&uartData);

        // 等待发送完成信号量
        if (xSemaphoreTake(txCompleteSemaphore, pdMS_TO_TICKS(1000)) != pdTRUE) {
            // 超时处理，可以记录日志或采取其他措施
        }
    }

    // 现在可以安全地清空发送缓冲区
    sendBuffer.fill(0);

    return bytesSent;
}

/**
 * @brief 添加数据到发送缓冲区
 * @param [in] data 要发送的数据对象
 * @return size_t 实际添加到缓冲区的数据长度
 * @details 将数据写入发送消息缓冲区，等待send()函数发送
 */
/**
 * @brief 将待发送的数据添加到发送消息缓冲区，并通知发送任务
 * @param [in] data 包含待发送数据的 UartData 对象
 * @return size_t 成功添加到缓冲区的数据字节数。如果输入数据为空，则返回 0。
 * @details 此函数负责将应用层准备好的数据放入 `txMessageBuffer`。
 *          它并不直接执行物理发送，而是通过向消息总线发布 `UART1_TRANSMIT` 消息，
 *          来触发 `usart1Task` 任务调用 `send()`
 * 方法，后者才真正负责从缓冲区读取数据并通过 HAL 发送。
 */
size_t USART1Driver::sendTxDataMessage(UartData data)
{
    // 检查传入的数据对象是否为空，避免无效操作
    if (data.empty()) {
        return 0; // 如果没有数据，直接返回0
    }

    // 将 UartData 对象中的数据复制到发送消息缓冲区 (txMessageBuffer)。
    // txMessageBuffer 是一个 FreeRTOS Message Buffer，用于在任务间安全地传递数据流。
    // portMAX_DELAY
    // 表示如果缓冲区已满，此函数将阻塞，直到有足够空间写入或超时（此处设置为无限等待）。
    size_t bytesWritten = xMessageBufferSend(txMessageBuffer, // 目标消息缓冲区句柄
                                             data.data(), // 指向数据源的指针
                                             data.size(), // 要发送的数据字节数
                                             portMAX_DELAY); // 阻塞等待时间

    // 检查是否成功写入数据到缓冲区 (理论上使用 portMAX_DELAY
    // 应该总是成功，除非配置错误)
    if (bytesWritten == data.size()) {
        // 获取全局消息总线单例实例，用于任务间通信
        MessageBus& messageBus = MessageBus::getInstance();

        // 准备要发布的消息
        usart1DriverMessage.message
            = Message::UART1_TRANSMIT; // 设置消息类型为 UART1 发送请求
        // 设置消息负载。注意：这里 payload.usartData = pdTRUE 仅作为信号，
        // 表明有数据需要发送，并不传递数据指针或具体内容。
        // usart1Task 收到此消息后会调用 send() 方法处理 txMessageBuffer 中的数据。
        usart1DriverMessage.payload.usartData = pdTRUE;

        // 向消息总线发布消息，通知 usart1Task 有数据待发送
        messageBus.publish(usart1DriverMessage);
    } else {
        // 如果写入缓冲区的字节数与预期不符（例如缓冲区大小不足或配置问题），
        // 此处可以添加错误处理逻辑，例如记录日志。
        // 对于 portMAX_DELAY，理论上不应发生这种情况，除非缓冲区大小为0或句柄无效。
    }

    // 返回实际写入发送消息缓冲区的数据长度
    return bytesWritten;
}

/**
 * @brief 接收数据
 * @return UartData 接收到的数据对象
 * @details 从接收消息缓冲区读取数据并返回UartData对象
 */
UartData USART1Driver::receive()
{
    // 从消息缓冲区读取数据
    // 使用portMAX_DELAY表示无限等待，直到有数据可读
    size_t bytesReceived = 0;

    // 获取互斥锁后，从消息缓冲区读取数据到处理缓冲区
    bytesReceived = xMessageBufferReceive(rxMessageBuffer,   // 接收缓冲区
                                          procBuffer.data(), // 处理缓冲区数组
                                          MAX_RECEIVE_BUFFER_SIZE, // 处理缓冲区大小
                                          portMAX_DELAY);          // 等待时间

    if (bytesReceived > 0) {
        // 成功读取数据，返回数据对象
        return UartData(procBuffer.data(), bytesReceived);
    }

    // 无法获取互斥锁或读取数据失败，返回空对象
    return UartData();
}

/**
 * @brief 缓冲区切换函数
 * @param [in] bytesReceived 接收到的字节数
 * @details 在DMA接收中断中调用，将数据从DMA缓冲区发送到消息缓冲区
 */
void USART1Driver::sendRxDataMessage(size_t bytesReceived) noexcept
{
    // 定义变量用于追踪是否有高优先级任务被唤醒
    BaseType_t higherPriorityTaskWoken = pdFALSE;

    // 在中断中从DMA缓冲区接收数据到消息缓冲区
    if (bytesReceived > 0) {
        // 将DMA接收的数据发送到消息缓冲区
        xMessageBufferSendFromISR(rxMessageBuffer,
                                  dmaBuffer.data(),
                                  bytesReceived,
                                  &higherPriorityTaskWoken);

        // 清空DMA缓冲区，准备下一次接收
        dmaBuffer.fill(0);

        // 获取消息总线实例
        MessageBus& messageBus = MessageBus::getInstance();

        // 初始化消息
        usart1DriverMessage.message = Message::UART1_RECEIVE;
        usart1DriverMessage.payload.usartData = pdTRUE;

        // 从ISR中发布消息到总线
        messageBus.publishFromISR(usart1DriverMessage, &higherPriorityTaskWoken);

        // 在所有可能引起任务唤醒的操作之后检查是否需要任务切换
        portYIELD_FROM_ISR(higherPriorityTaskWoken);
    }
}

/**
 * @brief 获取DMA缓冲区指针
 * @return uint8_t* DMA缓冲区指针
 * @details 返回DMA缓冲区的起始地址，用于BSP层配置DMA
 */
uint8_t* USART1Driver::getDmaBufferPtr() noexcept { return dmaBuffer.data(); }

/**
 * @brief 保存测试数据
 * @param [in] data 测试数据
 * @details 保存测试数据
 */
void USART1Driver::saveTestData(uint32_t data) noexcept
{
    // 保存测试数据
    testData = data;
}

/**
 * @brief 获取测试数据
 * @return uint32_t 测试数据
 * @details 获取测试数据
 */
uint32_t USART1Driver::getTestData() noexcept
{
    // 返回测试数据
    return testData;
}

/**
 * @brief 发送测试数据
 * @param [in] data 测试数据
 * @details 发送测试数据
 */
void USART1Driver::sendTestDataMessage(uint32_t data) noexcept
{
    // 初始化消息
    SystemMessage testDataMsg;
    testDataMsg.message = Message::TEST_MESSAGE;
    testDataMsg.payload.testData = data;

    // 获取消息总线并发布消息
    MessageBus& messageBus = MessageBus::getInstance();
    messageBus.publish(testDataMsg);
}

/* C接口实现
 * -------------------------------------------------------------*/

extern "C" {

/**
 * @brief 获取DMA缓冲区指针
 * @return uint8_t* DMA缓冲区指针
 * @details C接口函数，供BSP层配置DMA目标地址
 */
uint8_t* USART1_GetDmaBufferPtr(void)
{
    return USART1Driver::getInstance().getDmaBufferPtr();
}

/**
 * @brief 获取DMA缓冲区大小
 * @return uint32_t DMA缓冲区大小(字节数)
 * @details C接口函数，供BSP层配置DMA传输计数
 */
uint32_t USART1_GetDmaBufferSize(void) { return USART1Driver::getDmaBufferSize(); }

/**
 * @brief 处理接收到的数据
 * @param [in] bytesReceived 接收到的字节数
 * @details C接口函数，供BSP层在DMA接收完成或超时中断中调用
 */
void USART1_ProcessReceivedData(size_t bytesReceived)
{
    USART1Driver::getInstance().sendRxDataMessage(bytesReceived);
}

/**
 * @brief USART1 发送完成通知
 * @details 在 USART1 发送完成中断中调用此函数通知 C++ 驱动
 */
void USART1_NotifyTxComplete(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // 从中断上下文释放信号量
    if (USART1Driver::txCompleteSemaphore != nullptr) {
        xSemaphoreGiveFromISR(USART1Driver::txCompleteSemaphore,
                              &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}
}
