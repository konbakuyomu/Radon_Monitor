#ifndef CMD_PROCESSOR_C_H
#define CMD_PROCESSOR_C_H

#include <stdint.h>
#include <stdbool.h>
#include "message_bus.h" // 需包含MsgBusSystemMessage定义

#ifdef __cplusplus
extern "C" {
#endif

#define CMD_PROC_MAX_TYPE 32

typedef void (*CmdHandlerC)(const MsgBusSystemMessage* msg);

/**
 * @brief 注册命令处理函数
 * @param msg_type 消息类型
 * @param handler 处理函数指针
 * @return true 注册成功，false 失败（如表满）
 */
bool cmd_processor_register(MsgBusMessageType msg_type, CmdHandlerC handler);

/**
 * @brief 注销命令处理函数
 * @param msg_type 消息类型
 * @return true 注销成功，false 未找到
 */
bool cmd_processor_unregister(MsgBusMessageType msg_type);

/**
 * @brief 执行命令处理（分发消息到对应handler）
 * @param msg 消息指针
 * @return true 找到并执行了handler，false 未注册
 */
bool cmd_processor_execute(const MsgBusSystemMessage* msg);

/**
 * @brief 清空所有注册的handler（可选）
 */
void cmd_processor_clear(void);

#ifdef __cplusplus
}
#endif

#endif // CMD_PROCESSOR_C_H