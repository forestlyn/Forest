#include "ProjectExporter.h"
#include "Project.h"
#include "ProjectScripts.h"
#include <Windows.h>
#include <yaml-cpp/yaml.h>
#include <fstream>
#include <atomic>
#include <cwctype>

namespace fs = std::filesystem;
namespace Engine
{
    namespace
    {
        fs::path Canonical(const fs::path &path) { return fs::weakly_canonical(fs::absolute(path)); }

        bool Within(const fs::path &path, const fs::path &parent)
        {
            auto lower = [](const fs::path &value) {
                auto text = value.generic_wstring();
                for (auto &c : text) c = std::towlower(c);
                if (text.back() != L'/') text += L'/';
                return text;
            };
            return lower(path).rfind(lower(parent), 0) == 0;
        }

        void RequireFile(const fs::path &path)
        {
            if (!fs::is_regular_file(path)) throw std::runtime_error("Export file missing: " + path.string());
        }

        void RejectLink(const fs::path &path)
        {
            const DWORD attributes = GetFileAttributesW(path.c_str());
            if (attributes == INVALID_FILE_ATTRIBUTES || (attributes & FILE_ATTRIBUTE_REPARSE_POINT))
                throw std::runtime_error("Export does not support links/junctions: " + path.string());
        }

        void CopyTree(const fs::path &source, const fs::path &destination)
        {
            if (!fs::is_directory(source)) throw std::runtime_error("Export directory missing: " + source.string());
            RejectLink(source);
            fs::create_directories(destination);
            for (const auto &entry : fs::recursive_directory_iterator(source))
            {
                RejectLink(entry.path());
                const auto target = destination / entry.path().lexically_relative(source);
                if (entry.is_directory()) fs::create_directories(target);
                else if (entry.is_regular_file()) fs::copy_file(entry.path(), target);
                else throw std::runtime_error("Unsupported export file: " + entry.path().string());
            }
        }

        void Write(const fs::path &path, const std::string &text)
        {
            std::ofstream stream;
            stream.exceptions(std::ios::failbit | std::ios::badbit);
            stream.open(path);
            stream << text;
            stream.close();
        }

        void ValidateReferences(const YAML::Node &node, const fs::path &assets,
                                const fs::path &runtime, const fs::path &file, unsigned depth = 0)
        {
            if (depth > 128) throw std::runtime_error("Asset YAML nesting is too deep: " + file.string());
            if (node.IsSequence())
                for (const auto &item : node) ValidateReferences(item, assets, runtime, file, depth + 1);
            if (!node.IsMap()) return;
            for (const auto &entry : node)
            {
                const auto key = entry.first.as<std::string>();
                if ((key == "path" || key == "atlas" || key == "image") && entry.second.IsScalar())
                {
                    const fs::path reference = fs::u8path(entry.second.as<std::string>());
                    if (reference.empty()) continue;
                    if (reference.is_absolute() || reference.has_root_path())
                        throw std::runtime_error("Use relative resource paths before export: " + file.string() + ": " + reference.string());
                    const auto normalized = reference.lexically_normal().generic_string();
                    const bool engine = normalized.rfind("resources/", 0) == 0;
                    const auto base = engine ? runtime : assets;
                    const auto resolved = Canonical(base / reference);
                    if (!Within(resolved, engine ? runtime / "resources" : assets))
                        throw std::runtime_error("Resource path escapes export roots: " + file.string() + ": " + reference.string());
                    RequireFile(resolved);
                }
                else ValidateReferences(entry.second, assets, runtime, file, depth + 1);
            }
        }

        struct TemporaryDirectory
        {
            fs::path Path;
            ~TemporaryDirectory()
            {
                // Only this uniquely created staging directory is owned by the exporter.
                if (!Path.empty()) { std::error_code error; fs::remove_all(Path, error); }
            }
        };
    }

    fs::path ProjectExporter::DefaultRuntimeDirectory() { return fs::path(FOREST_RUNTIME_RELEASE_DIR); }

