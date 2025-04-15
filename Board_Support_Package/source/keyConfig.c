/**
 * @file keyConfig.c
 * @brief 按键配置和控制接口实现
 * @date 2025-03-07
 * @details 实现按键初始化和配置的接口函数，支持多个按键的独立控制和定时器配置
 */

#include "globalConfig.h"

/* 常量定义
 * -------------------------------------------------------------*/

/**
 * @defgroup KEY_Constants 按键相关常量定义
 * @brief 按键相关的常量值定义
 * @{
 */

/**
 * @var KEY_TMRA_PERIOD_VALUE
 * @brief TMRA周期值，用于设置定时器周期
 * @details Timera 定时器的时钟源是PCLK1，频率为100MHz
 *          Frequency = (Clock freq / div) / (Compare value + 1)
 *          当前频率 = (100000000 / 2) / (49999 + 1) = 1000Hz
 *          按照这个按钮库的说法：btn_tic_ms 心跳的频率在 1-3ms 为最佳
 */
static const uint32_t KEY_TMRA_PERIOD_VALUE = 49999U;

/**
 * @}
 */

/* 静态函数
 * -------------------------------------------------------------*/

/**
 * @brief 按键定时器中断处理函数
 * @details 处理TMRA中断，用于按键扫描的定时触发
 * @return 无
 */
static void KEY_TMRA_IRQHandler(void)
{
    if (TMRA_GetStatus(KEY_TMRA_UNIT, KEY_TMRA_INT_FLAG) == SET) {
        TMRA_ClearStatus(KEY_TMRA_UNIT, KEY_TMRA_INT_FLAG);
        buttonTickMilliseconds(1);
    }
}

/**
 * @brief 读取按键输入电平
 * @param [in] keyNumber 按键编号（1-4）
 * @return uint8_t 按键电平状态
 *         - 1: 表示高电平
 *         - 0: 表示低电平
 *         - KEY_BUTTON_ERROR: 表示无效按键编号
 * @details 根据按键编号读取对应GPIO的输入电平状态
 */
static uint8_t readKeyInputLevel(uint8_t keyNumber)
{
    uint8_t keyLevel = 1;

    switch (keyNumber) {
    case KEY_BUTTON_1:
        keyLevel = GPIO_ReadInputPins(KEY_UP_PORT, KEY_UP_PIN);
        break;
    case KEY_BUTTON_2:
        keyLevel = GPIO_ReadInputPins(KEY_DOWN_PORT, KEY_DOWN_PIN);
        break;
    case KEY_BUTTON_3:
        keyLevel = GPIO_ReadInputPins(KEY_RIGHT_PORT, KEY_RIGHT_PIN);
        break;
    case KEY_BUTTON_4:
        keyLevel = GPIO_ReadInputPins(KEY_CONFIRM_PORT, KEY_CONFIRM_PIN);
        break;
    default:
        keyLevel = KEY_BUTTON_ERROR;
        break;
    }

    return keyLevel;
}

/* 函数实现
 * -------------------------------------------------------------*/

/**
 * @brief 配置按键扫描定时器
 * @details 初始化TMRA用于按键扫描，配置中断和回调函数
 * @return 无
 */
void configureKeyTimer(void)
{
    stc_tmra_init_t stcTmraInit;
    stc_irq_signin_config_t stcIrqSignConfig;

    /* 使能 TMRA 时钟 */
    FCG_Fcg2PeriphClockCmd(KEY_TMRA_CLK, ENABLE);

    /* 配置 TMRA */
    (void)TMRA_StructInit(&stcTmraInit);
    stcTmraInit.u8CountSrc = TMRA_CNT_SRC_SW;
    stcTmraInit.sw_count.u8ClockDiv = KEY_TMRA_CLK_DIV;
    stcTmraInit.sw_count.u8CountMode = TMRA_MD_SAWTOOTH;
    stcTmraInit.sw_count.u8CountDir = TMRA_DIR_UP;
    stcTmraInit.u32PeriodValue = KEY_TMRA_PERIOD_VALUE;
    (void)TMRA_Init(KEY_TMRA_UNIT, &stcTmraInit);

    /* 配置中断 */
    TMRA_IntCmd(KEY_TMRA_UNIT, KEY_TMRA_INT_TYPE, ENABLE);

    stcIrqSignConfig.enIntSrc = KEY_TMRA_INT_SRC;
    stcIrqSignConfig.enIRQn = KEY_TMRA_IRQn;
    stcIrqSignConfig.pfnCallback = &KEY_TMRA_IRQHandler;
    (void)INTC_IrqSignIn(&stcIrqSignConfig);
    NVIC_ClearPendingIRQ(stcIrqSignConfig.enIRQn);
    NVIC_SetPriority(stcIrqSignConfig.enIRQn, DDL_IRQ_PRIO_DEFAULT);
    NVIC_EnableIRQ(stcIrqSignConfig.enIRQn);
}

/**
 * @brief 启动按键扫描定时器
 * @details 启动TMRA开始按键扫描
 * @return 无
 */
void startKeyTimer(void)
{
    /* 启动定时器 */
    TMRA_Start(KEY_TMRA_UNIT);
}

/**
 * @brief 初始化按键GPIO配置
 * @details 配置所有按键GPIO为输入模式，带上拉电阻
 * @return 无
 */
void initializeKeyGpio(void)
{
    stc_gpio_init_t stcGpioInit;

    /* 初始化结构体 */
    (void)GPIO_StructInit(&stcGpioInit);

    /* 配置所有按键GPIO */
    (void)GPIO_Init(KEY_UP_PORT, KEY_UP_PIN, &stcGpioInit);
    (void)GPIO_Init(KEY_DOWN_PORT, KEY_DOWN_PIN, &stcGpioInit);
    (void)GPIO_Init(KEY_RIGHT_PORT, KEY_RIGHT_PIN, &stcGpioInit);
    (void)GPIO_Init(KEY_CONFIRM_PORT, KEY_CONFIRM_PIN, &stcGpioInit);
}

/**
 * @brief 注册按键读取函数
 * @details 将按键读取函数注册到按键处理模块，并初始化所有按键
 * @return 无
 */
void initializeKeyConfiguration(void)
{
    /* 注册按键读取函数 */
    buttonAttachReadInputOutputFunction(readKeyInputLevel);

    /* 初始化所有按键，第二个参数为按键按下去后的电平状态，这里默认是高(光耦未导通)，按下是低(光耦导通)
     */
    buttonAttach(KEY_BUTTON_1, 0);
    buttonAttach(KEY_BUTTON_2, 0);
    buttonAttach(KEY_BUTTON_3, 0);
    buttonAttach(KEY_BUTTON_4, 0);
}
