/**
 * @file drvHal.c
 * @brief 驱动HAL实现
 * @author konbakuyomu
 * @date 2025-03-07
 */
#include "globalConfig.h"

/* 基础接口实现
 * -------------------------------------------------------------*/

/**
 * @brief 系统初始化函数
 */
static void drvInitSystem(void)
{
    /* 对指定的外设寄存器解锁（解锁后才能写入） */
    LL_PERIPH_WE(LL_PERIPH_ALL);

    // 初始化系统时钟
    BSP_CLK_Init();

    // 配置定时器
    configureCpuTimer();
    configureKeyTimer();
    configureBeatTimer();

    // 初始化USART1
    USART1_Init();

    // 初始化LED
    ledConfigInit();

    // 初始化按键GPIO
    initializeKeyGpio();

    // 注册按键读取函数
    initializeKeyConfiguration();

    // 注册定时任务互斥锁回调
    registerBeatTaskMutex();

    // 开启定时器
    startCpuUsageStatistics();
    startKeyTimer();
    startBeatTimer();

    // 锁定外设寄存器
    LL_PERIPH_WP(LL_PERIPH_ALL);
}

/**
 * @brief 延迟毫秒数
 * @param [in] ms 延迟时间(毫秒)
 */
static void drvDelayMillis(unsigned long ms) { vTaskDelay(pdMS_TO_TICKS(ms)); }

/**
 * @brief 获取系统滴答计数
 * @return 滴答计数值
 */
static unsigned long drvGetTick(void) { return xTaskGetTickCount(); }

/* LED接口实现
 * -------------------------------------------------------------*/

/**
 * @brief 打开LED
 * @param [in] led LED标识符
 * @return hal_status_t HAL_OK表示成功，其他表示失败
 */
static hal_status_t drvLedTurnOn(hal_led_t led)
{
    ledTurnOn(led);
    return HAL_OK;
}

/**
 * @brief 关闭LED
 * @param [in] led LED标识符
 * @return hal_status_t HAL_OK表示成功，其他表示失败
 */
static hal_status_t drvLedTurnOff(hal_led_t led)
{
    ledTurnOff(led);
    return HAL_OK;
}

/**
 * @brief 切换LED状态
 * @param [in] led LED标识符
 * @return hal_status_t HAL_OK表示成功，其他表示失败
 */
static hal_status_t drvLedToggle(hal_led_t led)
{
    ledToggle(led);
    return HAL_OK;
}

/* PWM接口实现
 * -------------------------------------------------------------*/

/**
 * @brief 启动PWM
 * @param [in] channel PWM通道号
 * @return hal_status_t HAL_OK表示成功，其他表示失败
 */
static hal_status_t drvPwmStart(uint32_t channel)
{
    // 暂未实现
    return HAL_OK;
}

/**
 * @brief 停止PWM
 * @param [in] channel PWM通道号
 * @return hal_status_t HAL_OK表示成功，其他表示失败
 */
static hal_status_t drvPwmStop(uint32_t channel)
{
    // 暂未实现
    return HAL_OK;
}

/**
 * @brief 配置PWM
 * @param [in] config PWM配置参数
 * @return hal_status_t HAL_OK表示成功，其他表示失败
 */
static hal_status_t drvPwmConfigure(const hal_pwm_config_t* config)
{
    // 暂未实现
    return HAL_OK;
}

/**
 * @brief 设置PWM方向
 * @param [in] channel PWM通道号
 * @param [in] direction PWM方向
 * @return hal_status_t HAL_OK表示成功，其他表示失败
 */
static hal_status_t drvPwmSetDirection(uint32_t channel,
                                       hal_pwm_direction_t direction)
{
    // 暂未实现
    return HAL_OK;
}

/**
 * @brief 获取PWM方向
 * @param [in] channel PWM通道号
 * @param [out] direction PWM方向
 * @return hal_status_t HAL_OK表示成功，其他表示失败
 */
static hal_status_t drvPwmGetDirection(uint32_t channel,
                                       hal_pwm_direction_t* direction)
{
    // 暂未实现
    return HAL_OK;
}

/* UART接口实现
 * -------------------------------------------------------------*/

/**
 * @brief 发送UART数据
 * @param [in] data UART数据配置
 * @return hal_status_t HAL_OK表示成功，其他表示失败
 */
static hal_status_t drvUartSend(const hal_uart_data_t* data)
{
    if (!data || !data->data || data->length == 0) {
        return HAL_INVALID_ARG;
    }

    // 实际的发送实现
    switch (data->channel) {
    case UART_CHANNEL_1:
        USART1_Send(data->data, data->length);
        break;
    case UART_CHANNEL_2:
        // USART2_Send(data->data, data->length);
        break;
    case UART_CHANNEL_3:
        // USART3_Send(data->data, data->length);
        break;
    case UART_CHANNEL_4:
        // USART4_Send(data->data, data->length);
        break;
    default:
        return HAL_INVALID_ARG;
    }

    return HAL_OK;
}

/**
 * @brief 接收UART数据
 * @param [in,out] data UART数据配置
 * @param [in] timeout 超时时间(毫秒)
 * @return hal_status_t HAL_OK表示成功，其他表示失败
 */
static hal_status_t drvUartReceive(hal_uart_data_t* data, uint32_t timeout)
{
    // 暂未实现
    return HAL_OK;
}

/* CAN接口实现
 * -------------------------------------------------------------*/

/**
 * @brief 发送CAN消息
 * @param [in] message CAN消息
 * @return hal_status_t HAL_OK表示成功，其他表示失败
 */
static hal_status_t drvCanSend(const hal_can_message_t* message)
{
    if (!message || !message->data || message->length == 0) {
        return HAL_INVALID_ARG;
    }

    // 实际的发送实现

    return HAL_OK;
}

/**
 * @brief 向HAL模块注册驱动实现
 */
void HAL_injectDrvHal(void)
{
    // 注册核心功能
    HAL_registerCoreFunctions(drvInitSystem, drvDelayMillis, drvGetTick);

    // 注册LED功能
    HAL_registerLedFunctions(drvLedTurnOn, drvLedTurnOff, drvLedToggle);

    // 注册PWM功能
    HAL_registerPwmFunctions(drvPwmStart,
                             drvPwmStop,
                             drvPwmConfigure,
                             drvPwmSetDirection,
                             drvPwmGetDirection);

    // 注册UART功能
    HAL_registerUartFunctions(drvUartSend, drvUartReceive);

    // 注册CAN功能
    HAL_registerCanFunctions(drvCanSend);

    // 初始化HAL
    HAL_init();
}
