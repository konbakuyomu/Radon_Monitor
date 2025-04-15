#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H


#include "hc32_ll.h"
#include "cpuUtilization.h"

/***************************************************************************************************************/
/*                             不依赖于 assert.h 头文件提供的常规 assert() 语义                                     */
/***************************************************************************************************************/
#define configASSERT( x ) if ((x) == 0) {taskDISABLE_INTERRUPTS(); for( ;; );}

/***************************************************************************************************************/
/*                                        FreeRTOS基础配置配置选项                                                */
/***************************************************************************************************************/
#define configUSE_PREEMPTION                      1 /* 1: 抢占式调度器, 0: 协程式调度器, 无默认需定义 */
#define configUSE_TIME_SLICING                    1 /* 1: 使能时间片调度 (默认使能) */
#define configUSE_PORT_OPTIMISED_TASK_SELECTION   1 /* 架构优化，使用硬件计算下一个要运行的任务, 0: 使用软件算法计算下一个要运行的任务, 默认: 0 */
#define configUSE_TICKLESS_IDLE                   0 /* 1: 启用低功耗 tickless 模式 */
#define configUSE_QUEUE_SETS                      1 /* 为1时启用队列 */
#define configCPU_CLOCK_HZ                        ( SystemCoreClock ) /* 系统主频。一般SystemCoreClock会和系统主频相等。不同时直接设置主频数，单位Hz */
#define configTICK_RATE_HZ                        ((TickType_t)1000) /* 嘀嗒计时频率，1ms触发一次 */
#define configMAX_PRIORITIES                      ( 7 ) /* 最大优先级，-1，0为最低优先级 */
#define configMINIMAL_STACK_SIZE                  ((uint16_t)128) /* idle任务堆栈大小，定义空闲任务的栈空间大小, 单位: Word, 无默认需定义 */
#define configMAX_TASK_NAME_LEN                   ( 16 ) /* 定义任务名最大字符数, 默认: 16 */

#define configUSE_16_BIT_TICKS                    0 /* 1: 定义系统时钟节拍计数器的数据类型为16位无符号数, 无默认需定义 */
#define configIDLE_SHOULD_YIELD                   1 /* 空闲任务让出 CPU 给同优先级任务 */
#define configUSE_TASK_NOTIFICATIONS              1 /* 启用任务通知 */
#define configUSE_MUTEXES                         1 /* 1: 使能互斥信号量, 默认: 0 */
#define configQUEUE_REGISTRY_SIZE                 8 /* 定义可以注册的信号量和消息队列的个数, 默认: 0 */
#define configCHECK_FOR_STACK_OVERFLOW            0 /* 启用堆栈溢出检测 (0 禁用, 1 或 2 启用) */
#define configUSE_RECURSIVE_MUTEXES               1 /* 启用递归互斥信号量 */
#define configUSE_MALLOC_FAILED_HOOK              0 /* 禁用内存申请失败钩子 */
#define configUSE_APPLICATION_TASK_TAG            0 /* 任务标签 */
#define configUSE_COUNTING_SEMAPHORES             1 /* 启用计数信号量 */
#define configMESSAGE_BUFFER_LENGTH_TYPE          size_t /* 定义消息缓冲区中消息长度的数据类型, 默认: size_t */

/***************************************************************************************************************/
/*                                FreeRTOS与内存申请有关配置选项                                                    */
/***************************************************************************************************************/
#define configSUPPORT_STATIC_ALLOCATION           1 /* 1: 支持静态申请内存, 默认: 0 */
#define configSUPPORT_DYNAMIC_ALLOCATION          1 /* 1: 支持动态申请内存, 默认: 1 */
#define configTOTAL_HEAP_SIZE                     ((size_t)15360) /* 栈空间大小 FreeRTOS堆中可用的RAM总量, 单位: Byte, 无默认需定义 */

/***************************************************************************************************************/
/*                                FreeRTOS与钩子函数有关的配置选项                                                  */
/***************************************************************************************************************/
#define configUSE_IDLE_HOOK                       0 /* 空闲钩子函数，勾选后必须定义void vApplicationIdleHook( void ) */
#define configUSE_TICK_HOOK                       0 /* 1: 使能系统时钟节拍中断钩子函数, 无默认需定义 */

