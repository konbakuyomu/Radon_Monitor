/**
 * @file usart1Config.c
 * @brief USART1外设配置和初始化的源文件
 * @date 2025-03-10
 * @details 本文件实现了USART1相关的GPIO、DMA、中断和定时器配置，
 *          以及USART1初始化函数。
 */

#include "globalConfig.h"

/* 常量定义
 * -------------------------------------------------------------*/

/**
 * @defgroup USART1_Constants USART1相关常量定义
 * @{
 */

/**
 * @var USART1_TMR0_PERIOD_VALUE
 * @brief USART1接收超时定时器比较值
 * @details Timer0 定时器的时钟源是 LRC 振荡器，频率为 32.768kHz
 *          Frequency = (Clock freq / div) / (Compare value + 1)
 *          当前频率 = (32768 / 8) / (512 + 1) = 7.98Hz(相当于125ms触发一次)
 */
static const uint32_t USART1_TMR0_PERIOD_VALUE = 512UL;

/**
 * @var USART1_BAUDRATE
 * @brief USART1的波特率，单位为bps
 */
static const uint32_t USART1_BAUDRATE = 115200UL;

/**
 * @var MAX_RECV_BUFSIZE
 * @brief USART1接收缓冲区大小
 */
static const uint16_t MAX_RECV_BUFSIZE = 50U;

/**
 * @}
 */

/* 静态函数声明
 * -------------------------------------------------------------*/

static void ReconfigureUartDma(void);
static void USART1_RxTimeout_IrqCallback(void);
static void USART1_RxError_IrqCallback(void);
static void USART1_RX_DMA_TC_IrqCallback(void);
static void USART1_TX_DMA_TC_IrqCallback(void);
static void USART1_TxComplete_IrqCallback(void);
static void USART1_TMR0_Config(void);
static int32_t USART1_DMA_Config(void);
static void USART1_IrqConfig(void);
static void USART1_GPIO_Config(void);

/* 静态函数实现
 * -------------------------------------------------------------*/

/**
 * @brief 重新配置UART的DMA接收
 * @details 重置DMA接收配置，准备接收新的数据
 */
static void ReconfigureUartDma(void)
{
    stc_dma_init_t stcDmaInit;

    (void)DMA_StructInit(&stcDmaInit);
    stcDmaInit.u32IntEn = DMA_INT_ENABLE;
    stcDmaInit.u32BlockSize = 1UL;
    stcDmaInit.u32TransCount = USART1_GetDmaBufferSize_use_c();
    stcDmaInit.u32DataWidth = DMA_DATAWIDTH_8BIT;

    // 使用C++驱动提供的DMA缓冲区
    stcDmaInit.u32DestAddr = (uint32_t)USART1_GetDmaBufferPtr_use_c();

    stcDmaInit.u32SrcAddr = (uint32_t)(&USART1_UNIT->RDR);
    stcDmaInit.u32SrcAddrInc = DMA_SRC_ADDR_FIX;
    stcDmaInit.u32DestAddrInc = DMA_DEST_ADDR_INC;
    DMA_Init(USART1_RX_DMA_UNIT, USART1_RX_DMA_CH, &stcDmaInit);
}

/**
 * @brief USART1接收超时中断回调函数
 * @details 当接收超时时，计算已接收的数据量，通过任务通知发送给处理任务
 */
static void USART1_RxTimeout_IrqCallback(void)
{
    // 计算已接收的字节数
    uint16_t bytesReceived = MAX_RECV_BUFSIZE
        - (uint16_t)DMA_GetTransCount(USART1_RX_DMA_UNIT, USART1_RX_DMA_CH);

    if (bytesReceived <= MAX_RECV_BUFSIZE) {
        // 使用外部接口处理接收到的数据(这里是缓冲区未满的情况)
        USART1_ProcessReceivedData_use_c(bytesReceived);

        // 重新配置DMA接收
        ReconfigureUartDma();

        // 停止定时器
        TMR0_Stop(USART1_TMR0_UNIT, USART1_TMR0_CH);
        // 清除接收超时标志位
        USART_ClearStatus(USART1_UNIT, USART_FLAG_RX_TIMEOUT);
    }
}

/**
 * @brief USART1接收错误中断回调函数
 * @details 处理USART1接收过程中的错误，包括奇偶校验错误、帧错误和溢出错误
 */
static void USART1_RxError_IrqCallback(void)
{
    // 首先从USART中读取数据
    (void)USART_ReadData(USART1_UNIT);
    // 然后清除USART的错误状态
    USART_ClearStatus(
        USART1_UNIT,
        (USART_FLAG_PARITY_ERR | USART_FLAG_FRAME_ERR | USART_FLAG_OVERRUN));
}

