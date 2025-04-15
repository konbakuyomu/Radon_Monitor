/**
 * @file usart1Driver.c
 * @brief USART1驱动器实现文件
 * @date 2025-04-15
 * @details 实现USART1驱动相关的消息缓冲、DMA、FreeRTOS集成等功能。
 */

/* 头文件
 * -------------------------------------------------------------*/
#include "globalConfig.h"

/* 静态变量
 * -------------------------------------------------------------*/
/**
 * @var static Usart1Driver g_usart1DriverInstance
 * @brief USART1驱动主实例，仅在本文件内有效
 */
static Usart1Driver g_usart1DriverInstance;

/**
 * @var MsgBusSystemMessage g_usart1DriverMessage
 * @brief USART1驱动消息对象，仅在本文件内有效
 */
MessageBusMessage g_usart1DriverMessage;

/* 公共函数实现
 * -------------------------------------------------------------*/
/**
 * @addtogroup Usart1Driver_APIs
 * @{
 */
/**
 * @brief 初始化USART1驱动器
 */
void initializeUsart1Driver(void)
{
    Usart1Driver* driver = &g_usart1DriverInstance;

    driver->transmitMessageBuffer
        = xMessageBufferCreateStatic(USART1_MAX_SEND_BUFFER_SIZE,
                                     driver->transmitMessageBufferMemory,
                                     &driver->transmitMessageBufferStorage);

    driver->receiveMessageBuffer = xMessageBufferCreateStatic(USART1_MAX_RECEIVE_BUFFER_SIZE,
                                                              driver->receiveMessageBufferMemory,
                                                              &driver->receiveMessageBufferStorage);

    driver->transmitCompleteSemaphore
        = xSemaphoreCreateBinaryStatic(&driver->transmitCompleteSemaphoreBuffer);

    driver->testData = 0;

    g_usart1DriverMessage.type = MESSAGE_BUS_TYPE_NONE;
    g_usart1DriverMessage.payload.usartData = pdFALSE;
}

/**
 * @brief 发送数据（从transmitMessageBuffer取出并发送）
 * @retval size_t 实际发送的字节数
 */
size_t sendUsart1Data(void)
{
    Usart1Driver* driver = &g_usart1DriverInstance;
    size_t sentByteCount = 0;

    sentByteCount = xMessageBufferReceive(driver->transmitMessageBuffer,
                                          driver->sendBuffer,
                                          USART1_MAX_SEND_BUFFER_SIZE,
                                          portMAX_DELAY);

    if (sentByteCount > 0) {
        xSemaphoreTake(driver->transmitCompleteSemaphore, 0);

        UartTransferData uartData;
        uartData.data = driver->sendBuffer;
        uartData.length = sentByteCount;
        uartData.channel = UART_CHANNEL_1;
        HAL_uartSend(&uartData);

        if (xSemaphoreTake(driver->transmitCompleteSemaphore, pdMS_TO_TICKS(1000)) != pdTRUE) {
            // 超时处理
        }
    }

    memset(driver->sendBuffer, 0, sizeof(driver->sendBuffer));
    return sentByteCount;
}

/**
 * @brief 添加数据到发送缓冲区
 * @param [in] data 数据指针
 * @param [in] dataSize 数据长度
 * @retval size_t 实际写入的字节数
 */
size_t addUsart1TransmitData(const uint8_t* data, size_t dataSize)
{
    Usart1Driver* driver = &g_usart1DriverInstance;
    if (data == NULL || dataSize == 0)
        return 0;

    size_t writtenByteCount
        = xMessageBufferSend(driver->transmitMessageBuffer, data, dataSize, portMAX_DELAY);

    if (writtenByteCount == dataSize) {
        g_usart1DriverMessage.type = MESSAGE_BUS_TYPE_UART1_TRANSMIT;
        g_usart1DriverMessage.payload.usartData = 0x02;
        publishMessage(&g_usart1DriverMessage);
    }

    return writtenByteCount;
}

