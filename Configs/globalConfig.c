/**
 * @file globalConfig.c
 * @brief 全局配置源文件
 * @date 2025-04-15
 * @details 本文件实现了全局变量的定义和FreeRTOS静态内存分配回调函数，
 *          为整个项目提供统一的配置参数和静态内存管理支持。
 */

/* 头文件
 * -------------------------------------------------------------*/
#include "globalConfig.h"

/* 全局变量实现
 * -------------------------------------------------------------*/
/**
 * @addtogroup Global_Variables
 * @{
 */
/** @var INIT_TASK_PRIORITY
 *  @brief 初始化任务优先级，实际值定义
 */
const uint8_t INIT_TASK_PRIORITY = 1U;
/** @var APP_TASK_PRIORITY
 *  @brief 应用任务优先级，实际值定义
 */
const uint8_t APP_TASK_PRIORITY = 3U;
/**
 * @}
 */

/* FreeRTOS静态内存分配回调函数
 * -------------------------------------------------------------*/
/**
 * @brief 获取空闲任务的内存
 * @param [out] ppxIdleTaskTCBBuffer 指向空闲任务TCB指针的指针，用于返回静态TCB的地址
 * @param [out] ppxIdleTaskStackBuffer 指向空闲任务堆栈数组首地址指针的指针，用于返回静态堆栈的地址
 * @param [out] pulIdleTaskStackSize 用于返回空闲任务堆栈的大小
 * @details 当configSUPPORT_STATIC_ALLOCATION为1时，FreeRTOS在创建系统空闲任务时
 *          不会通过动态分配内存，而是调用此函数获得用户提供的静态内存。
 *          该函数需要返回一个指向静态TCB（任务控制块）的指针、一个指向静态堆栈数组的指针，
 *          以及堆栈的大小。
 */
void vApplicationGetIdleTaskMemory(StaticTask_t** ppxIdleTaskTCBBuffer,
                                   StackType_t** ppxIdleTaskStackBuffer,
                                   uint32_t* pulIdleTaskStackSize)
{
    /* 声明一个静态变量xIdleTaskTCB用于存储空闲任务的TCB，
       使用static确保内存生命周期覆盖整个程序运行期 */
    static StaticTask_t xIdleTaskTCB;
    /* 声明一个静态数组uxIdleTaskStack，其尺寸为configMINIMAL_STACK_SIZE，
       用作空闲任务的堆栈内存区域 */
    static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];

    /* 将传入的ppxIdleTaskTCBBuffer指针指向xIdleTaskTCB变量的地址，
       以便FreeRTOS得到该任务的TCB内存 */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
    /* 将传入的ppxIdleTaskStackBuffer指针指向空闲任务堆栈数组的首地址 */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;
    /* 指定传出的空闲任务堆栈大小为configMINIMAL_STACK_SIZE，
       该值通常由应用在FreeRTOSConfig.h中配置 */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

/**
 * @brief 获取定时器任务的内存
 * @param [out] ppxTimerTaskTCBBuffer 指向定时器任务TCB指针的指针，用于返回静态TCB的地址
 * @param [out] ppxTimerTaskStackBuffer 指向定时器任务堆栈数组首地址指针的指针，用于返回静态堆栈的地址
 * @param [out] pulTimerTaskStackSize 用于返回定时器任务堆栈的大小
 * @details 当需要使用软件定时器功能（configUSE_TIMERS = 1）且启用静态分配时，
 *          FreeRTOS会自动创建一个定时器任务。为了避免运行时动态分配，
 *          此函数被调用以提供定时器任务所需的静态TCB和堆栈内存。
 */
void vApplicationGetTimerTaskMemory(StaticTask_t** ppxTimerTaskTCBBuffer,
                                    StackType_t** ppxTimerTaskStackBuffer,
                                    uint32_t* pulTimerTaskStackSize)
{
    /* 声明一个静态变量xTimerTaskTCB用于存储定时器任务的TCB，
       确保内存在任务运行期间保持有效 */
    static StaticTask_t xTimerTaskTCB;
    /* 声明一个静态数组uxTimerTaskStack，其尺寸为configTIMER_TASK_STACK_DEPTH，
       用作定时器任务的堆栈内存区域 */
    static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];

    /* 将传入的ppxTimerTaskTCBBuffer指针指向xTimerTaskTCB的地址，
       让系统获得定时器任务TCB的静态内存 */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
    /* 将传入的ppxTimerTaskStackBuffer指针指向定时器任务堆栈数组的首地址 */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;
    /* 指定传出的定时器任务堆栈大小为configTIMER_TASK_STACK_DEPTH，
       该值通常在FreeRTOSConfig.h中配置 */
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
