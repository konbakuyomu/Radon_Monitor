/**
 * @file heartbeatTaskConfig.c
 * @brief 心跳定时器和定时任务配置实现
 * @date 2025-04-15
 * @details 本文件实现了心跳定时器的配置和初始化功能，
 *          以及定时任务互斥锁的注册和管理，用于系统定时任务调度。
 */

/* 头文件
 * -------------------------------------------------------------*/
#include "heartbeatTaskConfig.h"
#include "globalConfig.h"


/* 静态变量
 * -------------------------------------------------------------*/
/**
 * @defgroup HeartbeatTask_StaticVariables 心跳任务静态变量
 * @brief 定时任务相关的静态变量定义
 * @{
 */

/**
 * @var heartbeatTaskMutex
 * @brief 定时任务互斥锁句柄
 * @details 用于保护定时任务的并发访问，确保任务调度的线程安全
 */
static SemaphoreHandle_t heartbeatTaskMutex = NULL;

/**
 * @var heartbeatTaskHandler
 * @brief 心跳任务句柄
 * @details 用于存储心跳任务的标识符，便于后续对任务进行控制
 */
static TaskHandle_t heartbeatTaskHandler = NULL;

/**
 * @var heartbeatTaskControlBlock
 * @brief 心跳任务静态创建控制块
 * @details 用于静态创建FreeRTOS任务，存储任务控制块
 */
static StaticTask_t heartbeatTaskControlBlock;

/**
 * @var heartbeatTaskStack
 * @brief 心跳任务堆栈
 * @details 为心跳任务分配的静态堆栈空间
 */
static StackType_t heartbeatTaskStack[SMALL_TASK_STACK_SIZE];

/**
 * @}
 */

/* 全局变量
 * -------------------------------------------------------------*/
/**
 * @defgroup HeartbeatTask_Constants 心跳任务常量
 * @brief 定时任务相关的常量定义
 * @{
 */

/**
 * @var HEARTBEAT_TIMER_PERIOD_VALUE
 * @brief 定时器周期值
 * @details 用于设置定时器周期，基于以下计算：
 *          - Timera 定时器的时钟源是PCLK1，频率为100MHz
 *          - Frequency = (Clock freq / div) / (Compare value + 1)
 *          - 当前频率 = (100000000 / 2) / (49999 + 1) = 1000Hz
 *          - 需要给这个定时任务库的 timerTask_incrementTick() 提供 1ms 的心跳
 */
static const uint32_t HEARTBEAT_TIMER_PERIOD_VALUE = 49999U;

/**
 * @}
 */

/* 静态函数声明
 * -------------------------------------------------------------*/
/**
 * @defgroup HeartbeatTask_StaticFunctions 心跳任务静态函数
 * @brief 定时任务相关的内部静态函数
 * @{
 */

/**
 * @brief 获取定时任务互斥锁
 * @details 当互斥锁有效时，尝试获取互斥锁，用于保护定时任务的并发访问
 * @note 此函数仅在文件内部使用
 */
static void acquireHeartbeatTaskMutex(void);

/**
 * @brief 释放定时任务互斥锁
 * @details 当互斥锁有效时，释放互斥锁，允许其他任务访问定时任务资源
 * @note 此函数仅在文件内部使用
 */
static void releaseHeartbeatTaskMutex(void);

/**
 * @brief 心跳任务处理函数
 * @param [in] parameters 任务参数，未使用
 * @details 周期性调用timerTask_handler，用于处理定时任务，通过vTaskDelayUntil确保1ms执行一次
 * @note 此函数作为FreeRTOS任务入口点
 */
static void heartbeatTaskFunction(void* parameters);

/**
 * @brief 定时器中断处理函数
 * @details 当定时器溢出时，清除中断标志并增加定时任务滴答计数
 * @note 此函数作为定时器中断处理程序
 */
static void HEARTBEAT_TIMER_InterruptHandler(void);

/**
 * @}
 */

/* 函数实现
 * -------------------------------------------------------------*/
/**
 * @addtogroup HeartbeatTask_StaticFunctions
 * @{
 */

/**
 * @brief 获取定时任务互斥锁
 * @details 当互斥锁有效时，尝试获取互斥锁，用于保护定时任务的并发访问
 */
static void acquireHeartbeatTaskMutex(void)
{
    if (heartbeatTaskMutex != NULL) {
        xSemaphoreTake(heartbeatTaskMutex, portMAX_DELAY);
    }
}

/**
 * @brief 释放定时任务互斥锁
 * @details 当互斥锁有效时，释放互斥锁，允许其他任务访问定时任务资源
 */
static void releaseHeartbeatTaskMutex(void)
{
    if (heartbeatTaskMutex != NULL) {
        xSemaphoreGive(heartbeatTaskMutex);
    }
}

/**
 * @brief 心跳任务处理函数
 * @param [in] parameters 任务参数，未使用
 * @details
 * 周期性调用timerTask_handler，用于处理定时任务，通过vTaskDelayUntil确保1ms执行一次
 */