    bool ProjectExporter::Export(const fs::path &outputDirectory, const fs::path &runtimeDirectory, std::string &error)
    {
        error.clear();
        try
        {
            if (!Project::GetActiveProject()) throw std::runtime_error("No active project to export");
            if (outputDirectory.empty()) throw std::runtime_error("Export output directory is empty");
            const auto output = Canonical(outputDirectory);
            const auto runtime = Canonical(runtimeDirectory.empty() ? DefaultRuntimeDirectory() : runtimeDirectory);
            const auto projectDirectory = Canonical(Project::GetActiveProjectDirectory());
            const auto assets = Canonical(Project::GetActiveProjectAssetDirectory());
            for (const auto &source : {runtime, projectDirectory, assets})
                if (Within(output, source) || Within(source, output))
                    throw std::runtime_error("Export output overlaps a source directory");
            if (fs::exists(output)) throw std::runtime_error("Export output already exists; choose a new directory");

            RequireFile(runtime / "runtime-build.yaml");
            const auto manifest = YAML::LoadFile((runtime / "runtime-build.yaml").string());
            if (!manifest.IsMap() || manifest["Configuration"].as<std::string>("") != "Release")
                throw std::runtime_error("Export requires a prepared Release runtime directory");
            for (const auto *file : {"ForestRuntime_release.exe", "Managed/Engine-ScriptCore.dll",
                                    "Mono/4.5/mscorlib.dll", "mono-2.0-sgen.dll", "MonoPosixHelper.dll",
                                    "resources/assets/shaders/Renderer2D_QuadShader.glsl",
                                    "resources/assets/shaders/Renderer2D_CircleShader.glsl",
                                    "resources/assets/shaders/Renderer2D_LineShader.glsl"})
                RequireFile(runtime / file);
            const auto scene = Canonical(Project::GetActiveProjectStartScene());
            RequireFile(scene);
            if (!Within(scene, assets)) throw std::runtime_error("Startup scene must be inside the asset directory");
            const auto settings = Project::GetActiveProject()->GetProjectSettings();
            for (const auto &entry : fs::recursive_directory_iterator(assets))
            {
                RejectLink(entry.path());
                const auto extension = entry.path().extension();
                if (entry.is_regular_file() && (extension == ".scene" || extension == ".yaml" || extension == ".yml"))
                    ValidateReferences(YAML::LoadFile(entry.path().string()), assets, runtime, entry.path());
            }

            fs::create_directories(output.parent_path());
            TemporaryDirectory temporary;
            static std::atomic<unsigned> serial{0};
            for (unsigned attempt = 0; attempt < 100; ++attempt)
            {
                const auto candidate = output.parent_path() / (".forest-export-" + std::to_string(GetCurrentProcessId()) +
                                       "-" + std::to_string(serial++));
                if (fs::create_directory(candidate)) { temporary.Path = candidate; break; }
            }
            if (temporary.Path.empty()) throw std::runtime_error("Cannot create export staging directory");
            const auto package = temporary.Path / "Package";
            fs::create_directory(package);
            for (const auto *directory : {"resources", "Managed", "Mono"}) CopyTree(runtime / directory, package / directory);
            fs::copy_file(runtime / "ForestRuntime_release.exe", package / "ForestGame.exe");
            for (const auto &entry : fs::directory_iterator(runtime))
                if (entry.path().extension() == ".dll")
                {
                    RejectLink(entry.path());
                    fs::copy_file(entry.path(), package / entry.path().filename());
                }
            CopyTree(assets, package / "Game/Assets");
            const std::string assembly = settings.ScriptAssembly.empty() ? "" : "Scripts/bin/Release/Game.dll";
            if (!assembly.empty())
            {
                // Build directly into the export copy. Never overwrite the development assembly.
                const auto destination = package / "Game/Assets" / assembly;
                fs::create_directories(destination.parent_path());
                if (!ProjectScripts::Build(ProjectScripts::SourceDirectory(), destination, "Release",
                                           package / "Managed/Engine-ScriptCore.dll", temporary.Path / "Build", error))
                    throw std::runtime_error(error);
            }
            YAML::Emitter project;
            project << YAML::BeginMap
                    << YAML::Key << "Name" << YAML::Value << settings.Name
                    << YAML::Key << "AssetDirectory" << YAML::Value << "Assets"
                    << YAML::Key << "StartScene" << YAML::Value << scene.lexically_relative(assets).generic_string()
                    << YAML::Key << "ScriptAssembly" << YAML::Value << assembly
                    << YAML::Key << "ScriptBuildConfiguration" << YAML::Value << "Release"
                    << YAML::EndMap;
            Write(package / "Game/Game.forestproj", project.c_str());
            Write(package / "runtime.yaml", "Project: Game/Game.forestproj\n");
            // Windows rename fails if the destination appeared in the meantime; nothing is overwritten.
            fs::rename(package, output);
            ENGINE_INFO("Game exported: {}", output.string());
            return true;
        }
        catch (const std::exception &exception) { error = exception.what(); return false; }
    }
}
