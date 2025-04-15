/**
 * @file drvHal.h
 * @brief 驱动层硬件抽象接口头文件
 * @author konbakuyomu
 * @date 2025-03-07
 * @details 定义驱动层与硬件抽象层的接口函数，实现底层驱动与上层应用的解耦
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* 头文件
 * -------------------------------------------------------------*/
#include "hc32_ll.h"

/* 函数声明
 * -------------------------------------------------------------*/
/**
 * @brief 注册驱动层硬件抽象接口到HAL层
 * @details 将驱动层实现的各功能接口注入到HAL抽象层中，实现硬件抽象
 * @return 无
 */
void HAL_injectDriverImplementation(void);

#ifdef __cplusplus
}
#endif
