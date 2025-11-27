/*
 * @Author: aurson jassimxiong@gmail.com
 * @Date: 2025-09-14 17:33:37
 * @LastEditors: aurson jassimxiong@gmail.com
 * @LastEditTime: 2025-11-27 14:30:49
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
#ifndef __SUBSCRIBER_H__
#define __SUBSCRIBER_H__

#include "broker.h"
#include "singleton.h"
#include <cstdint>
#include <mutex>

class MakeId {
public:
    uint64_t request() {
        std::lock_guard<std::mutex> glck(mutex_);
        return count_++;
    }

private:
    uint64_t count_ = 0;
    std::mutex mutex_;
};
class Subscriber {
public:
    Subscriber() {
        id_ = Singleton<MakeId>::instance().request();
    }

    template <typename DataType>
    bool subscribe(const typename Broker<DataType>::Topic &topic,
        const typename Broker<DataType>::Callback &callback) {
        return Singleton<Broker<DataType>>::instance().subscribe(id_, topic, callback);
    }

    template <typename DataType>
    void unsubscribe(const typename Broker<DataType>::Topic &topic) {
        Singleton<Broker<DataType>>::instance().unsubscribe(id_, topic);
    }

    uint64_t id() const {
        return id_;
    }

private:
    uint64_t id_;
};
#endif // __SUBSCRIBER_H__