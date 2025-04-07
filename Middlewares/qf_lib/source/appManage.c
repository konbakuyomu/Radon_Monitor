/**
 * @file appManage.c
 * @brief APP管理器实现
 * @date 2023-06-21
 * @details 本文件实现了一个简单的APP管理器，支持APP的安装、加载、关闭和卸载等操作。
 *          可用于管理多个应用程序，支持前台和后台应用的切换。
 */

#include "appManage.h"

/* 结构体
 * ----*/
/**
 * @typedef app_func_t
 * @brief APP函数集合结构体
 */
typedef struct {
    void (*app_kill)();      /**< 彻底关闭APP入口 */
    void (*app_load)();      /**< 加载APP到前台入口 */
    void (*app_close)();     /**< APP切到后台入口 */
    void (*app_power_off)(); /**< 彻底关机处理函数 */
} app_func_t;

/**
 * @typedef app_list_t
 * @brief APP链表节点结构体
 */
typedef struct _app_list_t {
    app_func_t func;          /**< APP函数集合 */
    app_obj_t obj;            /**< APP对象信息 */
    app_handle_t handle;      /**< APP句柄 */
    struct _app_list_t* next; /**< 下一个节点指针 */
} app_list_t;

/* 全局变量
 * ----*/
/**
 * @var list_head
 * @brief APP链表头指针
 */
static app_list_t* list_head = NULL;

/**
 * @var list_tail
 * @brief APP链表尾指针
 */
static app_list_t* list_tail = NULL;

/**
 * @var app_cnt
 * @brief 已安装APP计数
 */
static int32_t app_cnt = 0;

/**
 * @var app_loaded
 * @brief 当前加载的前台APP
 */
static app_obj_t* app_loaded = NULL;

/* 内部函数
 * ----*/
/**
 * @brief 比较两个字符串是否相同
 *
 * @param [in] str1 第一个字符串
 * @param [in] str2 第二个字符串
 * @return int
 *         - 0: 相同
 *         - 1: 不同
 */
static int _strcmp(const char* str1, const char* str2)
{
    for (; *str1 == *str2; ++str1, ++str2) {
        if (*str1 == '\0')
            return (0);
    }
    return 1;
}

/**
 * @brief 添加APP到链表
 *
 * @param [in] app APP节点指针
 */
static void app_list_add(app_list_t* app)
{
    app->next = NULL;
    if (list_head == NULL) {
        list_head = app;
    } else {
        list_tail->next = app;
    }
    list_tail = app;
}

/**
 * @brief 通过名称查找APP
 *
 * @param [in] name APP名称
 * @return app_list_t*
 *         - 返回找到的APP节点指针
 *         - NULL: 未找到
 */
static app_list_t* app_list_find(const char* name)
{
    app_list_t* move = list_head;
    while (move != NULL) {
        if (_strcmp(name, move->obj.name) == 0)
            return move;
        move = move->next;
    }
    return NULL;
}

/**
 * @brief 通过句柄查找APP
 *
 * @param [in] handle APP句柄
 * @return app_list_t*
 *         - 返回找到的APP节点指针
 *         - NULL: 未找到
 */
static app_list_t* app_list_find_id(app_handle_t handle)
{
    if (handle >= app_cnt)
        return NULL;
    app_list_t* move = list_head;
    for (app_handle_t i = 0; i < handle; i++)
        move = move->next;
    return move;
}

/* 外部函数实现
 * ----*/
/**
 * @brief 安装APP并运行APP初始化函数
 *
 * @param [in] cfg APP配置参数
 * @return app_handle_t
 *         - 返回ID句柄
 *         - app_none: 安装失败
 */
app_handle_t app_install(app_config_t* cfg)
{
    app_list_t* app = malloc(sizeof(app_list_t));
    if (app == NULL)
        return app_none;

    app->func.app_load = cfg->app_load;
    app->func.app_close = cfg->app_close;
    app->func.app_kill = cfg->app_kill;
    app->func.app_power_off = cfg->app_power_off;

    app->obj.name = cfg->name;
    app->obj.name_font = cfg->name_font;
    app->obj.icon = cfg->icon;
    *(uint8_t*)&app->obj.has_gui = cfg->has_gui;

    app->handle = app_cnt;
    app_cnt++;
    app_list_add(app);

    if (cfg->app_init != NULL)
        cfg->app_init();

    return (app_cnt - 1);
}

/**
 * @brief 打开APP（前台）
 *
 * @param [in] name APP名称，如果不为NULL则使用名称查找APP
 * @param [in] handle APP句柄，如果name为NULL则使用句柄查找APP(高效率)
 * @return uint8_t
 *         - 1: 成功
 *         - 0: 不存在APP
 */