/**
 * @brief USART1接收DMA传输完成中断回调函数
 * @details 这个一般是不触发的，前面在配置接收的DMA的时候，配置了 DMA_DTCTLx 的 CNT
 * 寄存器
 *          配置的是接收缓冲区数组的大小，按照芯片手册，必须要把这个数组塞满才会触发这个寄存器
 *          一般预留的数组都比协议规定的数据包要大，所以这个一般不触发
 */
static void USART1_RX_DMA_TC_IrqCallback(void)
{
    // 使用外部接口处理接收到的数据(这里是缓冲区已满的情况)
    USART1_ProcessReceivedData_use_c(MAX_RECV_BUFSIZE);
    // 重新配置DMA接收
    ReconfigureUartDma();

    // 关闭接收超时功能
    USART_FuncCmd(USART1_UNIT, USART_RX_TIMEOUT, DISABLE);
    // 清除接收超时标志位
    USART_ClearStatus(USART1_UNIT, USART_FLAG_RX_TIMEOUT);
    // 停止定时器
    TMR0_Stop(USART1_TMR0_UNIT, USART1_TMR0_CH);
    // 重新使能接收超时功能
    USART_FuncCmd(USART1_UNIT, USART_RX_TIMEOUT, ENABLE);

    // 清除DMA传输完成标志
    DMA_ClearTransCompleteStatus(USART1_RX_DMA_UNIT, USART1_RX_DMA_TC_FLAG);
}

/**
 * @brief USART1发送DMA传输完成中断回调函数
 * @details 当DMA发送完成时，使能USART1发送完成中断，并清除DMA传输完成标志
 */
static void USART1_TX_DMA_TC_IrqCallback(void)
{
    USART_FuncCmd(USART1_UNIT, USART_INT_TX_CPLT, ENABLE); // 使能发送完成中断
    DMA_ClearTransCompleteStatus(USART1_TX_DMA_UNIT, USART1_TX_DMA_TC_FLAG);
}

/**
 * @brief USART1发送完成中断回调函数
 * @details 当USART1发送完成时，关闭发送功能和中断，清除标志位，并准备接收新数据
 */
static void USART1_TxComplete_IrqCallback(void)
{
    // 关闭发送功能和中断
    USART_FuncCmd(USART1_UNIT, (USART_TX | USART_INT_TX_CPLT), DISABLE);
    
    // 通知串口1驱动发送完成
    USART1_NotifyTxComplete_use_c();
}

/**
 * @brief 配置USART1的Timer0
 * @details 配置Timer0用于USART1的接收超时检测，使用LRC振荡器作为时钟源
 */
static void USART1_TMR0_Config(void)
{
    stc_tmr0_init_t stcTmr0Init;
    stc_irq_signin_config_t stcIrqSignConfig;

    /* 使能 timer0 时钟 */
    FCG_Fcg2PeriphClockCmd(USART1_TMR0_CLK, ENABLE);
    /* LRC 振荡器开启 */
    (void)CLK_LrcCmd(ENABLE);

    /* 配置 timer0 */
    (void)TMR0_StructInit(&stcTmr0Init);
    stcTmr0Init.u32ClockSrc = TMR0_CLK_SRC_LRC;
    stcTmr0Init.u32ClockDiv = TMR0_CLK_DIV8;
    stcTmr0Init.u32Func = TMR0_FUNC_CMP;
    stcTmr0Init.u16CompareValue = USART1_TMR0_PERIOD_VALUE;
    (void)TMR0_Init(USART1_TMR0_UNIT, USART1_TMR0_CH, &stcTmr0Init);

    /* 异步时钟源，写入TMR0寄存器需要等待三个异步时钟。 */
    DDL_DelayMS(3U);

    /* 当内部硬件触发事件(这里是USART的TIMEOUT)时，定时器启动有效，开始计时 */
    TMR0_HWStartCondCmd(USART1_TMR0_UNIT, USART1_TMR0_CH, ENABLE);
    /* 当内部硬件触发事件(这里是USART的TIMEOUT)时，定时器清零有效，停止计时 */
    TMR0_HWClearCondCmd(USART1_TMR0_UNIT, USART1_TMR0_CH, ENABLE);
}

/**
 * @brief 配置USART1的DMA
 * @details 配置USART1的接收和发送DMA通道，设置中断和触发源
 * @return int32_t
 *         - LL_OK: 配置成功
 *         - 其他: 配置失败
 */
