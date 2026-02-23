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
                m_SelectionEntity = {};
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
            m_SelectionEntity = entity;
        }

        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Delete Entity"))
            {
                entity.GetComponent<Engine::TagComponent>().SetRemove(true);
                if (m_SelectionEntity == entity)
                {
                    m_SelectionEntity = {};
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

        UIUtils::DrawComponent<Engine::TransformComponent>("Transform", entity, [](Engine::TransformComponent &transform)
                                                           {
                                                               auto position = transform.Position;
                                                               auto rotation = transform.Rotation;
                                                               auto scale = transform.Scale;
                                                               if (UIUtils::DrawVector3Control("Position", position))
                                                               {
                                                                   transform.SetPosition(position);
                                                               }
                                                               ImGui::Separator();
                                                               if (UIUtils::DrawVector3Control("Rotation", rotation))
                                                               {
                                                                   transform.SetRotation(rotation);
                                                               }
                                                                ImGui::Separator();
                                                               if (UIUtils::DrawVector3Control("Scale", scale, 1.0f))
                                                               {
                                                                   transform.SetScale(scale);
                                                               } }, false);

        UIUtils::DrawComponent<Engine::SpriteComponent>("Sprite Renderer", entity, [](Engine::SpriteComponent &sprite)
                                                        {
                                                            ImGui::ColorEdit4("Color", glm::value_ptr(sprite.Color));
                                                            ImGui::Button("Texture");
                                                            if (ImGui::BeginDragDropTarget())
                                                            {
                                                                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
                                                                {
                                                                    const wchar_t *path = (const wchar_t *)payload->Data;
                                                                    std::filesystem::path texturePath = std::filesystem::path(path);
                                                                    sprite.Texture = Engine::Renderer::Texture2D::Create(texturePath.string());
                                                                    ENGINE_TRACE("Loaded texture from path: {}", texturePath.string());
                                                                }
                                                                ImGui::EndDragDropTarget();
                                                            }
                                                            ImGui::SameLine();
                                                            ImGui::Button("Clear Texture");
                                                            if (ImGui::IsItemClicked()){
                                                                sprite.Texture = nullptr;
                                                            }
                                                            ImGui::DragFloat("Tiling Factor", &sprite.TilingFactor, 0.1f, 0.01f, 10.0f); });

        UIUtils::DrawComponent<Engine::CameraComponent>("Camera", entity, [](Engine::CameraComponent &cameraComp)
                                                        {
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
                                                                float fov = sceneCamera->GetPerspectiveFOV();
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
                                                            } });

        UIUtils::DrawComponent<Engine::CircleComponent>("Circle", entity, [](Engine::CircleComponent &circle)
                                                        {
                                                                ImGui::ColorEdit4("Color", glm::value_ptr(circle.Color));
                                                                ImGui::DragFloat("Thickness", &circle.Thickness, 0.01f, 0.0f, 1.0f);
                                                                ImGui::DragFloat("Fade", &circle.Fade, 0.01f, 0.0f, 1.0f); });

        UIUtils::DrawComponent<Engine::Rigidbody2DComponent>("Rigidbody 2D", entity, [](Engine::Rigidbody2DComponent &rigidbody2D)
                                                             {
                                                                const char *bodyTypeStrings[] = {"Static", "Kinematic", "Dynamic"};
                                                                int currentBodyType = (int)rigidbody2D.Type;
                                                                if (ImGui::Combo("Body Type", &currentBodyType, bodyTypeStrings, IM_ARRAYSIZE(bodyTypeStrings)))
                                                                {
                                                                    rigidbody2D.Type = (Engine::Rigidbody2DComponent::BodyType)currentBodyType;
                                                                }
                                                                ImGui::DragFloat2("Velocity", glm::value_ptr(rigidbody2D.Velocity), 0.1f);
                                                                ImGui::DragFloat("Angular Velocity", &rigidbody2D.AngularVelocity, 0.1f);
                                                                ImGui::Checkbox("Fixed Rotation", &rigidbody2D.FixedRotation); });

        UIUtils::DrawComponent<Engine::BoxCollider2DComponent>("Box Collider 2D", entity, [](Engine::BoxCollider2DComponent &boxCollider)
                                                               {
                                                                  ImGui::DragFloat2("Offset", glm::value_ptr(boxCollider.Offset), 0.1f);
                                                                  ImGui::DragFloat2("Size", glm::value_ptr(boxCollider.Size), 0.1f, 0.01f);
                                                                  ImGui::DragFloat("Density", &boxCollider.Density, 0.1f, 0.0f);
                                                                  ImGui::DragFloat("Friction", &boxCollider.Friction, 0.01f, 0.0f, 1.0f);
                                                                  ImGui::DragFloat("Restitution", &boxCollider.Restitution, 0.01f, 0.0f, 1.0f);
                                                                  ImGui::DragFloat("Restitution Threshold", &boxCollider.RestitutionThreshold, 0.1f, 0.0f); });
        UIUtils::DrawComponent<Engine::CircleCollider2DComponent>("Circle Collider 2D", entity, [](Engine::CircleCollider2DComponent &circleCollider)
                                                                  {
                                                                    ImGui::DragFloat("Radius", &circleCollider.Radius, 0.1f, 0.01f);
                                                                    ImGui::DragFloat2("Offset", glm::value_ptr(circleCollider.Offset), 0.1f);
                                                                    ImGui::DragFloat("Density", &circleCollider.Density, 0.1f, 0.0f);
                                                                    ImGui::DragFloat("Friction", &circleCollider.Friction, 0.01f, 0.0f, 1.0f);
                                                                    ImGui::DragFloat("Restitution", &circleCollider.Restitution, 0.01f, 0.0f, 1.0f);
                                                                    ImGui::DragFloat("Restitution Threshold", &circleCollider.RestitutionThreshold, 0.1f, 0.0f); });

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
                            Engine::Ref<Engine::ScriptFieldInstance> fieldInstanceRef = Engine::CreateRef<Engine::ScriptFieldInstance>(fieldInstance);
                            UIUtils::DrawScriptField(field, fieldInstanceRef);
                        }
                        else
                        {
                            ImGui::Text("Field '%s' not found in script field map", field.Name.c_str());
                            auto &scriptFieldInstance = scriptFieldMap[fieldName];
                            scriptFieldInstance.Field = field;
                            switch (field.FieldType)
                            {
                            case Engine::ScriptFieldType::Float:
                            {
                                float value = 0.0f;
                                if (ImGui::DragFloat(field.Name.c_str(), &value))
                                {
                                    scriptFieldInstance.SetValue<float>(value);
                                }
                                break;
                            }
                            case Engine::ScriptFieldType::Double:
                            {
                                float value = 0.0;
                                if (ImGui::DragFloat(field.Name.c_str(), &value))
                                {
                                    double doubleValue = static_cast<double>(value);
                                    scriptFieldInstance.SetValue<double>(doubleValue);
                                }
                                break;
                            }
                            case Engine::ScriptFieldType::Bool:
                            {
                                bool value = false;
                                if (ImGui::Checkbox(field.Name.c_str(), &value))
                                {
                                    scriptFieldInstance.SetValue<bool>(value);
                                }
                                break;
                            }
                            case Engine::ScriptFieldType::Char:
                            {
                                char value = '\0';
                                if (ImGui::InputText(field.Name.c_str(), &value, sizeof(char)))
                                {
                                    scriptFieldInstance.SetValue<char>(value);
                                }
                                break;
                            }
                            case Engine::ScriptFieldType::UByte:
                            {
                                uint8_t value = 0;
                                if (ImGui::DragInt(field.Name.c_str(), (int *)&value, 1.0f, 0, 255))
                                {
                                    scriptFieldInstance.SetValue<uint8_t>(value);
                                }
                                break;
                            }
                            case Engine::ScriptFieldType::Byte:
                            {
                                int8_t value = 0;
                                if (ImGui::DragInt(field.Name.c_str(), (int *)&value, 1.0f, -128, 127))
                                {
                                    scriptFieldInstance.SetValue<int8_t>(value);
                                }
                                break;
                            }
                            case Engine::ScriptFieldType::Short:
                            {
                                int16_t value = 0;
                                if (ImGui::DragInt(field.Name.c_str(), (int *)&value, 1.0f, -32768, 32767))
                                {
                                    scriptFieldInstance.SetValue<int16_t>(value);
                                }
                                break;
                            }
                            case Engine::ScriptFieldType::UShort:
                            {
                                uint16_t value = 0;
                                if (ImGui::DragInt(field.Name.c_str(), (int *)&value, 1.0f, 0, 65535))
                                {
                                    scriptFieldInstance.SetValue<uint16_t>(value);
                                }
                                break;
                            }
                            case Engine::ScriptFieldType::Int:
                            {
                                int value = 0;
                                if (ImGui::DragInt(field.Name.c_str(), &value))
                                {
                                    scriptFieldInstance.SetValue<int>(value);
                                }
                                break;
                            }
                            case Engine::ScriptFieldType::UInt:
                            {
                                uint32_t value = 0;
                                if (ImGui::DragInt(field.Name.c_str(), (int *)&value))
                                {
                                    scriptFieldInstance.SetValue<uint32_t>(value);
                                }
                                break;
                            }
                            case Engine::ScriptFieldType::Long:
                            {
                                int64_t value = 0;
                                if (ImGui::DragInt(field.Name.c_str(), (int *)&value))
                                {
                                    scriptFieldInstance.SetValue<int64_t>(value);
                                }
                                break;
                            }
                            case Engine::ScriptFieldType::ULong:
                            {
                                uint64_t value = 0;
                                if (ImGui::DragInt(field.Name.c_str(), (int *)&value))
                                {
                                    scriptFieldInstance.SetValue<uint64_t>(value);
                                }
                                break;
                            }
                            case Engine::ScriptFieldType::Vector2:
                            {
                                glm::vec2 value(0.0f);
                                if (ImGui::DragFloat2(field.Name.c_str(), glm::value_ptr(value), 0.1f))
                                {
                                    scriptFieldInstance.SetValue<glm::vec2>(value);
                                }
                                break;
                            }
                            case Engine::ScriptFieldType::Vector3:
                            {
                                glm::vec3 value(0.0f);
                                if (ImGui::DragFloat3(field.Name.c_str(), glm::value_ptr(value), 0.1f))
                                {
                                    scriptFieldInstance.SetValue<glm::vec3>(value);
                                }
                                break;
                            }
                            case Engine::ScriptFieldType::Vector4:
                            {
                                glm::vec4 value(0.0f);
                                if (ImGui::DragFloat4(field.Name.c_str(), glm::value_ptr(value), 0.1f))
                                {
                                    scriptFieldInstance.SetValue<glm::vec4>(value);
                                }
                                break;
                            }
                            case Engine::ScriptFieldType::Entity:
                            {
                                ImGui::Text("Entity field type not supported in editor");
                                break;
                            }
                            default:
                            {
                                ImGui::Text("Unknown field type");
                                break;
                            }
                            }
                        }
                        if (!exists)
                        {
                            ImGui::PopStyleColor();
                        }
                    }}
                } });
    }
}