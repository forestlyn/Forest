"""Copy the prepared runtime and game outside the checkout; verify package-only resources."""
from pathlib import Path
import os
import shutil
import subprocess
import sys
import tempfile
import hashlib
import re
import time

root = Path(__file__).resolve().parents[2]
exe = Path(sys.argv[1]).resolve()
config = sys.argv[2] if len(sys.argv) > 2 else "Debug"
assert config in ("Debug", "Release")
for original in (root / "Engine/ThirdParty/mono/bin" / config).glob("*.dll"):
    assert hashlib.sha256(original.read_bytes()).digest() == hashlib.sha256((exe.parent / original.name).read_bytes()).digest()
if config == "Release":
    for dll in exe.parent.glob("*.dll"):
        assert not re.match(r"(msvcp[\d_]+d.*|vcruntime[\d_]+d|concrt\d+d|ucrtbased|shaderc_sharedd)\.dll$", dll.name, re.I), dll

with tempfile.TemporaryDirectory(prefix="Forest portable ") as temporary:
    base = Path(temporary)
    package = base / "Game Package"
    package.mkdir()
    shutil.copy2(exe, package / exe.name)
    for dll in exe.parent.glob("*.dll"):
        shutil.copy2(dll, package / dll.name)
    for name in ("resources", "Managed", "Mono"):
        shutil.copytree(exe.parent / name, package / name)
    game = package / "Game"
    shutil.copytree(root / "EngineEditor/Sandbox/Assets", game / "Assets",
                    ignore=shutil.ignore_patterns("CMakeLists.txt"))
    shutil.copy2(root / "EngineEditor/Sandbox/Sandbox.forestproj", game / "Game.forestproj")
    project = game / "Game.forestproj"
    project.write_text(re.sub(r"ScriptBuildConfiguration:.*", "ScriptBuildConfiguration: " + config,
                             project.read_text(encoding="utf-8")), encoding="utf-8")
    # Build in the temporary project before testing the package without development tools.
    build = subprocess.run([str(exe), str(project), "--build-only"], capture_output=True, timeout=120)
    assert build.returncode == 0, (build.stdout + build.stderr).decode("utf-8", errors="replace")
    cwd = base / "Unrelated Working Directory"
    cwd.mkdir()
    env = os.environ.copy()
    for key in list(env):
        if key.upper().startswith("MONO") or key.upper() == "VULKAN_SDK":
            del env[key]
    env["PATH"] = str(Path(os.environ["SystemRoot"]) / "System32")
    env["LOCALAPPDATA"] = str(base / "User Data")

    def snapshot():
        return {str(p.relative_to(package)): (p.stat().st_size, p.stat().st_mtime_ns)
                for p in package.rglob("*") if p.is_file()}

    before = snapshot()

    def run(success, marker):
        result = subprocess.run([str(package / exe.name), str(game / "Game.forestproj"), "--frames", "30"],
                                cwd=cwd, env=env, capture_output=True, timeout=45)
        output = (result.stdout + result.stderr).decode("utf-8", errors="replace")
        assert (result.returncode == 0) == success, output
        assert marker in output, output
        # Resource load logs must not contain paths back into the original checkout.
        assert str(root).lower() not in output.lower(), output
        return output

    run(True, "Runtime stopped after 30 frames")
    assert snapshot() == before, "Runtime wrote into the application/game directory"
    assert not list(cwd.iterdir()), "Runtime wrote into the caller's working directory"
    assert list((base / "User Data").rglob("*.spv")), "No shader cache in user directory"
    assert list((base / "User Data").rglob("runtime.log")), "No runtime log in user directory"
    print("PASS: relocated runtime/game, clean toolchain environment, user-only cache/log writes")

    startup = package / "runtime.yaml"

    def launch(args, expected, marker):
        result = subprocess.run([str(package / exe.name), *args], cwd=cwd, env=env,
                                capture_output=True, timeout=45)
        output = (result.stdout + result.stderr).decode("utf-8", errors="replace")
        assert result.returncode == expected and marker in output, output

    launch([], 1, "Startup config not found")
    for content in ("Project: [", "[]", "{}", "Project: null", 'Project: ""', "Project: []"):
        startup.write_text(content, encoding="utf-8")
        launch([], 1, "Invalid startup config")
    startup.write_text("Project: Missing.forestproj\n", encoding="utf-8")
    launch([], 1, "Required file not found")
    # Explicit CLI project bypasses even a malformed startup config.
    startup.write_text("Project: [", encoding="utf-8")
    launch([str(project), "--frames", "3"], 0, "Runtime stopped after 3 frames")
    launch(["--help"], 0, "Usage:")
    startup.write_text("Project: Game/Game.forestproj\n", encoding="utf-8")
    # A config in the working directory must have no effect.
    (cwd / "runtime.yaml").write_text("Project: Missing.forestproj\n", encoding="utf-8")
    launch(["--frames", "3"], 0, "Runtime stopped after 3 frames")
    launch(["--frames"], 1, "Missing value")
    launch(["--unknown"], 1, "Unknown option")
    # Exercise the actual zero-argument launch. Stop only our test process once startup completes.
    with (base / "startup.log").open("wb") as output:
        process = subprocess.Popen([str(package / exe.name)], cwd=cwd, env=env,
                                   stdout=output, stderr=subprocess.STDOUT)
        try:
            deadline = time.monotonic() + 40
            while time.monotonic() < deadline:
                log = (base / "startup.log").read_text(encoding="utf-8", errors="replace")
                if "Runtime started:" in log:
                    break
                assert process.poll() is None, log
                time.sleep(0.1)
            else:
                raise AssertionError("Zero-argument startup timed out")
        finally:
            if process.poll() is None:
                process.terminate()
            process.wait(timeout=10)
    print("PASS: zero-argument startup, EXE-relative config, CLI precedence and invalid config errors")

    # Source resources still exist: deleting only the package copy must fail, never fall back.
    for relative in ("resources/assets/shaders/Renderer2D_QuadShader.glsl",
                     "Managed/Engine-ScriptCore.dll", "Mono/4.5/mscorlib.dll"):
        path = package / relative
        held = path.with_name(path.name + ".held")
        path.rename(held)
        try:
            run(False, "Required file not found")
        finally:
            held.rename(path)
        print(f"PASS: no source-tree fallback for {relative}")
