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
 * @typedef hal_module_t
 * @brief HAL模块类型定义
 */
typedef struct {
    bool initialized;                       /**< 初始化标志 */
    uint8_t versionMajor;                   /**< 主版本号 */
    uint8_t versionMinor;                   /**< 次版本号 */
    void (*initSystem)(void);               /**< 系统初始化函数 */
    void (*delayMillisFunc)(unsigned long); /**< 延时函数 */
    unsigned long (*getTickFunc)(void);     /**< 获取滴答计数函数 */

    /* LED相关函数 */
    hal_status_t (*ledTurnOnFunc)(hal_led_t);
    hal_status_t (*ledTurnOffFunc)(hal_led_t);
    hal_status_t (*ledToggleFunc)(hal_led_t);

    /* PWM相关函数 */
    hal_status_t (*pwmStartFunc)(uint32_t);
    hal_status_t (*pwmStopFunc)(uint32_t);
    hal_status_t (*pwmConfigureFunc)(const hal_pwm_config_t*);
    hal_status_t (*pwmSetDirectionFunc)(uint32_t, hal_pwm_direction_t);
    hal_status_t (*pwmGetDirectionFunc)(uint32_t, hal_pwm_direction_t*);

    /* UART相关函数 */
    hal_status_t (*uartSendFunc)(const hal_uart_data_t*);
    hal_status_t (*uartReceiveFunc)(hal_uart_data_t*, uint32_t);

    /* CAN相关函数 */
    hal_status_t (*canSendFunc)(const hal_can_message_t*);
} hal_module_t;

/* 静态变量
 * -------------------------------------------------------------*/

/**
 * @var hal_module_t s_hal
 * @brief HAL模块全局实例，仅在当前文件内有效
 */
static hal_module_t s_hal = { .initialized = false,
                              .versionMajor = HAL_VERSION_MAJOR,
                              .versionMinor = HAL_VERSION_MINOR,
                              /* 所有函数指针初始为NULL */
                              .initSystem = NULL,
                              .delayMillisFunc = NULL,
                              .getTickFunc = NULL,
                              .ledTurnOnFunc = NULL,
                              .ledTurnOffFunc = NULL,
                              .ledToggleFunc = NULL,
                              .pwmStartFunc = NULL,
                              .pwmStopFunc = NULL,
                              .pwmConfigureFunc = NULL,
                              .pwmSetDirectionFunc = NULL,
                              .pwmGetDirectionFunc = NULL,
                              .uartSendFunc = NULL,
                              .uartReceiveFunc = NULL,
                              .canSendFunc = NULL };

/* 内部函数
 * -------------------------------------------------------------*/

/**
 * @brief 验证HAL已初始化（仅在本文件内使用）
 * @return bool true表示已初始化，false表示未初始化
 */
static inline bool verifyInitialized(void)
{
    if (!s_hal.initialized) {
        /* 可以在这里添加错误日志 */
        return false;
    }
    return true;
}

/* 函数实现
 * -------------------------------------------------------------*/

/**
 * @brief 注册HAL功能函数
 * @param [in] initFunc 系统初始化函数
 * @param [in] delayFunc 延时函数
 * @param [in] tickFunc 获取滴答计数函数
 * @return hal_status_t 
 *         - HAL_OK: 注册成功
 *         - HAL_INVALID_ARG: 参数无效
 */
hal_status_t HAL_registerCoreFunctions(void (*initFunc)(void),
                                       void (*delayFunc)(unsigned long),
                                       unsigned long (*tickFunc)(void))
{
    if (!initFunc || !delayFunc || !tickFunc) {
        return HAL_INVALID_ARG;
    }

    s_hal.initSystem = initFunc;
    s_hal.delayMillisFunc = delayFunc;
    s_hal.getTickFunc = tickFunc;

    return HAL_OK;
}

/**
 * @brief 注册LED相关功能函数
 * @param [in] turnOnFunc 打开LED函数
 * @param [in] turnOffFunc 关闭LED函数
 * @param [in] toggleFunc 切换LED状态函数
 * @return hal_status_t
 *         - HAL_OK: 注册成功
 *         - HAL_INVALID_ARG: 参数无效
 */
hal_status_t HAL_registerLedFunctions(hal_status_t (*turnOnFunc)(hal_led_t),
                                      hal_status_t (*turnOffFunc)(hal_led_t),
                                      hal_status_t (*toggleFunc)(hal_led_t))
{
    if (!turnOnFunc || !turnOffFunc || !toggleFunc) {
        return HAL_INVALID_ARG;
    }

    s_hal.ledTurnOnFunc = turnOnFunc;
    s_hal.ledTurnOffFunc = turnOffFunc;
    s_hal.ledToggleFunc = toggleFunc;

    return HAL_OK;
}

/**
 * @brief 注册PWM相关功能函数
 * @param [in] startFunc 启动PWM函数
 * @param [in] stopFunc 停止PWM函数
 * @param [in] configureFunc 配置PWM函数
 * @param [in] setDirectionFunc 设置PWM方向函数
 * @param [in] getDirectionFunc 获取PWM方向函数
 * @return hal_status_t
 *         - HAL_OK: 注册成功
 *         - HAL_INVALID_ARG: 参数无效
 */
