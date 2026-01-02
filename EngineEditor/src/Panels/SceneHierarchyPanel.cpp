#include "SceneHierarchyPanel.h"
#include "Engine/Profile/Instrumentor.h"
#include <imgui.h>
#include <imgui_internal.h>
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

    bool SceneHierarchyPanel::DrawVector3Control(const std::string &label, glm::vec3 &values, float resetValue, float columnWidth)
    {
        bool changed = false;
        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::TextUnformatted(label.c_str());
        ImGui::NextColumn();
        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});
        float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
        ImVec2 buttonSize = {lineHeight + 3.0f, lineHeight};
        // X
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
        if (ImGui::Button("X", buttonSize))
        {
            values.x = resetValue;
            changed = true;
        }
        ImGui::PopStyleColor();
        ImGui::SameLine();
        if (ImGui::DragFloat("##X", &values.x, 0.1f))
        {
            changed = true;
        }
        ImGui::PopItemWidth();
        ImGui::SameLine();
        // Y
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
        if (ImGui::Button("Y", buttonSize))
        {
            values.y = resetValue;
            changed = true;
        }
        ImGui::PopStyleColor();
        ImGui::SameLine();
        if (ImGui::DragFloat("##Y", &values.y, 0.1f))
        {
            changed = true;
        }
        ImGui::PopItemWidth();
        ImGui::SameLine();
        // Z
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.15f, 0.25f, 0.8f, 1.0f});
        if (ImGui::Button("Z", buttonSize))
        {
            values.z = resetValue;
            changed = true;
        }
        ImGui::PopStyleColor();
        ImGui::SameLine();
        if (ImGui::DragFloat("##Z", &values.z, 0.1f))
        {
            changed = true;
        }
        ImGui::PopItemWidth();
        ImGui::PopStyleVar();
        ImGui::Columns(1);
        ImGui::PopID();
        return changed;
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
                auto position = transform.Position;
                auto rotation = transform.Rotation;
                auto scale = transform.Scale;
                if (DrawVector3Control("Position", position))
                {
                    transform.SetPosition(position);
                }
                if (DrawVector3Control("Rotation", rotation))
                {
                    transform.SetRotation(rotation);
                }
                if (DrawVector3Control("Scale", scale, 1.0f))
                {
                    transform.SetScale(scale);
                }
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

        if (entity.HasComponent<Engine::CameraComponent>())
        {
            bool opened = ImGui::TreeNodeEx("Camera", ImGuiTreeNodeFlags_DefaultOpen);

            if (opened)
            {
                ImGui::Separator();
                auto &cameraComp = entity.GetComponent<Engine::CameraComponent>();
                auto &sceneCamera = cameraComp.Camera;

                const char *projectionTypeStrings[] = {"Orthographic", "Perspective"};
                int currentProjectionType = (int)sceneCamera->GetProjectionType();
                if (ImGui::Combo("Projection Type", &currentProjectionType, projectionTypeStrings, IM_ARRAYSIZE(projectionTypeStrings)))
                {
                    sceneCamera->SetProjectionType((Engine::SceneCamera::ProjectionType)currentProjectionType);
                }

                bool isPrimary = cameraComp.Primary;
                if (ImGui::Checkbox("Primary Camera", &isPrimary))
                {
                    cameraComp.Primary = isPrimary;
                }
                bool fixedAspectRatio = cameraComp.FixedAspectRatio;
                if (ImGui::Checkbox("Fixed Aspect Ratio", &fixedAspectRatio))
                {
                    cameraComp.FixedAspectRatio = fixedAspectRatio;
                }

                float aspectRatio = sceneCamera->GetAspectRatio();
                if (ImGui::DragFloat("Aspect Ratio", &aspectRatio, 0.01f, 0.1f, 10.0f))
                {
                    sceneCamera->SetAspectRatio(aspectRatio);
                }

                if (sceneCamera->GetProjectionType() == Engine::SceneCamera::ProjectionType::Orthographic)
                {
                    float orthoSize = sceneCamera->GetOrthographicSize();
                    if (ImGui::DragFloat("Orthographic Size", &orthoSize, 0.1f, 0.01f))
                    {
                        sceneCamera->SetOrthographicSize(orthoSize);
                    }
                    float nearClip = sceneCamera->GetOrthographicNearClip();
                    if (ImGui::DragFloat("Near Clip", &nearClip, 0.1f, 0.01f))
                    {
                        sceneCamera->SetOrthographicNearClip(nearClip);
                    }
                    float farClip = sceneCamera->GetOrthographicFarClip();
                    if (ImGui::DragFloat("Far Clip", &farClip, 0.1f))
                    {
                        sceneCamera->SetOrthographicFarClip(farClip);
                    }
                }
                else if (sceneCamera->GetProjectionType() == Engine::SceneCamera::ProjectionType::Perspective)
                {
                    float fov = glm::degrees(sceneCamera->GetPerspectiveFOV());
                    if (ImGui::DragFloat("Field of View", &fov))
                    {
                        sceneCamera->SetPerspectiveFOV(fov);
                    }
                    float nearClip = sceneCamera->GetPerspectiveNearClip();
                    if (ImGui::DragFloat("Near Clip", &nearClip))
                    {
                        sceneCamera->SetPerspectiveNearClip(nearClip);
                    }
                    float farClip = sceneCamera->GetPerspectiveFarClip();
                    if (ImGui::DragFloat("Far Clip", &farClip))
                    {
                        sceneCamera->SetPerspectiveFarClip(farClip);
                    }
                }
                ImGui::TreePop();
            }
        }
    }
}