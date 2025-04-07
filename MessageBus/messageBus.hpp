/**
 * @file messageBus.hpp
 * @brief 消息总线系统头文件
 * @date 2025-03-16
 * @details 实现基于FreeRTOS的线程安全消息传递系统，采用单例模式，
 *          支持发布-订阅模式，用于系统各组件间的解耦通信。
 */

#ifdef __cplusplus
#pragma once

/* 头文件
 * -------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "message_buffer.h"
#include "queue.h"
#include "semphr.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <unordered_set>

/* 枚举
 * -------------------------------------------------------------*/

/**
 * @enum Message
 * @brief 消息类型枚举
 * @details 系统中所有支持的消息类型
 */
enum class Message {
    NONE = 0,           /**< 无类型（无效消息） */
    UART1_TRANSMIT = 1, /**< 串口1发送消息 */
    UART1_RECEIVE = 2,  /**< 串口1接收消息 */
    EEPROM_WRITE = 3,   /**< EEPROM写消息 */
    LED_CONTROL = 4,    /**< LED控制消息 */
    TEST_MESSAGE = 5,   /**< 测试消息 */
    MAX_MESSAGE = 6     /**< 枚举边界标记 */
};

/**
 * @enum LedControl
 * @brief LED控制枚举
 * @details 定义LED控制方式
 */
enum class LedControl {
    TURN_ON = 0,  /**< 打开LED */
    TURN_OFF = 1, /**< 关闭LED */
    TOGGLE = 2,   /**< 切换LED状态 */
    NONE = 3      /**< 无效控制 */
};

/* 常量定义
 * -------------------------------------------------------------*/
constexpr size_t MAX_USART_DATA_SIZE = 50; /**< 串口数据最大大小 */

/* 结构体
 * -------------------------------------------------------------*/

/**
 * @struct Message
 * @brief 消息结构体
 * @details 定义系统中传递的消息格式，包含消息类型和负载
 */
struct SystemMessage {
    Message message; /**< 消息类型 */
    union {
        /**
         * @struct usartData
         * @brief 测试字节(实际上未使用)
         */
        uint8_t usartData;

        /**
         * @struct eepromData
         * @brief EEPROM数据结构
         */
        struct {
            uint32_t address; /**< EEPROM地址 */
            uint8_t data[16]; /**< 数据数组 */
            size_t len;       /**< 有效数据长度 */
        } eepromData;

        /**
         * @struct ledData
         * @brief LED控制数据结构
         */
        struct {
            uint8_t ledSelection; /**< 选择需要控制的LED */
            LedControl control;   /**< 控制方式 */
        } ledData;

        /**
         * @struct testData
         * @brief 测试数据结构
         */
        std::uint32_t testData;
    } payload;                    /**< 消息负载，根据消息类型解释 */
};

/* 类定义
 * -------------------------------------------------------------*/

/**
 * @class MessageBus
 * @brief 消息总线类
 * @details 实现单例模式的消息总线，提供发布-订阅接口，
 *          用于系统组件间的解耦通信
 */
class MessageBus
{
public:
    // 禁用拷贝构造和赋值操作
    MessageBus(const MessageBus&) = delete;
    MessageBus& operator=(const MessageBus&) = delete;

    /**
     * @brief 获取单例实例
     * @return MessageBus& 消息总线单例引用
     */
    static MessageBus& getInstance() noexcept;

    /**
     * @brief 订阅指定类型的消息
     * @param [in] type 消息类型
     * @param [in] queue FreeRTOS队列句柄，用于接收消息
     * @return bool 订阅成功返回true，失败返回false
     */
    bool subscribe(Message type, QueueHandle_t queue) noexcept;

    /**
     * @brief 取消订阅指定类型的消息
     * @param [in] type 消息类型
     * @param [in] queue 之前订阅时使用的队列句柄
     */
    void unsubscribe(Message type, QueueHandle_t queue) noexcept;

    /**
     * @brief 发布消息到总线
     * @param [in] msg 要发布的消息
     */
    void publish(const SystemMessage& msg) noexcept;

    /**
     * @brief 从ISR中发布消息到总线
     * @param [in] msg 要发布的消息
     * @param [in] pxHigherPriorityTaskWoken 是否需要任务切换
     */
    void publishFromISR(const SystemMessage& msg,
                        BaseType_t* pxHigherPriorityTaskWoken) noexcept;

    /**
     * @brief 等待消息
     * @param [in] queue 队列句柄
     * @param [out] msg 接收到的消息
     * @return bool 是否成功接收消息
     */
    bool waitForMessage(xQueueHandle queue, SystemMessage& msg) noexcept;

private:
    // 单例实例
    static std::unique_ptr<MessageBus> mInstance;

    // 最大订阅者数和消息类型数
    static constexpr int MAX_SUBSCRIBERS = 8; /**< 每种消息类型的最大订阅者数量 */
    static constexpr int MAX_TYPES
        = static_cast<int>(Message::MAX_MESSAGE); /**< 消息类型总数 */

    /**
     * @brief 私有构造函数
     * @details 创建互斥量用于线程安全操作
     */
    MessageBus() noexcept;

    /**
     * @brief 析构函数
     * @details 释放互斥量资源
     */
    ~MessageBus();

    // 订阅列表 - 使用现代容器替代原始数组(使用unordered_set)
    std::array<std::unordered_set<QueueHandle_t>, MAX_TYPES> subscribers;
};
#endif
