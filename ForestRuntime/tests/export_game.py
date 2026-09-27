"""Export a temporary project, relocate it, and verify source/output isolation."""
from pathlib import Path
import hashlib
import os
import shutil
import subprocess
import sys
import tempfile
import time

root = Path(__file__).resolve().parents[2]
exe = Path(sys.argv[1]).resolve()
runtime = Path(sys.argv[2]).resolve() if len(sys.argv) > 2 else exe.parent

def snapshot(directory):
    return {str(p.relative_to(directory)): hashlib.sha256(p.read_bytes()).hexdigest()
            for p in directory.rglob('*') if p.is_file()}

with tempfile.TemporaryDirectory(prefix='Forest export ') as temp:
    base = Path(temp)
    source = base / 'Source Project'
    shutil.copytree(root / 'EngineEditor/Sandbox/Assets', source / 'Assets')
    project = source / 'Test.forestproj'
    descriptor = ('Name: ExportTest\nAssetDirectory: Assets\nStartScene: Scenes/Physics2D.scene\n'
                  'ScriptAssembly: Scripts/bin/{config}/Game.dll\nScriptSourceDirectory: Scripts/src\n'
                  'ScriptBuildConfiguration: Debug\n')
    project.write_text(descriptor, encoding='utf-8')
    cwd = base / 'Unrelated Working Directory'
    cwd.mkdir()
    output = base / 'Exported Game'

    def export(target, success=True, marker='Game exported:', extra=()):
        result = subprocess.run([str(exe), str(project), '--export', str(target),
                                 '--runtime-directory', str(runtime), *extra], cwd=cwd,
                                capture_output=True, timeout=180)
        text = (result.stdout + result.stderr).decode('utf-8', errors='replace')
        assert (result.returncode == 0) == success and marker in text, text
        assert not list(base.glob('.forest-export-*')), 'Export staging directory leaked'

    before = snapshot(source)
    export(output)
    assert before == snapshot(source), 'Export modified the development project'
    assert 'ScriptBuildConfiguration: Release' in (output / 'Game/Game.forestproj').read_text()
    assert (output / 'Game/Assets/Scripts/bin/Release/Game.dll').is_file()
    assert not (output / 'Build').exists()
    exported = snapshot(output)
    export(output, False, 'already exists')
    assert snapshot(output) == exported
    export(source / 'Export', False, 'overlaps')
    export(base / 'Invalid Options', False, 'cannot be combined', ('--frames', '3'))
    print('PASS: Release export, unchanged development project, existing output and overlap protection', flush=True)

    bad_runtime = base / 'Debug Runtime'
    bad_runtime.mkdir()
    (bad_runtime / 'runtime-build.yaml').write_text('Configuration: Debug\n', encoding='utf-8')
    export(base / 'Wrong Runtime', False, 'requires a prepared Release', ('--runtime-directory', str(bad_runtime)))
    asset = source / 'Assets/Scenes/Sample.scene'
    original = asset.read_text(encoding='utf-8')
    asset.write_text(original.replace('Textures/Checkerboard.png', (base / 'External.png').as_posix()), encoding='utf-8')
    export(base / 'External Resource', False, 'Use relative resource paths')
    asset.write_text(original, encoding='utf-8')
    project.write_text(descriptor.replace('Scripts/bin/{config}/Game.dll', '""')
                       .replace('Scenes/Physics2D.scene', 'Scenes/Sample.scene'), encoding='utf-8')
    no_scripts = base / 'No Scripts'
    export(no_scripts)
    result = subprocess.run([str(no_scripts / 'ForestGame.exe'), '--frames', '3'], cwd=cwd,
                            capture_output=True, timeout=45)
    assert result.returncode == 0, result.stdout + result.stderr
    project.write_text(descriptor, encoding='utf-8')
    print('PASS: rejects Debug runtime and external references; exports projects without scripts', flush=True)

    # Change source after success; a failed compile must not publish a partial game.
    (source / 'Assets/Scripts/src/Broken.cs').write_text('this is invalid C#', encoding='utf-8')
    failed = base / 'Failed Export'
    export(failed, False, 'Script build failed')
    assert not failed.exists()
    print('PASS: failed compilation publishes no output and cleans staging', flush=True)

    moved = base / 'Moved Game'
    output.rename(moved)
    # The original project is absent at its previous path during all runtime checks.
    source.rename(base / 'Hidden Source')
    env = os.environ.copy()
    for key in list(env):
        if key.upper().startswith('MONO') or key.upper() == 'VULKAN_SDK':
            del env[key]
    env['PATH'] = str(Path(os.environ['SystemRoot']) / 'System32')
    env['LOCALAPPDATA'] = str(base / 'User Data')
    result = subprocess.run([str(moved / 'ForestGame.exe'), '--frames', '60'], cwd=cwd,
                            env=env, capture_output=True, timeout=45)
    text = (result.stdout + result.stderr).decode('utf-8', errors='replace')
    assert result.returncode == 0 and 'Runtime stopped after 60 frames' in text, text
    assert 'created with initial score' in text, text
    assert str(source).lower() not in text.lower(), text
    with (base / 'startup.log').open('wb') as log:
        process = subprocess.Popen([str(moved / 'ForestGame.exe')], cwd=cwd, env=env,
                                   stdout=log, stderr=subprocess.STDOUT)
        try:
            deadline = time.monotonic() + 40
            while time.monotonic() < deadline:
                text = (base / 'startup.log').read_text(encoding='utf-8', errors='replace')
                if 'Runtime started:' in text:
                    break
                assert process.poll() is None, text
                time.sleep(0.1)
            else:
                raise AssertionError('Zero-argument startup timed out')
        finally:
            if process.poll() is None:
                process.terminate()
            process.wait(timeout=10)
    assert snapshot(moved) == exported, 'Runtime changed exported files'
    print('PASS: relocated export, no original project, Release script execution and zero-argument startup', flush=True)
