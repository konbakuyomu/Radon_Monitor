/**
 * @file button.c
 * @brief 独立IO按钮库实现
 * @date 2025-04-15
 * @author kuma
 * @version 1.0.4
 * @details
 * 本文件实现了一个独立IO按钮库，支持多个按键的同时操作，提供按下、弹起、单击、双击、长按等事件检测。
 * 支持软件消抖，无需外部硬件消抖电路，可移植到任何能用C语言和带定时器中断的平台。
 *
 * 功能：
 * - 最多支持255（0-254）个按键
 * - 所有按键均可单独配置高（低）电平触发模式
 * - 所有按键可同时操作，无冲突，数据以队列形式提供用户使用
 * - 提供按下、弹起、单击、双击、长按五种事件
 * - 提供动态配置启用编译的事件功能的接口
 * - 集成软件消抖功能，无需外部硬件消抖电路
 */

#include "button.h"
#if BUTTON_NUM

/**
 * @defgroup Button_Global_Variables 按钮全局变量
 * @brief 按钮库使用的全局变量
 * @{
 */

/**
 * @var buttonInputOutputNumbers
 * @brief 按键IO号数组，存储注册的所有按键IO号
 */
static uint8_t buttonInputOutputNumbers[BUTTON_NUM] = { 255 }; // 初始化为无效值

/**
 * @var buttonTriggerLevels
 * @brief 按键触发电平数组，存储每个按键的触发电平（0或1）
 */
static uint8_t buttonTriggerLevels[BUTTON_NUM] = { 0 };

/**
 * @var buttonCurrentLevels
 * @brief 按键当前电平数组，记录每个按键的当前状态
 */
static uint8_t buttonCurrentLevels[BUTTON_NUM] = { 0 };

/**
 * @var buttonLowLevelCounts
 * @brief 按键低电平计数数组，用于消抖处理
 */
static uint8_t buttonLowLevelCounts[BUTTON_NUM] = { 0 };

/**
 * @var buttonHighLevelCounts
 * @brief 按键高电平计数数组，用于消抖处理
 */
static uint8_t buttonHighLevelCounts[BUTTON_NUM] = { 0 };

#if BUTTON_LONG_PRESS_ENABLE
/**
 * @var longPressFlags
 * @brief 长按标志数组，标记按键是否处于长按计时状态
 */
static uint8_t longPressFlags[BUTTON_NUM] = { 0 };

/**
 * @var buttonLongPressTimeMilliseconds
 * @brief 长按扫描计时数组，记录按键按下的时间用于长按判断
 */
static uint16_t buttonLongPressTimeMilliseconds[BUTTON_NUM] = { 0 };
#endif

#if BUTTON_DOUBLE_CLICK_ENABLE
/**
 * @var doubleClickFlags
 * @brief 双击标志数组，标记按键是否处于双击计时状态
 * @details 0-未触发，1-第一次点击后等待第二次点击，2-已识别为双击
 */
static uint8_t doubleClickFlags[BUTTON_NUM] = { 0 };

/**
 * @var buttonDoubleClickTimeMilliseconds
 * @brief 双击扫描计时数组，记录两次点击之间的间隔时间
 */
static uint16_t buttonDoubleClickTimeMilliseconds[BUTTON_NUM] = { 0 };
#endif

/**
 * @var buttonEventTypes
 * @brief 缓冲区事件数组，存储按钮事件类型
 */
static ButtonEventType buttonEventTypes[BUTTON_BUFFER_NUM] = { buttonNotPress };

/**
 * @var buttonEventInputOutputNumbers
 * @brief 缓冲区IO口数字数组，存储触发事件的按钮IO号
 */
static uint8_t buttonEventInputOutputNumbers[BUTTON_BUFFER_NUM] = { 0 };

/**
 * @var readInputOutputCallback
 * @brief 读取IO回调函数指针，用于读取按键IO状态
 */
