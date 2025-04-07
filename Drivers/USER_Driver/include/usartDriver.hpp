/**
 * @file usartDriver.hpp
 * @brief 串口驱动接口头文件 (使用MessageBuffer重构)
 * @date 2025-03-18
 * @details 本文件定义了串口通信接口的基类和USART1的具体实现类，
 *          使用FreeRTOS的MessageBuffer进行数据收发，支持双缓冲DMA接收机制。
 */

#ifdef __cplusplus
#pragma once

/* 头文件
 * -------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "messageBus.hpp"
#include "message_buffer.h"
#include "semphr.h"
#include <array>
#include <cstring>
#include <vector>

/* 常量定义
 * -------------------------------------------------------------*/

/**
 * @defgroup UART_Buffer_Sizes 串口缓冲区大小定义
 * @{
 */
constexpr uint32_t MAX_MESSAGE_BUFFER_SIZE = 256; /**< 消息缓冲区大小 */
constexpr uint32_t MAX_RECEIVE_BUFFER_SIZE = 50;  /**< 接收缓冲区大小 */
constexpr uint32_t MAX_SEND_BUFFER_SIZE = 50;     /**< 发送缓冲区大小 */
/**
 * @}
 */

/* 类定义
 * -------------------------------------------------------------*/

/**
 * @class UartData
 * @brief 串口数据封装类，管理数据和长度
 * @details 提供对串口接收数据的安全访问，支持数据有效性检查和多种访问方法
 */
class UartData
{
public:
    /**
     * @brief 默认构造函数
     * @details 创建一个空的数据对象
     */
    UartData() = default;

    /**
     * @brief 带参数构造函数
     * @param [in] ptr 数据指针
     * @param [in] len 数据长度
     * @details 创建一个对象并复制指定数据
     */
    UartData(const uint8_t* ptr, size_t len)
    {
        if (ptr != nullptr && len > 0) {
            mData.resize(len);
            std::memcpy(mData.data(), ptr, len);
        }
    }

    /**
     * @brief 获取数据指针
     * @return uint8_t* 指向数据的指针
     */
    uint8_t* data() { return mData.empty() ? nullptr : mData.data(); }

    /**
     * @brief 获取数据指针(const版本)
     * @return const uint8_t* 指向数据的const指针
     */
    const uint8_t* data() const { return mData.empty() ? nullptr : mData.data(); }

    /**
     * @brief 获取数据长度
     * @return size_t 数据的字节数
     */
    size_t size() const { return mData.size(); }

    /**
     * @brief 检查数据是否为空
     * @return bool 当数据为空时返回true，否则返回false
     */
    bool empty() const { return mData.empty(); }

    /**
     * @brief 布尔转换运算符
     * @return bool 当数据有效时返回true，否则返回false
     */
    operator bool() const { return !empty(); }

private:
    std::vector<uint8_t> mData; /**< 数据存储 */
};

/**
 * @class CommInterface
 * @brief 通信接口基类
 * @details 定义了所有通信接口共享的方法和属性，是一个抽象基类
 */
class CommInterface
{
public:
    /**
     * @brief 构造函数
     * @details 初始化消息缓冲区句柄为空
     */
    CommInterface();

    /**
     * @brief 虚析构函数
     * @details 基类虚析构函数，资源清理在子类中实现
     */
    virtual ~CommInterface();

    // 禁用拷贝
    CommInterface(const CommInterface&) = delete;
    CommInterface& operator=(const CommInterface&) = delete;

    /**
     * @brief 发送数据
     * @return size_t 成功发送的字节数
     * @details 从txMessageBuffer读取数据并通过串口发送
     */
    virtual size_t send() = 0;

    /**
     * @brief 接收数据
     * @return UartData 接收到的数据对象，包含数据指针和长度
     * @details 从rxMessageBuffer读取数据并返回封装后的UartData对象
     */
    virtual UartData receive() = 0;

protected:
    // 常量定义已移到类外部
};

/**
 * @class USART1Driver
 * @brief USART1驱动类，实现双缓冲接收
 * @details 实现基于FreeRTOS的USART1通信驱动，使用MessageBuffer进行数据收发，
 *          支持双缓冲DMA接收机制。采用单例模式设计。
 */
class USART1Driver : public CommInterface
{
public:
    /**
     * @brief 获取单例实例
     * @return USART1Driver& 单例实例的引用
     * @details 使用C++11静态局部变量的线程安全初始化特性
     */
    static USART1Driver& getInstance() noexcept;

    /**
     * @brief 发送数据
     * @return size_t 成功发送的字节数
     * @details 从消息缓冲区读取数据，通过USART1发送
     */
    size_t send() override;

    /**
     * @brief 添加数据到发送缓冲区
     * @param [in] data 要发送的数据对象
     * @return size_t 实际添加到缓冲区的数据长度
     * @details 将数据写入发送消息缓冲区，等待send()函数发送
     */
    size_t sendTxDataMessage(UartData data);

    /**
     * @brief 接收数据
     * @return UartData 接收到的数据对象
     * @details 从消息缓冲区读取数据，返回封装后的UartData对象
     */
    UartData receive() override;

    /**
     * @brief 缓冲区切换函数
     * @param [in] bytesReceived 接收到的字节数
     * @details 在DMA接收完成或超时时从中断上下文调用，
     *          将DMA缓冲区中的数据发送到消息缓冲区
     */
    void sendRxDataMessage(size_t bytesReceived) noexcept;

    /**
     * @brief 获取DMA缓冲区指针
     * @return uint8_t* DMA缓冲区指针
     * @details 用于BSP层获取DMA写入的缓冲区地址
     */
    uint8_t* getDmaBufferPtr() noexcept;

