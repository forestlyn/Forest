# Forest

轻量级 C++ 引擎示例与演示工程（Windows + Visual Studio）。包含一个作为动态库的 Engine（Engine/），和一个基于 Engine 的示例程序 Forest（Forest/）。

## 概览

- Engine：动态库，包含核心（Core）和事件系统（Events）。
  - 核心（Core）负责程序入口、日志与应用程序基类。
    - 应用基类：[`Engine::Core::Application`](Engine/src/Engine/Core/Application.h)
    - 入口示例 / main：[`Engine::Core::EntryPoint`](Engine/src/Engine/Core/EntryPoint.h)
    - 日志封装：[`Engine::Core::Log`](Engine/src/Engine/Core/Log.h), 实现文件 [`Log.cpp`](Engine/src/Engine/Core/Log.cpp)
  - 事件（Events）实现了基础事件类型与分发器：
    - 事件基类与分发器：[`Engine::Event::Event`](Engine/src/Engine/Events/Event.h) / [`Engine::Event::EventDispatcher`](Engine/src/Engine/Events/Event.h)
    - 键盘事件：[`Engine::Event::KeyEvent`](Engine/src/Engine/Events/KeyEvent.h)
    - 鼠标事件：[`Engine::Event::MouseMovedEvent` 等](Engine/src/Engine/Events/MouseEvent.h)
    - 窗口事件：[`Engine::Event::WindowResizeEvent` 等](Engine/src/Engine/Events/WindowEvent.h)
- ThirdParty：集成了 `spdlog`（日志库），源码在 [`Engine/ThirdParty/spdlog`](Engine/ThirdParty/spdlog)。
- Forest：示例应用，实现了 [`Engine::Core::CreateApplication()`](Forest/src/ForestApp.cpp) 并返回示例应用 `ForestApp`。


## 快速构建
1. 克隆仓库：
    ```sh
    git clone git@github.com:forestlyn/Forest.git
    cd Forest
    ```

2. module 初始化：
   ```sh
   git submodule update --init --recursive
   ```

3. 创建构建目录：
   这是Debug版本的构建示例：
   ```sh
   mkdir build
   cd build
   cmake ..
   cmake --build . --config Debug
   ```

   生成的可执行文件在 `build/bin/Debug/Forest.exe`。        