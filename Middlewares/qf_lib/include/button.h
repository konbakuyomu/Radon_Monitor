/**
 * @file button.h
 * @brief 独立IO按钮库头文件
 * @date 2025-04-15
 * @details
 * 本文件定义了一个独立IO按钮库的接口，支持多个按键的同时操作，提供按下、弹起、单击、双击、长按等事件检测。
 *          支持软件消抖，无需外部硬件消抖电路，可移植到任何能用C语言和带定时器中断的平台。
 */

#ifndef button_H
#define button_H
#ifdef __cplusplus
extern "C" {
#endif
/****************************************************************/
/*
    独立io按钮库 By 启凡科创

    使用前请自行初始化好对应的GPIO，如是低电平触发的按钮初始化为上拉输入，
    高电平触发的初始化为下拉输入，否则对应按键将绑定失败

    务必将读取IO电平状态的回调函数注册了后再注册按键
    绑定过程中不要按住按键否则会绑定失败

    功能：
        1.最多支持255（0-254）个按键
        2.所有按键均可单独配置高（低）电平触发模式
        3.所有按键可同时操作，无冲突，数据以队列形式提供用户使用
        4.提供按下、弹起、单击、双击、长按五种事件，宏定义可选编译使用的事件功能
        5.提供动态配置启用编译的事件功能的接口，用户可使用过程中实时屏蔽、启用指定功能识别
        6.集成软件消抖功能，无需外部硬件消抖电路
        7.兼容性强，可以移植到任何能用c和带定时器中断的且能读取IO电平状态的平台
        8.无任何阻塞性质
        9.可灵活配置的长按连续触发功能，可以配置连续触发的事件类型、触发间隔、功能使能

    移植步骤：具体接口说明查看对应接口的注释
        1）BUTTON_NUM
        配置是否启用编译，0为不编译；不为0时，是几就代表有几个按键
        2）BUTTON_BUFFER_NUM
        配置按键事件缓存数量，用户不处理按键结果且达到最多数量时，新按下的值会覆盖最久的键值
        3）对一些 如双击、长按等的时长手感进行配置，一般默认即可
        4）配置启用需要用到的触发事件功能
        5）将按键对应的GPIO初始化为上拉、下拉或输入等对应的触发模式
        6）将格式为 uint8_t readInputOutputFunction(uint8_t inputOutputNumber)
            返回为8位无符号0或1的形参为8位无符号的IO数字的读取管脚电平的函数通过
            buttonAttachReadInputOutputFunction(readInputOutputFunction)接口进行注册绑定，完成此步骤后才能绑定按键
        7）配置好系统定时器ms级中断，将buttonTickMilliseconds(milliseconds)接口放置于中断函数里提供心跳
        8）使用buttonAttach()接口将按键GPIO数和触发电平注册

        至此，移植工作完成

    使用步骤：
        1）按键库在移植注册好后，用户不用对任何接口进行实时扫描等操作
            有按键按下后，对应的操作事件结果会自动存储到缓存里
        2）我们可以在程序空闲时，通过buttonAvailableCount()接口获取有没有按键按下，如果没有返回值为0，
            有按下的话将返回有多少个键值
        3）得知有键值后，通过buttonReadEvent()接口获取键值对应的按键IO和事件类型，
            得到按键IO口和事件后，用户就可以用这两个参数进行任何的自定义功能操作

        4）使用过程中，假如在程序A中，我不需要双击功能，在B中需要，则可在进入A时使用buttonDisableEvent
            接口进行指定事件类型的暂时屏蔽，在回到B时使用buttonEnableEvent接口取消屏蔽指定事件类型

    Version:1.0.4 2025-4-15
*/
/****************************************************************/

// 用户配置包含
// #include "LIB_CONFIG.h"

// 必要依赖库包含
#include <String.H>
#include <stdint.h> //uint类型报错取消注释

// 功能使能配置
#ifndef BUTTON_NUM
/** @brief 按键数量，0则关闭按钮库功能 */
#define BUTTON_NUM 4 // 如果配置文件未定义，则用户自定义
#endif

#if BUTTON_NUM

#ifndef BUTTON_BUFFER_NUM
/** @brief 按钮存储缓冲区大小，此数值为最多存储多少个按钮事件 */
#define BUTTON_BUFFER_NUM 4 // 至少为1，不要比实际使用多设置，会消耗内存并占用更多CPU
#endif

/** @brief 双击识别间隔时间（毫秒） */
#define BUTTON_DOUBLE_CLICK_TIME_DEFAULT 300

/** @brief 长按识别间隔时间（毫秒） */
#define BUTTON_LONG_PRESS_TIME_DEFAULT 1000

/** @brief 长按时连续触发的开始时间（毫秒） */
#define BUTTON_LONG_PRESS_CONTINUOUS_TRIGGER_TIME 15000

/** @brief 长按时连续触发的每次触发间隔时间（毫秒） */
#define BUTTON_LONG_PRESS_TRIGGER_INTERVAL_TIME 100

/** @brief 抖动消除时间（毫秒） */
#define BUTTON_SHAKE_MS 20

//  下列选项可以让程序在编译时就不启用这些功能节省空间  1启用 0停用
//  同时，还提供了接口可以动态 屏蔽、启用
//  已开启编译的功能，以实现丰富且便捷的交互操作
/**
 * @defgroup Button_Event_Enable 按钮事件编译使能
 * @brief 下列选项可以让程序在编译时就不启用这些功能节省空间，1启用，0停用
 * @{
 */
/** @brief 按下事件编译使能 */
#define BUTTON_DOWN_ENABLE 1

/** @brief 弹起事件编译使能 */
#define BUTTON_UP_ENABLE 1

/** @brief 长按事件编译使能 */
#define BUTTON_LONG_PRESS_ENABLE 1

/** @brief 单击事件编译使能 */
#define BUTTON_CLICK_ENABLE 1

/** @brief 双击事件编译使能 */
#define BUTTON_DOUBLE_CLICK_ENABLE 1

/** @brief 长按时连续触发编译使能 */
#define BUTTON_LONG_PRESS_TRIGGER_ENABLE 0
/**
 * @}
 */

/**
 * @typedef ButtonEventType
 * @brief 按钮事件类型枚举
 */
typedef enum {
    buttonNotPress = 0x00,         /**< 未按下 */
    buttonDown = 0x01,             /**< 按下 */
    buttonUp = 0x02,               /**< 弹起 */
    buttonLongPress = 0x04,        /**< 长按 */
    buttonClick = 0x08,            /**< 单击 */
    buttonDoubleClick = 0x10,      /**< 双击 */
    buttonLongPressTrigger = 0x20, /**< 长按连续触发 */
    buttonEventAll = 0xff          /**< 所有事件 */
} ButtonEventType;

/**
 * @typedef LongPressTriggerCallback
 * @brief 长按连续触发回调函数类型
 */
typedef void (*LongPressTriggerCallback)(void* arg);

/**
 * @brief 提供心跳，此参数应当小于等于5ms,建议1-3ms最佳
 *
 * @param milliseconds 多少ms调用一次就写多少
 */
void buttonTickMilliseconds(uint8_t milliseconds);

/**
 * @brief 绑定读取指定IO口电平状态的回调函数
 *        格式为： 函数名(常规数字0-254)
 * 返回的值为0或1（无符号1字节整形）对应IO高低电平，
 *        如果你使用的平台读取电平函数为其他格式，请自行实现此格式后绑定
 *
 * @param func 回调函数  int gpio_read(io_num);//io_num:0-254,返回0、1
 */
void buttonAttachReadInputOutputFunction(
    uint8_t (*readInputOutputCallback)(uint8_t inputOutputNumber));

/**
 * @brief 绑定按键IO号和触发电平，对应IO口用户自行配置好对应的上下拉输入状态
 *
 * @param io_num 绑定按键对应的GPIO号，如23，41
 * @param --
 * @param level 按键按下去后的电平状态，0/1
 * @return 成功返回1，失败返回0
 */
uint8_t buttonAttach(uint8_t inputOutputNumber, uint8_t triggerLevel);

/**
 * @brief 注册长按连续触发功能在第一次触发时的回调函数
 *
 * @param callback 回调函数
 * @param userData 用户数据
 */
void buttonAttachLongPressTriggerCallback(LongPressTriggerCallback callback, void* userData);

/**
 * @brief 注销长按连续触发功能在第一次触发时的回调函数
 */
void buttonDetachLongPressTriggerCallback(void);

/**
 * @brief 注销按钮，注销后将不会进行对应按键扫描。
 *
 * @param io_num io_num：已绑定的按键对应的GPIO号
 */
void buttonDetach(uint8_t inputOutputNumber);

/**
 * @brief 返回按键事件缓冲区还有多少个键值
 *
 * @return uint8_t 未取出的键值数
 */
uint8_t buttonAvailableCount(void);

/**
 * @brief 读取按键缓冲区的数据，读取后对应键值自动销毁
 *
 * @param inputOutputNumber 返回事件对应的按键IO数字
 * @param --
 * @param eventType 返回按键事件类型,未启用编译的类型将不可用：
 * @param ··buttonDown,            //按下
 * @param ··buttonUp,              //弹起
 * @param ··buttonLongPress,      //长按
 * @param ··buttonClick,           //单击
 * @param ··buttonDoubleClick,    //双击
 */
void buttonReadEvent(uint8_t* inputOutputNumber, ButtonEventType* eventType);

#if BUTTON_LONG_PRESS_TRIGGER_ENABLE
/**
 * @brief 设置长按时连续触发的事件类型，默认为单击
 *
 * @param eventType 事件类型：
 * @param buttonDown,            //按下
 * @param buttonUp,              //弹起
 * @param buttonLongPress,      //长按
 * @param buttonClick,           //单击
 * @param buttonDoubleClick,    //双击
 */
void buttonLongPressTriggerEvent(ButtonEventType eventType);

/**
 * @brief 设置长按时连续触发的使能，默认关闭
 *
 * @param en -1启用长按时连续触发，0关闭
 */
void buttonLongPressTriggerEnable(uint8_t enable);

/**
 * @brief 设置长按连续触发的事件间隔
 *
 * @param ms 毫秒为单位
 */
void buttonLongPressTriggerIntervalMilliseconds(uint16_t milliseconds);
#endif

/*
    启用指定按键事件功能
    可以单独配置一项或多项功能的启用
    如：
        buttonEnableEvent(buttonDown);                       //启用按下检测
        buttonEnableEvent(buttonLongPress);                 //启用长按检测
        buttonEnableEvent(buttonDown | buttonLongPress);   //启用按下和长按检测
        buttonEnableEvent(buttonEventAll); //启用所有启用编译的事件检测
    所有功能：（具体实际有哪些请根据.h头文件的编译en启用情况而定）
        buttonDown,            //按下
        buttonUp,              //弹起
        buttonLongPress,      //长按
        buttonClick,           //单击
        buttonDoubleClick,    //双击
*/
void buttonEnableEvent(uint8_t configurationType);

/*
    停用指定按键事件功能
    可以单独配置一项或多项功能的停用
    如：
        buttonDisableEvent(buttonDown);                      //停用按下检测
        buttonDisableEvent(buttonLongPress);                //停用长按检测
        buttonDisableEvent(buttonDown | buttonLongPress);  //停用按下和长按检测
        buttonDisableEvent(buttonEventAll); //停用所有启用编译的事件检测
    所有功能：（具体实际有哪些请根据.h头文件的编译en启用情况而定）
        buttonDown,            //按下
        buttonUp,              //弹起
        buttonLongPress,      //长按
        buttonClick,           //单击
        buttonDoubleClick,    //双击
*/
void buttonDisableEvent(uint8_t configurationType);

#ifndef NULL
#define NULL ((void*)0)
#endif

#endif
#ifdef __cplusplus
}
#endif
#endif
