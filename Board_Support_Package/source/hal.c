/**
 * @file hal.c
 * @brief 硬件抽象层实现 (纯 C 版本)
 * @date 2025-03-07
 * @details 实现了硬件抽象层的接口函数，提供了统一的硬件操作方式
 */
#include "globalConfig.h"

/* 内部类型定义
 * -------------------------------------------------------------*/

/**
 * @typedef HardwareAbstractionModule
 * @brief HAL模块类型定义
 * @details 包含HAL模块的所有状态和函数指针，实现HAL的核心功能
 */
typedef struct {
    bool initialized;                                 /**< 初始化标志 */
    uint8_t versionMajor;                             /**< 主版本号 */
    uint8_t versionMinor;                             /**< 次版本号 */
    void (*initializeSystem)(void);                   /**< 系统初始化函数 */
    void (*delayMillisecondsFunction)(unsigned long); /**< 延时函数 */
    unsigned long (*getTickCountFunction)(void);      /**< 获取滴答计数函数 */

    /* LED相关函数 */
    HardwareStatus (*ledTurnOnFunction)(LedIdentifier);
    HardwareStatus (*ledTurnOffFunction)(LedIdentifier);
    HardwareStatus (*ledToggleFunction)(LedIdentifier);

    /* PWM相关函数 */
    HardwareStatus (*pwmStartFunction)(uint32_t);
    HardwareStatus (*pwmStopFunction)(uint32_t);
    HardwareStatus (*pwmConfigureFunction)(const PwmConfiguration*);
    HardwareStatus (*pwmSetDirectionFunction)(uint32_t, PwmDirection);
    HardwareStatus (*pwmGetDirectionFunction)(uint32_t, PwmDirection*);

    /* UART相关函数 */
    HardwareStatus (*uartSendFunction)(const UartTransferData*);
    HardwareStatus (*uartReceiveFunction)(UartTransferData*, uint32_t);

    /* CAN相关函数 */
    HardwareStatus (*canSendFunction)(const CanMessage*);
} HardwareAbstractionModule;

/* 静态变量
 * -------------------------------------------------------------*/

/**
 * @var g_hardwareModule
 * @brief HAL模块全局实例，仅在当前文件内有效
 * @details 保存HAL模块的状态和函数指针，实现HAL的状态管理
 */
static HardwareAbstractionModule g_hardwareModule = { .initialized = false,
                                                      .versionMajor = HAL_VERSION_MAJOR,
                                                      .versionMinor = HAL_VERSION_MINOR,
                                                      /* 所有函数指针初始为NULL */
                                                      .initializeSystem = NULL,
                                                      .delayMillisecondsFunction = NULL,
                                                      .getTickCountFunction = NULL,
                                                      .ledTurnOnFunction = NULL,
                                                      .ledTurnOffFunction = NULL,
                                                      .ledToggleFunction = NULL,
                                                      .pwmStartFunction = NULL,
                                                      .pwmStopFunction = NULL,
                                                      .pwmConfigureFunction = NULL,
                                                      .pwmSetDirectionFunction = NULL,
                                                      .pwmGetDirectionFunction = NULL,
                                                      .uartSendFunction = NULL,
                                                      .uartReceiveFunction = NULL,
                                                      .canSendFunction = NULL };

/* 内部函数
 * -------------------------------------------------------------*/

/**
 * @brief 验证HAL已初始化（仅在本文件内使用）
 * @return bool
 *         - true: 已初始化
 *         - false: 未初始化
 * @details 检查HAL模块是否已经完成初始化
 */
static inline bool verifyInitialized(void)
{
    if (!g_hardwareModule.initialized) {
        /* 可以在这里添加错误日志 */
        return false;
    }
    return true;
}

/* 函数实现
 * -------------------------------------------------------------*/

/**
 * @brief 注册HAL功能函数
 * @param [in] initializeFunction 系统初始化函数
 * @param [in] delayFunction 延时函数
 * @param [in] getTickFunction 获取滴答计数函数
 * @return HardwareStatus
 *         - HAL_OK: 注册成功
 *         - HAL_INVALID_ARG: 参数无效
 * @details 注册系统的核心功能函数，包括系统初始化、延时和获取系统时间
 */
HardwareStatus HAL_registerCoreFunctions(void (*initializeFunction)(void),
                                         void (*delayFunction)(unsigned long),
                                         unsigned long (*getTickFunction)(void))
{
    if (!initializeFunction || !delayFunction || !getTickFunction) {
        return HAL_INVALID_ARG;
    }

    g_hardwareModule.initializeSystem = initializeFunction;
    g_hardwareModule.delayMillisecondsFunction = delayFunction;
    g_hardwareModule.getTickCountFunction = getTickFunction;

    return HAL_OK;
}

