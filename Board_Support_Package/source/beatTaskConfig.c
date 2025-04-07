/**
 * @file beatTaskConfig.c
 * @brief 心跳定时器和定时任务配置实现
 * @date 2023-10-15
 * @details 本文件实现了心跳定时器的配置和初始化功能，
 *          以及定时任务互斥锁的注册和管理，用于系统定时任务调度。
 */

/* 头文件
 * -------------------------------------------------------------*/
#include "globalConfig.h"

/* 静态变量
 * -------------------------------------------------------------*/

/**
 * @var SemaphoreHandle_t beatTaskMutex
 * @brief 定时任务互斥锁句柄，用于保护定时任务的并发访问
 */
static SemaphoreHandle_t beatTaskMutex = NULL;

/**
 * @var TaskHandle_t beatTaskHandlerTask
 * @brief 心跳任务句柄，用于提供系统心跳
 */
static TaskHandle_t beatTaskHandlerTask = NULL;

/**
 * @var StaticTask_t beatTaskHandlerTaskBuffer
 * @brief 心跳任务静态创建缓冲区
 */
static StaticTask_t beatTaskHandlerTaskBuffer;

/**
 * @var StackType_t beatTaskHandlerStack[BEAT_TASK_STACK_SIZE]
 * @brief 心跳任务堆栈
 */
static StackType_t beatTaskHandlerStack[SMALL_TASK_STACK_SIZE];

/* 全局变量
 * -------------------------------------------------------------*/
/**
 * @var BEAT_TMRA_PERIOD_VALUE
 * @brief TMRA周期值，用于设置定时器周期
 * @details Timera 定时器的时钟源是PCLK1，频率为100MHz
 *          Frequency = (Clock freq / div) / (Compare value + 1)
 *          当前频率 = (100000000 / 2) / (49999 + 1) = 1000Hz
 *          需要给这个定时任务库的 btask_tick_inc() 提供 1ms 的心跳
 */
static const uint32_t BEAT_TMRA_PERIOD_VALUE = 49999U;

/* 函数实现
 * -------------------------------------------------------------*/

/**
 * @brief 获取定时任务互斥锁
 * @details 当互斥锁有效时，尝试获取互斥锁，用于保护定时任务的并发访问
 */
static void acquireBeatTaskMutex(void)
{
    if (beatTaskMutex != NULL) {
        xSemaphoreTake(beatTaskMutex, portMAX_DELAY);
    }
}

/**
 * @brief 释放定时任务互斥锁
 * @details 当互斥锁有效时，释放互斥锁，允许其他任务访问定时任务资源
 */
static void releaseBeatTaskMutex(void)
{
    if (beatTaskMutex != NULL) {
        xSemaphoreGive(beatTaskMutex);
    }
}

/**
 * @brief 心跳任务处理函数
 * @param [in] param 任务参数，未使用
 * @details
 * 周期性调用btask_handler，用于处理定时任务，通过vTaskDelayUntil确保1ms执行一次
 */
static void beatTaskHandlerFunction(void* param)
{
    const TickType_t xFrequency = pdMS_TO_TICKS(1);
    TickType_t xLastWakeTime = xTaskGetTickCount();

    for (;;) {
        taskENTER_CRITICAL();
        btask_handler();
        taskEXIT_CRITICAL();
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

/**
 * @brief TMRA定时器中断处理函数
 * @details 当TMRA定时器溢出时，清除中断标志并增加btask滴答计数
 */
static void BEAT_TMRA_IRQHandler(void)
{
    if (TMRA_GetStatus(BEAT_TMRA_UNIT, BEAT_TMRA_INT_FLAG) == SET) {
        TMRA_ClearStatus(BEAT_TMRA_UNIT, BEAT_TMRA_INT_FLAG);

        // 在中断中使用portDISABLE_INTERRUPTS/portENABLE_INTERRUPTS
        UBaseType_t uxSavedInterruptStatus = portSET_INTERRUPT_MASK_FROM_ISR();
        btask_tick_inc(1);
        portCLEAR_INTERRUPT_MASK_FROM_ISR(uxSavedInterruptStatus);
    }
}

/**
 * @brief 配置心跳定时器
 * @details 初始化TMRA定时器用于提供系统心跳，配置为1ms中断频率
 * @return 无
 */
void configureBeatTimer(void)
{
    stc_tmra_init_t stcTmraInit;
    stc_irq_signin_config_t stcIrqSignConfig;

    /* 使能 TMRA 时钟 */
    FCG_Fcg2PeriphClockCmd(BEAT_TMRA_CLK, ENABLE);

    /* 配置 TMRA */
    (void)TMRA_StructInit(&stcTmraInit);
    stcTmraInit.u8CountSrc = TMRA_CNT_SRC_SW;
    stcTmraInit.sw_count.u8ClockDiv = BEAT_TMRA_CLK_DIV;
    stcTmraInit.sw_count.u8CountMode = TMRA_MD_SAWTOOTH;
    stcTmraInit.sw_count.u8CountDir = TMRA_DIR_UP;
    stcTmraInit.u32PeriodValue = BEAT_TMRA_PERIOD_VALUE;
    (void)TMRA_Init(BEAT_TMRA_UNIT, &stcTmraInit);

    /* 使能中断 */
    TMRA_IntCmd(BEAT_TMRA_UNIT, BEAT_TMRA_INT_TYPE, ENABLE);

    /* 配置中断 */
    stcIrqSignConfig.enIntSrc = BEAT_TMRA_INT_SRC;
    stcIrqSignConfig.enIRQn = BEAT_TMRA_IRQn;
    stcIrqSignConfig.pfnCallback = &BEAT_TMRA_IRQHandler;
    (void)INTC_IrqSignIn(&stcIrqSignConfig);
    NVIC_ClearPendingIRQ(stcIrqSignConfig.enIRQn);
    NVIC_SetPriority(stcIrqSignConfig.enIRQn, DDL_IRQ_PRIO_DEFAULT);
    NVIC_EnableIRQ(stcIrqSignConfig.enIRQn);
}

/**
 * @brief 启动心跳定时器
 * @details 启动配置好的TMRA定时器，开始提供系统心跳，同时恢复心跳任务
 * @return 无
 */
void startBeatTimer(void)
{
    /* 启动定时器 */
    TMRA_Start(BEAT_TMRA_UNIT);

    /* 恢复心跳任务 */
    if (beatTaskHandlerTask != NULL) {
        vTaskResume(beatTaskHandlerTask);
    }
}

/**
 * @brief 注册定时任务互斥锁回调函数
 * @details 创建互斥锁和心跳任务，并将互斥锁回调函数注册到btask库
 * @return 无
 */
void registerBeatTaskMutex(void)
{
    // 创建 btask 互斥锁
    beatTaskMutex = xSemaphoreCreateMutex();

    // 创建 btask_handler 心跳任务
    beatTaskHandlerTask = xTaskCreateStatic(beatTaskHandlerFunction,
                                            "BeatTaskHandler",
                                            SMALL_TASK_STACK_SIZE,
                                            NULL,
                                            configMAX_PRIORITIES - 1,
                                            beatTaskHandlerStack,
                                            &beatTaskHandlerTaskBuffer);

    // 任务创建后先挂起，等startBeatTimer再启动
    vTaskSuspend(beatTaskHandlerTask);

    btask_mutex_cb_t mutexCallbacks = {
        .mutex_get_cb = acquireBeatTaskMutex,
        .mutex_give_cb = releaseBeatTaskMutex,
    };
    btask_mutex_register(&mutexCallbacks);
}