static uint8_t (*readInputOutputCallback)(uint8_t inputOutputNumber) = NULL;

/**
 * @var buttonEventCount
 * @brief 缓冲区事件计数，记录当前缓冲区中未读取的事件数量
 */
static uint8_t buttonEventCount = 0;

/**
 * @var buttonEventWritePointer
 * @brief 缓冲区写入指针，指向下一个要写入的位置
 */
static uint8_t buttonEventWritePointer = 0;

/**
 * @var buttonEventReadPointer
 * @brief 缓冲区读取指针，指向下一个要读取的位置
 */
static uint8_t buttonEventReadPointer = 0;

/**
 * @var buttonEventEnableState
 * @brief 按钮事件使能状态，按位表示各种事件是否启用
 */
static uint8_t buttonEventEnableState = 0xff;

#if BUTTON_LONG_PRESS_TRIGGER_ENABLE
/**
 * @var longPressTriggerFlag
 * @brief 长按触发标志，标记是否已触发长按连续事件
 */
static uint8_t longPressTriggerFlag = 0;

/**
 * @var longPressTriggerType
 * @brief 长按触发事件类型，指定长按连续触发时产生的事件类型
 */
static ButtonEventType longPressTriggerType = buttonClick;

/**
 * @var longPressTriggerIntervalMilliseconds
 * @brief 长按触发间隔时间，长按连续触发的时间间隔（毫秒）
 */
static uint16_t longPressTriggerIntervalMilliseconds = BUTTON_LONG_PRESS_TRIGGER_INTERVAL_TIME;

/**
 * @var longPressTriggerCount
 * @brief 长按触发计数，用于计算连续触发的时间间隔
 */
static uint16_t longPressTriggerCount = 0;

/**
 * @var longPressContinuousTriggerCount
 * @brief 长按触发长按计数，用于计算初次长按到连续触发的时间
 */
static uint16_t longPressContinuousTriggerCount = 0;

/**
 * @var longPressContinuousTriggerTimeMilliseconds
 * @brief 长按触发长按时间，开始连续触发前的等待时间
 */
static uint16_t longPressContinuousTriggerTimeMilliseconds
    = BUTTON_LONG_PRESS_CONTINUOUS_TRIGGER_TIME / 5;

/**
 * @var longPressTriggerCallback
 * @brief 长按触发回调函数，长按连续触发时调用
 */
static LongPressTriggerCallback longPressTriggerCallback = NULL;

/**
 * @var longPressTriggerUserData
 * @brief 长按触发回调函数用户数据，传递给回调函数
 */
static void* longPressTriggerUserData = NULL;
#endif

/** @} */

/**
 * @defgroup Button_Static_Functions 按钮静态函数
 * @brief 按钮库内部使用的静态函数
 * @{
 */

/**
 * @brief 写入按键事件到缓冲区
 *
 * @param [in] inputOutputNumber 按键IO号
 * @param [in] ret 按键事件类型
 */
static void buttonWriteBuffer(uint8_t inputOutputNumber, ButtonEventType ret)
{
    /* 检查事件是否启用 */
    if ((buttonEventEnableState & ret) == 0)
        return;

    /* 写入事件信息到缓冲区 */
    buttonEventTypes[buttonEventWritePointer] = ret;
    buttonEventInputOutputNumbers[buttonEventWritePointer++] = inputOutputNumber;

    /* 循环使用缓冲区 */
    if (buttonEventWritePointer == BUTTON_BUFFER_NUM)
        buttonEventWritePointer = 0;

    /* 更新事件计数，不超过缓冲区大小 */
    if (buttonEventCount < BUTTON_BUFFER_NUM)
        buttonEventCount++;
}

/** @} */

/**
 * @defgroup Button_Public_Functions 按钮公共函数
 * @brief 按钮库对外提供的公共函数
 * @{
 */

