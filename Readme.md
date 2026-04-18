# Forest

Forest 是一个基于 C++20 的 2D 游戏引擎，当前仓库按三层组织：

- Engine 核心层（运行时、渲染、场景、脚本宿主）
- EngineEditor 编辑层（编辑器应用与面板工作流）
- ScriptCore 脚本支持层（C# 侧 API 与托管脚本）

本文档聚焦这三层的职责边界、调用关系与构建方式。

## 仓库总览

根 CMake 会统一编译以下目标：

- Engine（静态库）
- EngineEditor（编辑器可执行文件）
- ScriptCore（C# 运行库）
- Sandbox（项目示例，位于 EngineEditor/Sandbox/Assets/Scripts）

关键目录：

- Engine/
- EngineEditor/
- ScriptCore/
- Forest/
- Scripts/

## 一、Engine 核心层

Engine 核心层提供应用生命周期、窗口与事件、渲染抽象、场景系统，以及脚本运行时宿主。

### 1) 生命周期与主循环

- 程序入口在 Engine/src/Engine/Core/EntryPoint.h。
- 入口会调用 CreateApplication(...) 创建具体应用，再依次执行 Init() / Run() / 析构。
- 核心循环在 Engine/src/Engine/Core/Application.cpp::Run：
  - 处理窗口更新与事件
  - 执行主线程任务队列（前/后队列）
  - 驱动 LayerStack 的 OnUpdate 与 OnImGuiRender
  - 刷新渲染命令

### 2) 渲染与线程化提交

- 渲染抽象在 Engine/src/Engine/Renderer。
- Application 提供提交接口：
  - SubmitRendererCommand(...)
  - FlushRendererCommands()
- 当启用 ENGINE_ENABLE_RENDERTHREAD 时，渲染命令通过提交队列与执行队列在渲染线程消费，即逻辑与渲染分离。

### 3) 场景与实体

- 场景核心在 Engine/src/Engine/Scene/Scene.h。
- 基于 entt 管理实体与组件，支持：
  - 编辑态更新 OnUpdateEditor
  - 运行态更新 OnUpdateRuntime
  - 模拟态更新 OnUpdateSimulate
- 集成 Box2D 物理世界与步进控制。

### 4) 脚本宿主（Native 侧）

- 宿主入口：Engine/src/Engine/Scripts/ScriptEngine.h 与 ScriptEngine.cpp。
- 主要职责：
  - 初始化 Mono Runtime
  - 加载 Engine-ScriptCore.dll 与 Sandbox.dll
  - 反射托管类型，构建脚本类与字段缓存
  - 驱动实体脚本 OnCreate / OnUpdate
  - 监听程序集变化并执行热重载
- C# 与 C++ 的桥接在 Engine/src/Engine/Scripts/ScriptGlue.cpp（mono_add_internal_call 注册）。

### 5) 项目系统

- 项目定义与活动项目管理：Engine/src/Engine/Project/Project.h。
- 支持创建/加载/保存 .forestproj，并解析资源目录、启动场景与脚本目录。

### 6) 序列化与反序列化（YAML）

Engine 的序列化模块位于 Engine/src/Engine/Serialization，采用 yaml-cpp 实现，核心分层如下：

- 场景级：SceneSerialize
  - 文件：Engine/src/Engine/Serialization/SceneSerialize.h、SceneSerialize.cpp
  - Serialize() 会输出 Scene + Entities 结构，并在写入前自动创建目录。
  - Deserialize() 会解析 YAML，按 EntityID 重建实体并委托实体反序列化。

- 实体级：EntitySerialize
  - 文件：Engine/src/Engine/Serialization/EntitySerialize.h、EntitySerialize.cpp
  - 负责实体壳（EntityID）以及各组件的统一编排。
  - ScriptComponent 除基础类名外，还会序列化脚本字段表（键名形如 ClassName_Fields）。

- 组件级：ComponentSerialize
  - 文件：Engine/src/Engine/Serialization/ComponentSerialize.h、ComponentSerialize.cpp
  - 已覆盖常用组件：Tag、Transform、Camera、Sprite、Circle、Rigidbody2D、BoxCollider2D、CircleCollider2D、Script。
  - 反序列化对缺失字段使用默认值并记录警告日志，保证旧数据文件具备一定兼容性。

- 项目级：ProjectSerialize
  - 文件：Engine/src/Engine/Serialization/ProjectSerialize.h、ProjectSerialize.cpp
  - 读写 .forestproj，主要字段包含 Name、AssetDirectory、StartScene、ScriptDLLDirectory。

- 基础类型适配：SerializeUtils
  - 文件：Engine/src/Engine/Serialization/SerializeUtils.h、SerializeUtils.cpp
  - 提供 glm::vec2/vec3/vec4 与 UUID 的 YAML 编解码扩展。

脚本字段持久化说明：