/**
 * @brief 接收数据（从receiveMessageBuffer取出）
 * @return UartData UART数据结构体
 */
UartData receiveUsart1Data(void)
{
    Usart1Driver* driver = &g_usart1DriverInstance;
    size_t receivedByteCount = 0;

    receivedByteCount = xMessageBufferReceive(driver->receiveMessageBuffer,
                                              driver->processBuffer,
                                              USART1_MAX_RECEIVE_BUFFER_SIZE,
                                              portMAX_DELAY);

    UartData uartData;
    if (receivedByteCount > 0) {
        uartData.data = driver->processBuffer;
        uartData.size = receivedByteCount;
    } else {
        uartData.data = NULL;
        uartData.size = 0;
    }
    return uartData;
}

/**
 * @brief DMA接收完成，转存数据并通知
 * @param [in] receivedByteCount 实际接收字节数
 */
void notifyUsart1DmaReceiveComplete(size_t receivedByteCount)
{
    Usart1Driver* driver = &g_usart1DriverInstance;
    BaseType_t higherPriorityTaskWoken = pdFALSE;

    if (receivedByteCount > 0) {
        xMessageBufferSendFromISR(driver->receiveMessageBuffer,
                                  driver->dmaBuffer,
                                  receivedByteCount,
                                  &higherPriorityTaskWoken);

        memset(driver->dmaBuffer, 0, sizeof(driver->dmaBuffer));

        g_usart1DriverMessage.type = MESSAGE_BUS_TYPE_UART1_RECEIVE;
        g_usart1DriverMessage.payload.usartData = 0x03;
        publishMessageFromInterrupt(&g_usart1DriverMessage, &higherPriorityTaskWoken);

        portYIELD_FROM_ISR(higherPriorityTaskWoken);
    }
}

/**
 * @brief 获取DMA缓冲区指针
 * @return uint8_t* DMA缓冲区指针
 */
uint8_t* getUsart1DmaBufferPointer(void) { return g_usart1DriverInstance.dmaBuffer; }

/**
 * @brief 获取DMA缓冲区大小
 * @return uint32_t DMA缓冲区大小
 */
uint32_t getUsart1DmaBufferSize(void) { return USART1_MAX_RECEIVE_BUFFER_SIZE; }

/**
 * @brief 处理接收数据（供BSP调用）
 * @param [in] receivedByteCount 实际接收字节数
 */
void processUsart1ReceivedData(size_t receivedByteCount)
{
    notifyUsart1DmaReceiveComplete(receivedByteCount);
}

/**
 * @brief 发送完成通知（供中断调用）
 */
void notifyUsart1TransmitComplete(void)
{
    Usart1Driver* driver = &g_usart1DriverInstance;
    BaseType_t higherPriorityTaskWoken = pdFALSE;

    if (driver->transmitCompleteSemaphore != NULL) {
        xSemaphoreGiveFromISR(driver->transmitCompleteSemaphore, &higherPriorityTaskWoken);
        portYIELD_FROM_ISR(higherPriorityTaskWoken);
    }
}

/**
 * @brief 保存测试数据
 * @param [in] testData 测试数据
 */
void saveUsart1TestData(uint32_t testData)
{
    Usart1Driver* driver = &g_usart1DriverInstance;
    driver->testData = testData;
}

/**
 * @brief 获取测试数据
 * @return uint32_t 测试数据
 */
uint32_t getUsart1TestData(void)
{
    Usart1Driver* driver = &g_usart1DriverInstance;
    return driver->testData;
}

/**
 * @brief 发送测试数据消息
 * @param [in] testData 测试数据
 */
void sendUsart1TestDataMessage(uint32_t testData)
{
    Usart1Driver* driver = &g_usart1DriverInstance;
    driver->testData = testData;
    g_usart1DriverMessage.type = MESSAGE_BUS_TYPE_TEST_MESSAGE;
    g_usart1DriverMessage.payload.testValue = testData;
    publishMessage(&g_usart1DriverMessage);
}
/**
 * @}
 */