static int32_t USART1_DMA_Config(void)
{
    int32_t result;
    stc_dma_init_t stcDmaInit;
    stc_irq_signin_config_t stcIrqSignConfig;

    /* 使能DMA时钟 */
    FCG_Fcg0PeriphClockCmd(FCG0_PERIPH_DMA1, ENABLE);
    /* 使能AOS时钟 */
    FCG_Fcg0PeriphClockCmd(FCG0_PERIPH_AOS, ENABLE);

    /* 配置 USART1_RX_DMA */
    (void)DMA_StructInit(&stcDmaInit);
    stcDmaInit.u32IntEn = DMA_INT_ENABLE;
    stcDmaInit.u32BlockSize = 1UL;
    stcDmaInit.u32TransCount = USART1_GetDmaBufferSize_use_c();
    stcDmaInit.u32DataWidth = DMA_DATAWIDTH_8BIT;
    stcDmaInit.u32DestAddr = (uint32_t)USART1_GetDmaBufferPtr_use_c();
    stcDmaInit.u32SrcAddr = (uint32_t)(&USART1_UNIT->RDR);
    stcDmaInit.u32SrcAddrInc = DMA_SRC_ADDR_FIX;
    stcDmaInit.u32DestAddrInc = DMA_DEST_ADDR_INC;
    result = DMA_Init(USART1_RX_DMA_UNIT, USART1_RX_DMA_CH, &stcDmaInit);
    if (result == LL_OK) {
        stcIrqSignConfig.enIntSrc = USART1_RX_DMA_TC_INT_SRC;
        stcIrqSignConfig.enIRQn = USART1_RX_DMA_TC_IRQn;
        stcIrqSignConfig.pfnCallback = &USART1_RX_DMA_TC_IrqCallback;
        (void)INTC_IrqSignIn(&stcIrqSignConfig);
        NVIC_ClearPendingIRQ(stcIrqSignConfig.enIRQn);
        NVIC_SetPriority(stcIrqSignConfig.enIRQn, DDL_IRQ_PRIO_DEFAULT);
        NVIC_EnableIRQ(stcIrqSignConfig.enIRQn);

        AOS_SetTriggerEventSrc(USART1_RX_DMA_TRIG_SEL, USART1_RX_DMA_TRIG_EVT_SRC);

        DMA_Cmd(USART1_RX_DMA_UNIT, ENABLE);
        DMA_TransCompleteIntCmd(USART1_RX_DMA_UNIT, USART1_RX_DMA_TC_INT, ENABLE);
        (void)DMA_ChCmd(USART1_RX_DMA_UNIT, USART1_RX_DMA_CH, ENABLE);
    }

    /* 配置 USART1_TX_DMA */
    (void)DMA_StructInit(&stcDmaInit);
    stcDmaInit.u32IntEn = DMA_INT_ENABLE;
    stcDmaInit.u32BlockSize = 1UL;
    stcDmaInit.u32DataWidth = DMA_DATAWIDTH_8BIT;
    stcDmaInit.u32DestAddr = (uint32_t)(&USART1_UNIT->TDR);
    stcDmaInit.u32SrcAddrInc = DMA_SRC_ADDR_INC;
    stcDmaInit.u32DestAddrInc = DMA_DEST_ADDR_FIX;
    result = DMA_Init(USART1_TX_DMA_UNIT, USART1_TX_DMA_CH, &stcDmaInit);
    if (result == LL_OK) {
        stcIrqSignConfig.enIntSrc = USART1_TX_DMA_TC_INT_SRC;
        stcIrqSignConfig.enIRQn = USART1_TX_DMA_TC_IRQn;
        stcIrqSignConfig.pfnCallback = &USART1_TX_DMA_TC_IrqCallback;
        (void)INTC_IrqSignIn(&stcIrqSignConfig);
        NVIC_ClearPendingIRQ(stcIrqSignConfig.enIRQn);
        NVIC_SetPriority(stcIrqSignConfig.enIRQn, DDL_IRQ_PRIO_DEFAULT);
        NVIC_EnableIRQ(stcIrqSignConfig.enIRQn);

        AOS_SetTriggerEventSrc(USART1_TX_DMA_TRIG_SEL, USART1_TX_DMA_TRIG_EVT_SRC);

        DMA_Cmd(USART1_TX_DMA_UNIT, ENABLE);
        DMA_TransCompleteIntCmd(USART1_TX_DMA_UNIT, USART1_TX_DMA_TC_INT, ENABLE);
    }

    return result;
}

/**
 * @brief 配置USART1的中断
 * @details 配置USART1的接收超时中断、接收错误中断和接收完成中断
 */
