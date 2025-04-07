/**
 * @file commandProcessorBase.hpp
 * @brief 命令处理器基类头文件
 * @details 定义通用命令处理器基类，可被不同协议解析器继承
 */

#pragma once

/* 头文件
 * -------------------------------------------------------------*/
#ifdef __cplusplus
#include <any>
#include <functional>
#include <typeindex>
#include <unordered_map>


/* 类定义
 * -------------------------------------------------------------*/

/**
 * @class CommandProcessorBase
 * @brief 命令处理器基类，用于注册和执行命令处理函数
 * @details 提供统一的命令处理机制，可以被不同协议解析器继承
 */
template <typename CommandType, typename MessageType> class CommandProcessorBase
{
public:
    /**
     * @brief 命令处理函数类型
     * @param msg 消息结构体
     */
    using CommandHandler = std::function<void(const MessageType&)>;

    /**
     * @brief 注册命令处理函数
     * @param [in] cmdType 命令类型
     * @param [in] handler 处理函数
     */
    void registerHandler(CommandType cmdType, CommandHandler handler)
    {
        handlers[cmdType] = std::move(handler);
    }

    /**
     * @brief 执行命令处理
     * @param [in] msg 接收到的消息
     * @return bool 是否成功处理命令
     */
    virtual bool executeCommand(const MessageType& msg)
    {
        auto it = handlers.find(getCommandFromMessage(msg));
        if (it != handlers.end()) {
            it->second(msg);
            return true;
        }
        return false;
    }

protected:
    /**
     * @brief 从消息中提取命令类型
     * @param [in] msg 接收到的消息
     * @return CommandType 对应的命令类型
     */
    virtual CommandType getCommandFromMessage(const MessageType& msg) = 0;

    /**
     * @brief 构造函数(保护)
     */
    CommandProcessorBase() = default;

    /**
     * @brief 析构函数(虚函数)
     */
    virtual ~CommandProcessorBase() = default;

    /**
     * @var handlers
     * @brief 命令处理函数映射表
     */
    std::unordered_map<CommandType, CommandHandler> handlers;
};
#endif