- Script 字段会按 ScriptFieldType 写入 Type + Value。
- 支持 float/double/bool/int/uint/long/ulong/vector2/3/4/entity 等类型。
- 反序列化后字段值会进入 ScriptEngine 的实体字段缓存（EntityFieldMaps），运行时创建脚本实例时再回填到托管对象。

## 二、EngineEditor 编辑层

EngineEditor 是基于 Engine 运行时的编辑器应用，负责内容编辑、场景运行控制与工具面板。

### 1) 编辑器应用入口

- 应用入口在 EngineEditor/src/EngineEditorApp.cpp。
- CreateApplication(...) 会创建 EngineEditorApp，并配置：
  - WorkingDirectory = ../../../EngineEditor/
  - MonoAssemblyPath = ../Engine/ThirdParty/mono/4.5
  - AppScriptPath = ../EngineEditor/Sandbox/Assets/Scripts/src

### 2) 编辑器主层

- 编辑器主 Layer 在 EngineEditor/src/EngineEditor.h 与 EngineEditor.cpp。
- 核心能力：
  - DockSpace + 菜单栏 + 场景视口
  - FrameBuffer 离屏渲染与拾取
  - Gizmo 操作（ImGuizmo）
  - 场景状态切换（Edit / Play / Simulate）
  - 项目与场景的新建/加载/保存

### 3) 面板体系

- 典型面板包括 SceneHierarchyPanel 与 ContentBrowserPanel（EngineEditor/src/Panels）。
- 资源拖拽到 Scene 视口后可触发场景加载。

序列化入口（编辑器）：

- 项目保存/加载通过 Engine::Project::SaveActiveProject / Load（内部走 ProjectSerialize）。
- 场景保存/加载通过 Engine::Serialization::SceneSerialize::Serialize / Deserialize。
- 主要调用点在 EngineEditor/src/EngineEditor.cpp 的 SaveProject/LoadProject/SaveScene/LoadScene。

### 4) 编辑器中的脚本编译与热更新

- EngineEditorApp 监听脚本源码目录变更（filewatch）。
- 变更后会触发 CMake 构建 Sandbox 目标。
- ScriptEngine 侧会监听 DLL 变化并在主线程执行程序集重载。

## 三、ScriptCore 脚本支持层

ScriptCore 是 C# 层的基础 API 程序集，为业务脚本（Sandbox）提供可继承实体类与引擎接口包装。

### 1) ScriptCore（Engine-ScriptCore.dll）

- CMake 目标：ScriptCore（ScriptCore/CMakeLists.txt）。
- 输出目录：EngineEditor/resources/scripts/bin。
- 关键 API：
  - Entity：ScriptCore/src/Engine/Scene/Entity.cs
  - Component 封装：ScriptCore/src/Engine/Scene/Components
  - Input：ScriptCore/src/Engine/Input/Input.cs
  - Log：ScriptCore/src/Engine/Log.cs
  - 数学类型：ScriptCore/src/Engine/Math.cs

### 2) Sandbox（业务脚本层）

- CMake 目标：Sandbox（EngineEditor/Sandbox/Assets/Scripts/CMakeLists.txt）。
- 脚本源码目录：EngineEditor/Sandbox/Assets/Scripts/src。
- 通过 target_link_libraries(Sandbox PRIVATE ScriptCore) 依赖 ScriptCore。

### 3) 三层调用链（脚本方向）

- C# 脚本继承 Engine.Entity。
- ScriptGlue 注册 InternalCall，将 C# 调用映射到 C++。
- ScriptEngine 在运行时创建脚本实例并驱动生命周期。

## 依赖关系（简版）

- EngineEditor -> Engine
- Forest -> Engine
- Sandbox -> ScriptCore
- Engine（ScriptEngine）-> 加载 ScriptCore/Sandbox 生成的 DLL

## 构建与运行（Windows）

### 1) 前置依赖

- Visual Studio 2022（建议包含 C++ 与 CMake 工作负载）
- CMake >= 3.15
- Python（用于执行 Scripts/Setup.py）
- Vulkan SDK（仓库当前检查版本为 1.4.335.0，依赖环境变量 VULKAN_SDK）

建议先执行：

```bat
git submodule update --init --recursive
cd Scripts
python Setup.py
```

### 2) 生成与构建

```bat
mkdir build
cd build
cmake ..
cmake --build . --config Debug
```

### 3) 常用目标

- 启动编辑器：EngineEditor（输出名通常为 EngineEditor_debug.exe）
- 示例运行：Forest（输出名通常为 Forest_debug.exe）
- 脚本 API：ScriptCore（生成 Engine-ScriptCore.dll）
- 业务脚本：Sandbox（生成 Sandbox.dll）

默认可执行输出目录在 build/bin/<Config>/。
脚本 DLL 默认输出在 EngineEditor/resources/scripts/bin/。