/***************************************************************************************************************/
/*                        FreeRTOS与运行时间和任务状态收集有关的配置选项                                              */
/***************************************************************************************************************/
#define configRECORD_STACK_HIGH_ADDRESS          1 /* 记录堆栈最高地址 */
#define configGENERATE_RUN_TIME_STATS            1 /* 启用运行时间统计功能 */
#define configUSE_TRACE_FACILITY                 1 /* 启用可视化跟踪调试 */
#define configUSE_STATS_FORMATTING_FUNCTIONS     1 /* 启用任务状态统计格式化功能 */

#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() (g_processorRuntimeCounter = 0ul) /* 配置定时器用于运行时间统计 */
#define portGET_RUN_TIME_COUNTER_VALUE()         (g_processorRuntimeCounter)       /* 获取运行时间计数器值 */

/***************************************************************************************************************/
/*                                FreeRTOS与协程有关的配置选项                                                    */
/***************************************************************************************************************/
#define configUSE_CO_ROUTINES                     0 /* 1: 启用协程, 默认: 0 */
#define configMAX_CO_ROUTINE_PRIORITIES           ( 2 ) /* 定义协程的最大优先级, 最大优先级=configMAX_CO_ROUTINE_PRIORITIES-1, 无默认configUSE_CO_ROUTINES为1时需定义 */

/***************************************************************************************************************/
/*                                FreeRTOS与软件定时器有关的配置选项                                                */
/***************************************************************************************************************/
#define configUSE_TIMERS                          1 /* 1: 使能软件定时器, 默认: 0 */
#define configTIMER_TASK_PRIORITY                 ( configMAX_PRIORITIES - 1 ) /* 定义软件定时器任务的优先级, 无默认configUSE_TIMERS为1时需定义 */
#define configTIMER_QUEUE_LENGTH                  5 /* 定义软件定时器命令队列的长度, 无默认configUSE_TIMERS为1时需定义 */
#define configTIMER_TASK_STACK_DEPTH              ( configMINIMAL_STACK_SIZE * 2) /* 定义软件定时器任务的栈空间大小, 无默认configUSE_TIMERS为1时需定义 */

/***************************************************************************************************************/
/*                                FreeRTOS可选函数配置选项                                                        */
/***************************************************************************************************************/
#define INCLUDE_vTaskPrioritySet                  1 /* 设置任务优先级 */
#define INCLUDE_uxTaskPriorityGet                 1 /* 获取任务优先级 */
#define INCLUDE_vTaskDelete                       1 /* 删除任务 */
#define INCLUDE_vTaskCleanUpResources             0
#define INCLUDE_vTaskSuspend                      1 /* 挂起任务 */
#define INCLUDE_vTaskDelayUntil                   1 /* 任务绝对延时 */
#define INCLUDE_vTaskDelay                        1 /* 任务延时 */
#define INCLUDE_xTaskGetSchedulerState            1 /* 获取任务调度器状态 */
#define INCLUDE_xTimerPendFunctionCall            1 /* 将函数的执行挂到定时器服务任务 */
#define INCLUDE_eTaskGetState                     1 /* 获取任务状态 */

/***************************************************************************************************************/
/*                         FreeRTOS与中断服务函数有关的配置选项                                                     */
/***************************************************************************************************************/
/* Cortex-M specific definitions.中断嵌套行为配置 */
#ifdef __NVIC_PRIO_BITS
 /* __BVIC_PRIO_BITS will be specified when CMSIS is being used. */
 #define configPRIO_BITS                          __NVIC_PRIO_BITS
#else
 #define configPRIO_BITS                          4
#endif
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY       15 /* 中断最低优先级 */
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY  5  /* FreeRTOS可管理的最高中断优先级 */
#define configKERNEL_INTERRUPT_PRIORITY                ( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) ) /* 内核中断的优先级 */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY           ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) ) /* FreeRTOS 可管理的最高中断优先级 */

#define vPortSVCHandler                               SVC_Handler /* 系统服务调用中断处理函数 */
#define xPortPendSVHandler                            PendSV_Handler /* 挂起服务调用中断处理函数 */
#define xPortSysTickHandler                           SysTick_Handler /* 系统滴答中断处理函数 */

/***************************************************************************************************************
 * Stream Buffer 配置
 ***************************************************************************************************************/
#define configUSE_STREAM_BUFFERS                      1   /* 启用流缓冲区功能 */

#endif /* FREERTOS_CONFIG_H */
