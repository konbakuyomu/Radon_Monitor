/**
 * @file uartProtocolProcessor.cpp
 * @brief 串口协议处理器实现
 * @date 2024-07-07
 * @details 实现串口协议处理器各功能，包括消息解析、校验和命令处理
 */
#include "globalConfig.h"

/* 函数实现
 * -------------------------------------------------------------*/

/**
 * @brief 获取命令处理器单例
 * @return UartProtocolProcessor& 命令处理器单例引用
 */
UartProtocolProcessor& UartProtocolProcessor::getInstance()
{
    static UartProtocolProcessor instance;
    return instance;
}

/**
 * @brief 构造函数(私有)
 * @note 注册各种命令的处理函数
 */
UartProtocolProcessor::UartProtocolProcessor()
{
    registerHandler(UartCommand::ALARM_REPORT, [this](const UartMessage& msg) {
        // 注册命令处理函数
        return handleAlarmReport(msg);
    });
    registerHandler(UartCommand::STATUS_REPORT, [this](const UartMessage& msg) {
        // 状态信息上报
        return handleStatusReport(msg);
    });
    registerHandler(UartCommand::QUERY_DATA, [this](const UartMessage& msg) {
        // 查询监测数据
        return handleQueryData(msg);
    });
    registerHandler(UartCommand::TIME_SYNC, [this](const UartMessage& msg) {
        // 授时命令
        return handleTimeSync(msg);
    });
    registerHandler(UartCommand::SET_PARAMETERS, [this](const UartMessage& msg) {
        // 参数设置
        return handleSetParameters(msg);
    });
    registerHandler(UartCommand::QUERY_MAC, [this](const UartMessage& msg) {
        // 查询MAC地址
        return handleQueryMac(msg);
    });
    registerHandler(UartCommand::QUERY_VERSION, [this](const UartMessage& msg) {
        // 查询固件版本
        return handleQueryVersion(msg);
    });
    registerHandler(UartCommand::CONTROL_DEVICE, [this](const UartMessage& msg) {
        // 控制设备
        return handleControlDevice(msg);
    });
}

/**
 * @brief 解析原始串口数据
 * @param [in] rawData 原始数据字节数组
 * @param [in] length 数据长度
 * @return bool 解析并处理是否成功
 *         - true: 解析并处理成功
 *         - false: 解析或处理失败
 */
bool UartProtocolProcessor::parseAndExecute(const uint8_t* rawData, size_t length)
{
    UartMessage msg;
    if (parseRawData(rawData, length, msg)) {
        // 由于协议中没有校验和，直接执行命令
        return executeCommand(msg);
    }
    return false;
}

/**
 * @brief 解析原始数据为消息结构
 * @param [in] rawData 原始数据
 * @param [in] length 数据长度
 * @param [out] message 解析后的消息
 * @return bool 解析是否成功
 *         - true: 解析成功
 *         - false: 解析失败，数据格式不符合协议要求
 */
bool UartProtocolProcessor::parseRawData(const uint8_t* rawData,
                                         size_t length,
                                         UartMessage& message)
{
    // 氡监测仪以太网通信协议帧格式: [命令码(1字节)][数据长度(1字节)][数据(n字节)]
    if (length < 2) { // 最小长度检查
        return false;
    }

    message.command = static_cast<UartCommand>(rawData[0]);
    message.dataLength = rawData[1];

    if (length != 2 + message.dataLength) { // 检查数据长度是否匹配
        return false;
    }

    // 复制数据部分
    if (message.dataLength > 0) {
        message.data.assign(rawData + 2, rawData + 2 + message.dataLength);
    } else {
        message.data.clear();
    }

    return true;
}

/**
 * @brief 验证校验和
 * @param [in] msg 消息结构
 * @return bool 校验是否通过
 *         - true: 校验通过
 *         - false: 校验失败
 * @note 当前协议没有校验和字段，保留接口兼容性
 */
bool UartProtocolProcessor::validateChecksum(const UartMessage& msg)
{
    // 根据氡监测仪以太网通信协议，没有校验和字段
    // 此函数保留是为了兼容现有架构，始终返回true
    return true;
}

/**
 * @brief 处理报警信息上报命令
 * @param [in] msg 接收到的消息
 * @return bool 处理是否成功
 *         - true: 处理成功
 *         - false: 处理失败，数据格式错误
 */
