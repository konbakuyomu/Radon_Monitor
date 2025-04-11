/**
 * @file cmd_processor_c.c
 * @brief 纯C命令处理器实现，支持动态注册/注销/分发消息处理函数
 * @date 2025-04-11
 */

#include "cmd_processor_c.h"
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"

typedef struct {
    MsgBusMessageType msg_type;
    CmdHandlerC handler;
} CmdProcEntry;

static CmdProcEntry handler_table[CMD_PROC_MAX_TYPE];

void cmd_processor_clear(void)
{
    taskENTER_CRITICAL();
    memset(handler_table, 0, sizeof(handler_table));
    taskEXIT_CRITICAL();
}

bool cmd_processor_register(MsgBusMessageType msg_type, CmdHandlerC handler)
{
    if (handler == NULL) return false;
    taskENTER_CRITICAL();
    for (int i = 0; i < CMD_PROC_MAX_TYPE; ++i) {
        if (handler_table[i].handler == NULL) {
            handler_table[i].msg_type = msg_type;
            handler_table[i].handler = handler;
            taskEXIT_CRITICAL();
            return true;
        }
        // 已注册则覆盖
        if (handler_table[i].msg_type == msg_type) {
            handler_table[i].handler = handler;
            taskEXIT_CRITICAL();
            return true;
        }
    }
    taskEXIT_CRITICAL();
    return false;
}

bool cmd_processor_unregister(MsgBusMessageType msg_type)
{
    taskENTER_CRITICAL();
    for (int i = 0; i < CMD_PROC_MAX_TYPE; ++i) {
        if (handler_table[i].handler && handler_table[i].msg_type == msg_type) {
            handler_table[i].handler = NULL;
            handler_table[i].msg_type = 0;
            taskEXIT_CRITICAL();
            return true;
        }
    }
    taskEXIT_CRITICAL();
    return false;
}

bool cmd_processor_execute(const MsgBusSystemMessage* msg)
{
    if (!msg) return false;
    for (int i = 0; i < CMD_PROC_MAX_TYPE; ++i) {
        if (handler_table[i].handler && handler_table[i].msg_type == msg->message) {
            handler_table[i].handler(msg);
            return true;
        }
    }
    return false;
}

/* 用法示例（在 commonControl_c.c 中）：
#include "cmd_processor_c.h"

static void handleLedControl(const MsgBusSystemMessage* msg) {
    // 处理LED控制
}

void commonControlTask_c(void* pvParameters) {
    ...
    cmd_processor_register(MSGBUS_MSG_LED_CONTROL, handleLedControl);
    ...
    for (;;) {
        if (msgbus_wait_for_message(..., &msg)) {
            cmd_processor_execute(&msg);
        }
    }
}
*/