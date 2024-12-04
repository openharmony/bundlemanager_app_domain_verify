/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef APP_DOMAIN_VERIFY_APP_DETAILS_LRU_H
#define APP_DOMAIN_VERIFY_APP_DETAILS_LRU_H

#include <string>
#include <unordered_map>
#include <list>
#include <mutex>
#include <memory>

namespace OHOS {
namespace AppDomainVerify {
template <typename K, typename V>
class LruCacheUtil {
public:
    LruCacheUtil<K, V>() = default;
    LruCacheUtil<K, V>(int maxCap):maxCap_(maxCap){};
    bool SetMaxCap(int cap);
    bool Get(const K& key, V& value);
    void Put(const K& key, const V& value);
    void Clear();
    bool IsEmpty();
private:
    std::atomic<size_t> maxCap_ = 10;
    std::list<std::pair<K, V>> m_cache;
    std::unordered_map<K, typename std::list<std::pair<K, V>>::iterator> lruMap_;
};

template<typename K, typename V>
bool LruCacheUtil<K, V>::SetMaxCap(int cap) {
    maxCap_ = cap;
};

template<typename K, typename V>
bool LruCacheUtil<K, V>::Get(const K& key, V& value) {
    if (lruMap_.find(key) == lruMap_.end()) {
        return false;
    }
    value = lruMap_[key]->second;
    m_cache.erase(lruMap_[key]);
    m_cache.push_front({key, value});
    lruMap_[key] = m_cache.begin();
    return true;
};

template<typename K, typename V>
void LruCacheUtil<K, V>::Put(const K& key, const V& value) {
    auto iter = lruMap_.find(key);
    if (iter != lruMap_.end()) {
        V val = lruMap_[key]->second;
        m_cache.erase(iter->second);
        m_cache.push_front({key, val});
        lruMap_[key] = m_cache.begin();
        return;
    }
    if (lruMap_.size() == maxCap_) {
        lruMap_.erase(m_cache.back().first);
        m_cache.pop_back();
    }
    m_cache.push_front({key, value});
    lruMap_[key] = m_cache.begin();
    return;
};

template<typename K, typename V>
void LruCacheUtil<K, V>::Clear() {
    m_cache.clear();
    lruMap_.clear();
};

template<typename K, typename V>
bool LruCacheUtil<K, V>::IsEmpty() {
    return lruMap_.empty();
};

}
}

#endif