/**
 * @brief 注册LED相关功能函数
 * @param [in] turnOnFunction 打开LED函数
 * @param [in] turnOffFunction 关闭LED函数
 * @param [in] toggleFunction 切换LED状态函数
 * @return HardwareStatus
 *         - HAL_OK: 注册成功
 *         - HAL_INVALID_ARG: 参数无效
 * @details 注册LED控制相关的功能函数，包括开启、关闭和切换LED状态
 */
HardwareStatus HAL_registerLedFunctions(HardwareStatus (*turnOnFunction)(LedIdentifier),
                                        HardwareStatus (*turnOffFunction)(LedIdentifier),
                                        HardwareStatus (*toggleFunction)(LedIdentifier))
{
    if (!turnOnFunction || !turnOffFunction || !toggleFunction) {
        return HAL_INVALID_ARG;
    }

    g_hardwareModule.ledTurnOnFunction = turnOnFunction;
    g_hardwareModule.ledTurnOffFunction = turnOffFunction;
    g_hardwareModule.ledToggleFunction = toggleFunction;

    return HAL_OK;
}

/**
 * @brief 注册PWM相关功能函数
 * @param [in] startFunction 启动PWM函数
 * @param [in] stopFunction 停止PWM函数
 * @param [in] configureFunction 配置PWM函数
 * @param [in] setDirectionFunction 设置PWM方向函数
 * @param [in] getDirectionFunction 获取PWM方向函数
 * @return HardwareStatus
 *         - HAL_OK: 注册成功
 *         - HAL_INVALID_ARG: 参数无效
 * @details 注册PWM控制相关的功能函数，包括启动、停止、配置和方向控制
 */
HardwareStatus
HAL_registerPwmFunctions(HardwareStatus (*startFunction)(uint32_t),
                         HardwareStatus (*stopFunction)(uint32_t),
                         HardwareStatus (*configureFunction)(const PwmConfiguration*),
                         HardwareStatus (*setDirectionFunction)(uint32_t, PwmDirection),
                         HardwareStatus (*getDirectionFunction)(uint32_t, PwmDirection*))
{
    if (!startFunction || !stopFunction || !configureFunction || !setDirectionFunction
        || !getDirectionFunction) {
        return HAL_INVALID_ARG;
    }

    g_hardwareModule.pwmStartFunction = startFunction;
    g_hardwareModule.pwmStopFunction = stopFunction;
    g_hardwareModule.pwmConfigureFunction = configureFunction;
    g_hardwareModule.pwmSetDirectionFunction = setDirectionFunction;
    g_hardwareModule.pwmGetDirectionFunction = getDirectionFunction;

    return HAL_OK;
}

/**
 * @brief 注册UART相关功能函数
 * @param [in] sendFunction 发送UART数据函数
 * @param [in] receiveFunction 接收UART数据函数
 * @return HardwareStatus
 *         - HAL_OK: 注册成功
 *         - HAL_INVALID_ARG: 参数无效
 * @details 注册UART通信相关的功能函数，包括数据发送和接收
 */
HardwareStatus HAL_registerUartFunctions(HardwareStatus (*sendFunction)(const UartTransferData*),
                                         HardwareStatus (*receiveFunction)(UartTransferData*,
                                                                           uint32_t))
{
    if (!sendFunction || !receiveFunction) {
        return HAL_INVALID_ARG;
    }

    g_hardwareModule.uartSendFunction = sendFunction;
    g_hardwareModule.uartReceiveFunction = receiveFunction;

    return HAL_OK;
}

/**
 * @brief 注册CAN相关功能函数
 * @param [in] sendFunction 发送CAN消息函数
 * @return HardwareStatus
 *         - HAL_OK: 注册成功
 *         - HAL_INVALID_ARG: 参数无效
 * @details 注册CAN通信相关的功能函数，包括消息发送
 */
HardwareStatus HAL_registerCanFunctions(HardwareStatus (*sendFunction)(const CanMessage*))
{
    if (!sendFunction) {
        return HAL_INVALID_ARG;
    }

    g_hardwareModule.canSendFunction = sendFunction;

    return HAL_OK;
}

/**
 * @brief 初始化HAL模块
 * @return HardwareStatus
 *         - HAL_OK: 初始化成功
 *         - HAL_ERROR: 必要函数未注册
 * @details 初始化硬件抽象层，准备系统硬件使用
 */
