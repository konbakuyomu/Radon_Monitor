/**
 * @file taskFactory.h
 * @brief 任务工厂头文件
 * @date 2025-04-15
 * @details 实现任务工厂功能，动态创建或者静态创建任务
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* 头文件
 * -------------------------------------------------------------*/
#include <stdio.h>

/* 函数声明
 * -------------------------------------------------------------*/
void taskFactoryInit(void);
void taskFactoryInitStatic(void);

#ifdef __cplusplus
}
#endif