#if BUTTON_LONG_PRESS_TRIGGER_ENABLE
/**
 * @brief 设置长按时连续触发的事件类型
 *
 * @param [in] eventType
 * 事件类型，可以是buttonDown/buttonUp/buttonLongPress/buttonClick/buttonDoubleClick
 * @note 默认事件类型是buttonClick
 */
void buttonLongPressTriggerEvent(ButtonEventType eventType)
{
    /* 不允许设置为所有事件 */
    if (eventType == buttonEventAll)
        return;
    longPressTriggerType = eventType;
}

/**
 * @brief 设置长按连续触发的事件间隔
 *
 * @param [in] milliseconds 触发间隔，单位毫秒
 */
void buttonLongPressTriggerIntervalMilliseconds(uint16_t milliseconds)
{
    longPressTriggerIntervalMilliseconds = milliseconds;
}

/**
 * @brief 注册长按连续触发功能在第一次触发时的回调函数
 *
 * @param [in] callback 回调函数
 * @param [in] userData 传递给回调函数的用户数据
 */
void buttonAttachLongPressTriggerCallback(LongPressTriggerCallback callback, void* userData)
{
    longPressTriggerCallback = callback;
    longPressTriggerUserData = userData;
}

/**
 * @brief 注销长按连续触发功能的回调函数
 */
void buttonDetachLongPressTriggerCallback(void)
{
    longPressTriggerCallback = NULL;
    longPressTriggerUserData = NULL;
}
#endif

/**
 * @brief 返回按键事件缓冲区中未读取的键值数量
 *
 * @return uint8_t 未取出的键值数
 */
uint8_t buttonAvailableCount(void) { return buttonEventCount; }

/**
 * @brief 绑定按键IO号和触发电平
 *
 * @param [in] inputOutputNumber 绑定按键对应的GPIO号
 * @param [in] triggerLevel 按键按下时的电平状态，0或1
 * @return uint8_t
 *         - 1: 绑定成功
 *         - 0: 绑定失败（参数错误或没有空闲按键位置）
 * @note 对应IO口用户需自行配置好对应的上下拉输入状态
 */
uint8_t buttonAttach(uint8_t inputOutputNumber, uint8_t triggerLevel)
{
    uint8_t i;

    /* 检查触发电平参数 */
    if (triggerLevel != 0 && triggerLevel != 1)
        return 0;

    /* 查找空闲按键位置 */
    for (i = 0; i < BUTTON_NUM; i++) {
        if (buttonInputOutputNumbers[i] == 0xff) {
            buttonInputOutputNumbers[i] = inputOutputNumber;
            buttonTriggerLevels[i] = triggerLevel;
            buttonCurrentLevels[i] = !triggerLevel; /* 初始化为未按下状态 */
            return 1;
        }
    }
    return 0; /* 没有空闲按键位置 */
}

/**
 * @brief 注销按钮，注销后将不会进行对应按键扫描
 *
 * @param [in] inputOutputNumber 已绑定的按键对应的GPIO号
 */
void buttonDetach(uint8_t inputOutputNumber)
{
    uint8_t i;

    /* 查找要注销的按键 */
    for (i = 0; i < BUTTON_NUM; i++) {
        if (buttonInputOutputNumbers[i] == inputOutputNumber) {
            buttonInputOutputNumbers[i] = 0xff; /* 标记为未使用 */
            return;
        }
    }
}

/**
 * @brief 提供心跳，用于定期检测按键状态
 *
 * @param [in] milliseconds 调用间隔，单位毫秒，应当小于等于5ms，建议1-3ms最佳
 * @note 此函数应当在定时器中断中调用
 */
