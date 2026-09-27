# ForestRuntime（开发期独立运行器）

运行器和编辑器共享 `.forestproj` 中的脚本配置。脚本产物按项目隔离，不再输出到编辑器公共目录。
构建会准备自包含的运行目录，运行时从可执行文件旁加载引擎资源、Mono 和 ScriptCore，不再回退到源码仓库。提供命令行及编辑器菜单游戏导出。

## 构建与运行

在仓库根目录执行：

```powershell
cmake -S . -B build
cmake --build build --config Debug --target ForestRuntime EngineEditor
# 只编译指定项目的脚本（不创建窗口）
./build/runtime/Debug/ForestRuntime_debug.exe ./EngineEditor/Sandbox/Sandbox.forestproj --build-only
# 直接运行项目，不需要再指定 DLL
./build/runtime/Debug/ForestRuntime_debug.exe ./EngineEditor/Sandbox/Sandbox.forestproj
# 也可以先构建、成功后再启动；构建失败返回非零状态，不启动旧 DLL
./build/runtime/Debug/ForestRuntime_debug.exe ./EngineEditor/Sandbox/Sandbox.forestproj --build-scripts
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
python ForestRuntime/tests/smoke.py build/runtime/Debug/ForestRuntime_debug.exe
python ForestRuntime/tests/project_scripts.py build/runtime/Debug/ForestRuntime_debug.exe
```

检查只使用临时项目或已有资源，不改动示例源码。

验证同一进程内项目切换、程序集重载和脚本字段保留：

```powershell
cmake --build build --config Debug --target ProjectScriptLifecycleTests
python ForestRuntime/tests/project_scripts.py build/runtime/Debug/ForestRuntime_debug.exe build/runtime/Debug/ProjectScriptLifecycleTests.exe
```

## 运行目录与资源路径

`build/runtime/Debug/` 会包含：

```text
ForestRuntime_debug.exe
*.dll                       原生运行依赖
resources/assets/           引擎 shader、默认纹理、字体等
Managed/Engine-ScriptCore.dll
Mono/4.5/                   Mono 托管类库
```

每次构建都会同步资源；编辑器在 `build/bin/Debug/` 准备同样的资源结构，并额外复制项目模板。
移动运行器时需复制整个运行目录，并带上游戏项目、Assets 和已编译的游戏程序集。
启动可通过命令行指定 `.forestproj`，也可通过 EXE 旁的 `runtime.yaml` 指定默认项目。运行已编译项目不需要引擎源码；`--build-only` / `--build-scripts` 属于开发工具，仍需要源码中的构建驱动和 VS/CMake。

- `resources/...` 统一解析为可执行文件旁的引擎资源目录。
- 游戏相对资源路径解析为当前项目的 AssetDirectory；绝对路径按原路径加载。
- 缓存和日志写入 `%LOCALAPPDATA%/Forest/<可执行文件名（不含扩展名）>/`。
- 资源缓存按解析后的完整路径和资源类型隔离，切换项目不会复用其他项目的同名资源。

仓库外复制运行、缺失资源不回退、运行目录无写入验证：

```powershell
python ForestRuntime/tests/portable_runtime.py build/runtime/Debug/ForestRuntime_debug.exe
cmake --build build --config Debug --target ResourcePathTests
# 参数为专用测试输出目录，测试会在其中创建 A/B 项目
./build/runtime/Debug/ResourcePathTests.exe "$env:TEMP/ForestResourcePathTests"
```

Debug 和 Release 分别选择对应的 shader/Mono 原生库及 ScriptCore 产物。Release 运行目录不复制 Debug CRT，并在依赖扫描中拒绝已识别的 Debug DLL。ScriptCore 构建产物位于 `build/managed/<Config>/`，再复制到应用的 `Managed/` 目录。

```powershell
.\Rebuild_Release.bat
python ForestRuntime/tests/portable_runtime.py build/runtime/Release/ForestRuntime_release.exe Release
```

该测试会在临时项目中编译对应配置的游戏脚本，再将复制的运行目录置于无开发工具 PATH 的环境中运行，并检查 Mono DLL 配置、Release 中的 Debug CRT 残留以及资源缺失不回退。
干净机器验证仍为后续阶段。

## 编辑器布局

