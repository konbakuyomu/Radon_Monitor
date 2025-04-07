/**
 * @file keyValueTransation.c
 * @brief 简单键值对数据交互库实现
 * @date 2023-10-21
 * @details 本文件实现了一个简单的键值对数据交互库，支持发布-订阅模式的消息传递。
 *          支持在RTOS环境下使用，提供线程安全机制。
 */

#include "keyValueTransation.h"

#if key_value_transation_compile_en

/* 全局变量
 * ----*/
/**
 * @var register_head
 * @brief 注册链表头指针
 */
static key_value_register_t* register_head = NULL;

/**
 * @var register_tail
 * @brief 注册链表尾指针
 */
static key_value_register_t* register_tail = NULL;

/* 函数声明
 * ----*/
/**
 * @brief 检查句柄是否已注册
 * 
 * @param [in] handle 要检查的句柄
 * @return key_value_register_t* 
 *         - NULL: 未找到句柄
 *         - 非NULL: 找到的节点指针
 */
static key_value_register_t* chek_has_register(key_value_handle_t handle);

#if key_value_support_rtos
/* RTOS支持相关定义
 * ----*/
/**
 * @typedef del_list_t
 * @brief 延迟删除列表节点结构体
 */
typedef struct del_list_s {
    key_value_handle_t handle;    /**< 要删除的句柄 */
    struct del_list_s* next;      /**< 下一个节点指针 */
} del_list_t;

/**
 * @var del_head
 * @brief 延迟删除链表头指针
 */
static del_list_t* del_head = NULL;

/**
 * @var del_tail
 * @brief 延迟删除链表尾指针
 */
static del_list_t* del_tail = NULL;

/**
 * @var mutex_cb
 * @brief 互斥锁回调函数结构体
 */
static key_value_mutex_cb_t mutex_cb;

/**
 * @var msg_busy
 * @brief 消息处理忙标志
 */
static uint8_t msg_busy = 0;

/**
 * @var del_busy
 * @brief 删除操作忙标志
 */
static uint8_t del_busy = 0;

/**
 * @brief 注册互斥锁回调函数
 * 
 * @param [in] cb 互斥锁回调函数结构体指针
 */
void key_value_mutex_register(key_value_mutex_cb_t* cb) { mutex_cb = *cb; }
#endif

/**
 * @brief 比较两个键名是否相同
 * 
 * @param [in] key1 第一个键名
 * @param [in] key2 第二个键名
 * @return int 
 *         - 0: 相同
 *         - 1: 不同
 */
static int key_cmp(const char* key1, const char* key2)
{
    for (; *key1 == *key2; ++key1, ++key2) {
        if (*key1 == '\0')
            return (0);
    }
    return 1;
}

/**
 * @brief 计算键名字符和
 * 
 * @param [in] key 键名
 * @return int 字符和
 */
static int get_key_sum(const char* key)
{
    int sum = 0;
    size_t len = strlen(key); // 先获取字符串长度
    size_t i;                 // 使用size_t类型的循环变量

    for (i = 0; i < len; i++) {
        sum += key[i];
    }
    return sum;
}

/**
 * @brief 检查句柄是否已注册
 * 
 * @param [in] handle 要检查的句柄
 * @return key_value_register_t* 
 *         - NULL: 未找到句柄
 *         - 非NULL: 找到的节点指针
 */
static key_value_register_t* chek_has_register(key_value_handle_t handle)
{
    key_value_handle_t move = (key_value_handle_t)register_head;

    if (register_head == NULL)
        return NULL;

    for (;;) {
        if (handle != move)
            goto _chek_next;

        return move;

    _chek_next:
        if (move->next == NULL)
            return NULL;
        move = move->next;
    }
}

/**
 * @brief 删除节点
 *
 * @param [in] handle 句柄
 * @return int 
 *         - 0: 成功
 *         - 1: 失败
 */
static int deL_handle(key_value_handle_t handle)
{
    key_value_register_t* tmp = chek_has_register(handle);
    if (tmp == NULL)
        return 1;

    if (tmp == register_head) {
        if (register_head == register_tail) {
            register_head = NULL;
            register_tail = NULL;
        } else
            register_head = register_head->next;
    } else if (tmp == register_tail) {
        register_tail = tmp->last;
        register_tail->next = NULL;
    } else {
        key_value_register_t* last = tmp->last;
        key_value_register_t* next = tmp->next;
        last->next = next;
        next->last = last;
    }

    key_value_free_func((void*)tmp->key);
    key_value_free_func(tmp);

    return 0;
}