HardwareStatus HAL_initialize(void)
{
    if (g_hardwareModule.initialized) {
        return HAL_OK; // 已经初始化过
    }

    if (!g_hardwareModule.initializeSystem) {
        return HAL_ERROR; // 必要函数未注册
    }

    g_hardwareModule.initializeSystem();
    g_hardwareModule.initialized = true;
    return HAL_OK;
}

/**
 * @brief 检查HAL是否已初始化
 * @return bool
 *         - true: 已初始化
 *         - false: 未初始化
 * @details 检查HAL模块是否已经完成初始化
 */
bool HAL_isInitialized(void) { return g_hardwareModule.initialized; }

/**
 * @brief 获取HAL版本信息
 * @param [out] majorVersion 主版本号
 * @param [out] minorVersion 次版本号
 * @details 获取当前HAL的版本信息
 */
void HAL_getVersion(uint8_t* majorVersion, uint8_t* minorVersion)
{
    if (majorVersion)
        *majorVersion = g_hardwareModule.versionMajor;
    if (minorVersion)
        *minorVersion = g_hardwareModule.versionMinor;
}

/**
 * @brief 延迟指定毫秒数
 * @param [in] milliseconds 延迟时间(毫秒)
 * @details 实现系统的毫秒级延时
 */
void HAL_delayMilliseconds(unsigned long milliseconds)
{
    if (verifyInitialized() && g_hardwareModule.delayMillisecondsFunction) {
        g_hardwareModule.delayMillisecondsFunction(milliseconds);
    }
}

/**
 * @brief 获取当前系统滴答计数
 * @return 当前系统滴答计数值
 * @details 获取系统启动以来的滴答计数，用于时间计算
 */
unsigned long HAL_getTickCount(void)
{
    if (verifyInitialized() && g_hardwareModule.getTickCountFunction) {
        return g_hardwareModule.getTickCountFunction();
    }
    return 0;
}

/* LED相关函数实现
 * -------------------------------------------------------------*/

/**
 * @brief 打开指定LED
 * @param [in] ledId LED标识符
 * @return HardwareStatus
 *         - HAL_OK: 操作成功
 *         - HAL_ERROR: HAL未初始化或函数未注册
 * @details 控制指定的LED点亮
 */
HardwareStatus HAL_ledTurnOn(LedIdentifier ledId)
{
    if (!verifyInitialized()) {
        return HAL_ERROR;
    }

    if (g_hardwareModule.ledTurnOnFunction) {
        return g_hardwareModule.ledTurnOnFunction(ledId);
    }
    return HAL_ERROR;
}

/**
 * @brief 关闭指定LED
 * @param [in] ledId LED标识符
 * @return HardwareStatus
 *         - HAL_OK: 操作成功
 *         - HAL_ERROR: HAL未初始化或函数未注册
 * @details 控制指定的LED熄灭
 */
HardwareStatus HAL_ledTurnOff(LedIdentifier ledId)
{
    if (!verifyInitialized()) {
        return HAL_ERROR;
    }

    if (g_hardwareModule.ledTurnOffFunction) {
        return g_hardwareModule.ledTurnOffFunction(ledId);
    }
    return HAL_ERROR;
}

/**
 * @brief 切换指定LED状态
 * @param [in] ledId LED标识符
 * @return HardwareStatus
 *         - HAL_OK: 操作成功
 *         - HAL_ERROR: HAL未初始化或函数未注册
 * @details 将指定LED的状态在开和关之间切换
 */
HardwareStatus HAL_ledToggle(LedIdentifier ledId)
{
    if (!verifyInitialized()) {
        return HAL_ERROR;
    }

    if (g_hardwareModule.ledToggleFunction) {
        return g_hardwareModule.ledToggleFunction(ledId);
    }
    return HAL_ERROR;
}

/* PWM相关函数实现
 * -------------------------------------------------------------*/

/**
 * @brief 启动指定的PWM
 * @param [in] channelNumber PWM通道号
 * @return HardwareStatus
 *         - HAL_OK: 操作成功
 *         - HAL_ERROR: HAL未初始化或函数未注册
 * @details 启动指定通道的PWM输出
 */
HardwareStatus HAL_pwmStart(uint32_t channelNumber)
{
    if (!verifyInitialized()) {
        return HAL_ERROR;
    }

    if (g_hardwareModule.pwmStartFunction) {
        return g_hardwareModule.pwmStartFunction(channelNumber);
    }
    return HAL_ERROR;
}

/**
 * @brief 停止指定的PWM
 * @param [in] channelNumber PWM通道号
 * @return HardwareStatus
 *         - HAL_OK: 操作成功
 *         - HAL_ERROR: HAL未初始化或函数未注册
 * @details 停止指定通道的PWM输出
 */
