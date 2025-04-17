/**
 * @file usartCommandHandlers.h
 * @brief 串口命令处理函数头文件
 * @date 2025-04-15
 * @details 声明串口命令处理函数接口
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup UART_Command_Handlers 串口命令处理器函数模块
 * @brief 串口命令处理相关函数
 * @{
 */

/* 串口协议功能码定义 */
#define UART_FUNC_REAL_TIME   0x10  /**< 查询实时数据 */
#define UART_FUNC_NUCLIDE     0x11  /**< 查询核素数据 */
#define UART_FUNC_SET_PARAM   0x30  /**< 设置设备参数 */
#define UART_FUNC_GET_PARAM   0x40  /**< 查询设备参数 */

/* 错误码定义 */
#define UART_ERR_INVALID_CRC  0x01  /**< CRC校验失败 */
#define UART_ERR_UNKNOWN_CMD  0x02  /**< 未知命令 */

/* 设备地址 */
#define UART_DEVICE_ADDRESS   0x01  /**< 本设备地址 */

/**
 * @brief 注册所有串口命令处理函数
 * @details 在系统初始化时调用，注册所有串口命令的处理函数
 */
void registerUartCommandHandlers(void);

/**
 * @brief 发送错误响应
 * @param [in] func 功能码
 * @param [in] errCode 错误码
 */
void sendUartErrorResponse(uint8_t func, uint8_t errCode);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif 