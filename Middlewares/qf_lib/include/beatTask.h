/**
 * @file beatTask.h
 * @brief 定时任务库头文件
 * @date 2023-10-22
 * @details 本文件定义了一个定时任务库的接口，支持实时动态创建、删除任务，
 *          对任务可进行暂停、恢复、计时重置等操作，支持用户带入用户数据指针，
 *          任务支持指定运行次数，运行结束后自行销毁。
 */

#ifndef __beat_task_H__
#define __beat_task_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
    Chinese is encoded in Unicode. If it is garbled, please change the encoding
   method of the editor. 定时任务库 By启凡科创 适用于任意C/C++平台MCU

    功能：
    1）实时动态创建、删除任务
    2）对任务可进行：暂停、恢复、计时重置的操作
    3）支持用户带入用户数据指针
    4）任务支持指定运行次数，运行结束后自行销毁

    移植步骤：
    1）确保malloc函数可申请的内存池大小足够，1个任务使用内存大概为72字节
    2）在当前平台MCU中配置好中断在ms等级的定时器，将
   btask_tick_inc()函数放置于定时器的中断函数里提供心跳 3）将
   btask_handler()接口放置于主函数main的死循环下进行循环扫描，或将
   btask_delay()接口放进去也是一样的，区别在于有没有延时，
        btask_delay时也在循环扫描btask_handler(),所以在使用delay时可不用btask_handler;

    至此，你已经完成了beat_task库的移植

    使用步骤:
    1）使用creat相关接口将要定时执行的子函数注册任务，定时到达后将自动执行
    2）使用del可注销指定任务,指定了次数的任务可中途销毁也可任其执行完次数后自动销毁
    3）使用creat注册成功后，会返回一个类型为btask_handle_t的句柄，后续对任务的操作都将采用此句柄，也可使用注册时给定的任务名字进行操作
        如需对任务进行干预，请务必创建好一个静态局部句柄或全局句柄用于保存任务对象，或给任务取好名字

    注意：任务内尽量不要阻塞（如大量的串口发送等任务），耗时短的任务才是应该采用beat_task处理的
        请确保内存池可用内存充足
        任务的名字字符串不能是动态的，需要是静态的

    一些小细节：
        使用句柄管理任务：效率相对于使用字符串管理效率更高，但是跨文件管理任务需要传递句柄，不跨文件时推荐使用句柄进行管理
        注意：任务回调函数内不能调用删除任务！！
        同时，不支持任务的回调函数里再调用任务函数递归，调用将无效

    beatTask 库在以下场景中比 FreeRTOS 原生机制更适合：
    1）资源极度受限的微控制器，无法承担完整 RTOS 开销
    2）简单的嵌入式系统，主要需要定时执行短小任务
    3）作为 FreeRTOS 的补充，处理大量简单的定时事件
    4）需要频繁创建和销毁定时任务的场景
    5）裸机系统需要简单的定时任务管理
    但对于需要复杂任务调度、严格实时性保证、任务间复杂同步的系统，FreeRTOS
   的原生机制更为合适。 这个库的价值在于它提供了一种介于裸机轮询和完整 RTOS
   之间的中间解决方案，为特定应用场景提供了更轻量、更简单的选择

    version:

    v1.0.2  2023-10-22
    更改支持RTOS多线程的方式，需使用回调函数注册
    修复删除任务可能会导致死机的问题

    v1.0.1  2023-6-20
*/

// #include "LIB_CONFIG.h" //移植删除
// #include "freertos/FreeRTOS.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 宏定义
 * ----*/
/**
 * @defgroup BeatTask_Config 定时任务库配置宏
 * @{
 */
#ifndef btask_compile_en   // LIB_CONFIG配置
#define btask_compile_en 1 // 1:启用库编译，0：不编译库
#endif

// 内存管理函数宏
#define btask_malloc(x) malloc(x) // 申请内存函数
#define btask_free(x)   free(x)   // 释放内存函数

// 是（1）否（0）支持多线程访问,开启后在使用任意API前必须使用btask_mutex_register注册线程锁（仅第一次）
#define btask_support_rtos 1

#if btask_compile_en

/* 类型定义
 * ----*/
/**
 * @typedef btask_handle_t
 * @brief 定时任务句柄类型
 */
typedef const void* btask_handle_t;

/**
 * @typedef btask_event_t
 * @brief 定时事件回调函数带入的数据指针指向的结构
 */
typedef struct {
    btask_handle_t handle; // 指向定时器句柄
    void* userdata;        // 用户数据
} btask_event_t;

#if btask_support_rtos
/**
 * @typedef btask_mutex_cb_t
 * @brief RTOS互斥锁回调函数结构体
 */
typedef struct {
    void (*mutex_get_cb)();
    void (*mutex_give_cb)();
} btask_mutex_cb_t;
#endif

