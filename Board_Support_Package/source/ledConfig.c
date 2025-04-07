/**
 * @file ledConfig.c
 * @brief LED配置和控制接口实现
 * @author konbakuyomu
 * @date 2025-03-07
 * @details 实现LED初始化和控制的接口函数，支持多个LED的独立或组合控制
 */

#include "globalConfig.h"

/* 常量定义
 * -------------------------------------------------------------*/

/**
 * @defgroup LED_Selection_Implementation LED选择定义实现
 * @brief LED选择标识符的具体实现值
 * @{
 */
/** @brief 状态LED标识符 */
const uint8_t LED_STATUS = 0x01U;
/** @brief 网络LED标识符 */
const uint8_t LED_NETWORK = 0x02U;
/** @brief 故障LED标识符 */
const uint8_t LED_FAULT = 0x04U;
/** @brief 报警LED标识符 */
const uint8_t LED_ALARM = 0x08U;
/** @brief 所有LED的组合标识符 */
const uint8_t STATUS_LED_ALL = (LED_STATUS | LED_NETWORK | LED_FAULT | LED_ALARM);
/**
 * @}
 */

/* 静态变量
 * -------------------------------------------------------------*/

/**
 * @defgroup LED_Port_Pin_Definitions LED端口和引脚定义
 * @brief 各LED对应的GPIO端口和引脚定义
 * @{
 */
/** @brief 状态LED端口 */
static const uint8_t STATUS_LED_PORT = GPIO_PORT_B;
/** @brief 状态LED引脚 */
static const uint16_t STATUS_LED_PIN = GPIO_PIN_13;
/** @brief 网络LED端口 */
static const uint8_t NETWORK_LED_PORT = GPIO_PORT_B;
/** @brief 网络LED引脚 */
static const uint16_t NETWORK_LED_PIN = GPIO_PIN_15;
/** @brief 故障LED端口 */
static const uint8_t FAULT_LED_PORT = GPIO_PORT_D;
/** @brief 故障LED引脚 */
static const uint16_t FAULT_LED_PIN = GPIO_PIN_09;
/** @brief 报警LED端口 */
static const uint8_t ALARM_LED_PORT = GPIO_PORT_D;
/** @brief 报警LED引脚 */
static const uint16_t ALARM_LED_PIN = GPIO_PIN_10;
/**
 * @}
 */

/**
 * @var ledPortPin
 * @brief LED端口和引脚配置数组
 * @details
 * 该数组包含所有LED的端口和引脚配置，按照状态灯、网络灯、故障灯、报警灯的顺序排列
 */
static const PortPinConfiguration ledPortPin[4]
    = { { STATUS_LED_PORT, STATUS_LED_PIN },
        { NETWORK_LED_PORT, NETWORK_LED_PIN },
        { FAULT_LED_PORT, FAULT_LED_PIN },
        { ALARM_LED_PORT, ALARM_LED_PIN } };

/* 函数定义
 * -------------------------------------------------------------*/

/**
 * @brief 初始化所有LED
 * @details 配置LED引脚为输出模式，并将所有LED初始状态设置为关闭
 */
void ledConfigInit(void)
{
    stc_gpio_init_t stcGpioInit;
    uint8_t i;

    /* 配置结构体初始化 */
    (void)GPIO_StructInit(&stcGpioInit);

    /* 设置LED引脚为输出模式 */
    stcGpioInit.u16PinDir = PIN_DIR_OUT;

    /* 初始化所有LED引脚 */
    for (i = 0U; i < 4U; i++) {
        (void)GPIO_Init(
            ledPortPin[i].portNumber, ledPortPin[i].pinNumber, &stcGpioInit);
    }

    /* 初始状态下关闭所有LED */
    ledTurnOff(STATUS_LED_ALL);
}

/**
 * @brief 打开指定的LED
 * @param [in] ledSelection 要打开的LED选择
 *                         - LED_STATUS: 状态LED
 *                         - LED_NETWORK: 网络LED
 *                         - LED_FAULT: 故障LED
 *                         - LED_ALARM: 报警LED
 *                         - STATUS_LED_ALL: 所有LED
 */
void ledTurnOn(uint8_t ledSelection)
{
    uint8_t i;

    for (i = 0U; i < 4U; i++) {
        if (0U != ((ledSelection >> i) & 1U)) {
            GPIO_SetPins(ledPortPin[i].portNumber, ledPortPin[i].pinNumber);
        }
    }
}

/**
 * @brief 关闭指定的LED
 * @param [in] ledSelection 要关闭的LED选择
 *                         - LED_STATUS: 状态LED
 *                         - LED_NETWORK: 网络LED
 *                         - LED_FAULT: 故障LED
 *                         - LED_ALARM: 报警LED
 *                         - STATUS_LED_ALL: 所有LED
 */
void ledTurnOff(uint8_t ledSelection)
{
    uint8_t i;

    for (i = 0U; i < 4U; i++) {
        if (0U != ((ledSelection >> i) & 1U)) {
            GPIO_ResetPins(ledPortPin[i].portNumber, ledPortPin[i].pinNumber);
        }
    }
}

/**
 * @brief 切换指定的LED状态
 * @param [in] ledSelection 要切换的LED选择
 *                         - LED_STATUS: 状态LED
 *                         - LED_NETWORK: 网络LED
 *                         - LED_FAULT: 故障LED
 *                         - LED_ALARM: 报警LED
 *                         - STATUS_LED_ALL: 所有LED
 */
void ledToggle(uint8_t ledSelection)
{
    uint8_t i;

    for (i = 0U; i < 4U; i++) {
        if (0U != ((ledSelection >> i) & 1U)) {
            GPIO_TogglePins(ledPortPin[i].portNumber, ledPortPin[i].pinNumber);
        }
    }
}