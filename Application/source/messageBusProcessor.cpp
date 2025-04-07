/**
 * @file messageBusProcessor.cpp
 * @brief 消息总线命令处理器实现
 */

/* 头文件
 * -------------------------------------------------------------*/
#include "globalConfig.h"

/* 函数实现
 * -------------------------------------------------------------*/

/**
 * @brief 获取消息总线命令处理器单例
 * @return MessageBusProcessor& 消息总线命令处理器单例引用
 */
MessageBusProcessor& MessageBusProcessor::getInstance()
{
    static MessageBusProcessor instance;
    return instance;
}