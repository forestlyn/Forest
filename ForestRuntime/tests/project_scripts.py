"""Build/run independent C# projects, failed-build gating, recovery and source additions."""
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile

root = Path(__file__).resolve().parents[2]
exe = Path(sys.argv[1]).resolve()


def run(project, *args, success=True, marker=None):
    result = subprocess.run([str(exe), str(project), *args], cwd=project.parent,
                            capture_output=True, timeout=120)
    output = (result.stdout + result.stderr).decode("utf-8", errors="replace")
    assert (result.returncode == 0) == success, output
    if marker:
        assert marker in output, output
    return output


with tempfile.TemporaryDirectory(prefix="Forest script projects ") as temporary:
    projects = []
    for name in ("First", "Second"):
        directory = Path(temporary) / name
        source = directory / "Assets/Scripts/src"
        source.mkdir(parents=True)
        scenes = directory / "Assets/Scenes"
        scenes.mkdir()
        shutil.copytree(root / "EngineEditor/Sandbox/Assets/Textures", directory / "Assets/Textures")
        # Minimal scene in the existing serializer's format, with a camera and one script.
        scene = (root / "EngineEditor/Sandbox/Assets/Scenes/Physics2D.scene").read_text(encoding="utf-8")
        (scenes / "Main.scene").write_text(scene.replace("Sandbox.Player", f"{name}.Player"), encoding="utf-8")
        code = f'''namespace {name} {{
            public class Player : Engine.Entity {{
                public int Value = 123;
                public override void OnCreate() {{ Engine.Log.NativeLog("PROJECT_{name}"); }}
                public override void OnUpdate(float dt) {{ }}
            }}
        }}'''
        (source / "Player.cs").write_text(code, encoding="utf-8")
        project = directory / f"{name}.forestproj"
        project.write_text(f'''Name: {name}
AssetDirectory: Assets
StartScene: Scenes/Main.scene
ScriptAssembly: Scripts/bin/{{config}}/Game.dll
ScriptSourceDirectory: Scripts/src
ScriptBuildConfiguration: Debug
''', encoding="utf-8")
        run(project, "--build-scripts", "--frames", "3", marker=f"PROJECT_{name}")
        projects.append(project)
    first, second = projects
    first_dll = first.parent / "Assets/Scripts/bin/Debug/Game.dll"
    second_dll = second.parent / "Assets/Scripts/bin/Debug/Game.dll"
    first_bytes = first_dll.read_bytes()
    assert first_bytes != second_dll.read_bytes()
    run(first, "--frames", "3", marker="PROJECT_First")
    print("PASS: project-local builds and launches without DLL override")

    bad_source = first.parent / "Assets/Scripts/src/Broken.cs"
    bad_source.write_text("this is not valid C#", encoding="utf-8")
    run(first, "--build-scripts", "--frames", "3", success=False, marker="Script build failed")
    assert first_dll.read_bytes() == first_bytes  # old DLL remains, but must never be launched
    run(first, "--frames", "3", success=False, marker="Rebuild before running")
    run(second, "--frames", "3", marker="PROJECT_Second")
    print("PASS: failed build blocks old DLL; other project unaffected")

    bad_source.unlink()
    run(first, "--build-only", marker="Project scripts built")
    run(first, "--frames", "3", marker="PROJECT_First")
    print("PASS: successful rebuild clears failure state")

    source = first.parent / "Assets/Scripts/src"
    (source / "Added.cs").write_text('namespace First { public static class Added { public const string Value = "ADDED_SOURCE"; } }', encoding="utf-8")
    player = source / "Player.cs"
    player.write_text(player.read_text(encoding="utf-8").replace('"PROJECT_First"', 'Added.Value'), encoding="utf-8")
    run(first, "--build-scripts", "--frames", "3", marker="ADDED_SOURCE")
    print("PASS: newly added source included")

    first.write_text(first.read_text(encoding="utf-8").replace("ScriptBuildConfiguration: Debug", "ScriptBuildConfiguration: Release"), encoding="utf-8")
    run(first, "--build-scripts", "--frames", "3", marker="ADDED_SOURCE")
    assert (first.parent / "Assets/Scripts/bin/Release/Game.dll").is_file()
    assert first_dll.is_file()
    print("PASS: Debug and Release script outputs isolated")

    if len(sys.argv) > 2:
        lifecycle = Path(sys.argv[2]).resolve()
        result = subprocess.run([str(lifecycle), str(first), str(second)],
                                capture_output=True, timeout=40)
        output = (result.stdout + result.stderr).decode("utf-8", errors="replace")
        assert result.returncode == 0, output
        assert "PASS: project switch" in output, output
        print("PASS: same-process project switch and reload lifecycle")

    # New-project template must build without editing any CMake files.
    template_dir = Path(temporary) / "Template"
    shutil.copytree(root / "EngineEditor/resources/template/project", template_dir)
    template_project = template_dir / "Template.forestproj"
    template_project.write_text('Name: Template\nAssetDirectory: Assets\nScriptAssembly: Scripts/bin/{config}/Game.dll\n', encoding="utf-8")
    run(template_project, "--build-only", marker="Project scripts built")
    print("PASS: unmodified new-project template builds")
