/**
 * @file uartProcessor.c
 * @brief 串口协议命令处理器实现文件
 * @date 2025-04-11
 * @details 实现串口协议命令处理器的相关功能，包括从消息解析命令类型，初始化处理器和获取单例等
 */

/* 头文件
 * -------------------------------------------------------------*/
#include "uartProcessor.h"
#include "globalConfig.h"
#include <string.h>

/**
 * @addtogroup UART_Processor
 * @{
 */

/* 宏定义
 * -------------------------------------------------------------*/
/**
 * @brief 串口命令处理器支持的最大命令数量
 */
#define UART_MAX_COMMAND_COUNT 32

/* 静态变量
 * -------------------------------------------------------------*/
/**
 * @brief Command Processor相关数组
 * @details 用于存储命令类型、处理函数和上下文
 */
static uint32_t uartCommandTypes[UART_MAX_COMMAND_COUNT];           /**< 支持串口命令 */
static CommandHandler uartHandlerFunctions[UART_MAX_COMMAND_COUNT]; /**< 对应的处理函数 */
static void* uartContextPointers[UART_MAX_COMMAND_COUNT];           /**< 对应的上下文 */

/**
 * @brief 串口协议的Command Processor单例对象
 */
static UartProtocolProcessor uartProcessorInstance;

/* 静态函数声明
 * -------------------------------------------------------------*/
/**
 * @brief 从串口消息构建命令类型
 * @param [in] processor 处理器指针，此处未使用
 * @param [in] message 串口消息指针
 * @return 命令类型
 * @details 从串口消息中提取命令标识，构建命令类型
 */
static uint32_t UartCommand_getTypeFromMessage(void* processor, const void* message)
{
    // 解析后的UartFrame直接作为输入
    const UartFrame* frame = (const UartFrame*)message;
    return (uint32_t)frame->functionCode;
}

/* 全局函数实现
 * -------------------------------------------------------------*/
/**
 * @brief 初始化串口协议命令处理器
 * @param [in,out] processor 指向处理器结构体的指针
 * @param [in] configuration 指向配置结构体的指针
 * @return 无返回值
 * @details
 *   - 调用基类初始化
 *   - 设置getCommandTypeFromMessage为串口协议专用实现
 */
void UartProtocolProcessor_initialize(UartProtocolProcessor* processor,
                                      const CommandProcessorConfiguration* configuration)
{
    CommandProcessor_initialize(&processor->base, configuration);
    processor->base.getCommandTypeFromMessage = UartCommand_getTypeFromMessage;
}

/**
 * @brief 获取串口协议命令处理器单例
 * @return 指向单例的指针
 * @details
 *   - 单例模式，避免重复分配资源
 *   - 首次调用时自动初始化
 */
UartProtocolProcessor* UartProtocolProcessor_getInstance(void)
{
    static bool isInitialized = false;
    if (!isInitialized) {
        CommandProcessorConfiguration configuration = { .capacity = UART_MAX_COMMAND_COUNT,
                                                        .commandTypes = uartCommandTypes,
                                                        .handlerFunctions = uartHandlerFunctions,
                                                        .contextPointers = uartContextPointers };
        UartProtocolProcessor_initialize(&uartProcessorInstance, &configuration);
        isInitialized = true;
    }
    return &uartProcessorInstance;
}

// CRC16-IBM 计算
static uint16_t calcCRC16Impl(const uint8_t* data, size_t len)
{
    uint16_t crc = 0x0000;
    for (size_t i = 0; i < len; ++i) {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; ++j) {
            if (crc & 0x0001)
                crc = (crc >> 1) ^ 0xA001;
            else
                crc >>= 1;
        }
    }
    return crc;
}

uint16_t calcCRC16(const uint8_t* data, size_t len) { return calcCRC16Impl(data, len); }

bool parseUartFrame(const UartData* raw, UartFrame* frame)
{
    if (!raw || !raw->data || raw->size < 7)
        return false;
    const uint8_t* buf = raw->data;
    size_t sz = raw->size;
    if (buf[0] != 0x68 || buf[sz - 1] != 0x16)
        return false;
    uint8_t da = buf[1];
    uint8_t lenField = buf[2];
    if ((size_t)lenField + 4 != sz)
        return false;
    uint16_t crcIn = (uint16_t)buf[sz - 3] | ((uint16_t)buf[sz - 2] << 8);
    uint16_t crcCalc = calcCRC16(&buf[1], sz - 4);
    if (crcIn != crcCalc)
        return false;
    frame->address = da;
    frame->functionCode = buf[3];
    frame->data = &buf[4];
    frame->dataLength = (size_t)lenField - 1;
    return true;
}

size_t buildUartFrame(const UartFrame* frame, uint8_t* buffer)
{
    if (!frame || !buffer)
        return 0;
    buffer[0] = 0x68;
    buffer[1] = frame->address;
    buffer[2] = (uint8_t)(frame->dataLength + 1);
    buffer[3] = frame->functionCode;
    memcpy(&buffer[4], frame->data, frame->dataLength);
    size_t pos = 4 + frame->dataLength;
    uint16_t crc = calcCRC16(&buffer[1], frame->dataLength + 3);
    buffer[pos] = (uint8_t)(crc & 0xFF);
    buffer[pos + 1] = (uint8_t)((crc >> 8) & 0xFF);
    buffer[pos + 2] = 0x16;
    return pos + 3;
}

/**
 * @}
 */
