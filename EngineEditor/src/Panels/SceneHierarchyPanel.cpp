#include "SceneHierarchyPanel.h"
#include "Engine/Profile/Instrumentor.h"
#include "Engine/Scripts/ScriptEngine.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>
#include "UIUtils.h"
#include <filesystem>
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

        if (m_Context)
        {
            if (ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered())
            {
                SetSelectedEntity({});
            }

            if (ImGui::BeginPopupContextWindow("HierarchyContextMenu", ImGuiPopupFlags_MouseButtonRight))
            {
                if (ImGui::MenuItem("Create Empty Entity"))
                {
                    m_Context->CreateEntity("Empty Entity");
                }
                ImGui::EndPopup();
            }

            auto view = m_Context->m_Registry.view<Engine::TagComponent>();
            for (auto entity : view)
            {
                Engine::Entity e{entity, m_Context.get()};
                DrawEntityNode(e);
            }
            if (view.empty())
            {
                ImGui::Text("Right click to create an entity.");
            }
        }
        else
        {
            ImGui::Text("No scene loaded.");
        }

        ImGui::End();

        ImGui::Begin("Properties");
        if (m_SelectionEntity)
        {
            DrawComponents(m_SelectionEntity);
        }
        ImGui::End();
    }

    void SceneHierarchyPanel::DrawEntityNode(Engine::Entity entity)
    {
        const auto &tag = entity.GetComponent<Engine::TagComponent>().Tag;

        ImGuiTreeNodeFlags flags = ((m_SelectionEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
        flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
        bool opened = ImGui::TreeNodeEx((void *)(uint64_t)(uint32_t)(entt::entity)entity, flags, tag.c_str());
        if (ImGui::IsItemClicked())
        {
            SetSelectedEntity(entity);
        }

        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Delete Entity"))
            {
                entity.GetComponent<Engine::TagComponent>().SetRemove(true);
                ENGINE_INFO("Marked entity {} for deletion", tag);
                if (m_SelectionEntity == entity)
                {
                    SetSelectedEntity({});
                }
            }
            ImGui::EndPopup();
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
            auto &tag = entity.GetComponent<Engine::TagComponent>().Tag;
            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            strcpy_s(buffer, sizeof(buffer), tag.c_str());
            if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
            {
                tag = std::string(buffer);
            }

            // Add Component
            ImGui::SameLine();
            if (ImGui::Button("Add Component"))
            {
                ImGui::OpenPopup("AddComponent");
            }
            if (ImGui::BeginPopup("AddComponent"))
            {
                UIUtils::DrawAddComponents(Engine::AddableComponents{}, entity);
                ImGui::EndPopup();
            }
        }

        UIUtils::DrawComponent<Engine::TransformComponent>("Transform", entity, false);

        UIUtils::DrawComponent<Engine::SpriteComponent>("Sprite Renderer", entity);

        UIUtils::DrawComponent<Engine::CameraComponent>("Camera", entity);

        UIUtils::DrawComponent<Engine::CircleComponent>("Circle", entity);

        UIUtils::DrawComponent<Engine::Rigidbody2DComponent>("Rigidbody 2D", entity);

        UIUtils::DrawComponent<Engine::BoxCollider2DComponent>("Box Collider 2D", entity);
        UIUtils::DrawComponent<Engine::CircleCollider2DComponent>("Circle Collider 2D", entity);

        UIUtils::DrawComponent<Engine::ScriptComponent>("Script", entity, [entity, scene = m_Context](Engine::ScriptComponent &scriptComponent)
                                                        {
            bool exists = Engine::ScriptEngine::EntityClassExists(scriptComponent.ScriptClassName);
            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            strcpy_s(buffer, sizeof(buffer), scriptComponent.ScriptClassName.c_str());
            if (!exists)
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.2f, 0.3f, 1.0f));
            if (ImGui::InputText("Script Class", buffer, sizeof(buffer)))
            {
                scriptComponent.ScriptClassName = std::string(buffer);
            }
            if (scene->IsRunning())
            {
                auto instance = Engine::ScriptEngine::GetEntityScriptInstance(entity.GetUUID());
                if (!instance)
                {
                    ImGui::Text("Script instance not found");
                    if (!exists)
                        ImGui::PopStyleColor();
                    return;
                }
                for (const auto &fieldPair : instance->GetScriptClass()->GetFields())
                {
                    UIUtils::DrawScriptInstance(fieldPair.second, instance);
                }
            }
            else
            {
                if (exists)
                {
                    auto entityClass = Engine::ScriptEngine::GetEntityClass(scriptComponent.ScriptClassName);
                    if (!entityClass)
                    {
                        ImGui::Text("Script class '%s' not found", scriptComponent.ScriptClassName.c_str());
                        return;
                    }
                    const auto &fields = entityClass->GetFields();
                    if (fields.empty())
                    {
                        ImGui::Text("No script fields found in class '%s'", scriptComponent.ScriptClassName.c_str());
                        return;
                    }
                    auto &scriptFieldMap = Engine::ScriptEngine::GetScriptFieldMap(entity.GetUUID());
                    for (const auto &[fieldName, field] : fields)
                    {
                        if (scriptFieldMap.find(fieldName) != scriptFieldMap.end())
                        {
                            auto &fieldInstance = scriptFieldMap.at(fieldName);
                            UIUtils::DrawScriptField(field, fieldInstance);
                        }
                        else
                        {
                            ImGui::Text("Field '%s' not found in script field map", field.Name.c_str());
                            auto &scriptFieldInstance = scriptFieldMap[fieldName];
                            scriptFieldInstance.Field = field;
                            ENGINE_INFO("Creating new script field instance for field '{}' with size {} value {}", 
                                field.Name, sizeof(field.DefaultValue), *(int32_t *)field.DefaultValue);
                            scriptFieldInstance.CopyValueToBuffer(field.DefaultValue, sizeof(field.DefaultValue));
                            UIUtils::DrawScriptField(field, scriptFieldInstance);
                        }
                        if (!exists)
                        {
                            ImGui::PopStyleColor();
                        }
                    }}
                } });
    }
}