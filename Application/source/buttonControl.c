/**
 * @file buttonControl.cpp
 * @brief 按键控制任务处理实现文件
 * @date 2025-03-15
 * @details 实现按键任务函数，处理按键输入和LED控制功能
 */

/* 头文件
 * -------------------------------------------------------------*/
#include "globalConfig.h"
#include "buttonControl.h"

/* 函数实现
 * -------------------------------------------------------------*/

/**
 * @var TaskHandle_t buttonTaskHandle
 * @brief 按键任务句柄，用于控制和引用按键处理任务
 */
TaskHandle_t buttonTaskHandle = NULL;

/**
 * @brief 按键处理任务函数，处理按键输入和LED控制
 * @param [in] pvParameters FreeRTOS任务参数
 *        This parameter is not used in this task
 * @return 无返回值
 * @note 任务会一直运行，等待并处理按键输入事件
 */
void buttonTask(void* pvParameters)
{
    // 防止未使用参数警告
    (void)pvParameters;

    // 按键事件
    btn_event_t ret;
    // 按键编号
    uint8_t keyNumber;

    for (;;) {
        // 按键测试
        if (btn_available() != 0) {
            btn_read_event(&keyNumber, &ret);

            switch (ret) {
            case btn_click:
                if (keyNumber == KEY_BUTTON_1)
                    CommonDriver_LedToggle(LED_STATUS);
                else if (keyNumber == KEY_BUTTON_2)
                    CommonDriver_LedToggle(LED_NETWORK);
                else if (keyNumber == KEY_BUTTON_3)
                    CommonDriver_LedToggle(LED_FAULT);
                else if (keyNumber == KEY_BUTTON_4)
                    CommonDriver_LedToggle(LED_ALARM);
                break;
            default:
                break;
            }
        }

        HAL_delayMillis(10);
    }
}
