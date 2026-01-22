#ifndef BROKER_H
#define BROKER_H

#include <functional>
#include <unordered_map>
#include <shared_mutex>
#include <mutex>
#include <string>
#include <vector>
#include <cstdint>

namespace Aurson {
    using Sid = uint64_t;
    using Topic = std::string;
    template <typename DataType>
    class Broker {
    private:
        Broker(const Broker &) = delete;
        Broker &operator=(const Broker &) = delete;
        Broker() = default;
        ~Broker() = default;

    public:
        using Callback = std::function<void(const DataType &)>;
        using IdMap = std::unordered_map<Sid, Callback>;

    public:
        static Broker &instance() {
            static Broker obj;
            return obj;
        }

        bool subscribe(Sid sid, const Topic &topic, const Callback &callback) {
            std::unique_lock<std::shared_mutex> uslocker(mutex_);
            auto &sid_map = topic_map_[topic];
            auto it = sid_map.find(sid);
            if (it == sid_map.end()) {
                sid_map[sid] = callback;
                return true;
            }
            return false;
        }

        void unsubscribe(Sid sid, const Topic &topic) {
            std::unique_lock<std::shared_mutex> uslocker(mutex_);
            auto it = topic_map_.find(topic);
            if (it != topic_map_.end()) {
                it->second.erase(sid);
                if (it->second.empty()) {
                    topic_map_.erase(it);
                }
            }
        }

        void publish(const Topic &topic, const DataType &data) {
            // 防止死锁
            std::vector<Callback> callbacks;
            {
                std::shared_lock<std::shared_mutex> sslocker(mutex_);
                auto it1 = topic_map_.find(topic);
                if (it1 != topic_map_.end()) {
                    callbacks.reserve(it1->second.size());
                    for (const auto &it2 : it1->second) {
                        if (it2.second) {
                            callbacks.push_back(it2.second);
                        }
                    }
                }
            }
            for (const auto &cb : callbacks) {
                try {
                    if (cb) {
                        cb(data);
                    }
                } catch (...) {
                }
            }
        }

    private:
        std::shared_mutex mutex_;
        std::unordered_map<Topic, IdMap> topic_map_;
    };
    class Publisher {
    public:
        template <typename DataType>
        void publish(const Topic &topic, const DataType &data) {
            Broker<DataType>::instance().publish(topic, data);
        }
    };
    class Subscriber {
    public:
        Subscriber() {
            sid_ = request_sid();
        }

        template <typename DataType>
        bool subscribe(const Topic &topic,
            const typename Broker<DataType>::Callback &callback) {
            return Broker<DataType>::instance().subscribe(sid_, topic, callback);
        }

        template <typename DataType>
        void unsubscribe(const Topic &topic) {
            Broker<DataType>::instance().unsubscribe(sid_, topic);
        }

    private:
        Sid request_sid() {
            static Sid sid = 0;
            static std::mutex mutex_;
            std::lock_guard<std::mutex> glck(mutex_);
            return sid++;
        }

    private:
        Sid sid_ = 0;
    };
} // namespace Aurson

#endif // BROKER_H