#pragma once
#include "Engine/Core/Core.h"
#include "Engine/Scene/Scene.h"

namespace Engine::Serialization
{
    class SceneSerialize
    {
    public:
        SceneSerialize(const Ref<Scene> &scene);
        ~SceneSerialize();
        void Serialize(const std::string &filePath);
        bool Deserialize(const std::string &filePath);

    private:
        Ref<Scene> m_Scene;
    };
} // namespace Engine::Serialization