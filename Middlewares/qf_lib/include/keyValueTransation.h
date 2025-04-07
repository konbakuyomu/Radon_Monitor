/**
 * @file keyValueTransation.h
 * @brief 简单键值对数据交互库头文件
 * @date 2023-10-21
 * @details 本文件定义了一个简单的键值对数据交互库的接口，支持发布-订阅模式的消息传递。
 *          支持在RTOS环境下使用，提供线程安全机制。
 */

#ifndef _key_value_transation_H__
#define _key_value_transation_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
    Chinese is encoded in Unicode. If it is garbled, please change the encoding
   method of the editor. 简单键值对数据交互库 By启凡科创 version:v1.0.2  2023-10-21

    注意：不同API内，如msg触发的回调函数内不能调用del，可以利用创建定时任务在指定时间后再del，同时独立调用是允许的，不能嵌套！！！
        同时，msg原理上支持msg的回调函数里再msg实现递归，但是原则上不能这么做，可能会导致宕机

    1.0.2:
        支持RTOS添加线程锁，防止宕机

    1.0.1:
        修复删除句柄指针跑飞问题
*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 宏定义
 * ----*/
/**
 * @defgroup KeyValue_Config 键值对库配置宏
 * @{
 */
/** @brief 编译开关，1表示启用库，0表示禁用 */
#define key_value_transation_compile_en 1

/** @brief 内存分配函数 */
#define key_value_malloc_func(x) malloc(x)
/** @brief 内存释放函数 */
#define key_value_free_func(x)   free(x)

/** @brief 是否支持多线程访问，1表示支持，0表示不支持 */
#define key_value_support_rtos 1
/**
 * @}
 */

#if key_value_transation_compile_en

/* 类型定义
 * ----*/
/**
 * @typedef key_value_cb_t
 * @brief 键值对回调函数类型定义
 * 
 * @param value [in] 值数据指针
 * @param lenth [in] 数据长度（字节）
 */
typedef void (*key_value_cb_t)(void* value, size_t lenth);

/**
 * @typedef key_value_register_t
 * @brief 键值对注册节点结构体
 */
typedef struct _key_value_register_t {
    key_value_cb_t _cb;                    /**< 回调函数 */
    const char* key;                       /**< 键名 */
    struct _key_value_register_t* next;    /**< 下一个节点指针 */
    struct _key_value_register_t* last;    /**< 上一个节点指针 */
    int key_sum;                           /**< 键名字符和 */
} key_value_register_t;

#if key_value_support_rtos
/**
 * @typedef key_value_mutex_cb_t
 * @brief RTOS互斥锁回调函数结构体
 */
typedef struct {
    void (*mutex_get_cb)();    /**< 获取互斥锁回调函数 */
    void (*mutex_give_cb)();   /**< 释放互斥锁回调函数 */
} key_value_mutex_cb_t;
#endif

/**
 * @typedef key_value_handle_t
 * @brief 键值对句柄类型定义
 */
typedef key_value_register_t* key_value_handle_t;

/* 函数声明
 * ----*/
#if key_value_support_rtos
/**
 * @brief 注册线程锁回调函数
 *
 * @param [in] cb 获取和释放互斥锁的回调函数结构体
 */
void key_value_mutex_register(key_value_mutex_cb_t* cb);
#endif

/**
 * @brief 注册(订阅)有指定键时的回调函数
 *
 * @param [out] handle 用于接收返回句柄
 * @param [in] key 键名
 * @param [in] cb 回调函数
 * @return int 
 *         - 0: 成功
 *         - 1: 键值对内存申请失败
 *         - 2: 键名内存申请失败
 * @note 注册后的句柄可用于后续删除操作
 */
int key_value_register(key_value_handle_t* handle,
                       const char* key,
                       key_value_cb_t cb);

/**
 * @brief 删除已注册的键
 *
 * @param [in] handle 注册的句柄
 * @return int 
 *         - 0: 成功
 *         - 1: 不存在句柄
 *         - 2: 空间不足（仅在RTOS模式下可能返回）
 * @warning 不同API内，如msg触发的回调函数内不能直接调用del，可以利用创建定时任务在指定时间后再del
 */
int key_value_del(key_value_handle_t handle);

/**
 * @brief 创建(发布)一条键值对通知
 *
 * @param [in] key 键名，可以是动态的
 * @param [in] value 指向数据，可以是动态的
 * @param [in] lenth 数据长度，字节
 * @return int 
 *         - 0: 成功
 *         - 1: 无注册键
 * @note msg原理上支持在回调函数里再次调用msg实现递归，但原则上不建议这么做，可能导致宕机
 */
int key_value_msg(const char* key, void* value, size_t lenth);

#endif

#ifdef __cplusplus
}
#endif
#endif
