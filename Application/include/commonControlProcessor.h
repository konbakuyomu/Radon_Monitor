/**
 * @file commonControlProcessor.h
 * @brief 通用控制命令处理器头文件
 * @date 2025-04-11
 * @details 提供针对通用控制事件的命令处理器，包括LED控制等功能
 */

#pragma once

/* 头文件
 * -------------------------------------------------------------*/
#include "baseProcessor.h"

/**
 * @defgroup Common_Control_Processor 通用控制处理器模块
 * @brief 通用控制事件处理相关定义
 * @{
 */

/* 结构体
 * -------------------------------------------------------------*/
/**
 * @brief 通用控制命令处理器结构体（继承自基类）
 *        用于处理LED控制等通用事件消息
 */
typedef struct {
    CommandProcessor commandProcessor; /**< 命令处理器结构体，继承命令处理器功能 */
    // 可扩展其他成员
} DeviceControlProcessor;

/* 函数声明
 * -------------------------------------------------------------*/
/**
 * @brief 初始化通用控制命令处理器
 * @param [in,out] processor 指向处理器结构体的指针
 * @param [in] configuration 指向配置结构体的指针
 * @return 无返回值
 * @note 必须在使用前调用此函数进行初始化
 */
void DeviceControlProcessor_initialize(DeviceControlProcessor* processor,
                                       const CommandProcessorConfiguration* configuration);

/**
 * @brief 获取通用控制命令处理器单例（全局唯一实例）
 * @return 指向单例的指针
 */
DeviceControlProcessor* DeviceControlProcessor_getInstance(void);

/* 宏定义
 * -------------------------------------------------------------*/
/**
 * @brief 用于构建LED控制命令类型的宏定义
 * @details 将LED控制类型和LED选择组合成一个32位命令类型
 */
#define CREATE_LED_CONTROL_COMMAND(controlType, ledSelection) \
    (((uint32_t)(controlType) << 16) | (ledSelection))

/**
 * @}
 */