编辑器在项目根目录（与 `.forestproj` 同级）读取和保存 `imgui.ini`；没有活动项目时，使用编辑器 EXE 所在目录的 `imgui.ini`。
切换项目会先保存旧布局，再加载目标项目布局。Debug/Release 编辑器打开同一个项目时共享这份布局。
项目模板 `EngineEditor/resources/template/project/imgui.ini` 提供默认布局，新建项目时随模板复制。
旧项目或无项目状态缺少布局文件时，也会从随编辑器分发的模板初始化；已有布局不会被覆盖。
独立 ForestRuntime 仍关闭 ImGui，不读写该文件。

## 无参数启动

在 ForestRuntime EXE 旁放置 `runtime.yaml`：

```yaml
Project: Game/Game.forestproj
```

例如：

```text
ForestRuntime_release.exe
runtime.yaml
Game/Game.forestproj
Game/Assets/...
resources/...
Managed/...
Mono/...
*.dll
```

双击 EXE 将读取上述项目，并运行项目的 `StartScene`。`Project` 相对路径以配置文件所在目录（EXE 目录）为基准，也支持绝对路径；移动游戏目录时应使用相对路径。
没有命令行项目参数时，也可使用 `ForestRuntime_release.exe --frames 120` 进行验证。
显式传入项目路径则优先使用该项目，不读取 `runtime.yaml`；命令行相对路径仍以调用者的工作目录为基准。
配置缺失、YAML 格式错误、Project 字段无效或项目文件不存在时，程序打印错误并返回非零退出码。`--help` 不要求配置存在。

命令行与编辑器菜单均可自动生成启动配置和游戏目录。

## 导出游戏（Windows Release）

先构建 Release Runtime，再执行导出（输出目录必须尚不存在）：

```powershell
cmake --build build --config Release --target ForestRuntime
./build/runtime/Release/ForestRuntime_release.exe ./EngineEditor/Sandbox/Sandbox.forestproj --export D:/Games/MyGame
```

默认使用本次引擎构建目录下的 `runtime/Release`。也可以通过 `--runtime-directory <路径>` 指定已准备好的 Release Runtime 目录；该目录需要含构建生成的 `runtime-build.yaml` 配置标记。Debug 运行器也可调用导出，输出始终使用指定的 Release Runtime。

输出结构：

```text
MyGame/
  ForestGame.exe
  runtime.yaml
  *.dll
  resources/
  Managed/
  Mono/
  Game/
    Game.forestproj
    Assets/
```

双击 `ForestGame.exe` 即可运行。复制或移动时需带上整个目录。

导出逻辑位于 `Engine::ProjectExporter`，由命令行与编辑器入口共用。它读取活动项目设置，完整复制 AssetDirectory，在导出副本中使用 `Assets` 和 Release 配置；如果配置了游戏程序集，会从原脚本源码单独构建 `Scripts/bin/Release/Game.dll`。开发项目配置、程序集和构建目录均不修改。未配置脚本的项目无需执行脚本编译。

第一版完整复制资源，不做资源裁剪，因此 Assets 中已有的源码和其他产物也可能随目录复制。启动场景必须位于 Assets 内，场景和 YAML 资源的 `path` / `atlas` / `image` 引用必须是可解析的项目相对路径或 `resources/...` 引擎路径；不支持导出目录链接和联接点，也不会分析脚本代码里自行打开的外部文件。

输出目录不能与项目、Assets 或 Runtime 源目录重叠，也不会覆盖已有输出。导出先在同级临时目录组装，成功后才生成最终目录；失败会报告错误并清理临时产物。脚本导出仍需 VS/CMake 构建工具；导出的游戏运行时无需这些工具。

验证导出隔离、失败清理和移动后的运行：

```powershell
python ForestRuntime/tests/export_game.py build/runtime/Release/ForestRuntime_release.exe
```

## 编辑器导出入口

打开项目，在编辑状态下选择 **File → Export Game...**：

1. 选择导出父目录，填写尚不存在的新文件夹名称。
2. 默认使用当前引擎构建的 Release Runtime；如需使用另一份已准备好的运行目录，可点击 **Choose Release runtime...**。
3. 点击 **Export**，窗口显示成功位置或详细错误。成功后运行输出目录中的 `ForestGame.exe`。

没有项目或正在 Play/Simulate 时，导出入口不可用。导出读取磁盘上的场景文件，需先保存场景修改；不会自动覆盖项目文件。当前导出为同步操作，编译和复制期间编辑器会暂时等待。
