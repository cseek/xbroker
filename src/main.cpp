/*
 * @Author: aurson jassimxiong@gmail.com
 * @Date: 2025-09-14 17:33:37
 * @LastEditors: aurson jassimxiong@gmail.com
 * @LastEditTime: 2026-01-22 16:30:48
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
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include "publisher.h"
#include "subscriber.h"

int main() {
    Publisher publisher;
    Subscriber subscriber;

    subscriber.subscribe<std::string>("/news", [](const std::string &msg) {
        std::cout << "#1   topic: /news, message: " << msg << std::endl;
    });
    subscriber.subscribe<std::string>("/news1", [](const std::string &msg) {
        std::cout << "#2   topic: /news1, message: " << msg << std::endl;
    });
    subscriber.subscribe<std::string>("/news", [](const std::string &msg) {
        std::cout << "#3   topic: /news, message: " << msg << std::endl;
    });

    while (true) {
        publisher.publish<std::string>("/news", std::string("hello xbroker"));
        publisher.publish<std::string>("/news1", std::string("hello xbroker1"));
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    return 0;
}