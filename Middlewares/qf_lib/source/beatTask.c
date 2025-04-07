/**
 * @file beatTask.c
 * @brief 定时任务库实现
 * @date 2023-10-22
 * @details 本文件实现了一个定时任务库，支持实时动态创建、删除任务，
 *          对任务可进行暂停、恢复、计时重置等操作，支持用户带入用户数据指针，
 *          任务支持指定运行次数，运行结束后自行销毁。
 */

#include "beatTask.h"
#if btask_compile_en

/* 全局变量
 * ----*/
/**
 * @var list_head
 * @brief 任务链表头指针
 */
static btask_datas_t* list_head = NULL;

/**
 * @var list_tail
 * @brief 任务链表尾指针
 */
static btask_datas_t* list_tail = NULL;

/**
 * @var delay_cb
 * @brief 延时回调函数
 */
static btask_cb_t delay_cb = NULL;

/**
 * @var delay_userdata
 * @brief 延时回调函数用户数据
 */
static btask_event_t delay_userdata;

/**
 * @var btask_delay_t
 * @brief 延时计数器
 */
static int32_t btask_delay_t = 0;

/**
 * @var _timer_get_count
 * @brief 获取定时器计数值的回调函数
 */
static uint32_t (*_timer_get_count)() = NULL;

/**
 * @var _count_mode
 * @brief 定时器计数模式
 */
static btask_timer_count_mode _count_mode;

/**
 * @var tic_count
 * @brief 心跳计数
 */
static uint32_t tic_count = 0;

/**
 * @var tic_flg
 * @brief 心跳标志
 */
static int tic_flg = 0;

/**
 * @var pass_time_ms
 * @brief 系统运行时间，单位ms
 */
static uint32_t pass_time_ms = 0;

#if btask_support_rtos
/**
 * @var mutex
 * @brief 互斥锁回调函数结构体
 */
static btask_mutex_cb_t mutex;

/**
 * @var del_get_key_flg
 * @brief 删除操作获取锁标志
 */
static uint8_t del_get_key_flg = 0;

/**
 * @brief 注册线程锁回调函数
 *
 * @param [in] cb 获取和释放互斥锁的回调函数结构体
 */
void btask_mutex_register(btask_mutex_cb_t* cb) { mutex = *cb; }
#endif

/**
 * @brief 通过名称查找任务
 *
 * @param [in] name 任务名称
 * @return btask_datas_t*
 *         - 返回找到的任务节点指针
 *         - NULL: 未找到
 */
static btask_datas_t* _find_task_handle(const char* name)
{
    btask_datas_t* move = list_head;
    if (list_head == NULL)
        return NULL;
    for (;;) {
        if (strcmp(move->name, name) == 0)
            return move;
        if (move->next == NULL)
            return NULL;
        move = move->next;
    }
}

/**
 * @brief 绑定定时器计数寄存器值读取函数
 *
 * @param [in] timer_get_count 获取定时器寄存器值的回调函数
 * @param [in] count_mode 定时器计数模式
 */
void btask_set_timer_count_cb(uint32_t (*timer_get_count)(),
                              btask_timer_count_mode count_mode)
{
    _timer_get_count = timer_get_count;
    _count_mode = count_mode;
}

/**
 * @brief 注销定时器计数寄存器值读取函数
 */
void btask_del_timer_count_cb() { _timer_get_count = NULL; }

/**
 * @brief 注册在延时阻塞delay时的循环回调函数
 *
 * @param [in] cb 回调函数
 * @param [in] userdata 指向用户数据
 */
void btask_set_delay_cb(btask_cb_t cb, void* userdata)
{
    delay_cb = cb;
    delay_userdata.handle = NULL;
    delay_userdata.userdata = userdata;
}

/**
 * @brief 注销在延时阻塞delay时的循环回调函数
 */
void btask_del_delay_cb() { delay_cb = NULL; }

/**
 * @brief 注册定时多少ms执行一次任务
 *
 * @param [in] million ms数
 * @param [in] cb 回调函数
 * @param [in] count 执行次数
 * @param [in] name 任务名称
 * @param [in] userdata 用户数据
 * @return btask_handle_t 任务句柄
 */