/**
 * @typedef btask_cb_t
 * @brief 回调函数格式类型
 *
 * @param e [in] 指向回调结构体
 */
typedef void (*btask_cb_t)(btask_event_t* e); // 回调函数格式类型

/**
 * @typedef btask_datas_t
 * @brief 内部数据结构体
 */
typedef struct _btask_datas_t {
    btask_cb_t tick_p;
    btask_cb_t del_cb;
    btask_event_t userdata;
    btask_event_t del_userdata;
    uint32_t tick_t_count;
    uint32_t tick_t_count_max;
    int16_t tick_t_mode_flg;
    struct _btask_datas_t* next;
    struct _btask_datas_t* last;
    const char* name;
    uint8_t _busy;
} btask_datas_t;

/**
 * @typedef btask_timer_count_mode
 * @brief 定时器计数模式枚举
 */
typedef enum {
    count_down, // 减计数
    count_up    // 加计数
} btask_timer_count_mode;

/**
 * @defgroup BeatTask_Constants 定时任务常量
 * @{
 */
enum {
    btask_none = -1,
    btask_infinite = 0, // 无限次数
};
/**
 * @}
 */

#if btask_support_rtos
/**
 * @brief 注册线程锁回调函数
 *
 * @param cb 获取，归还钥匙的回调函数
 */
void btask_mutex_register(btask_mutex_cb_t* cb);
#endif

/**
 * @brief 注册在延时阻塞delay时的循环回调函数，如将喂狗等操作绑定防止阻塞导致重启
 *
 * @param call_back 回调函数，格式为void function(btask_event_t *e)，形参说明如下：
 * @param ··e 指向回调结构体，其内部成员包括：
 * @param ····handle 当前执行任务的句柄,当然，delay任务没有句柄，此参数无实际意义
 * @param ····userdata 指向注册时的用户数据，类型为void
 * *，类型自行转换为需要使用的类型
 * @param --
 * @param userdata 指向用户数据，未使用请填入NULL
 */
void btask_set_delay_cb(btask_cb_t call_back, void* userdata);

/**
 * @brief 注销在延时阻塞delay时的循环回调函数
 */
void btask_del_delay_cb(void);

/**
 * @brief
 * 注册定时多少ms执行一次任务,同一函数可进行多任务注册，可利用句柄在回调函数内进行区分
 *
 * @param million ms数，最大值2^31,不可为0
 * @param --
 * @param cb 回调函数，格式为void function(btask_event_t
 * *e)，不能为NULL，形参说明如下：
 * @param ··e 指向回调结构体，其内部成员包括：
 * @param ····handle 当前执行任务的定时器句柄
 * @param ····userdata 指向注册时的用户数据，类型为void
 * *，类型自行转换为需要使用的类型
 * @param --
 * @param count 执行多少次，0(btask_infinite)为无限次，不能为负数，最大指定32767次
 * @param --
 * @param name 字符串名字，可用于管理任务，不使用则填NULL以节省内存
 * @param --
 * @param userdata 指向用户数据，未使用请填入NULL
 *
 * @return btask_handle
 * --返回句柄，可用于判断是否注册成功和用于注销任务，为NULL则注册失败，任务数已达上限或其他错误
 */
btask_handle_t btask_creat_ms(uint32_t million,
                              btask_cb_t cb,
                              int16_t count,
                              const char* name,
                              void* userdata);

/**
 * @brief
 * 注册定时多少s执行一次任务,同一函数可进行多任务注册，可利用句柄在回调函数内进行区分
 *
 * @param second 秒数,最大值65535
 * @param --
 * @param callb 回调函数，格式为void function(btask_event_t *e)，形参说明如下：
 * @param ··e 指向回调结构体，其内部成员包括：
 * @param ····handle 当前执行任务的定时器句柄
 * @param ····userdata 指向注册时的用户数据，类型为void
 * *，类型自行转换为需要使用的类型
 * @param --
 * @param count
 * 执行多少次，0(btask_infinite)为无限次，不能为负数，为负数注册失败，最大指定32767次
 * @param --
 * @param name 字符串名字，可用于管理任务，不使用则填NULL以节省内存
 * @param --
 * @param userdata 指向用户数据，未使用请填入NULL
 *
 * @return btask_handle
 * --返回句柄，可用于判断是否注册成功和用于注销任务，为NULL则注册失败，任务数已达上限或其他错误
 */
btask_handle_t btask_creat(
    uint16_t second, btask_cb_t cb, int16_t count, const char* name, void* userdata);

/**
 * @brief 检查是否存在任务
 *
 * @param handle 注册时返回的任务句柄，使用名字定位任务填NULL
 * @param name 任务字符串名字，使用句柄定位任务填NULL
 * @return uint8_t 1：存在任务，0：无此任务
 */
uint8_t btask_has_task(btask_handle_t handle, const char* name);