hal_status_t HAL_registerPwmFunctions(
    hal_status_t (*startFunc)(uint32_t),
    hal_status_t (*stopFunc)(uint32_t),
    hal_status_t (*configureFunc)(const hal_pwm_config_t*),
    hal_status_t (*setDirectionFunc)(uint32_t, hal_pwm_direction_t),
    hal_status_t (*getDirectionFunc)(uint32_t, hal_pwm_direction_t*))
{
    if (!startFunc || !stopFunc || !configureFunc || !setDirectionFunc
        || !getDirectionFunc) {
        return HAL_INVALID_ARG;
    }

    s_hal.pwmStartFunc = startFunc;
    s_hal.pwmStopFunc = stopFunc;
    s_hal.pwmConfigureFunc = configureFunc;
    s_hal.pwmSetDirectionFunc = setDirectionFunc;
    s_hal.pwmGetDirectionFunc = getDirectionFunc;

    return HAL_OK;
}

/**
 * @brief 注册UART相关功能函数
 * @param [in] sendFunc 发送UART数据函数
 * @param [in] receiveFunc 接收UART数据函数
 * @return hal_status_t
 *         - HAL_OK: 注册成功
 *         - HAL_INVALID_ARG: 参数无效
 */
hal_status_t
HAL_registerUartFunctions(hal_status_t (*sendFunc)(const hal_uart_data_t*),
                          hal_status_t (*receiveFunc)(hal_uart_data_t*, uint32_t))
{
    if (!sendFunc || !receiveFunc) {
        return HAL_INVALID_ARG;
    }

    s_hal.uartSendFunc = sendFunc;
    s_hal.uartReceiveFunc = receiveFunc;

    return HAL_OK;
}

/**
 * @brief 注册CAN相关功能函数
 * @param [in] sendFunc 发送CAN消息函数
 * @return hal_status_t
 *         - HAL_OK: 注册成功
 *         - HAL_INVALID_ARG: 参数无效
 */
hal_status_t
HAL_registerCanFunctions(hal_status_t (*sendFunc)(const hal_can_message_t*))
{
    if (!sendFunc) {
        return HAL_INVALID_ARG;
    }

    s_hal.canSendFunc = sendFunc;

    return HAL_OK;
}

/**
 * @brief 初始化HAL模块
 * @return hal_status_t
 *         - HAL_OK: 初始化成功
 *         - HAL_ERROR: 必要函数未注册
 */
hal_status_t HAL_init(void)
{
    if (s_hal.initialized) {
        return HAL_OK; // 已经初始化过
    }

    if (!s_hal.initSystem) {
        return HAL_ERROR; // 必要函数未注册
    }

    s_hal.initSystem();
    s_hal.initialized = true;
    return HAL_OK;
}

/**
 * @brief 检查HAL是否已初始化
 * @return bool true表示已初始化，false表示未初始化
 */
bool HAL_isInitialized(void) { return s_hal.initialized; }

/**
 * @brief 获取HAL版本信息
 * @param [out] major 主版本号
 * @param [out] minor 次版本号
 */
void HAL_getVersion(uint8_t* major, uint8_t* minor)
{
    if (major)
        *major = s_hal.versionMajor;
    if (minor)
        *minor = s_hal.versionMinor;
}

/**
 * @brief 延迟指定毫秒数
 * @param [in] ms 延迟时间(毫秒)
 */
void HAL_delayMillis(unsigned long ms)
{
    if (verifyInitialized() && s_hal.delayMillisFunc) {
        s_hal.delayMillisFunc(ms);
    }
}

/**
 * @brief 获取当前系统滴答计数
 * @return 当前系统滴答计数值
 */
unsigned long HAL_getTick(void)
{
    if (verifyInitialized() && s_hal.getTickFunc) {
        return s_hal.getTickFunc();
    }
    return 0;
}

/* LED相关函数实现 */

/**
 * @brief 打开指定LED
 * @param [in] led LED标识符
 * @return hal_status_t
 *         - HAL_OK: 操作成功
 *         - HAL_ERROR: HAL未初始化或函数未注册
 */
hal_status_t HAL_ledTurnOn(hal_led_t led)
{
    if (!verifyInitialized()) {
        return HAL_ERROR;
    }

    if (s_hal.ledTurnOnFunc) {
        return s_hal.ledTurnOnFunc(led);
    }
    return HAL_ERROR;
}

/**
 * @brief 关闭指定LED
 * @param [in] led LED标识符
 * @return hal_status_t
 *         - HAL_OK: 操作成功
 *         - HAL_ERROR: HAL未初始化或函数未注册
 */
hal_status_t HAL_ledTurnOff(hal_led_t led)
{
    if (!verifyInitialized()) {
        return HAL_ERROR;
    }

    if (s_hal.ledTurnOffFunc) {
        return s_hal.ledTurnOffFunc(led);
    }
    return HAL_ERROR;
}

