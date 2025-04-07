/**
 * @file uartProtocolProcessor.hpp
 * @brief 串口协议解析处理器头文件
 * @date 2024-07-07
 * @details
 * 处理特定串口协议的命令解析，包括各类消息的编码和解码，支持不同类型的串口命令处理。
 */

#pragma once

/* 头文件
 * -------------------------------------------------------------*/
#ifdef __cplusplus
#include "commandProcessorBase.hpp"
#include <cstdint>
#include <vector>

/* 枚举定义
 * -------------------------------------------------------------*/

/**
 * @defgroup UartProtocol_Commands 串口协议命令集
 * @{
 */
/**
 * @enum UartCommand
 * @brief 串口命令类型枚举
 */
enum class UartCommand {
    NONE = 0,
    ALARM_REPORT = 0x01,   // 氡监测仪主动上报报警信息: --> 发送
    STATUS_REPORT = 0x02,  // 氡监测仪主动上报状态信息: --> 发送
    QUERY_DATA = 0x03,     // 氡监测仪主动上报监测数据: --> 发送
    TIME_SYNC = 0x0A,      // 上位机下发授时命令: <-- 接收
    SET_PARAMETERS = 0x0B, // 上位机下发参数设置: <-- 接收
    QUERY_MAC = 0x0D,      // 上位机下发查询MAC地址: <-- 接收
    QUERY_VERSION = 0x0E,  // 上位机下发查询固件版本: <-- 接收
    CONTROL_DEVICE = 0x0F  // 上位机下发控制设备运行/停止: <-- 接收
};
/**
 * @}
 */

/* 结构体定义
 * -------------------------------------------------------------*/

/**
 * @defgroup UartProtocol_DataStructures 串口协议数据结构
 * @{
 */
/**
 * @struct UartMessage
 * @brief 串口消息结构体
 */
struct UartMessage {
    UartCommand command;       /**< 命令类型 */
    std::vector<uint8_t> data; /**< 数据部分 */
    uint8_t dataLength;        /**< 数据长度 */
};
/**
 * @}
 */

/* 类定义
 * -------------------------------------------------------------*/

/**
 * @class UartProtocolProcessor
 * @brief 串口协议处理器
 * @details 负责解析串口协议数据，并将命令分发到对应的处理函数
 */
class UartProtocolProcessor : public CommandProcessorBase<UartCommand, UartMessage>
{
public:
    /**
     * @brief 获取命令处理器单例
     * @return UartProtocolProcessor& 命令处理器单例引用
     */
    static UartProtocolProcessor& getInstance() noexcept;

    /**
     * @brief 解析原始串口数据
     * @param [in] rawData 原始数据字节数组
     * @param [in] length 数据长度
     * @return bool 解析并处理是否成功
     *         - true: 解析并处理成功
     *         - false: 解析或处理失败
     */
    bool parseAndExecute(const uint8_t* rawData, size_t length);

protected:
    /**
     * @brief 从消息中提取命令类型
     * @param [in] msg 接收到的串口消息
     * @return UartCommand 对应的命令类型
     */
    UartCommand getCommandFromMessage(const UartMessage& msg) noexcept override
    {
        return msg.command;
    }

private:
    /**
     * @brief 构造函数(私有)
     * @note 单例模式，通过getInstance()访问实例
     */
    UartProtocolProcessor();

    /**
     * @brief 析构函数(私有)
     */
    ~UartProtocolProcessor() = default;

    /**
     * @brief 解析原始数据为消息结构
     * @param [in] rawData 原始数据
     * @param [in] length 数据长度
     * @param [out] message 解析后的消息
     * @return bool 解析是否成功
     *         - true: 解析成功
     *         - false: 解析失败
     */
    bool parseRawData(const uint8_t* rawData, size_t length, UartMessage& message);

    /**
     * @brief 验证校验和
     * @param [in] msg 消息结构
     * @return bool 校验是否通过
     *         - true: 校验通过
     *         - false: 校验失败
     */
    bool validateChecksum(const UartMessage& msg);

    /**
     * @brief 处理报警信息上报命令
     * @param [in] msg 接收到的消息
     * @return bool 处理是否成功
     *         - true: 处理成功
     *         - false: 处理失败
     */
    bool handleAlarmReport(const UartMessage& msg);

    /**
     * @brief 处理状态信息上报命令
     * @param [in] msg 接收到的消息
     * @return bool 处理是否成功
     *         - true: 处理成功
     *         - false: 处理失败
     */
    bool handleStatusReport(const UartMessage& msg);

    /**
     * @brief 处理查询监测数据命令并生成回复
     * @param [in] msg 接收到的消息
     * @return bool 处理是否成功
     *         - true: 处理成功
     *         - false: 处理失败
     */
    bool handleQueryData(const UartMessage& msg);

    /**
     * @brief 处理授时命令并生成回复
     * @param [in] msg 接收到的消息
     * @return bool 处理是否成功
     *         - true: 处理成功
     *         - false: 处理失败
     */
    bool handleTimeSync(const UartMessage& msg);

    /**
     * @brief 处理参数设置命令并生成回复
     * @param [in] msg 接收到的消息
     * @return bool 处理是否成功
     *         - true: 处理成功
     *         - false: 处理失败
     */
    bool handleSetParameters(const UartMessage& msg);

    /**
     * @brief 处理查询MAC地址命令并生成回复
     * @param [in] msg 接收到的消息
     * @return bool 处理是否成功
     *         - true: 处理成功
     *         - false: 处理失败
     */
    bool handleQueryMac(const UartMessage& msg);

    /**
     * @brief 处理查询固件版本命令并生成回复
     * @param [in] msg 接收到的消息
     * @return bool 处理是否成功
     *         - true: 处理成功
     *         - false: 处理失败
     */
    bool handleQueryVersion(const UartMessage& msg);

    /**
     * @brief 处理控制设备命令并生成回复
     * @param [in] msg 接收到的消息
     * @return bool 处理是否成功
     *         - true: 处理成功
     *         - false: 处理失败
     */
    bool handleControlDevice(const UartMessage& msg);
};
#endif