/**
 * @brief 注册任务销毁后的回调函数
 *
 * @param handle 注册时返回的任务句柄，使用名字定位任务填NULL
 * @param name 任务字符串名字，使用句柄定位任务填NULL
 * @param cb 格式与任务的回调函数相同
 * @param userdata 指向用户数据，未使用请填入NULL
 */
void btask_set_del_cb(btask_handle_t handle,
                      const char* name,
                      btask_cb_t cb,
                      void* userdata);

/**
 * @brief
 * 绑定定时器计数寄存器值读取函数，绑定后可获得准确的短时间delay，不绑定精度将无法保证，如心跳提供1ms，则delay(1)可能在0到1ms之间
 *
 * @param timer_get_count 获取定时器寄存器值的回调函数
 * @param ··
 * @param count_mode 定时器计数模式
 * @param ··count_down 减计数
 * @param ··count_up 加计数
 */
void btask_set_timer_count_cb(uint32_t (*timer_get_count)(),
                              btask_timer_count_mode count_mode);

/**
 * @brief 注销定时器计数寄存器值读取函数
 */
void btask_del_timer_count_cb(void);

/**
 * @brief 使指定任务就绪，下一次调用btask_handler接口时将会直接运行一次任务
 *
 * @param handle 注册时返回的任务句柄，使用名字定位任务填NULL
 * @param --
 * @param name 任务字符串名字，使用句柄定位任务填NULL
 * @param --
 * @param clr_count 1:清除任务计数值，0：保留计数
 */
void btask_ready(btask_handle_t handle, const char* name, uint8_t clr_count);

/**
 * @brief 注销任务登记，任务将在下一次调用btask_handler接口时被注销
 *
 * @param handle 注册时返回的任务句柄，使用名字定位任务填NULL
 * @param name 任务字符串名字，使用句柄定位任务填NULL
 */
void btask_del(btask_handle_t handle, const char* name);

/**
 * @brief 暂停任务，参数二选一，另一个填入NULL
 *
 * @param handle 注册时返回的任务句柄
 * @param name 任务字符串名字
 */
void btask_pause(btask_handle_t handle, const char* name);

/**
 * @brief 恢复任务，参数二选一，另一个填入NULL
 *
 * @param handle 注册时返回的任务句柄
 * @param name 任务字符串名字
 */
void btask_resume(btask_handle_t handle, const char* name);

/**
 * @brief 重置任务计时，参数二选一，另一个填入NULL
 *
 * @param handle 注册时返回的任务句柄
 * @param name 任务字符串名字
 */
void btask_reset(btask_handle_t handle, const char* name);

/**
 * @brief   改变任务计时，会重置已经计的时，
 *          如任务A在创建时定时500ms执行一次，现调用此接口将500改为100ms，如更改时已经计时300ms，则会清空300ms从头计时，
 *          句柄和名字二选一，另一个填入NULL
 *
 * @param handle 注册时返回的任务句柄
 * @param name 任务字符串名字
 * @param ms 更改后的计时时长ms
 */
void btask_set_time(btask_handle_t handle, const char* name, uint32_t ms);

/**
 * @brief 更改任务次数，句柄和名字二选一，另一个填入NULL
 *
 * @param handle 注册时返回的任务句柄
 * @param name 任务字符串名字
 * @param count 执行次数，0为无数次，1-32767为指定次数
 */
void btask_set_count(btask_handle_t handle, const char* name, int16_t count);

/**
 * @brief
 * 获取任务剩余次数，0(btask_infinite)为无限次，-1(btask_none)为任务未注册，句柄和名字二选一，另一个填入NULL
 *
 * @param handle 注册时返回的任务句柄
 * @param name 任务字符串名字
 */
int16_t btask_get_count(btask_handle_t handle, const char* name);

/**
 * @brief 提供心态，放于配置好的的定时器中断下提供心跳节拍
 *
 * @param tick_ms 心跳多少ms一次就写多少,如1ms一次写1，
 * 建议提供标准1ms心跳，这样delay函数才能比较准确
 */
void btask_tick_inc(int tick_ms);

/**
 * @brief 任务执行函数，请放置于循环扫描的接口内
 */
void btask_handler(void);

/**
 * @brief ms延时,延时过程中会继续执行创建的任务
 *
 * @param ms 范围2^31,必须提供心跳后才能工作！
 */
void btask_delay(uint32_t ms);

/**
 * @brief ms延时,延时过程中断执行创建的任务
 *
 * @param ms 范围2^31,必须提供心跳后才能工作！
 */
void btask_delay_without_task(uint32_t ms);

/**
 * @brief 获取开机后的系统运行时间，单位ms
 *
 * @return uint32_t
 */
uint32_t btask_get_time_ms();

#endif
#ifdef __cplusplus
}
#endif
#endif
