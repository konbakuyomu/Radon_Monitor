/**
 * @file taskFactory.h
 * @brief 任务工厂头文件
 * @date 2025-02-13
 */
#ifndef __TASK_FACTORY_H__
#define __TASK_FACTORY_H__

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

#endif
