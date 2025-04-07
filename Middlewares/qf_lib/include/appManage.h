/**
 * @file appManage.h
 * @brief APP管理器头文件
 * @date 2023-06-21
 * @details
 * 本文件定义了一个简单的APP管理器的接口，支持APP的安装、加载、关闭和卸载等操作。
 *          可用于管理多个应用程序，支持前台和后台应用的切换。
 */

#ifndef APP_MANAGE_H
#define APP_MANAGE_H

#ifdef __cplusplus
extern "C" {
#endif
/****************************************************************/
/*
    APP管理器 By 启凡科创。
    Version:1.0.1 2023-6-21
*/
/****************************************************************/

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// app自行通过RTOS或btask进行创建后自行管理

/**
 * @def app_none
 * @brief 表示无效的APP句柄
 */
#define app_none -1

/**
 * @typedef app_config_t
 * @brief APP配置结构体，用于安装APP时提供必要信息
 */
typedef struct _app_config_t {
    const char* name;      /**< APP名称，静态字符串 */
    const void* name_font; /**< 名称使用的字体 */
    const void* icon;      /**< APP图标，静态图标 */
    void (*app_init)();    /**< 初始化APP入口，在安装APP时运行一次 */
    void (*app_kill)();    /**< 彻底关闭APP入口 */
    void (*app_load)();    /**< 加载APP到前台入口 */
    void (*app_close)();   /**< APP切到后台入口 */
    void (*app_power_off)(); /**< 彻底关机处理函数，在此保存数据到EEPROM */
    uint8_t
        has_gui; // 1：APP有GUI界面，0：无GUI界面（桌面不展示APP图标名称，默认底层后台程序）
} app_config_t;

/**
 * @typedef app_obj_t
 * @brief APP对象结构体，包含APP的基本信息
 */
typedef struct _app_obj_t {
    const char* name;      // app名称，静态字符串
    const void* name_font; // 指向app名称使用的字体
    const void* icon;      // app图标，静态图标
    const uint8_t
        has_gui; // 1：APP有GUI界面，0：无GUI界面（桌面不展示APP图标名称，默认底层后台程序）
} app_obj_t;

/**
 * @typedef app_handle_t
 * @brief APP句柄类型定义
 */
typedef int32_t app_handle_t;

/**
 * @brief 安装APP并运行app初始化函数
 *
 * @param cfg app配置参数
 * @return app_handle_t 返回ID句柄，如果为app_none，则安装失败
 */
app_handle_t app_install(app_config_t* cfg);

/**
 * @brief 获取APP句柄
 *
 * @param name app名称
 * @return app_handle_t app_none：不存在app
 */
app_handle_t app_get_handle(const char* name);

/**
 * @brief 获取app名称
 *
 * @param handle app句柄
 * @return const char* NULL:不存在app
 */
const char* app_get_name(app_handle_t handle);

/**
 * @brief 获取已安装app数量
 *
 * @return int32_t
 */
int32_t app_get_cnt();

/**
 * @brief
 * 打开APP（前台），参数二选一，名称不为NULL使用名称查找APP，名称为NULL则使用句柄查找APP(高效率)
 *
 * @param name app名称
 * @param handle app句柄
 * @return uint8_t 1：成功，0：不存在APP
 */
uint8_t app_load(const char* name, app_handle_t handle);

/**
 * @brief
 * 关闭APP（后台），参数二选一，名称不为NULL使用名称查找APP，名称为NULL则使用句柄查找APP(高效率)
 *
 * @param name app名称
 * @param handle app句柄
 * @return uint8_t 1：成功，0：不存在APP
 */
uint8_t app_close(const char* name, app_handle_t handle);

/**
 * @brief
 * 杀死APP（清理后台），参数二选一，名称不为NULL使用名称查找APP，名称为NULL则使用句柄查找APP(高效率)
 *
 * @param name app名称
 * @param handle app句柄
 * @return uint8_t 1：成功，0：不存在APP
 */
uint8_t app_kill(const char* name, app_handle_t handle);

/**
 * @brief
 * 关闭所有app进程，在息屏前会调用此函数，需要掉电保存的数据可以在kill函数里进行相关处理
 */
void app_kill_all();

/**
 * @brief 彻底关机时调用，以通知APP把数据转存到EEPROM内
 */
void app_power_off_all();

/**
 * @brief
 * 获取app对象，参数二选一，名称不为NULL使用名称查找APP，名称为NULL则使用句柄查找APP(高效率)
 *
 * @param name app名称
 * @param handle app句柄
 * @return app_obj_t 指针，使用下标访问APP内容
 */
app_obj_t* app_get(const char* name, app_handle_t handle);

/**
 * @brief 获取前台程序
 *
 * @return app_obj_t*
 */
app_obj_t* app_get_loaded();

#ifdef __cplusplus
}
#endif

#endif
