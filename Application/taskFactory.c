/**
 * @file taskFactory.c
 * @brief 任务工厂源文件
 * @date 2025-04-15
 * @details 实现任务工厂功能，动态创建或者静态创建任务
 */

#include "globalConfig.h"

/* 静态变量
 * -------------------------------------------------------------*/
// 为Uart1接收任务定义静态TCB和堆栈
static StaticTask_t usart1TaskTCB;
static StackType_t usart1TaskStack[MEDIUM_TASK_STACK_SIZE];

// 为按键任务定义静态TCB和堆栈
static StaticTask_t buttonTaskTCB;
static StackType_t buttonTaskStack[SMALL_TASK_STACK_SIZE];

// 为通用控制任务定义静态TCB和堆栈
static StaticTask_t commonControlTaskTCB;
static StackType_t commonControlTaskStack[SMALL_TASK_STACK_SIZE];

/* 函数定义
 * -------------------------------------------------------------*/
/**
 * @brief 任务工厂初始化
 * @detail 动态创建任务
 */
void taskFactoryInit(void) { }

/**
 * @brief 任务工厂静态初始化
 * @detail 静态创建任务
 */
void taskFactoryInitStatic(void)
{
    // 创建USART1任务
    usart1TaskHandle = xTaskCreateStatic((TaskFunction_t)usart1Task,
                                         "USART1 Task",
                                         MEDIUM_TASK_STACK_SIZE,
                                         NULL,
                                         (UBaseType_t)APP_TASK_PRIORITY,
                                         usart1TaskStack,
                                         &usart1TaskTCB);

    // 创建按键任务
    buttonTaskHandle = xTaskCreateStatic((TaskFunction_t)buttonTask,
                                         "Button Task",
                                         SMALL_TASK_STACK_SIZE,
                                         NULL,
                                         (UBaseType_t)APP_TASK_PRIORITY,
                                         buttonTaskStack,
                                         &buttonTaskTCB);

    // 创建通用控制任务
    commonControlTaskHandle = xTaskCreateStatic((TaskFunction_t)commonControlTask,
                                                "Common Control Task",
                                                SMALL_TASK_STACK_SIZE,
                                                NULL,
                                                (UBaseType_t)APP_TASK_PRIORITY,
                                                commonControlTaskStack,
                                                &commonControlTaskTCB);
}