void buttonTickMilliseconds(uint8_t milliseconds)
{
    static uint8_t i;
    static uint8_t changeFlag = 0;

    /* 如果读取回调未注册，则退出 */
    if (readInputOutputCallback == NULL)
        return;

    /* 扫描所有按键 */
    for (i = 0; i < BUTTON_NUM; i++) {
        /* 跳过未注册的按键 */
        if (buttonInputOutputNumbers[i] == 0xff)
            continue;

        /* 读取按键当前电平状态 */
        if (readInputOutputCallback(buttonInputOutputNumbers[i])) /* 高电平 */
        {
            buttonLowLevelCounts[i] = 0;                          /* 清空低电平计数 */
            if (buttonHighLevelCounts[i] < BUTTON_SHAKE_MS)       /* 消抖计数 */
                buttonHighLevelCounts[i] += milliseconds;
            else if (buttonCurrentLevels[i] == 0) /* 消抖时间达到且上一次为低电平，表示电平变化 */
            {
                changeFlag = 1;                            /* 事件标志 */
                buttonCurrentLevels[i] = 1;                /* 翻转电平 */
            }
        } else                                             /* 低电平 */
        {
            buttonHighLevelCounts[i] = 0;                  /* 清空高电平计数 */
            if (buttonLowLevelCounts[i] < BUTTON_SHAKE_MS) /* 消抖计数 */
                buttonLowLevelCounts[i] += milliseconds;
            else if (buttonCurrentLevels[i] == 1) /* 消抖时间达到且上一次为高电平，表示电平变化 */
            {
                changeFlag = 1;
                buttonCurrentLevels[i] = 0;
            }
        }

        /* 按键状态处理 */
        if (buttonCurrentLevels[i] == buttonTriggerLevels[i]) /* 按键按下 */
        {
            if (changeFlag == 1) /* 电平刚刚变化为按下状态 */
            {
                changeFlag = 0;
#if BUTTON_DOWN_ENABLE
                /* 触发按下事件 */
                buttonWriteBuffer(buttonInputOutputNumbers[i], buttonDown);
#endif
#if BUTTON_LONG_PRESS_ENABLE
                /* 开始长按检测 */
                if ((buttonEventEnableState & buttonLongPress) != 0) {
                    longPressFlags[i] = 1; /* 长按预备标志 */
                    buttonLongPressTimeMilliseconds[i] = 0;
                }
#endif
#if BUTTON_DOUBLE_CLICK_ENABLE
                /* 双击检测 */
                if ((buttonEventEnableState & buttonDoubleClick) != 0) {
                    if (doubleClickFlags[i] == 0) { /* 第一次点击 */
                        doubleClickFlags[i] = 1;
                        buttonDoubleClickTimeMilliseconds[i] = 0;
                    } else if (doubleClickFlags[i] == 1) { /* 第二次点击，形成双击 */
                        buttonWriteBuffer(buttonInputOutputNumbers[i], buttonDoubleClick);
                        doubleClickFlags[i] = 2;
                    }
                }
#endif
            }

#if BUTTON_LONG_PRESS_ENABLE
            /* 长按检测 */
            if (longPressFlags[i] == 1) /* 长按计时中 */
            {
                buttonLongPressTimeMilliseconds[i] += milliseconds;
                if (buttonLongPressTimeMilliseconds[i] >= BUTTON_LONG_PRESS_TIME_DEFAULT) {
                    /* 达到长按时间，触发长按事件 */
                    buttonWriteBuffer(buttonInputOutputNumbers[i], buttonLongPress);
                    longPressFlags[i] = 0;
                }
            }
#endif

#if BUTTON_LONG_PRESS_TRIGGER_ENABLE
            /* 长按连续触发检测 */
            if (buttonEventEnableState & buttonLongPressTrigger) {
                if (longPressContinuousTriggerCount < longPressContinuousTriggerTimeMilliseconds)
                    longPressContinuousTriggerCount++;
                else {
                    longPressTriggerCount += milliseconds;
                    if (longPressTriggerCount >= longPressTriggerIntervalMilliseconds) {
                        if (longPressTriggerFlag == 0) {
                            /* 第一次触发调用回调函数 */
                            if (longPressTriggerCallback != NULL)
                                longPressTriggerCallback(longPressTriggerUserData);
                            longPressTriggerFlag = 1;
                        }
                        longPressTriggerCount -= longPressTriggerIntervalMilliseconds;
                        /* 触发连续长按事件 */
                        buttonWriteBuffer(buttonInputOutputNumbers[i], longPressTriggerType);
                    }
                }
            }
#endif
        } else if (changeFlag == 1) /* 按键弹起 */
        {
#if BUTTON_LONG_PRESS_TRIGGER_ENABLE
            /* 重置长按连续触发状态 */
            longPressTriggerCount = 0;
            longPressContinuousTriggerCount = 0;
            longPressTriggerFlag = 0;
#endif
            changeFlag = 0;

#if BUTTON_LONG_PRESS_ENABLE
            /* 长按状态处理 */
            {
                if (longPressFlags[i] == 1)              /* 按键时间短于长按 */
                {
                    longPressFlags[i] = 0;               /* 清除标志 */

#if (!BUTTON_DOUBLE_CLICK_ENABLE && BUTTON_CLICK_ENABLE) /* 未启用双击但启用了单击 */
                    buttonWriteBuffer(buttonInputOutputNumbers[i], buttonClick);
#endif
#if BUTTON_DOUBLE_CLICK_ENABLE && BUTTON_CLICK_ENABLE
                    if ((buttonEventEnableState & buttonDoubleClick) == 0) /* 关闭双击使能 */
                        buttonWriteBuffer(buttonInputOutputNumbers[i], buttonClick);
#endif
                }
#if BUTTON_DOUBLE_CLICK_ENABLE && BUTTON_CLICK_ENABLE
                if ((buttonEventEnableState & buttonLongPress) == 0)       /* 长按动态关闭 */
                {
                    if ((buttonEventEnableState & buttonDoubleClick) == 0) /* 关闭双击使能 */
                        buttonWriteBuffer(buttonInputOutputNumbers[i], buttonClick);
                }
#endif
            }
#else
#if BUTTON_DOUBLE_CLICK_ENABLE && BUTTON_CLICK_ENABLE
            /* 未启用长按时的单击处理 */
            if ((buttonEventEnableState & buttonDoubleClick) == 0) /* 关闭双击使能 */
                buttonWriteBuffer(buttonInputOutputNumbers[i], buttonClick);
            else if (doubleClickFlags[i] == 0)
                buttonWriteBuffer(buttonInputOutputNumbers[i], buttonClick);
#endif
#endif

#if BUTTON_UP_ENABLE
            /* 触发弹起事件 */
            buttonWriteBuffer(buttonInputOutputNumbers[i], buttonUp);
#endif

#if (BUTTON_LONG_PRESS_ENABLE == 0 && BUTTON_DOUBLE_CLICK_ENABLE == 0 && BUTTON_CLICK_ENABLE)
            /* 未启用长按、双击，但启用单击时 */
            buttonWriteBuffer(buttonInputOutputNumbers[i], buttonClick);
#endif

#if BUTTON_DOUBLE_CLICK_ENABLE
            /* 重置双击状态 */
            if (doubleClickFlags[i] == 2)
                doubleClickFlags[i] = 0;
#endif
        }

#if BUTTON_DOUBLE_CLICK_ENABLE
        /* 双击超时检测 */
        if (doubleClickFlags[i] == 1) /* 第一次点击后等待第二次点击 */
        {
            buttonDoubleClickTimeMilliseconds[i] += milliseconds;
            if (buttonDoubleClickTimeMilliseconds[i] >= BUTTON_DOUBLE_CLICK_TIME_DEFAULT) {
                /* 超时未收到第二次点击，视为单击 */
#if (BUTTON_LONG_PRESS_ENABLE && BUTTON_CLICK_ENABLE)
                if (longPressFlags[i] == 0) {
#endif
#if BUTTON_CLICK_ENABLE
                    if (buttonCurrentLevels[i] != buttonTriggerLevels[i])
                        buttonWriteBuffer(buttonInputOutputNumbers[i], buttonClick);
#endif
#if (BUTTON_LONG_PRESS_ENABLE && BUTTON_CLICK_ENABLE)
                }
#endif
                doubleClickFlags[i] = 0;
            }
        }
#endif
    }
}

