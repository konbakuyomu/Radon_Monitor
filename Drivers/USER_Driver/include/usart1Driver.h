#ifndef USART1_DRIVER_H
#define USART1_DRIVER_H

#include "FreeRTOS.h"
#include "message_buffer.h"
#include "message_bus.h" // 使用已有的MsgBusSystemMessage定义
#include "semphr.h"
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define USART1_MAX_MESSAGE_BUFFER_SIZE 256U
#define USART1_MAX_RECEIVE_BUFFER_SIZE 50U
#define USART1_MAX_SEND_BUFFER_SIZE    50U

typedef struct {
    uint8_t* data;
    size_t size;
} UartData_t;

typedef struct {
    // 发送缓冲区
    uint8_t sendBuffer[USART1_MAX_SEND_BUFFER_SIZE];
    // DMA缓冲区
    uint8_t dmaBuffer[USART1_MAX_RECEIVE_BUFFER_SIZE];
    // 处理缓冲区
    uint8_t procBuffer[USART1_MAX_RECEIVE_BUFFER_SIZE];

    // 信号量
    SemaphoreHandle_t txCompleteSemaphore;
    StaticSemaphore_t txCompleteSemaphoreBuffer;

    // 消息缓冲区
    MessageBufferHandle_t txMessageBuffer;
    MessageBufferHandle_t rxMessageBuffer;
    StaticMessageBuffer_t txMessageBufferStorage;
    StaticMessageBuffer_t rxMessageBufferStorage;
    uint8_t txMessageBufferMemory[USART1_MAX_SEND_BUFFER_SIZE];
    uint8_t rxMessageBufferMemory[USART1_MAX_RECEIVE_BUFFER_SIZE];

    // 测试数据
    uint32_t testData;

    // 不再包含usart1DriverMessage，避免不完整类型错误
} USART1Driver_t;

// 初始化
void USART1Driver_Init(void);

// 发送数据（从txMessageBuffer取出并发送）
size_t USART1Driver_Send(void);

// 添加数据到发送缓冲区
size_t USART1Driver_SendTxDataMessage(const uint8_t* data, size_t size);

// 接收数据（从rxMessageBuffer取出）
UartData_t USART1Driver_Receive(void);

// DMA接收完成，转存数据并通知
void USART1Driver_SendRxDataMessage(size_t bytesReceived);

// 获取DMA缓冲区指针
uint8_t* USART1_GetDmaBufferPtr_use_c(void);

// 获取DMA缓冲区大小
uint32_t USART1_GetDmaBufferSize_use_c(void);

// 处理接收数据（供BSP调用）
void USART1_ProcessReceivedData_use_c(size_t bytesReceived);

// 发送完成通知（供中断调用）
void USART1_NotifyTxComplete_use_c(void);

// 测试数据
void USART1Driver_SaveTestData(uint32_t data);
uint32_t USART1Driver_GetTestData(void);
void USART1Driver_SendTestDataMessage(uint32_t data);

#ifdef __cplusplus
}
#endif

#endif // USART1_DRIVER_H