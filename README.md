
# xbroker

轻量级的基于主题的发布/订阅（Publisher / Subscriber）示例库，使用模板与单例模式实现线程安全的消息分发。

项目目录结构

- `include/`：头文件，实现了 `Broker`、`Publisher`、`Subscriber`、`Singleton` 等。
- `src/`：示例/入口 `main.cpp`。
- `Makefile`：构建脚本（已提供 `debug` / `release` / `clean` / `run` 目标）。

要求

- Linux / Unix 环境
- `g++` 支持 C++17（例如 `g++` >= 7）
- `make`

构建

在仓库根目录运行：

```bash
make          # 默认构建 release
make debug    # 带调试信息的构建
make clean    # 清理
make run      # 构建并运行（相当于 ./xbroker）
```

生成的可执行文件为 `./xbroker`。

使用说明与示例

代码通过 `Broker<DataType>` 模板维护话题到回调的映射；`Publisher` 用于发布消息，`Subscriber` 用于订阅并接收回调。

下面是一个简单的使用示例（可把它放到 `src/main.cpp` 中运行或按需调整）：

```cpp
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include "publisher.h"
#include "subscriber.h"

int main() {
	Publisher pub;
	Subscriber sub;

	// 订阅一个 string 类型的话题 "news"
	sub.subscribe<std::string>("news", [](const std::string &msg){
		std::cout << "收到消息: " << msg << std::endl;
	});

	// 发布消息
	pub.publish<std::string>("news", std::string("hello xbroker"));

	// 为了演示，多线程或延时发布也可以
	std::thread t([&](){
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		pub.publish<std::string>("news", std::string("from thread"));
	});
	t.join();

	return 0;
}
```

实现细节说明

- `Broker` 使用 `std::shared_mutex` + `std::shared_lock` 来在发布时安全地读取订阅者列表并避免长时间持锁。
- `Subscriber` 使用 `MakeId` + `Singleton` 来为每个订阅者生成唯一 ID，方便退订。
- `Publisher` 仅是对 `Broker` 的包装，简化调用。

下一步建议

- 若希望有更完整的示例，是否要我将上面的示例直接写入 `src/main.cpp` 并构建运行？
- 可以加入单元测试或更多示例（多类型消息、并发发布性能演示等）。