btask_handle_t btask_creat_ms(
    uint32_t million, btask_cb_t cb, int16_t count, const char* name, void* userdata)
{
    btask_datas_t* tmp = btask_malloc(sizeof(btask_datas_t));

    if (tmp == NULL)
        return NULL;

#if btask_support_rtos
    mutex.mutex_get_cb();
#endif

    if (list_tail != NULL) {
        list_tail->next = tmp;
        tmp->last = list_tail;
    }
    if (list_head == NULL)
        list_head = tmp;

    list_tail = tmp;
    tmp->_busy = 0;
    tmp->name = name;
    tmp->next = NULL;
    tmp->del_cb = NULL;
    tmp->tick_p = cb;                        // 注册回调函数
    tmp->tick_t_count = 0;                   // 更新计数
    tmp->tick_t_count_max = million;         // 更新定时
    tmp->tick_t_mode_flg = count;            // 更新次数
    tmp->userdata.handle = (const void*)tmp; // 获取句柄地址
    tmp->userdata.userdata = userdata;       // 更新用户指针
    tmp->del_userdata.handle = tmp->userdata.handle;
    tmp->del_userdata.userdata = NULL;

#if btask_support_rtos
    mutex.mutex_give_cb();
#endif
    return tmp->userdata.handle; // 返回句柄
}

/**
 * @brief 注册定时多少s执行一次任务
 *
 * @param [in] second 秒数
 * @param [in] cb 回调函数
 * @param [in] count 执行次数
 * @param [in] name 任务名称
 * @param [in] userdata 用户数据
 * @return btask_handle_t 任务句柄
 */
btask_handle_t btask_creat(
    uint16_t second, btask_cb_t cb, int16_t count, const char* name, void* userdata)
{
    return btask_creat_ms((uint32_t)second * 1000, cb, count, name, userdata);
}

/**
 * @brief 注册任务销毁后的回调函数
 *
 * @param [in] handle 任务句柄
 * @param [in] name 任务名称
 * @param [in] cb 回调函数
 * @param [in] userdata 用户数据
 */
void btask_set_del_cb(btask_handle_t handle,
                      const char* name,
                      btask_cb_t cb,
                      void* userdata)
{
    btask_datas_t* tmp;
#if btask_support_rtos
    mutex.mutex_get_cb();
#endif
    if (handle == NULL) {
        tmp = _find_task_handle(name);
        if (tmp == NULL) {
#if btask_support_rtos
            mutex.mutex_give_cb();
#endif
            return;
        }
    } else
        tmp = (btask_datas_t*)handle;
    tmp->del_cb = cb;
    tmp->del_userdata.userdata = userdata;
#if btask_support_rtos
    mutex.mutex_give_cb();
#endif
}

/**
 * @brief 检查是否存在任务
 *
 * @param [in] handle 任务句柄
 * @param [in] name 任务名称
 * @return uint8_t
 *         - 1: 存在任务
 *         - 0: 无此任务
 */
uint8_t btask_has_task(btask_handle_t handle, const char* name)
{
    if (list_head == NULL)
        return 0;
    if (handle == NULL) {
        if (_find_task_handle(name) == NULL)
            return 0;
        else
            return 1;
    } else {
        btask_datas_t* move = list_head;
        btask_datas_t* tmp = (btask_datas_t*)handle;
        for (;;) {
            if (move == tmp)
                return 1;
            if (move->next == NULL)
                return 0;
            move = move->next;
        }
    }
}

/**
 * @brief 注销任务登记
 *
 * @param [in] handle 任务句柄
 * @param [in] name 任务名称
 */
void btask_del(btask_handle_t handle, const char* name)
{
    btask_datas_t* tmp;
    btask_datas_t* move;

#if btask_support_rtos
    if (del_get_key_flg == 0)
        mutex.mutex_get_cb();
#endif

    if (handle == NULL) {
        tmp = _find_task_handle(name);
        if (tmp == NULL) {
#if btask_support_rtos
            if (del_get_key_flg == 0)
                mutex.mutex_give_cb();
#endif
            return;
        }
    } else
        tmp = (btask_datas_t*)handle;

    if (tmp->del_cb != NULL)
        tmp->del_cb(&tmp->del_userdata);

    if (tmp == list_head) {
        if (list_head->next == NULL) {
            move = NULL;
            list_tail = NULL;
        } else {
            move = list_head->next;
            move->last = NULL;
        }

        btask_free(list_head);
        list_head = move;
    } else if (tmp == list_tail) {
        if (list_tail->last == NULL) {
            move = NULL;
            list_head = NULL;
        } else {
            move = list_tail->last;
            move->next = NULL;
        }
        btask_free(list_tail);
        list_tail = move;
    } else {
        btask_datas_t* _next;
        move = tmp->last;
        _next = tmp->next;
        move->next = tmp->next;
        _next->last = move;
        btask_free(tmp);
    }
#if btask_support_rtos
    if (del_get_key_flg == 0)
        mutex.mutex_give_cb();
#endif
}

/**
 * @brief 暂停任务
 *
 * @param [in] handle 任务句柄
 * @param [in] name 任务名称
 */
