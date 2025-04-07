/**
 * @file keyValueMap.hpp
 * @brief 键值对管理头文件
 * @author konbakuyomu
 * @version 1.0
 * @date 2024-11-14
 *
 * @details 利用C++ std::map 实现键值对管理，支持注册回调函数，发送消息，触发回调
 *
 */

#pragma once
#ifdef __cplusplus

#include "FreeRTOS.h"
#include "semphr.h"
#include <functional>
#include <map>
#include <memory>
#include <queue>
#include <string>

using Callback = std::function<void(void*, size_t)>;

class KeyValueManager
{
public:
    KeyValueManager() { mutex_ = xSemaphoreCreateMutex(); }

    ~KeyValueManager()
    {
        if (mutex_ != nullptr) {
            vSemaphoreDelete(mutex_);
        }
    }

    static KeyValueManager* getInstance();
    // 注册回调
    bool registerKey(const std::string& key, Callback cb);
    // 删除注册（支持延迟删除）
    bool unregisterKey(const std::string& key);
    // 发送消息，触发回调
    bool sendMessage(const std::string& key, void* value, size_t length);

private:
    // 删除键值对
    bool doUnregister(const std::string& key);
    // 处理延迟删除的键值对
    void processPendingDeletes();

    std::map<std::string, Callback> callbacks_;
    std::queue<std::string> pending_deletes_;
    SemaphoreHandle_t mutex_;
    bool msg_busy_ = false;
    static std::unique_ptr<KeyValueManager> m_kv;
};

#endif
