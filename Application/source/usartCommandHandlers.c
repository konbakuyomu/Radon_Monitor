/**
 * @file usartCommandHandlers.c
 * @brief 串口命令处理函数实现文件
 * @date 2025-04-15
 * @details 实现各种串口命令的处理函数，供UartProtocolProcessor调用
 */

/* 头文件
 * -------------------------------------------------------------*/
#include "usartCommandHandlers.h"
#include "globalConfig.h"
#include "uartProcessor.h"
#include "usart1Driver.h" // for addUsart1TransmitData
#include <string.h>


/**
 * @addtogroup UART_Processor
 * @{
 */

/* 宏定义
 * -------------------------------------------------------------*/
/**
 * @brief 串口命令ID定义
 */
#define UART_CMD_SYSTEM  0x01
#define UART_CMD_DATA    0x02
#define UART_CMD_CONTROL 0x03
#define UART_CMD_CONFIG  0x04

/**
 * @brief 系统命令子命令ID
 */
#define UART_SUBCMD_SYS_STATUS  0x01
#define UART_SUBCMD_SYS_VERSION 0x02
#define UART_SUBCMD_SYS_RESET   0x03

/**
 * @brief 数据命令子命令ID
 */
#define UART_SUBCMD_DATA_QUERY  0x01
#define UART_SUBCMD_DATA_UPLOAD 0x02
#define UART_SUBCMD_DATA_CLEAR  0x03

/**
 * @brief 控制命令子命令ID
 */
#define UART_SUBCMD_CTRL_START 0x01
#define UART_SUBCMD_CTRL_STOP  0x02
#define UART_SUBCMD_CTRL_PAUSE 0x03

/**
 * @brief 配置命令子命令ID
 */
#define UART_SUBCMD_CFG_READ    0x01
#define UART_SUBCMD_CFG_WRITE   0x02
#define UART_SUBCMD_CFG_DEFAULT 0x03

/* 命令辅助宏
 * -------------------------------------------------------------*/
/**
 * @brief 创建串口命令类型的宏
 */
#define CREATE_UART_COMMAND(cmd, subcmd) (((uint32_t)(cmd) << 8) | (subcmd))

/* 静态函数声明
 * -------------------------------------------------------------*/
/**
 * @brief 处理系统状态查询命令
 */
static bool handleSystemStatus(void* context, const void* message);

/**
 * @brief 处理系统版本查询命令
 */
static bool handleSystemVersion(void* context, const void* message);

/**
 * @brief 处理系统重置命令
 */
static bool handleSystemReset(void* context, const void* message);

/**
 * @brief 处理数据查询命令
 */
static bool handleDataQuery(void* context, const void* message);

/**
 * @brief 处理数据上传命令
 */
static bool handleDataUpload(void* context, const void* message);

/* 静态函数实现
 * -------------------------------------------------------------*/
/**
 * @brief 处理系统状态查询命令
 * @param [in] context 上下文指针（未使用）
 * @param [in] message 消息指针（串口数据）
 * @return 处理是否成功
 */
static bool handleSystemStatus(void* context, const void* message)
{
    (void)context;
    const UartData* uartData = (const UartData*)message;

    // 构建响应数据
    uint8_t responseData[5] = {
        UART_CMD_SYSTEM,        // 命令ID
        UART_SUBCMD_SYS_STATUS, // 子命令ID
        0x00,                   // 状态码：成功
        0x01,                   // 系统状态：正常运行
        0x00                    // 保留字节
    };

    // 发送响应
    addUsart1TransmitData(responseData, sizeof(responseData));

    return true;
}

/**
 * @brief 处理系统版本查询命令
 * @param [in] context 上下文指针（未使用）
 * @param [in] message 消息指针（串口数据）
 * @return 处理是否成功
 */
static bool handleSystemVersion(void* context, const void* message)
{
    (void)context;
    const UartData* uartData = (const UartData*)message;

    // 构建响应数据
    uint8_t responseData[8] = {
        UART_CMD_SYSTEM,         // 命令ID
        UART_SUBCMD_SYS_VERSION, // 子命令ID
        0x00,                    // 状态码：成功
        0x01,                    // 主版本号
        0x02,                    // 次版本号
        0x03,                    // 修订版本号
        0x04,                    // 构建版本号
        0x00                     // 保留字节
    };

    // 发送响应
    addUsart1TransmitData(responseData, sizeof(responseData));

    return true;
}

/**
 * @brief 处理系统重置命令
 * @param [in] context 上下文指针（未使用）
 * @param [in] message 消息指针（串口数据）
 * @return 处理是否成功
 */
static bool handleSystemReset(void* context, const void* message)
{
    (void)context;
    const UartData* uartData = (const UartData*)message;

    // 构建响应数据
    uint8_t responseData[4] = {
        UART_CMD_SYSTEM,       // 命令ID
        UART_SUBCMD_SYS_RESET, // 子命令ID
        0x00,                  // 状态码：成功
        0x00                   // 保留字节
    };

    // 发送响应
    addUsart1TransmitData(responseData, sizeof(responseData));

    // 在实际应用中，这里可能需要执行系统重置操作
    // systemReset();

    return true;
}

/**
 * @brief 处理数据查询命令
 * @param [in] context 上下文指针（未使用）
 * @param [in] message 消息指针（串口数据）
 * @return 处理是否成功
 */