static void USART1_IrqConfig(void)
{
    stc_irq_signin_config_t stcIrqSigninConfig;

    /* 配置发送完成中断 */
    stcIrqSigninConfig.enIRQn = USART1_TX_CPLT_IRQn;
    stcIrqSigninConfig.enIntSrc = USART1_TX_CPLT_INT_SRC;
    stcIrqSigninConfig.pfnCallback = &USART1_TxComplete_IrqCallback;
    (void)INTC_IrqSignIn(&stcIrqSigninConfig);
    NVIC_ClearPendingIRQ(stcIrqSigninConfig.enIRQn);
    NVIC_SetPriority(stcIrqSigninConfig.enIRQn, DDL_IRQ_PRIO_DEFAULT);
    NVIC_EnableIRQ(stcIrqSigninConfig.enIRQn);

    /* 配置接收错误中断 */
    stcIrqSigninConfig.enIRQn = USART1_RX_ERR_IRQn;
    stcIrqSigninConfig.enIntSrc = USART1_RX_ERR_INT_SRC;
    stcIrqSigninConfig.pfnCallback = &USART1_RxError_IrqCallback;
    (void)INTC_IrqSignIn(&stcIrqSigninConfig);
    NVIC_ClearPendingIRQ(stcIrqSigninConfig.enIRQn);
    NVIC_SetPriority(stcIrqSigninConfig.enIRQn, DDL_IRQ_PRIO_DEFAULT);
    NVIC_EnableIRQ(stcIrqSigninConfig.enIRQn);

    /* 配置接收超时中断 */
    stcIrqSigninConfig.enIRQn = USART1_RX_TIMEOUT_IRQn;
    stcIrqSigninConfig.enIntSrc = USART1_RX_TIMEOUT_INT_SRC;
    stcIrqSigninConfig.pfnCallback = &USART1_RxTimeout_IrqCallback;
    (void)INTC_IrqSignIn(&stcIrqSigninConfig);
    NVIC_ClearPendingIRQ(stcIrqSigninConfig.enIRQn);
    NVIC_SetPriority(stcIrqSigninConfig.enIRQn, DDL_IRQ_PRIO_DEFAULT);
    NVIC_EnableIRQ(stcIrqSigninConfig.enIRQn);
}

/**
 * @brief 配置USART1的GPIO
 * @details 配置USART1的接收和发送引脚，并初始化USART1
 */
static void USART1_GPIO_Config(void)
{
    stc_usart_uart_init_t stcUartInit;
    stc_gpio_init_t stcGpioInit;

    /* 使能USART1时钟 */
    FCG_Fcg1PeriphClockCmd(FCG1_PERIPH_USART1, ENABLE);

    /* 配置GPIO */
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinState = PIN_STAT_SET;
    stcGpioInit.u16PullUp = PIN_PU_ON;
    stcGpioInit.u16PinDrv = PIN_HIGH_DRV;
    GPIO_Init(USART1_RX_PORT, USART1_RX_PIN, &stcGpioInit);
    GPIO_Init(USART1_TX_PORT, USART1_TX_PIN, &stcGpioInit);
    GPIO_SetFunc(USART1_RX_PORT, USART1_RX_PIN, USART1_RX_GPIO_FUNC);
    GPIO_SetFunc(USART1_TX_PORT, USART1_TX_PIN, USART1_TX_GPIO_FUNC);

    /* 配置USART */
    (void)USART_UART_StructInit(&stcUartInit);
    stcUartInit.u32ClockDiv = USART_CLK_DIV64;
    stcUartInit.u32CKOutput = USART_CK_OUTPUT_ENABLE;
    stcUartInit.u32Baudrate = USART1_BAUDRATE;
    stcUartInit.u32OverSampleBit = USART_OVER_SAMPLE_8BIT;
    (void)USART_UART_Init(USART1_UNIT, &stcUartInit, NULL);
}

/**
 * @brief 初始化USART1外设
 * @details 依次配置USART1的GPIO、DMA、中断和定时器，并使能USART1功能
 */
void USART1_Init(void)
{
    USART1_GPIO_Config();
    USART1_DMA_Config();
    USART1_IrqConfig();
    USART1_TMR0_Config();
    USART1_FunctionEnable();
}

/**
 * @brief 通过USART1发送数据
 * @param [in] data 指向要发送数据的指针
 * @param [in] length 要发送的数据长度（字节数）
 * @details 使用DMA方式通过USART1发送指定长度的数据。
 *          该函数配置DMA传输计数和源地址，然后使能DMA通道和USART发送功能。
 *          发送完成后会触发DMA传输完成中断，在中断处理函数中会关闭发送功能。
 */
void USART1_Send(uint8_t* data, uint16_t length)
{
    DMA_SetTransCount(USART1_TX_DMA_UNIT, USART1_TX_DMA_CH, length);
    DMA_SetSrcAddr(USART1_TX_DMA_UNIT, USART1_TX_DMA_CH, (uint32_t)data);
    (void)DMA_ChCmd(USART1_TX_DMA_UNIT, USART1_TX_DMA_CH, ENABLE);
    USART_FuncCmd(USART1_UNIT, USART_TX, ENABLE);
}