/**
 * @brief 切换指定LED状态
 * @param [in] led LED标识符
 * @return hal_status_t
 *         - HAL_OK: 操作成功
 *         - HAL_ERROR: HAL未初始化或函数未注册
 */
hal_status_t HAL_ledToggle(hal_led_t led)
{
    if (!verifyInitialized()) {
        return HAL_ERROR;
    }

    if (s_hal.ledToggleFunc) {
        return s_hal.ledToggleFunc(led);
    }
    return HAL_ERROR;
}

/* PWM相关函数实现 */

/**
 * @brief 启动指定的PWM
 * @param [in] channel PWM通道号
 * @return hal_status_t
 *         - HAL_OK: 操作成功
 *         - HAL_ERROR: HAL未初始化或函数未注册
 */
hal_status_t HAL_pwmStart(uint32_t channel)
{
    if (!verifyInitialized()) {
        return HAL_ERROR;
    }

    if (s_hal.pwmStartFunc) {
        return s_hal.pwmStartFunc(channel);
    }
    return HAL_ERROR;
}

/**
 * @brief 停止指定的PWM
 * @param [in] channel PWM通道号
 * @return hal_status_t
 *         - HAL_OK: 操作成功
 *         - HAL_ERROR: HAL未初始化或函数未注册
 */
hal_status_t HAL_pwmStop(uint32_t channel)
{
    if (!verifyInitialized()) {
        return HAL_ERROR;
    }

    if (s_hal.pwmStopFunc) {
        return s_hal.pwmStopFunc(channel);
    }
    return HAL_ERROR;
}

/**
 * @brief 配置PWM参数
 * @param [in] config PWM配置参数
 * @return hal_status_t
 *         - HAL_OK: 操作成功
 *         - HAL_ERROR: HAL未初始化或函数未注册
 */
hal_status_t HAL_pwmConfigure(const hal_pwm_config_t* config)
{
    if (!verifyInitialized()) {
        return HAL_ERROR;
    }

    if (s_hal.pwmConfigureFunc) {
        return s_hal.pwmConfigureFunc(config);
    }
    return HAL_ERROR;
}

/**
 * @brief 设置PWM方向
 * @param [in] channel PWM通道号
 * @param [in] direction PWM方向
 * @return hal_status_t
 *         - HAL_OK: 操作成功
 *         - HAL_ERROR: HAL未初始化或函数未注册
 */
hal_status_t HAL_pwmSetDirection(uint32_t channel, hal_pwm_direction_t direction)
{
    if (!verifyInitialized()) {
        return HAL_ERROR;
    }

    if (s_hal.pwmSetDirectionFunc) {
        return s_hal.pwmSetDirectionFunc(channel, direction);
    }
    return HAL_ERROR;
}

/**
 * @brief 获取PWM方向
 * @param [in] channel PWM通道号
 * @param [out] direction PWM方向
 * @return hal_status_t
 *         - HAL_OK: 操作成功
 *         - HAL_ERROR: HAL未初始化或函数未注册
 */
hal_status_t HAL_pwmGetDirection(uint32_t channel, hal_pwm_direction_t* direction)
{
    if (!verifyInitialized()) {
        return HAL_ERROR;
    }

    if (s_hal.pwmGetDirectionFunc) {
        return s_hal.pwmGetDirectionFunc(channel, direction);
    }
    return HAL_ERROR;
}

/* UART相关函数实现 */

/**
 * @brief 发送UART数据
 * @param [in] data UART数据配置
 * @return hal_status_t
 *         - HAL_OK: 操作成功
 *         - HAL_ERROR: HAL未初始化或函数未注册
 */
hal_status_t HAL_uartSend(const hal_uart_data_t* data)
{
    if (!verifyInitialized()) {
        return HAL_ERROR;
    }

    if (s_hal.uartSendFunc) {
        return s_hal.uartSendFunc(data);
    }
    return HAL_ERROR;
}

/**
 * @brief 接收UART数据
 * @param [in,out] data UART数据配置
 * @param [in] timeout 超时时间(毫秒)
 * @return hal_status_t
 *         - HAL_OK: 操作成功
 *         - HAL_ERROR: HAL未初始化或函数未注册
 *         - HAL_TIMEOUT: 接收超时
 */
hal_status_t HAL_uartReceive(hal_uart_data_t* data, uint32_t timeout)
{
    if (!verifyInitialized()) {
        return HAL_ERROR;
    }

    if (s_hal.uartReceiveFunc) {
        return s_hal.uartReceiveFunc(data, timeout);
    }
    return HAL_ERROR;
}

/* CAN相关函数实现 */

/**
 * @brief 发送CAN消息
 * @param [in] message CAN消息
 * @return hal_status_t
 *         - HAL_OK: 操作成功
 *         - HAL_ERROR: HAL未初始化或函数未注册
 */
hal_status_t HAL_canSend(const hal_can_message_t* message)
{
    if (!verifyInitialized()) {
        return HAL_ERROR;
    }

    if (s_hal.canSendFunc) {
        return s_hal.canSendFunc(message);
    }
    return HAL_ERROR;
}
