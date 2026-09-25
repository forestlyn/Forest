# ForestRuntime（开发期独立运行器）

ForestRuntime 直接加载 `.forestproj` 的启动场景，不创建编辑器面板。
当前仍使用构建时仓库中的引擎资源、Mono 基础库和 ScriptCore，不是可分发的游戏包。

## 构建与启动

在仓库根目录执行：

```powershell
cmake -S . -B build
cmake --build build --config Debug --target ForestRuntime Sandbox
./build/bin/Debug/ForestRuntime_debug.exe ./EngineEditor/Sandbox/Sandbox.forestproj --script-assembly ./EngineEditor/resources/scripts/bin/Sandbox.dll
```

运行器不会自动编译游戏脚本。请先构建相应程序集；`Sandbox` 仅是上述示例的脚本构建目标。

## 项目脚本配置

可在 `.forestproj` 中设置：

```yaml
ScriptAssembly: Scripts/bin/Game.dll
```

该路径相对于项目的 `AssetDirectory`。也可使用 `--script-assembly <dll>` 覆盖，命令行路径相对于启动目录。
旧项目不配置该字段时可用命令行指定；运行器不会根据项目名猜测 DLL，也不会默认加载 Sandbox。
无游戏程序集时仍加载 ScriptCore，可运行无 C# 脚本的场景；场景引用未加载的脚本类会报错退出。

项目路径在切换工作目录之前转换为绝对路径，所以可以从其他目录启动。
启动场景需要主相机。运行器使用窗口默认帧缓冲，支持窗口缩放、现有键鼠输入和物理更新。
失焦后继续更新，最小化时暂停更新。默认不启用 ImGui、性能面板、脚本调试等待和热重载。

## 有界运行检查

添加 `--frames 120` 可在完成 120 次场景更新后正常退出，便于检查启动、脚本更新和资源清理。
此参数不能代替画面、输入和窗口缩放的交互验证。
启动失败返回非零退出码，错误写入控制台。

集成检查（先构建 Runtime 和 Sandbox）：

```powershell
python ForestRuntime/tests/smoke.py build/bin/Debug/ForestRuntime_debug.exe
```

检查会创建临时项目描述并从不同工作目录启动，不修改示例项目。