HardwareStatus HAL_pwmStop(uint32_t channelNumber)
{
    if (!verifyInitialized()) {
        return HAL_ERROR;
    }

    if (g_hardwareModule.pwmStopFunction) {
        return g_hardwareModule.pwmStopFunction(channelNumber);
    }
    return HAL_ERROR;
}

/**
 * @brief 配置PWM参数
 * @param [in] configuration PWM配置参数
 * @return HardwareStatus
 *         - HAL_OK: 操作成功
 *         - HAL_ERROR: HAL未初始化或函数未注册
 * @details 根据配置参数设置PWM通道的工作模式
 */
HardwareStatus HAL_pwmConfigure(const PwmConfiguration* configuration)
{
    if (!verifyInitialized()) {
        return HAL_ERROR;
    }

    if (g_hardwareModule.pwmConfigureFunction) {
        return g_hardwareModule.pwmConfigureFunction(configuration);
    }
    return HAL_ERROR;
}

/**
 * @brief 设置PWM方向
 * @param [in] channelNumber PWM通道号
 * @param [in] direction PWM方向
 * @return HardwareStatus
 *         - HAL_OK: 操作成功
 *         - HAL_ERROR: HAL未初始化或函数未注册
 * @details 设置指定PWM通道的输出方向
 */
HardwareStatus HAL_pwmSetDirection(uint32_t channelNumber, PwmDirection direction)
{
    if (!verifyInitialized()) {
        return HAL_ERROR;
    }

    if (g_hardwareModule.pwmSetDirectionFunction) {
        return g_hardwareModule.pwmSetDirectionFunction(channelNumber, direction);
    }
    return HAL_ERROR;
}

/**
 * @brief 获取PWM方向
 * @param [in] channelNumber PWM通道号
 * @param [out] direction PWM方向
 * @return HardwareStatus
 *         - HAL_OK: 操作成功
 *         - HAL_ERROR: HAL未初始化或函数未注册
 * @details 获取指定PWM通道的当前输出方向
 */
HardwareStatus HAL_pwmGetDirection(uint32_t channelNumber, PwmDirection* direction)
{
    if (!verifyInitialized()) {
        return HAL_ERROR;
    }

    if (g_hardwareModule.pwmGetDirectionFunction) {
        return g_hardwareModule.pwmGetDirectionFunction(channelNumber, direction);
    }
    return HAL_ERROR;
}

/* UART相关函数实现
 * -------------------------------------------------------------*/

/**
 * @brief 发送UART数据
 * @param [in] transferData UART数据配置
 * @return HardwareStatus
 *         - HAL_OK: 操作成功
 *         - HAL_ERROR: HAL未初始化或函数未注册
 * @details 通过指定的UART通道发送数据
 */
HardwareStatus HAL_uartSend(const UartTransferData* transferData)
{
    if (!verifyInitialized()) {
        return HAL_ERROR;
    }

    if (g_hardwareModule.uartSendFunction) {
        return g_hardwareModule.uartSendFunction(transferData);
    }
    return HAL_ERROR;
}

/**
 * @brief 接收UART数据
 * @param [in,out] transferData UART数据配置
 * @param [in] timeoutMilliseconds 超时时间(毫秒)
 * @return HardwareStatus
 *         - HAL_OK: 操作成功
 *         - HAL_ERROR: HAL未初始化或函数未注册
 *         - HAL_TIMEOUT: 接收超时
 * @details 从指定的UART通道接收数据，支持超时机制
 */
HardwareStatus HAL_uartReceive(UartTransferData* transferData, uint32_t timeoutMilliseconds)
{
    if (!verifyInitialized()) {
        return HAL_ERROR;
    }

    if (g_hardwareModule.uartReceiveFunction) {
        return g_hardwareModule.uartReceiveFunction(transferData, timeoutMilliseconds);
    }
    return HAL_ERROR;
}

/* CAN相关函数实现
 * -------------------------------------------------------------*/

/**
 * @brief 发送CAN消息
 * @param [in] message CAN消息
 * @return HardwareStatus
 *         - HAL_OK: 操作成功
 *         - HAL_ERROR: HAL未初始化或函数未注册
 * @details 发送CAN总线消息
 */
HardwareStatus HAL_canSend(const CanMessage* message)
{
    if (!verifyInitialized()) {
        return HAL_ERROR;
    }

    if (g_hardwareModule.canSendFunction) {
        return g_hardwareModule.canSendFunction(message);
    }
    return HAL_ERROR;
}
