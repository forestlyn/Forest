#include "ContentBrowserPanel.h"
#include "imgui.h"
namespace EngineEditor
{
    const static std::string AssetsDirectory = "assets";
    ContentBrowserPanel::ContentBrowserPanel()
    {
        m_CurrentDirectory = AssetsDirectory;
        m_DirectoryIcon = Engine::Renderer::Texture2D::Create("assets/textures/icon/folder_2377907.png");
        m_FileIcon = Engine::Renderer::Texture2D::Create("assets/textures/icon/document_3677274.png");
        m_BackIcon = Engine::Renderer::Texture2D::Create("assets/textures/icon/BackIcon.png");
    }
    void ContentBrowserPanel::OnImGuiRender()
    {
        ImGui::Begin("Content Browser");
        if (m_CurrentDirectory != AssetsDirectory)
        {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            if (ImGui::ImageButton("##back", ImTextureRef((void *)(uintptr_t)m_BackIcon->GetRendererID()), ImVec2(25, 25)))
            {
                m_CurrentDirectory = m_CurrentDirectory.parent_path();
            }
            ImGui::PopStyleColor();
        }
        ImGui::SameLine();
        ImGui::Text("Content Browser Panel: %s", m_CurrentDirectory.c_str());
        ImGui::Separator();
        static float padding = 16.0f;
        static float thumbnailWidth = 64.0f;
        static float thumbnailHeight = 64.0f;
        float cellSize = thumbnailWidth + padding;
        ImVec2 panelSize = ImGui::GetContentRegionAvail();
        int columnCount = (int)(panelSize.x / cellSize);
        if (columnCount < 1)
            columnCount = 1;
        ImGui::Columns(columnCount, 0, false);
        for (const auto &entry : std::filesystem::directory_iterator(m_CurrentDirectory))
        {
            const auto &path = entry.path();
            // ImGui::Text("%s", path.string().c_str());
            auto relativePath = std::filesystem::relative(path, AssetsDirectory);
            std::string filenameString = relativePath.filename().string();

            ImGui::PushID(filenameString.c_str());
            Engine::Ref<Engine::Renderer::Texture2D> icon = entry.is_directory() ? m_DirectoryIcon : m_FileIcon;
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::ImageButton("##icon", ImTextureRef((void *)(uintptr_t)icon->GetRendererID()), ImVec2(thumbnailWidth, thumbnailHeight));
            if (ImGui::BeginDragDropSource())
            {
                const wchar_t *itemPath = path.c_str();
                ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
                ImGui::EndDragDropSource();
            }
            ImGui::PopStyleColor();

            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            {
                if (entry.is_directory())
                {
                    m_CurrentDirectory /= path.filename();
                }
            }

            ImGui::TextWrapped("%s", filenameString.c_str());
            ImGui::NextColumn();
            ImGui::PopID();
        }
        ImGui::End();
    }
}