void btask_pause(btask_handle_t handle, const char* name)
{
    btask_datas_t* tmp;
#if btask_support_rtos
    mutex.mutex_get_cb();
#endif
    if (handle == NULL) {
        tmp = _find_task_handle(name);
        if (tmp == NULL) {
#if btask_support_rtos
            mutex.mutex_give_cb();
#endif
            return;
        }
    } else
        tmp = (btask_datas_t*)handle;
    if (tmp->tick_t_mode_flg < 0) // 已经暂停
    {
#if btask_support_rtos
        mutex.mutex_give_cb();
#endif
        return;
    }
#if btask_support_rtos
    mutex.mutex_give_cb();
#endif
    tmp->tick_t_mode_flg = -tmp->tick_t_mode_flg - 1; //-1 -- -32768
}

/**
 * @brief 恢复任务
 *
 * @param [in] handle 任务句柄
 * @param [in] name 任务名称
 */
void btask_resume(btask_handle_t handle, const char* name)
{
    btask_datas_t* tmp;
#if btask_support_rtos
    mutex.mutex_get_cb();
#endif
    if (handle == NULL) {
        tmp = _find_task_handle(name);
        if (tmp == NULL) {
#if btask_support_rtos
            mutex.mutex_give_cb();
#endif
            return;
        }
    } else
        tmp = (btask_datas_t*)handle;
    if (tmp->tick_t_mode_flg >= 0) // 已经恢复
    {
#if btask_support_rtos
        mutex.mutex_give_cb();
#endif
        return;
    }
#if btask_support_rtos
    mutex.mutex_give_cb();
#endif
    tmp->tick_t_mode_flg = -tmp->tick_t_mode_flg - 1; //-1 -- -32768
}

/**
 * @brief 使指定任务就绪
 *
 * @param [in] handle 任务句柄
 * @param [in] name 任务名称
 * @param [in] clr_count 是否清除任务计数值
 */
void btask_ready(btask_handle_t handle, const char* name, uint8_t clr_count)
{
    btask_datas_t* tmp;
#if btask_support_rtos
    mutex.mutex_get_cb();
#endif
    if (handle == NULL) {
        tmp = _find_task_handle(name);
        if (tmp == NULL) {
#if btask_support_rtos
            mutex.mutex_give_cb();
#endif
            return;
        }
    } else
        tmp = (btask_datas_t*)handle;

    if (clr_count == 0)
        tmp->tick_t_count += tmp->tick_t_count_max;
    else
        tmp->tick_t_count = tmp->tick_t_count_max;

#if btask_support_rtos
    mutex.mutex_give_cb();
#endif
}

/**
 * @brief 重置任务计时
 *
 * @param [in] handle 任务句柄
 * @param [in] name 任务名称
 */
void btask_reset(btask_handle_t handle, const char* name)
{
    btask_datas_t* tmp;
#if btask_support_rtos
    mutex.mutex_get_cb();
#endif
    if (handle == NULL) {
        tmp = _find_task_handle(name);
        if (tmp == NULL) {
#if btask_support_rtos
            mutex.mutex_give_cb();
#endif
            return;
        }
    } else
        tmp = (btask_datas_t*)handle;
    tmp->tick_t_count = 0;
#if btask_support_rtos
    mutex.mutex_give_cb();
#endif
}

/**
 * @brief 改变任务计时
 *
 * @param [in] handle 任务句柄
 * @param [in] name 任务名称
 * @param [in] ms 更改后的计时时长ms
 */
void btask_set_time(btask_handle_t handle, const char* name, uint32_t ms)
{
    btask_datas_t* tmp;
#if btask_support_rtos
    mutex.mutex_get_cb();
#endif
    if (handle == NULL) {
        tmp = _find_task_handle(name);
        if (tmp == NULL) {
#if btask_support_rtos
            mutex.mutex_give_cb();
#endif
            return;
        }
    } else
        tmp = (btask_datas_t*)handle;
    tmp->tick_t_count = 0;
    tmp->tick_t_count_max = ms;
#if btask_support_rtos
    mutex.mutex_give_cb();
#endif
}

/**
 * @brief 更改任务次数
 *
 * @param [in] handle 任务句柄
 * @param [in] name 任务名称
 * @param [in] count 执行次数
 */
void btask_set_count(btask_handle_t handle, const char* name, int16_t count)
{
    btask_datas_t* tmp;
#if btask_support_rtos
    mutex.mutex_get_cb();
#endif
    if (handle == NULL) {
        tmp = _find_task_handle(name);
        if (tmp == NULL) {
#if btask_support_rtos
            mutex.mutex_give_cb();
#endif
            return;
        }
    } else
        tmp = (btask_datas_t*)handle;
    if (count < 0) {
#if btask_support_rtos
        mutex.mutex_give_cb();
#endif
        return;
    }
    tmp->tick_t_mode_flg = count;
#if btask_support_rtos
    mutex.mutex_give_cb();
#endif
}

