# ForestRuntime（开发期独立运行器）

运行器和编辑器共享 `.forestproj` 中的脚本配置。脚本产物按项目隔离，不再输出到编辑器公共目录。
当前仍依赖构建时仓库中的引擎资源、Mono 和 ScriptCore，尚不包含游戏打包。

## 构建与运行

在仓库根目录执行：

```powershell
cmake -S . -B build
cmake --build build --config Debug --target ForestRuntime EngineEditor
# 只编译指定项目的脚本（不创建窗口）
./build/bin/Debug/ForestRuntime_debug.exe ./EngineEditor/Sandbox/Sandbox.forestproj --build-only
# 直接运行项目，不需要再指定 DLL
./build/bin/Debug/ForestRuntime_debug.exe ./EngineEditor/Sandbox/Sandbox.forestproj
# 也可以先构建、成功后再启动；构建失败返回非零状态，不启动旧 DLL
./build/bin/Debug/ForestRuntime_debug.exe ./EngineEditor/Sandbox/Sandbox.forestproj --build-scripts
```

`cmake --build build --config Debug --target Sandbox TestProject` 是两个示例项目的构建快捷入口。
它们调用相同的项目构建服务。新建项目无需向引擎根 CMake 添加目标。

## 项目配置

```yaml
ScriptAssembly: Scripts/bin/{config}/Game.dll
ScriptSourceDirectory: Scripts/src
ScriptBuildConfiguration: Debug
```

前两项相对于 `AssetDirectory`；`{config}` 替换为 `ScriptBuildConfiguration`（Debug 或 Release）。
默认源码目录为 `Scripts/src`，构建配置为 Debug；空的 ScriptAssembly 表示不构建、不加载游戏程序集。
Debug/Release 产物默认分目录。C# 命名空间与 DLL 文件名互相独立。
构建使用 VS 2022 的 C# 工具链以及引擎当前的 ScriptCore，递归收集源码目录中的 `.cs` 文件。
运行器自身的 C++ 构建配置不改变项目选定的脚本构建配置。

- 构建中间文件和日志：`项目/Intermediate/Scripts/<配置>/`。
- 失败时在 DLL 旁留下 `.build-failed` 状态文件；直接运行也会拒绝使用该产物，直到成功重建。
- 手动修改源码后直接运行不会自动检查源码版本；需要 `--build-scripts` 或先执行 `--build-only`。
- `--script-assembly <dll>` 仍可覆盖加载路径，相对于启动目录；不能和构建选项同时使用。
- 当前构建驱动提供 ScriptCore、System、System.Core、System.Numerics 引用；额外第三方托管依赖配置暂未提供。

编辑器打开项目时构建并加载项目脚本；切换项目时同步切换程序集和源码监听目录。
源码变化会在编辑状态下延迟约 500ms 构建；运行中修改代码则等停止后处理。
菜单 **Build Scripts / Ctrl+R** 可手动重建。Play 前也会构建，失败会显示 Script Build Error 并阻止 Play。
构建当前为同步操作，期间编辑器可能短暂停顿。构建失败后不会继续加载上一份游戏程序集。

## 运行与验证

启动场景需要主相机。运行器支持窗口缩放、现有键鼠输入和物理更新。
失焦后继续更新，最小化时暂停；默认关闭 ImGui、性能面板、脚本调试等待和 DLL 热重载。
项目路径先转换为绝对路径，可以从其他目录启动。

`--frames 120` 会在完成 120 次更新后退出；不能代替画面和输入的交互验收。

```powershell
python ForestRuntime/tests/smoke.py build/bin/Debug/ForestRuntime_debug.exe
python ForestRuntime/tests/project_scripts.py build/bin/Debug/ForestRuntime_debug.exe
```

检查只使用临时项目或已有资源，不改动示例源码。

验证同一进程内项目切换、程序集重载和脚本字段保留：

```powershell
cmake --build build --config Debug --target ProjectScriptLifecycleTests
python ForestRuntime/tests/project_scripts.py build/bin/Debug/ForestRuntime_debug.exe build/bin/Debug/ProjectScriptLifecycleTests.exe
```