/**
 * @brief 绑定读取指定IO口电平状态的回调函数
 *
 * @param [in] callback 回调函数，格式为：uint8_t callback(uint8_t inputOutputNumber)
 * @note 回调函数参数为IO号（0-254），返回值为电平状态（0或1）
 */
void buttonAttachReadInputOutputFunction(uint8_t (*callback)(uint8_t inputOutputNumber))
{
    /* 保存回调函数指针 */
    readInputOutputCallback = callback;

    /* 初始化按钮数组 */
    memset(buttonInputOutputNumbers, 255, BUTTON_NUM);

#if BUTTON_LONG_PRESS_ENABLE
    /* 初始化长按标志 */
    memset(longPressFlags, 0, BUTTON_NUM);
#endif

#if BUTTON_DOUBLE_CLICK_ENABLE
    /* 初始化双击标志 */
    memset(doubleClickFlags, 0, BUTTON_NUM);
#endif

    /* 初始化缓冲区指针和计数 */
    buttonEventWritePointer = 0;
    buttonEventReadPointer = 0;
    buttonEventCount = 0;

    /* 初始化事件使能状态 */
    buttonEventEnableState = 0;
    buttonEventEnableState |= BUTTON_DOWN_ENABLE;
    buttonEventEnableState |= BUTTON_UP_ENABLE << 1;
    buttonEventEnableState |= BUTTON_LONG_PRESS_ENABLE << 2;
    buttonEventEnableState |= BUTTON_CLICK_ENABLE << 3;
    buttonEventEnableState |= BUTTON_DOUBLE_CLICK_ENABLE << 4;
    buttonEventEnableState |= BUTTON_LONG_PRESS_TRIGGER_ENABLE << 5;
}