#if key_value_support_rtos
/**
 * @brief 检查并处理延迟删除列表
 */
static void chek_del_list()
{
    if (msg_busy)
        return;

    mutex_cb.mutex_get_cb();
    while (del_busy)
        ;

    if (del_head == NULL) {
        mutex_cb.mutex_give_cb();
        return;
    }

    for (;;) {
        del_list_t* tmp = del_head->next;
        deL_handle(del_head->handle);
        key_value_free_func(del_head);
        del_head = tmp;
        if (del_head == NULL) {
            del_tail = NULL;
            break;
        }
    }
    mutex_cb.mutex_give_cb();
}
#endif

/**
 * @brief 创建(发布)一条键值对通知
 *
 * @param [in] key 键名，可以是动态的
 * @param [in] value 指向数据，可以是动态的
 * @param [in] lenth 数据长度，字节
 * @return int 
 *         - 0: 成功
 *         - 1: 无注册键
 */
int key_value_msg(const char* key, void* value, size_t lenth)
{
    int ret = 0;
    int sum = 0;

#if key_value_support_rtos

    if (msg_busy == 0) {
        mutex_cb.mutex_get_cb();
    }

    msg_busy++;
#endif

    key_value_register_t* move = register_head;

    if (move == NULL) {
        ret = 1;
        goto clear_out;
    }

    sum = get_key_sum(key);

    for (;;) {
        if (move->key_sum != sum)
            goto chek_null;

        if (key_cmp(key, move->key) == 0) {
            move->_cb(value, lenth);
        }

    chek_null:
        if (move->next == NULL)
            break;

        move = move->next;
    }

clear_out:
#if key_value_support_rtos
    msg_busy--;
    if (msg_busy == 0)
        mutex_cb.mutex_give_cb();

    chek_del_list();

#endif
    return ret;
}

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
 */
int key_value_register(key_value_handle_t* handle,
                       const char* key,
                       key_value_cb_t cb)
{
    size_t _len = 0;
    key_value_register_t* tmp = key_value_malloc_func(sizeof(key_value_register_t));

    if (tmp == NULL)
        return 1;

    _len = strlen(key) + 1;
    tmp->key = key_value_malloc_func(_len);
    if (tmp->key == NULL) {
        key_value_free_func(tmp);
        return 2;
    }
    memcpy((void*)tmp->key, key, _len);

    tmp->key_sum = get_key_sum(key);
    tmp->next = NULL;
    tmp->last = NULL;
    tmp->_cb = cb;

#if key_value_support_rtos
    mutex_cb.mutex_get_cb();
#endif

    if (handle != NULL)
        *handle = tmp;

    if (register_head == NULL) {
        register_head = tmp;
        register_tail = tmp;
    } else {
        register_tail->next = tmp;
        tmp->last = register_tail;
        register_tail = tmp;
    }

#if key_value_support_rtos
    mutex_cb.mutex_give_cb();
#endif

    return 0;
}

/**
 * @brief 删除已注册的键
 *
 * @param [in] handle 注册的句柄
 * @return int 
 *         - 0: 成功
 *         - 1: 不存在句柄
 *         - 2: 空间不足（仅在RTOS模式下可能返回）
 */
int key_value_del(key_value_handle_t handle)
{

#if key_value_support_rtos
    if (msg_busy) {
        del_list_t* tmp = key_value_malloc_func(sizeof(del_list_t));
        if (tmp == NULL)
            return 2;
        del_busy = 1;
        tmp->next = NULL;
        tmp->handle = handle;

        if (del_tail != NULL)
            del_tail->next = tmp;
        del_tail = tmp;
        if (del_head == NULL)
            del_head = tmp;
        del_busy = 0;
        return 0;
    }

    mutex_cb.mutex_get_cb();
#endif

    int ret = deL_handle(handle);

#if key_value_support_rtos
    mutex_cb.mutex_give_cb();
#endif

    return ret;
}

#endif
