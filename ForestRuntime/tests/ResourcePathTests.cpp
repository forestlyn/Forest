#include "Engine/Core/RuntimePaths.h"
#include "Engine/Core/Log.h"
#include "Engine/Resource/ResourceManager.h"
#include <fstream>
#include <stdexcept>

struct TextAsset { std::string Value; };
struct OtherAsset { std::string Value; };
namespace Engine
{
    template <> Ref<TextAsset> ResourceManager::LoadAssetFromFile<TextAsset>(const std::string &path)
    {
        std::ifstream stream(path);
        if (!stream) return nullptr;
        auto value = CreateRef<TextAsset>();
        stream >> value->Value;
        return value;
    }
    template <> Ref<OtherAsset> ResourceManager::LoadAssetFromFile<OtherAsset>(const std::string &path)
    {
        auto value = CreateRef<OtherAsset>();
        value->Value = "other-type";
        return value;
    }
}

int main(int argc, char **argv)
{
    Engine::Core::Log::Init();
    try
    {
        if (argc != 2) throw std::runtime_error("Expected a test output directory");
        namespace fs = std::filesystem;
        const auto root = fs::absolute(argv[1]);
        Engine::Core::RuntimePaths::Configure(root / "engine", root / "user");
        auto check = [](bool condition) { if (!condition) throw std::runtime_error("Resource path check failed"); };
        for (const auto *name : {"A", "B"})
        {
            fs::create_directories(root / name / "Assets");
            std::ofstream(root / name / "Game.forestproj") << "Name: " << name << "\nAssetDirectory: Assets\n";
            std::ofstream(root / name / "Assets/shared.txt") << name;
        }
        std::ofstream(root / "B/Assets/missing.txt") << "found";
        Engine::Project::Load(root / "A/Game.forestproj");
        auto manager = Engine::ResourceManager::Get();
        auto a = manager->GetOrLoad<TextAsset>("shared.txt");
        check(a && a->Value == "A");
        check(manager->GetOrLoad<TextAsset>("./shared.txt") == a);
        check(!manager->GetOrLoad<TextAsset>("missing.txt"));
        auto other = manager->GetOrLoad<OtherAsset>("shared.txt");
        check(other && other->Value == "other-type");
        Engine::Project::Load(root / "B/Game.forestproj");
        auto b = manager->GetOrLoad<TextAsset>("shared.txt");
        check(b && b != a && b->Value == "B");
        check(manager->GetOrLoad<TextAsset>("missing.txt")->Value == "found");
        check(Engine::Core::RuntimePaths::ResolveAsset("resources/assets/a.txt") ==
              fs::weakly_canonical(root / "engine/assets/a.txt"));
        check(Engine::Core::RuntimePaths::Cache() == root / "user/Cache");
        ENGINE_INFO("PASS: resource roots, normalized cache keys, type isolation and project switching");
        return 0;
    }
    catch (const std::exception &error) { ENGINE_ERROR("{}", error.what()); return 1; }
}
