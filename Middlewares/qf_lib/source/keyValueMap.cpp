/*
 * @file key_value_map.cpp
 * @brief 键值对管理实现文件
 * @author konbakuyomu
 * @version 1.0
 * @date 2024-11-14
 */

#include "keyValueMap.hpp"

std::unique_ptr<KeyValueManager> KeyValueManager::m_kv = nullptr;

/**
 * @brief 获取实例
 * @return 实例指针
 */
KeyValueManager* KeyValueManager::getInstance()
{
    if (m_kv == nullptr) {
        m_kv = std::make_unique<KeyValueManager>();
    }
    return m_kv.get();
}

/**
 * @brief 注册键值对
 * @param key 键
 * @param cb 回调函数
 * @return 是否注册成功
 */
bool KeyValueManager::registerKey(const std::string& key, Callback cb)
{
    if (xSemaphoreTake(mutex_, portMAX_DELAY) == pdTRUE) {
        auto result = callbacks_.emplace(key, cb);
        xSemaphoreGive(mutex_);
        return result.second;
    }
    return false;
}

/**
 * @brief 删除键值对
 * @param key 键
 * @return 是否删除成功
 */
bool KeyValueManager::unregisterKey(const std::string& key)
{
    /* 如果消息正在发送，则将键添加到延迟删除队列 */
    if (msg_busy_) {
        if (xSemaphoreTake(mutex_, portMAX_DELAY) == pdTRUE) {
            pending_deletes_.push(key);
            xSemaphoreGive(mutex_);
            return true;
        }
        return false;
    }

    /* 如果消息未发送，则直接删除键值对 */
    return doUnregister(key);
}

/**
 * @brief 发送消息
 * @param key 键
 * @param value 值
 * @param length 长度
 * @return 是否发送成功
 */
bool KeyValueManager::sendMessage(const std::string& key, void* value, size_t length)
{
    if (xSemaphoreTake(mutex_, portMAX_DELAY) != pdTRUE) {
        return false;
    }

    msg_busy_ = true;
    auto it = callbacks_.find(key);
    bool result = false;

    if (it != callbacks_.end()) {
        it->second(value, length);
        result = true;
    }

    msg_busy_ = false;
    processPendingDeletes(); // 处理所有待删除的键值对

    xSemaphoreGive(mutex_);
    return result;
}

/**
 * @brief 删除键值对
 * @param key 键
 * @return 是否删除成功
 */
bool KeyValueManager::doUnregister(const std::string& key)
{
    if (xSemaphoreTake(mutex_, portMAX_DELAY) == pdTRUE) {
        // erase 方法会返回实际删除的元素数量
        // 对于 map 来说，由于键是唯一的，所以返回值只可能是 0 或 1
        // 返回 0 表示没有找到要删除的键, 返回 1 表示成功删除了一个键值对
        auto result = callbacks_.erase(key) > 0;
        xSemaphoreGive(mutex_);
        return result;
    }
    return false;
}

/**
 * @brief 处理延迟删除的键值对
 */
void KeyValueManager::processPendingDeletes()
{
    /* 处理队列中所有待删除的key */
    while (!pending_deletes_.empty()) {
        auto key = pending_deletes_.front(); // 获取队首元素
        pending_deletes_.pop();              // 将该元素从队列中移除
        callbacks_.erase(key); // 从回调映射表中删除对应的键值对
    }
}
