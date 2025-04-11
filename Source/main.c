/**
 * @file main.c
 * @brief 主程序入口
 * @date 2025-04-11
 * @details 包含主函数和系统初始化，创建FreeRTOS任务，实现LED控制
 */

/* 头文件
 * -------------------------------------------------------------*/
#include "globalConfig.h"

/* 静态变量
 * -------------------------------------------------------------*/

/**
 * @var btask_handle_t task1_handle
 * @brief 任务1句柄，用于LED控制
 */
static btask_handle_t task1_handle;

/**
 * @var TaskHandle_t AppTaskCreate_Handle
 * @brief 任务创建句柄
 */
static TaskHandle_t AppTaskCreate_Handle = NULL;

/* 函数定义
 * -------------------------------------------------------------*/

/**
 * @brief 任务1回调函数
 * @param [in] e 任务事件指针
 */
void task1_callback(btask_event_t* e)
{
    // 发送测试数据
    // USART1Driver_SendTestDataMessage(0x123456);
}

/**
 * @brief 任务创建函数（仅在本文件内使用）
 * @details 创建所有应用任务，完成后删除自身
 */
static void AppTaskCreateProcess(void)
{
    // 进入临界区
    taskENTER_CRITICAL();

    // 创建任务
    taskFactoryInitStatic();

    // 退出临界区
    taskEXIT_CRITICAL();
    // 删除自身任务
    vTaskDelete(NULL);
}

/**
 * @brief 主函数
 * @details 初始化系统，创建任务，启动调度器
 * @return int 正常情况下不会返回
 * @note 正常情况下不会返回，会一直运行在FreeRTOS调度器中
 */
int main(void)
{
    // 初始化HAL硬件抽象层
    HAL_injectDrvHal();

    task1_handle = btask_creat_ms(1000,           // 周期为1000ms
                                  task1_callback, // 回调函数
                                  btask_infinite, // 无限循环
                                  "Task1",        // 任务名称
                                  NULL);          // 任务参数

    // 定义一个创建信息返回值，默认为pdPASS
    BaseType_t xReturn = pdPASS;

    // 创建初始任务
    xReturn = xTaskCreate((TaskFunction_t)AppTaskCreateProcess,
                          (const char*)"AppTaskCreate",
                          (uint16_t)INIT_TASK_STACK_SIZE,
                          (void*)NULL,
                          (UBaseType_t)INIT_TASK_PRIORITY,
                          (TaskHandle_t*)&AppTaskCreate_Handle);

    // 如果创建任务失败
    if (xReturn != pdPASS) {
        while (1) { }
    }

    // 启动调度器
    vTaskStartScheduler();

    for (;;) { }
}