uint8_t app_load(const char* name, app_handle_t handle)
{
    app_list_t* tmp;
    if (name == NULL && handle == app_none)
        return 0;
    if (name == NULL) {
        tmp = app_list_find_id(handle);
    } else {
        tmp = app_list_find(name);
    }
    if (tmp == NULL) {
        return 0;
    }

    if (app_loaded != NULL) {
        app_close(app_loaded->name, app_none);
    }

    app_loaded = &tmp->obj;

    if (tmp->func.app_load != NULL) {
        tmp->func.app_load();
    }

    return 1;
}

/**
 * @brief 关闭APP（后台）
 *
 * @param [in] name APP名称，如果不为NULL则使用名称查找APP
 * @param [in] handle APP句柄，如果name为NULL则使用句柄查找APP(高效率)
 * @return uint8_t
 *         - 1: 成功
 *         - 0: 不存在APP
 */
uint8_t app_close(const char* name, app_handle_t handle)
{
    app_list_t* tmp;
    if (name == NULL && handle == app_none)
        return 0;
    if (name == NULL)
        tmp = app_list_find_id(handle);
    else
        tmp = app_list_find(name);
    if (tmp == NULL)
        return 0;
    if (tmp->func.app_close != NULL)
        tmp->func.app_close();
    app_loaded = NULL;
    return 1;
}

/**
 * @brief 杀死APP（清理后台）
 *
 * @param [in] name APP名称，如果不为NULL则使用名称查找APP
 * @param [in] handle APP句柄，如果name为NULL则使用句柄查找APP(高效率)
 * @return uint8_t
 *         - 1: 成功
 *         - 0: 不存在APP
 */
uint8_t app_kill(const char* name, app_handle_t handle)
{
    app_list_t* tmp;
    if (name == NULL && handle == app_none)
        return 0;
    if (name == NULL)
        tmp = app_list_find_id(handle);
    else
        tmp = app_list_find(name);
    if (tmp == NULL)
        return 0;
    if (tmp->func.app_kill != NULL)
        tmp->func.app_kill();
    return 1;
}

/**
 * @brief 关闭所有APP进程
 *
 * @note 在息屏前会调用此函数，需要掉电保存的数据可以在kill函数里进行相关处理
 */
void app_kill_all()
{
    app_list_t* tmp = list_head;
    for (app_handle_t i = 0; i < app_cnt; i++) {
        if (tmp->func.app_kill != NULL)
            tmp->func.app_kill();
        tmp = tmp->next;
    }
}

/**
 * @brief 彻底关机时调用，以通知APP把数据转存到EEPROM内
 */
void app_power_off_all()
{
    app_list_t* tmp = list_head;
    for (app_handle_t i = 0; i < app_cnt; i++) {
        if (tmp->func.app_power_off != NULL)
            tmp->func.app_power_off();
        tmp = tmp->next;
    }
}

/**
 * @brief 获取APP句柄
 *
 * @param [in] name APP名称
 * @return app_handle_t
 *         - 返回APP句柄
 *         - app_none: 不存在APP
 */
app_handle_t app_get_handle(const char* name)
{
    app_list_t* tmp = app_list_find(name);
    if (tmp == NULL)
        return (app_none);
    else
        return (tmp->handle);
}

/**
 * @brief 获取APP名称
 *
 * @param [in] handle APP句柄
 * @return const char*
 *         - 返回APP名称
 *         - NULL: 不存在APP
 */
const char* app_get_name(app_handle_t handle)
{
    app_list_t* tmp = app_list_find_id(handle);
    if (tmp == NULL)
        return (NULL);
    else
        return (tmp->obj.name);
}

/**
 * @brief 获取已安装APP数量
 *
 * @return int32_t 已安装的APP数量
 */
int32_t app_get_cnt() { return app_cnt; }

/**
 * @brief 获取APP对象
 *
 * @param [in] name APP名称，如果不为NULL则使用名称查找APP
 * @param [in] handle APP句柄，如果name为NULL则使用句柄查找APP(高效率)
 * @return app_obj_t*
 *         - 返回APP对象指针，使用下标访问APP内容
 */
app_obj_t* app_get(const char* name, app_handle_t handle)
{
    if (name == NULL)
        return (&app_list_find_id(handle)->obj);
    return (&app_list_find(name)->obj);
}

/**
 * @brief 获取前台程序
 *
 * @return app_obj_t* 当前前台APP对象指针
 */
app_obj_t* app_get_loaded() { return app_loaded; }
