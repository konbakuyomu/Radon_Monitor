/**
 * @file usart1Driver.h
 * @brief USART1驱动器头文件
 * @date 2025-04-15
 * @details 声明USART1驱动相关的数据结构和API，支持消息缓冲、DMA、FreeRTOS集成。
 */

#pragma once

/* 头文件
 * -------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "message_buffer.h"
#include "messageBus.h"
#include "semphr.h"
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 宏定义
 * -------------------------------------------------------------*/
/**
 * @defgroup Usart1Driver_Macros USART1驱动相关宏
 * @brief USART1缓冲区大小等宏定义
 * @{
 */
#define USART1_MAX_MESSAGE_BUFFER_SIZE 256U
#define USART1_MAX_RECEIVE_BUFFER_SIZE 50U
#define USART1_MAX_SEND_BUFFER_SIZE    50U
/**
 * @}
 */

/* 结构体与类型定义
 * -------------------------------------------------------------*/
/**
 * @defgroup Usart1Driver_Types USART1驱动相关类型
 * @brief USART1驱动相关结构体和类型
 * @{
 */
/**
 * @struct UartData
 * @brief UART数据结构体
 * @var UartData::data 数据指针
 * @var UartData::size 数据长度
 */
typedef struct {
    uint8_t* data; /**< 数据指针 */
    size_t size;   /**< 数据长度 */
} UartData;

/**
 * @struct Usart1Driver
 * @brief USART1驱动主结构体，包含缓冲区、信号量、消息缓冲等
 */
typedef struct {
    uint8_t sendBuffer[USART1_MAX_SEND_BUFFER_SIZE];
    uint8_t dmaBuffer[USART1_MAX_RECEIVE_BUFFER_SIZE];
    uint8_t processBuffer[USART1_MAX_RECEIVE_BUFFER_SIZE];
    SemaphoreHandle_t transmitCompleteSemaphore;
    StaticSemaphore_t transmitCompleteSemaphoreBuffer;
    MessageBufferHandle_t transmitMessageBuffer;
    MessageBufferHandle_t receiveMessageBuffer;
    StaticMessageBuffer_t transmitMessageBufferStorage;
    StaticMessageBuffer_t receiveMessageBufferStorage;
    uint8_t transmitMessageBufferMemory[USART1_MAX_SEND_BUFFER_SIZE];
    uint8_t receiveMessageBufferMemory[USART1_MAX_RECEIVE_BUFFER_SIZE];
    uint32_t testData;
} Usart1Driver;
/**
 * @}
 */

/* 函数声明
 * -------------------------------------------------------------*/
/**
 * @defgroup Usart1Driver_APIs USART1驱动API
 * @brief USART1驱动对外API
 * @{
 */
/**
 * @brief 初始化USART1驱动器
 */
void initializeUsart1Driver(void);

/**
 * @brief 发送数据（从transmitMessageBuffer取出并发送）
 * @retval size_t 实际发送的字节数
 */
size_t sendUsart1Data(void);

/**
 * @brief 添加数据到发送缓冲区
 * @param [in] data 数据指针
 * @param [in] dataSize 数据长度
 * @retval size_t 实际写入的字节数
 */
size_t addUsart1TransmitData(const uint8_t* data, size_t dataSize);

/**
 * @brief 接收数据（从receiveMessageBuffer取出）
 * @return UartData UART数据结构体
 */
UartData receiveUsart1Data(void);

/**
 * @brief DMA接收完成，转存数据并通知
 * @param [in] receivedByteCount 实际接收字节数
 */
void notifyUsart1DmaReceiveComplete(size_t receivedByteCount);

/**
 * @brief 获取DMA缓冲区指针
 * @return uint8_t* DMA缓冲区指针
 */
uint8_t* getUsart1DmaBufferPointer(void);

/**
 * @brief 获取DMA缓冲区大小
 * @return uint32_t DMA缓冲区大小
 */
uint32_t getUsart1DmaBufferSize(void);

/**
 * @brief 处理接收数据（供BSP调用）
 * @param [in] receivedByteCount 实际接收字节数
 */
void processUsart1ReceivedData(size_t receivedByteCount);

/**
 * @brief 发送完成通知（供中断调用）
 */
void notifyUsart1TransmitComplete(void);

/**
 * @brief 保存测试数据
 * @param [in] testData 测试数据
 */
void saveUsart1TestData(uint32_t testData);

/**
 * @brief 获取测试数据
 * @return uint32_t 测试数据
 */
uint32_t getUsart1TestData(void);

/**
 * @brief 发送测试数据消息
 * @param [in] testData 测试数据
 */
void sendUsart1TestDataMessage(uint32_t testData);
/**
 * @}
 */

#ifdef __cplusplus
}
#endif