bool UartProtocolProcessor::handleAlarmReport(const UartMessage& msg)
{
    // 示例实现
    if (msg.dataLength != 2) {
        return false;
    }

    // // 获取USART1实例
    // USART1Driver& usart1Driver = USART1Driver::getInstance();

    // uint8_t radonAlarmStatus = msg.data[0];
    // uint8_t flowAlarmStatus = msg.data[1];

    // // 处理报警状态...
    
    // // 创建扩展的数据，复制原始数据并添加2个字节
    // std::vector<uint8_t> extendedData = msg.data;
    // extendedData.push_back(0xAA);  // 添加第一个额外字节
    // extendedData.push_back(0xBB);  // 添加第二个额外字节

    // // 获取测试数据
    // uint32_t testData = usart1Driver.getTestData();
    // extendedData.push_back(testData & 0xFF);
    // extendedData.push_back((testData >> 8) & 0xFF);
    // extendedData.push_back((testData >> 16) & 0xFF);
    // extendedData.push_back((testData >> 24) & 0xFF);

    // // 使用扩展后的数据创建UartData并发送
    // UartData alarmData(extendedData.data(), extendedData.size());
    // usart1Driver.sendTxDataMessage(alarmData);

    return true;
}

/**
 * @brief 处理状态信息上报命令
 * @param [in] msg 接收到的消息
 * @return bool 处理是否成功
 *         - true: 处理成功
 *         - false: 处理失败，数据格式错误
 */
bool UartProtocolProcessor::handleStatusReport(const UartMessage& msg)
{
    // 示例实现
    if (msg.dataLength != 2) {
        return false;
    }

    uint8_t workStatus = msg.data[0];
    uint8_t faultStatus = msg.data[1];

    // 处理工作状态和故障状态...

    return true;
}

/**
 * @brief 处理查询监测数据命令并生成回复
 * @param [in] msg 接收到的消息
 * @return bool 处理是否成功
 *         - true: 处理成功
 *         - false: 处理失败
 */
bool UartProtocolProcessor::handleQueryData(const UartMessage& msg)
{
    // 示例实现
    // 生成响应数据...
    std::vector<uint8_t> responseData(18); // 0x12 = 18字节

    responseData[0] = 0x01;                // 工作状态：运行
    responseData[1] = 0x00;                // 故障状态：无故障

    // 设置氡气浓度、氡子体浓度、钍子体浓度和滤纸剩余量
    // 需要将float值转换为字节数组

    // 发送响应...

    return true;
}

/**
 * @brief 处理授时命令并生成回复
 * @param [in] msg 接收到的消息
 * @return bool 处理是否成功
 *         - true: 处理成功
 *         - false: 处理失败，数据长度错误
 */
bool UartProtocolProcessor::handleTimeSync(const UartMessage& msg)
{
    // 时间同步处理逻辑
    if (msg.dataLength != 4 && msg.dataLength != 0) {
        return false;
    }

    // 如果是请求(数据长度为0)，生成回复
    if (msg.dataLength == 0) {
        // 获取当前时间戳并发送
        // ...
    } else {
        // 处理接收到的时间戳数据
        uint32_t timestamp = 0;
        timestamp |= (uint32_t)msg.data[0] << 24;
        timestamp |= (uint32_t)msg.data[1] << 16;
        timestamp |= (uint32_t)msg.data[2] << 8;
        timestamp |= (uint32_t)msg.data[3];

        // 设置系统时间
        // ...
    }

    return true;
}

/**
 * @brief 处理参数设置命令并生成回复
 * @param [in] msg 接收到的消息
 * @return bool 处理是否成功
 *         - true: 处理成功
 *         - false: 处理失败
 */
bool UartProtocolProcessor::handleSetParameters(const UartMessage& msg)
{
    // 参数设置处理逻辑
    // ...
    return true;
}

/**
 * @brief 处理查询MAC地址命令并生成回复
 * @param [in] msg 接收到的消息
 * @return bool 处理是否成功
 *         - true: 处理成功
 *         - false: 处理失败
 */
bool UartProtocolProcessor::handleQueryMac(const UartMessage& msg)
{
    // MAC地址查询处理逻辑
    // ...
    return true;
}

/**
 * @brief 处理查询固件版本命令并生成回复
 * @param [in] msg 接收到的消息
 * @return bool 处理是否成功
 *         - true: 处理成功
 *         - false: 处理失败
 */
bool UartProtocolProcessor::handleQueryVersion(const UartMessage& msg)
{
    // 固件版本查询处理逻辑
    // ...
    return true;
}

/**
 * @brief 处理控制设备命令并生成回复
 * @param [in] msg 接收到的消息
 * @return bool 处理是否成功
 *         - true: 处理成功
 *         - false: 处理失败
 */
bool UartProtocolProcessor::handleControlDevice(const UartMessage& msg)
{
    // 设备控制处理逻辑
    // ...
    return true;
}
