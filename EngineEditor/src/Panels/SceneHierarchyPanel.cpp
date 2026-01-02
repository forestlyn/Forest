#include "SceneHierarchyPanel.h"
#include "Engine/Profile/Instrumentor.h"
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
namespace EngineEditor
{
    SceneHierarchyPanel::SceneHierarchyPanel(const Engine::Ref<Engine::Scene> &context)
        : m_Context(context)
    {
    }

    void SceneHierarchyPanel::OnImGuiRender()
    {
        ENGINE_PROFILING_FUNC();

        ImGui::Begin("Scene Hierarchy");

        if (ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered())
        {
            m_SelectionContext = {};
        }

        auto view = m_Context->m_Registry.view<Engine::TagComponent>();
        for (auto entity : view)
        {
            Engine::Entity e{entity, m_Context.get()};
            DrawEntityNode(e);
        }
        ImGui::End();

        ImGui::Begin("Properties");
        if (m_SelectionContext)
        {
            DrawComponents(m_SelectionContext);
        }
        ImGui::End();
    }
    void SceneHierarchyPanel::DrawEntityNode(Engine::Entity entity)
    {
        const auto &tag = entity.GetComponent<Engine::TagComponent>().Tag;

        ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
        bool opened = ImGui::TreeNodeEx((void *)(uint64_t)(uint32_t)(entt::entity)entity, flags, tag.c_str());
        if (ImGui::IsItemClicked())
        {
            m_SelectionContext = entity;
        }

        if (opened)
        {
            ImGui::TreePop();
        }
    }
    void SceneHierarchyPanel::DrawComponents(Engine::Entity entity)
    {
        if (entity.HasComponent<Engine::TagComponent>())
        {
            bool opened = ImGui::TreeNodeEx("Tag", ImGuiTreeNodeFlags_DefaultOpen);

            if (opened)
            {
                auto &tag = entity.GetComponent<Engine::TagComponent>().Tag;
                char buffer[256];
                memset(buffer, 0, sizeof(buffer));
                strcpy_s(buffer, sizeof(buffer), tag.c_str());
                if (ImGui::InputText("Tag", buffer, sizeof(buffer)))
                {
                    tag = std::string(buffer);
                }
                ImGui::TreePop();
            }
        }

        if (entity.HasComponent<Engine::TransformComponent>())
        {
            bool opened = ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_DefaultOpen);

            if (opened)
            {
                ImGui::Separator();
                auto &transform = entity.GetComponent<Engine::TransformComponent>();
                ImGui::DragFloat3("Position", glm::value_ptr(transform.Position), 0.1f);
                ImGui::DragFloat3("Rotation", glm::value_ptr(transform.Rotation), 0.1f);
                ImGui::DragFloat3("Scale", glm::value_ptr(transform.Scale), 0.1f);
                ImGui::TreePop();
            }
        }

        if (entity.HasComponent<Engine::SpriteComponent>())
        {
            bool opened = ImGui::TreeNodeEx("Sprite Renderer", ImGuiTreeNodeFlags_DefaultOpen);

            if (opened)
            {
                ImGui::Separator();
                auto &sprite = entity.GetComponent<Engine::SpriteComponent>();
                ImGui::ColorEdit4("Color", glm::value_ptr(sprite.Color));
                ImGui::TreePop();
            }
        }
    }
}