    /**
     * @brief 获取DMA缓冲区大小
     * @return constexpr uint32_t 缓冲区大小(字节)
     * @details 以静态方法形式提供缓冲区大小，供C接口使用
     */
    static constexpr uint32_t getDmaBufferSize() noexcept
    {
        return MAX_RECEIVE_BUFFER_SIZE;
    }

    /**
     * @brief 获取发送消息缓冲区指针
     * @return MessageBufferHandle_t 发送消息缓冲区句柄
     * @details 以静态方法形式提供发送消息缓冲区句柄，用于外部访问
     */
    static MessageBufferHandle_t getTxMessageBufferPtr() noexcept;

    /**
     * @brief 获取接收消息缓冲区指针
     * @return MessageBufferHandle_t 接收消息缓冲区句柄
     * @details 以静态方法形式提供接收消息缓冲区句柄，用于外部访问
     */
    static MessageBufferHandle_t getRxMessageBufferPtr() noexcept
    {
        return rxMessageBuffer;
    }

    /**
     * @brief 存储测试数据
     * @param [in] data 测试数据
     * @details 存储测试数据
     */
    void saveTestData(std::uint32_t data) noexcept;

    /**
     * @brief 获取测试数据
     * @return uint32_t 测试数据
     * @details 获取测试数据
     */
    std::uint32_t getTestData() noexcept;

    /**
     * @brief 发送测试数据
     * @param [in] data 测试数据
     * @details 发送测试数据
     */
    void sendTestDataMessage(std::uint32_t data) noexcept;

    /* 静态成员
     * -------------------------------------------------------------*/
public:
    /**
     * @var SemaphoreHandle_t txCompleteSemaphore
     * @brief 发送完成信号量，用于通知发送完成事件
     */
    static SemaphoreHandle_t txCompleteSemaphore;

    /**
     * @var StaticSemaphore_t txCompleteSemaphoreBuffer
     * @brief 发送完成信号量的静态存储区
     */
    static StaticSemaphore_t txCompleteSemaphoreBuffer;

    /**
     * @var MessageBufferHandle_t txMessageBuffer
     * @brief 发送消息缓冲区句柄
     */
    static MessageBufferHandle_t txMessageBuffer;

    /**
     * @var MessageBufferHandle_t rxMessageBuffer
     * @brief 接收消息缓冲区句柄
     */
    static MessageBufferHandle_t rxMessageBuffer;

    /**
     * @var StaticMessageBuffer_t txMessageBufferStorage
     * @brief 发送消息缓冲区的静态存储区
     */
    static StaticMessageBuffer_t txMessageBufferStorage;

    /**
     * @var StaticMessageBuffer_t rxMessageBufferStorage
     * @brief 接收消息缓冲区的静态存储区
     */
    static StaticMessageBuffer_t rxMessageBufferStorage;

    /**
     * @var uint8_t txMessageBufferMemory[]
     * @brief 发送消息缓冲区的存储空间
     */
    static uint8_t txMessageBufferMemory[MAX_SEND_BUFFER_SIZE];

    /**
     * @var uint8_t rxMessageBufferMemory[]
     * @brief 接收消息缓冲区的存储空间
     */
    static uint8_t rxMessageBufferMemory[MAX_RECEIVE_BUFFER_SIZE];

private:
    /**
     * @brief 私有构造函数
     * @details 创建消息缓冲区并初始化缓冲区
     */
    USART1Driver() noexcept;

    /**
     * @brief 私有析构函数
     * @details 释放消息缓冲区等资源
     */
    ~USART1Driver() noexcept override;

    // 禁止复制和移动
    USART1Driver(const USART1Driver&) = delete;
    USART1Driver& operator=(const USART1Driver&) = delete;

    /**
     * @var std::array<uint8_t, MAX_SEND_BUFFER_SIZE> sendBuffer
     * @brief 串口发送缓冲区
     */
    static std::array<uint8_t, MAX_SEND_BUFFER_SIZE> sendBuffer;

    /**
     * @var std::array<uint8_t, MAX_RECEIVE_BUFFER_SIZE> dmaBuffer
     * @brief 串口DMA缓冲区，用于接收DMA传输的数据
     */
    static std::array<uint8_t, MAX_RECEIVE_BUFFER_SIZE> dmaBuffer;

    /**
     * @var std::array<uint8_t, MAX_RECEIVE_BUFFER_SIZE> procBuffer
     * @brief 处理用的缓冲区，用于处理接收到的数据
     */
    static std::array<uint8_t, MAX_RECEIVE_BUFFER_SIZE> procBuffer;

    /**
     * @var SystemMessage msg
     * @brief 串口消息，用于通过消息总线传递串口事件
     */
    SystemMessage usart1DriverMessage;

    /**
     * @var std::uint32_t testData
     * @brief 测试数据，用于测试消息总线
     */
    std::uint32_t testData;
};
#endif

/* C接口声明
 * -------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 获取DMA缓冲区指针
 * @return uint8_t* DMA缓冲区指针
 * @details C接口函数，供BSP层配置DMA目标地址
 */
uint8_t* USART1_GetDmaBufferPtr(void);

/**
 * @brief 获取DMA缓冲区大小
 * @return uint32_t DMA缓冲区大小(字节数)
 * @details C接口函数，供BSP层配置DMA传输计数
 */
uint32_t USART1_GetDmaBufferSize(void);

/**
 * @brief 处理接收到的数据
 * @param [in] bytesReceived 接收到的字节数
 * @details C接口函数，供BSP层在DMA接收完成或超时中断中调用
 */
void USART1_ProcessReceivedData(size_t bytesReceived);

/**
 * @brief USART1 发送完成通知
 * @details 在 USART1 发送完成中断中调用此函数通知 C++ 驱动
 */
void USART1_NotifyTxComplete(void);

#ifdef __cplusplus
}
#endif
