"""Integration checks using a built development runner and the existing sample assets.
Run: python ForestRuntime/tests/smoke.py build/bin/Debug/ForestRuntime_debug.exe
Creates temporary project descriptors, never changes the source projects.
"""
import json
from pathlib import Path
import subprocess
import sys
import tempfile

root = Path(__file__).resolve().parents[2]
exe = Path(sys.argv[1]).resolve()
assets = root / "EngineEditor/Sandbox/Assets"
assembly = root / "EngineEditor/resources/scripts/bin/Sandbox.dll"


def run(args, expected, marker, cwd):
    result = subprocess.run([str(exe), *map(str, args)], cwd=cwd,
                            capture_output=True, timeout=40)
    output = (result.stdout + result.stderr).decode("utf-8", errors="replace")
    assert result.returncode == expected, output
    assert marker in output, output
    print(f"PASS: {marker}")


with tempfile.TemporaryDirectory(prefix="forest runtime ") as temporary:
    directory = Path(temporary)
    project = directory / "Test.forestproj"

    def write_project(scene, dll=""):
        project.write_text(
            "Name: RuntimeSmoke\n"
            f"AssetDirectory: {json.dumps(assets.as_posix())}\n"
            f"StartScene: {json.dumps(scene)}\n"
            f"ScriptAssembly: {json.dumps(dll)}\n", encoding="utf-8")

    run(["--help"], 0, "Usage:", directory)
    run(["missing.forestproj"], 1, "Required file not found", directory)
    write_project("Scenes/Sample.scene")
    run([project.name, "--frames", "3"], 0, "Runtime stopped after 3 frames", directory)
    run([project, "--frames", "zero"], 1, "positive integer", directory)
    run([project, "--script-assembly", "missing.dll"], 1, "Required file not found", directory)
    write_project("Scenes/Physics2D.scene")
    run([project, "--frames", "3"], 1, "Missing script class: Sandbox.Player", directory)
    # Absolute paths are also permitted in development project descriptors.
    write_project("Scenes/Physics2D.scene", assembly.as_posix())
    run([project.name, "--frames", "120"], 0, "Runtime stopped after 120 frames", directory)
    write_project("Scenes/Physics2D.scene")
    run([project, "--script-assembly", assembly, "--frames", "3"],
        0, "created with initial score", directory)
    no_camera = directory / "NoCamera.scene"
    no_camera.write_text((assets / "Scenes/Sample.scene").read_text(encoding="utf-8")
                         .replace("Primary: true", "Primary: false"), encoding="utf-8")
    write_project(no_camera.as_posix())
    run([project, "--frames", "3"], 1, "no primary camera", directory)
