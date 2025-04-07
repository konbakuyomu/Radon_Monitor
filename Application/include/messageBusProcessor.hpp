/**
 * @file messageBusProcessor.hpp
 * @brief 消息总线命令处理器头文件
 * @details 继承基类实现消息总线相关命令处理
 */

#pragma once

/* 头文件
 * -------------------------------------------------------------*/
#ifdef __cplusplus
#include "commandProcessorBase.hpp"
#include "messageBus.hpp"

/* 函数声明
 * -------------------------------------------------------------*/

/**
 * @class MessageBusProcessor
 * @brief 消息总线命令处理器，处理消息总线相关命令
 */
class MessageBusProcessor : public CommandProcessorBase<Message, SystemMessage>
{
public:
    /**
     * @brief 获取命令处理器单例
     * @return MessageBusProcessor& 命令处理器单例引用
     */
    static MessageBusProcessor& getInstance();

protected:
    /**
     * @brief 从消息中提取命令类型
     * @param [in] msg 接收到的系统消息
     * @return Message 对应的命令类型
     */
    Message getCommandFromMessage(const SystemMessage& msg) override
    {
        return msg.message;
    }

private:
    /**
     * @brief 构造函数(私有)
     */
    MessageBusProcessor() = default;

    /**
     * @brief 析构函数(私有)
     */
    ~MessageBusProcessor() = default;
};
#endif