static void heartbeatTaskFunction(void* parameters)
{
    const TickType_t executionFrequencyMs = pdMS_TO_TICKS(1);
    TickType_t lastWakeTimeMs = xTaskGetTickCount();

    for (;;) {
        taskENTER_CRITICAL();
        btask_handler();
        taskEXIT_CRITICAL();
        vTaskDelayUntil(&lastWakeTimeMs, executionFrequencyMs);
    }
}

/**
 * @brief 定时器中断处理函数
 * @details 当定时器溢出时，清除中断标志并增加定时任务滴答计数
 */
static void HEARTBEAT_TIMER_InterruptHandler(void)
{
    if (TMRA_GetStatus(HEARTBEAT_TIMER_UNIT, HEARTBEAT_TIMER_INT_FLAG) == SET) {
        TMRA_ClearStatus(HEARTBEAT_TIMER_UNIT, HEARTBEAT_TIMER_INT_FLAG);

        // 在中断中使用portDISABLE_INTERRUPTS/portENABLE_INTERRUPTS
        UBaseType_t savedInterruptStatus = portSET_INTERRUPT_MASK_FROM_ISR();
        btask_tick_inc(1);
        portCLEAR_INTERRUPT_MASK_FROM_ISR(savedInterruptStatus);
    }
}

/**
 * @}
 */

/**
 * @defgroup HeartbeatTask_PublicFunctions 心跳任务公共函数
 * @brief 定时任务相关的公共接口函数
 * @{
 */

/**
 * @brief 配置心跳定时器
 * @details 初始化TMRA定时器用于提供系统心跳，配置为1ms中断频率
 * @return 无
 */
void configureHeartbeatTimer(void)
{
    stc_tmra_init_t timerInitConfig;
    stc_irq_signin_config_t interruptConfig;

    /* 使能定时器时钟 */
    FCG_Fcg2PeriphClockCmd(HEARTBEAT_TIMER_CLOCK, ENABLE);

    /* 配置定时器 */
    (void)TMRA_StructInit(&timerInitConfig);
    timerInitConfig.u8CountSrc = TMRA_CNT_SRC_SW;
    timerInitConfig.sw_count.u8ClockDiv = HEARTBEAT_TIMER_CLOCK_DIV;
    timerInitConfig.sw_count.u8CountMode = TMRA_MD_SAWTOOTH;
    timerInitConfig.sw_count.u8CountDir = TMRA_DIR_UP;
    timerInitConfig.u32PeriodValue = HEARTBEAT_TIMER_PERIOD_VALUE;
    (void)TMRA_Init(HEARTBEAT_TIMER_UNIT, &timerInitConfig);

    /* 使能中断 */
    TMRA_IntCmd(HEARTBEAT_TIMER_UNIT, HEARTBEAT_TIMER_INT_TYPE, ENABLE);

    /* 配置中断 */
    interruptConfig.enIntSrc = HEARTBEAT_TIMER_INT_SRC;
    interruptConfig.enIRQn = HEARTBEAT_TIMER_IRQ_NUM;
    interruptConfig.pfnCallback = &HEARTBEAT_TIMER_InterruptHandler;
    (void)INTC_IrqSignIn(&interruptConfig);
    NVIC_ClearPendingIRQ(interruptConfig.enIRQn);
    NVIC_SetPriority(interruptConfig.enIRQn, DDL_IRQ_PRIO_DEFAULT);
    NVIC_EnableIRQ(interruptConfig.enIRQn);
}

/**
 * @brief 启动心跳定时器
 * @details 启动配置好的TMRA定时器，开始提供系统心跳，同时恢复心跳任务
 * @return 无
 */
void startHeartbeatTimer(void)
{
    /* 启动定时器 */
    TMRA_Start(HEARTBEAT_TIMER_UNIT);

    /* 恢复心跳任务 */
    if (heartbeatTaskHandler != NULL) {
        vTaskResume(heartbeatTaskHandler);
    }
}

/**
 * @brief 注册定时任务互斥锁回调函数
 * @details 创建互斥锁和心跳任务，并将互斥锁回调函数注册到定时任务库
 * @return 无
 */
void registerHeartbeatTaskMutex(void)
{
    // 创建定时任务互斥锁
    heartbeatTaskMutex = xSemaphoreCreateMutex();

    // 创建心跳任务
    heartbeatTaskHandler = xTaskCreateStatic(heartbeatTaskFunction,
                                             "HeartbeatTask",
                                             SMALL_TASK_STACK_SIZE,
                                             NULL,
                                             configMAX_PRIORITIES - 1,
                                             heartbeatTaskStack,
                                             &heartbeatTaskControlBlock);

    // 任务创建后先挂起，等startHeartbeatTimer再启动
    vTaskSuspend(heartbeatTaskHandler);

    btask_mutex_cb_t mutexCallbacks = {
        .mutex_get_cb = acquireHeartbeatTaskMutex,
        .mutex_give_cb = releaseHeartbeatTaskMutex,
    };
    btask_mutex_register(&mutexCallbacks);
}

/**
 * @}
 */
