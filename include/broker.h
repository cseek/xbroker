/*
 * @Author: aurson jassimxiong@gmail.com
 * @Date: 2025-09-14 17:33:37
 * @LastEditors: aurson jassimxiong@gmail.com
 * @LastEditTime: 2026-01-22 17:07:25
 * @Description:
 *     ___ ___ _________ ___  ___
 *    / _ `/ // / __(_-</ _ \/ _ \
 *    \_,_/\_,_/_/ /___/\___/_//_/
 *
 * Copyright (c) 2025 by Aurson, All Rights Reserved.
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
#ifndef BROKER_H
#define BROKER_H

#include <functional>
#include <unordered_map>
#include <shared_mutex>
#include <mutex>
#include <string>
#include <vector>

template <typename DataType>
class Broker {
public:
    using Topic = std::string;
    using Callback = std::function<void(const DataType &)>;
    using IdMap = std::unordered_map<uint64_t, Callback>;

    bool subscribe(uint64_t sid_, const Topic &topic, const Callback &callback) {
        std::unique_lock<std::shared_mutex> slock(mutex_);
        auto &idmap = topic_map_[topic];
        auto it = idmap.find(sid_);
        if (it == idmap.end()) {
            idmap[sid_] = callback;
            return true;
        }
        return false;
    }

    void unsubscribe(uint64_t sid_, const Topic &topic) {
        std::unique_lock<std::shared_mutex> slock(mutex_);
        auto it = topic_map_.find(topic);
        if (it != topic_map_.end()) {
            it->second.erase(sid_);
            if (it->second.empty()) {
                topic_map_.erase(it);
            }
        }
    }

    void publish(const Topic &topic, const DataType &data) {
        // 防止死锁
        std::vector<Callback> callbacks;
        {
            std::shared_lock<std::shared_mutex> slock(mutex_);
            auto topic_it = topic_map_.find(topic);
            if (topic_it != topic_map_.end()) {
                callbacks.reserve(topic_it->second.size());
                for (const auto &it : topic_it->second) {
                    if (it.second) {
                        callbacks.push_back(it.second);
                    }
                }
            }
        }
        for (const auto &cb : callbacks) {
            try {
                cb(data);
            } catch (...) {
            }
        }
    }

private:
    std::shared_mutex mutex_;
    std::unordered_map<Topic, IdMap> topic_map_;
};

#endif // BROKER_H