#pragma once
#include "Engine/Core/Core.h"
#include "Engine/Renderer/Shader/Texture.h"
#include <filesystem>
namespace EngineEditor
{
    class ContentBrowserPanel
    {
    public:
        ContentBrowserPanel();
        ~ContentBrowserPanel() = default;

        void OnImGuiRender();

    private:
        std::filesystem::path m_CurrentDirectory;
        Engine::Ref<Engine::Renderer::Texture2D> m_DirectoryIcon;
        Engine::Ref<Engine::Renderer::Texture2D> m_FileIcon;
        Engine::Ref<Engine::Renderer::Texture2D> m_BackIcon;
    };
};