/**
 * @brief 读取按键缓冲区的数据
 *
 * @param [out] inputOutputNumber 返回事件对应的按键IO号
 * @param [out] eventType 返回按键事件类型
 * @note 读取后对应键值自动从缓冲区中删除
 */
void buttonReadEvent(uint8_t* inputOutputNumber, ButtonEventType* eventType)
{
    /* 缓冲区为空 */
    if (buttonEventCount == 0) {
        if (inputOutputNumber != NULL)
            *inputOutputNumber = 255;
        if (eventType != NULL)
            *eventType = buttonNotPress;
        return;
    }

    /* 读取事件信息 */
    if (inputOutputNumber != NULL)
        *inputOutputNumber = buttonEventInputOutputNumbers[buttonEventReadPointer];
    if (eventType != NULL)
        *eventType = buttonEventTypes[buttonEventReadPointer];

    /* 更新读指针和计数 */
    buttonEventReadPointer++;
    if (buttonEventReadPointer == BUTTON_BUFFER_NUM)
        buttonEventReadPointer = 0;
    buttonEventCount--;
}

/**
 * @brief 启用指定按键事件功能
 *
 * @param [in] configurationType 要启用的事件类型，可以按位或组合多个事件
 * @note 可用值：buttonDown, buttonUp, buttonLongPress, buttonClick, buttonDoubleClick,
 * buttonEventAll
 */
void buttonEnableEvent(uint8_t configurationType) { buttonEventEnableState |= configurationType; }

/**
 * @brief 停用指定按键事件功能
 *
 * @param [in] configurationType 要停用的事件类型，可以按位或组合多个事件
 * @note 可用值：buttonDown, buttonUp, buttonLongPress, buttonClick, buttonDoubleClick,
 * buttonEventAll
 */
void buttonDisableEvent(uint8_t configurationType) { buttonEventEnableState &= ~configurationType; }

/** @} */

#endif /* BUTTON_NUM */