/**
 * @brief 获取任务剩余次数
 *
 * @param [in] handle 任务句柄
 * @param [in] name 任务名称
 * @return int16_t 剩余次数
 */
int16_t btask_get_count(btask_handle_t handle, const char* name)
{
    btask_datas_t* tmp;
#if btask_support_rtos
    mutex.mutex_get_cb();
#endif
    if (handle == NULL) {
        tmp = _find_task_handle(name);
        if (tmp == NULL) {
#if btask_support_rtos
            mutex.mutex_give_cb();
#endif
            return -1;
        }
    } else
        tmp = (btask_datas_t*)handle;

#if btask_support_rtos
    mutex.mutex_give_cb();
#endif

    return tmp->tick_t_mode_flg;
}

/**
 * @brief 提供心跳
 *
 * @param [in] _ms 心跳间隔ms
 */
void btask_tick_inc(int _ms)
{
    tic_count += _ms;
    pass_time_ms += _ms;
    tic_flg = 1;
    btask_delay_t -= _ms;
}

/**
 * @brief 任务执行函数
 */
void btask_handler()
{
    uint32_t _tic_count;

    btask_datas_t* node = list_head;
    btask_datas_t* next = NULL;

    if (tic_flg == 0) {
        return;
    }

    _tic_count = tic_count;
    tic_count = 0;
    tic_flg = 0;

    if (list_head == NULL) {
        return;
    }

#if btask_support_rtos
    mutex.mutex_get_cb();
    del_get_key_flg = 1;
#endif

    for (;;) {

        node->tick_t_count += _tic_count;
        // 注册了
        if (node->tick_t_count < node->tick_t_count_max) // 定时未到
            goto chek_null;

        node->tick_t_count -= node->tick_t_count_max; // 定时到了减去一次计时

        if (node->_busy != 1)                         // 防止无限递归
        {
            node->_busy = 1;
            node->tick_p(&node->userdata); // 调用回调
            node->_busy = 0;
        }

        if (node->tick_t_mode_flg == btask_infinite)
            goto chek_null;

        // 不是无限次执行
        node->tick_t_mode_flg--; // 次数减1
        if (node->tick_t_mode_flg != 0)
            goto chek_null;

        if (node->next == NULL) {

            btask_del((btask_handle_t)node, NULL);
            break;
        }

        next = node->next;

        btask_del((btask_handle_t)node, NULL);

    chek_null:

        if (next != NULL) {
            node = next;
            next = NULL;
            continue;
        }

        if (node->next == NULL)
            break;
        node = node->next;
    }
#if btask_support_rtos
    mutex.mutex_give_cb();
    del_get_key_flg = 0;
#endif
}

/**
 * @brief ms延时
 *
 * @param [in] ms 延时时间
 */
void btask_delay(uint32_t ms)
{
    uint32_t timer_count = 0;
    if (_timer_get_count != NULL)
        timer_count = _timer_get_count();
    btask_delay_t = ms;
    while (btask_delay_t > 0) {
        btask_handler();
        if (delay_cb != NULL)
            delay_cb(&delay_userdata);
    }
    if (_timer_get_count == NULL)
        return;
    if (_count_mode == count_down) {
        while (_timer_get_count() > timer_count) {
            btask_handler();
            if (delay_cb != NULL)
                delay_cb(&delay_userdata);
        }
        return;
    }
    while (_timer_get_count() < timer_count) {
        btask_handler();
        if (delay_cb != NULL)
            delay_cb(&delay_userdata);
    }
}

/**
 * @brief ms延时，不执行任务
 *
 * @param [in] ms 延时时间
 */
void btask_delay_without_task(uint32_t ms)
{
    uint32_t timer_count = 0;
    if (_timer_get_count != NULL)
        timer_count = _timer_get_count();
    btask_delay_t = ms;
    while (btask_delay_t > 0) {
        if (delay_cb != NULL)
            delay_cb(&delay_userdata);
    }
    if (_timer_get_count == NULL)
        return;
    if (_count_mode == count_down) {
        while (_timer_get_count() > timer_count) {
            if (delay_cb != NULL)
                delay_cb(&delay_userdata);
        }
        return;
    }
    while (_timer_get_count() < timer_count) {
        if (delay_cb != NULL)
            delay_cb(&delay_userdata);
    }
}

/**
 * @brief 获取开机后的系统运行时间
 *
 * @return uint32_t 系统运行时间，单位ms
 */
uint32_t btask_get_time_ms() { return pass_time_ms; }

#endif