static bool handleDataQuery(void* context, const void* message)
{
    (void)context;
    const UartData* uartData = (const UartData*)message;

    // 假设有一个数据ID在命令的第3个字节
    uint8_t dataId = (uartData->size >= 3) ? uartData->data[2] : 0;

    // 构建响应数据（示例：返回一些随机数据）
    uint8_t responseData[8] = {
        UART_CMD_DATA,          // 命令ID
        UART_SUBCMD_DATA_QUERY, // 子命令ID
        dataId,                 // 数据ID
        0x00,                   // 状态码：成功
        0xA1,                   // 数据字节1
        0xB2,                   // 数据字节2
        0xC3,                   // 数据字节3
        0xD4                    // 数据字节4
    };

    // 发送响应
    addUsart1TransmitData(responseData, sizeof(responseData));

    return true;
}

/**
 * @brief 处理数据上传命令
 * @param [in] context 上下文指针（未使用）
 * @param [in] message 消息指针（串口数据）
 * @return 处理是否成功
 */
static bool handleDataUpload(void* context, const void* message)
{
    (void)context;
    const UartData* uartData = (const UartData*)message;

    // 构建简单响应数据
    uint8_t responseData[4] = {
        UART_CMD_DATA,           // 命令ID
        UART_SUBCMD_DATA_UPLOAD, // 子命令ID
        0x00,                    // 状态码：成功
        0x00                     // 保留字节
    };

    // 发送响应
    addUsart1TransmitData(responseData, sizeof(responseData));

    // 在实际应用中，这里可以处理上传的数据
    // processUploadedData(&uartData->data[2], uartData->size - 2);

    return true;
}

/* 全局函数实现
 * -------------------------------------------------------------*/
/**
 * @brief 注册所有串口命令处理函数
 * @details 在系统初始化时调用，注册所有串口命令的处理函数
 */
void registerUartCommandHandlers(void)
{
    UartProtocolProcessor* processor = UartProtocolProcessor_getInstance();

    // 注册系统命令处理函数
    processor->base.registerHandler(&processor->base,
                                    CREATE_UART_COMMAND(UART_CMD_SYSTEM, UART_SUBCMD_SYS_STATUS),
                                    handleSystemStatus,
                                    NULL);

    processor->base.registerHandler(&processor->base,
                                    CREATE_UART_COMMAND(UART_CMD_SYSTEM, UART_SUBCMD_SYS_VERSION),
                                    handleSystemVersion,
                                    NULL);

    processor->base.registerHandler(&processor->base,
                                    CREATE_UART_COMMAND(UART_CMD_SYSTEM, UART_SUBCMD_SYS_RESET),
                                    handleSystemReset,
                                    NULL);

    // 注册数据命令处理函数
    processor->base.registerHandler(&processor->base,
                                    CREATE_UART_COMMAND(UART_CMD_DATA, UART_SUBCMD_DATA_QUERY),
                                    handleDataQuery,
                                    NULL);

    processor->base.registerHandler(&processor->base,
                                    CREATE_UART_COMMAND(UART_CMD_DATA, UART_SUBCMD_DATA_UPLOAD),
                                    handleDataUpload,
                                    NULL);

    // 可以根据需要继续注册更多命令处理函数...
}

// 发送错误响应
void sendUartErrorResponse(uint8_t func, uint8_t errCode)
{
    UartFrame resp = { .address = UART_DEVICE_ADDRESS,
                       .functionCode = (uint8_t)(func | 0x80),
                       .data = &errCode,
                       .dataLength = 1 };
    uint8_t buf[16];
    size_t len = buildUartFrame(&resp, buf);
    addUsart1TransmitData(buf, len);
}

// 实时数据查询处理
static bool handleRealTime(void* context, const void* message)
{
    (void)context;
    const UartFrame* frame = (const UartFrame*)message;
    uint8_t payload[6];
    // 示例：6字节实时数据
    payload[0] = 0x01; // 高字节
    payload[1] = 0x02;
    payload[2] = 0x03;
    payload[3] = 0x04;
    payload[4] = 0x05;
    payload[5] = 0x06;
    UartFrame resp = { UART_DEVICE_ADDRESS, UART_FUNC_REAL_TIME, payload, sizeof(payload) };
    uint8_t buf[32];
    size_t len = buildUartFrame(&resp, buf);
    addUsart1TransmitData(buf, len);
    return true;
}

// 核素数据查询处理
static bool handleNuclide(void* context, const void* message)
{
    (void)context;
    const UartFrame* frame = (const UartFrame*)message;
    uint8_t payload[4] = { 0x10, 0x20, 0x30, 0x40 };
    UartFrame resp = { UART_DEVICE_ADDRESS, UART_FUNC_NUCLIDE, payload, sizeof(payload) };
    uint8_t buf[16];
    size_t len = buildUartFrame(&resp, buf);
    addUsart1TransmitData(buf, len);
    return true;
}

// 设置参数处理
static bool handleSetParam(void* context, const void* message)
{
    (void)context;
    const UartFrame* frame = (const UartFrame*)message;
    // 假设第1字节是参数编号，第2~n是值
    uint8_t paramId = frame->dataLength > 0 ? frame->data[0] : 0;
    // ... 保存参数逻辑 ...
    uint8_t result = 0x00;
    UartFrame resp = { UART_DEVICE_ADDRESS, UART_FUNC_SET_PARAM, &result, 1 };
    uint8_t buf[16];
    size_t len = buildUartFrame(&resp, buf);
    addUsart1TransmitData(buf, len);
    return true;
}

// 查询参数处理
static bool handleGetParam(void* context, const void* message)
{
    (void)context;
    const UartFrame* frame = (const UartFrame*)message;
    uint8_t paramId = frame->dataLength > 0 ? frame->data[0] : 0;
    uint8_t value[2] = { 0xAA, 0xBB };
    UartFrame resp = { UART_DEVICE_ADDRESS, UART_FUNC_GET_PARAM, value, sizeof(value) };
    uint8_t buf[16];
    size_t len = buildUartFrame(&resp, buf);
    addUsart1TransmitData(buf, len);
    return true;
}

/**
 * @}
 */