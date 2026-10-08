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

    SceneHierarchyPanel::SceneHierarchyPanel()
    {
        LoadIcons();
    }

    SceneHierarchyPanel::SceneHierarchyPanel(const Engine::Ref<Engine::Scene> &context)
        : m_Context(context)
    {
        LoadIcons();
    }

    void SceneHierarchyPanel::LoadIcons()
    {
        m_LockIcon = Engine::Renderer::Texture2D::Create("resources/assets/textures/icon/lock.png");
        m_UnlockIcon = Engine::Renderer::Texture2D::Create("resources/assets/textures/icon/unlock.png");
    }

    Engine::Entity SceneHierarchyPanel::Resolve(Engine::UUID id) const
    {
        if (!m_Context || uint64_t(id) == 0)
            return {};
        // Resolve quietly: deleted selection/queued UUIDs are normal editor events.
        auto view = m_Context->GetRegistry().view<Engine::IDComponent>();
        for (auto handle : view)
            if (view.get<Engine::IDComponent>(handle).ID == id)
                return {handle, m_Context.get()};
        return {};
    }

    void SceneHierarchyPanel::SetContext(const Engine::Ref<Engine::Scene> &context)
    {
        if (ImGui::GetCurrentContext() && m_DragScene.lock() == m_Context && uint64_t(m_DragEntityID))
            ImGui::ClearDragDrop();
        m_DragScene.reset();
        m_DragEntityID = Engine::UUID(0);
        m_PendingActions.clear();
        m_Expanded.clear();
        m_HierarchyMessage.clear();
        // Never inspect an Entity handle after releasing its owning scene.
        m_SelectionEntity = {};
        m_Context = context;
        SetSelectedEntity(Resolve(m_SelectionEntityID));
    }

    Engine::Entity SceneHierarchyPanel::GetSelectedEntity() const
    {
        if (!m_Context || !m_Context->OwnsEntity(m_SelectionEntity) ||
            m_Context->IsPendingDestruction(m_SelectionEntity))
            return {};
        return m_SelectionEntity;
    }

    void SceneHierarchyPanel::RevealSelection()
    {
        for (auto parent = m_Context->GetParent(m_SelectionEntity); parent; parent = parent.GetParent())
            m_Expanded.insert(parent.GetUUID());
    }

    void SceneHierarchyPanel::SetSelectedEntity(Engine::Entity entity)
    {
        if (!m_Context || !m_Context->OwnsEntity(entity) || m_Context->IsPendingDestruction(entity))
            entity = {};
        m_SelectionEntity = entity;
        m_SelectionEntityID = entity ? entity.GetUUID() : Engine::UUID(0);
        if (entity)
            RevealSelection();
    }

    bool SceneHierarchyPanel::AcceptEntityDrop(Engine::UUID target)
    {
        bool accepted = false;
        if (ImGui::BeginDragDropTarget())
        {
            if (const auto *payload = ImGui::AcceptDragDropPayload(ResourcePayloadTrait<Engine::Entity>::value))
            {
                if (payload->DataSize == sizeof(Engine::UUID))
                {
                    const auto source = *static_cast<const Engine::UUID *>(payload->Data);
                    if (m_DragScene.lock() == m_Context && source == m_DragEntityID)
                    {
                        m_PendingActions.push_back({Action::Reparent, source, target});
                        accepted = true;
                    }
                    else
                        m_HierarchyMessage = "Cannot move an entity from another scene.";
                }
            }
            ImGui::EndDragDropTarget();
        }
        return accepted;
    }

    void SceneHierarchyPanel::OnImGuiRender()
    {
        ENGINE_PROFILING_FUNC();
        if (!GetSelectedEntity())
            SetSelectedEntity({});
        if (!ImGui::GetDragDropPayload())
        {
            m_DragScene.reset();
            m_DragEntityID = Engine::UUID(0);
        }
        if (ImGui::Begin("Scene Hierarchy"))
        {
            m_RowHovered = false;
            if (m_Context)
            {
                ImGui::PushID(m_Context.get());
                const auto roots = m_Context->GetRootEntities();
                for (auto root : roots)
                    DrawEntityNode(root);
                if (roots.empty())
                    ImGui::TextUnformatted("Right click to create an entity.");

                if (ImGui::BeginPopupContextWindow("HierarchyContextMenu",
                                                   ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
                {
                    if (ImGui::MenuItem("Create Empty Entity"))
                        m_PendingActions.push_back({Action::CreateRoot});
                    if (ImGui::MenuItem("Create Canvas"))
                        m_PendingActions.push_back({Action::CreateCanvas});
                    ImGui::EndPopup();
                }
                ImGui::PopID();
            }
            else
                ImGui::TextUnformatted("No scene loaded.");

            // All structural edits happen after traversal, before the Properties window.
            ApplyPendingActions();
            if (!m_HierarchyMessage.empty())
            {
                ImGui::Separator();
                ImGui::TextWrapped("%s", m_HierarchyMessage.c_str());
                if (ImGui::SmallButton("Dismiss"))
                    m_HierarchyMessage.clear();
            }
            // IsAnyItemHovered also includes the previous frame. Test only this frame,
            // after every row/message control, so moving directly into blank space works.
            if (m_Context && !m_LockSelection && !m_RowHovered && GImGui->HoveredId == 0 &&
                !ImGui::GetDragDropPayload() && ImGui::IsWindowHovered() &&
                ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                SetSelectedEntity({});
        }
        ImGui::End();

        if (ImGui::Begin("Properties"))
            if (auto selected = GetSelectedEntity())
                DrawComponents(selected);
        ImGui::End();
    }

    void SceneHierarchyPanel::DrawEntityNode(Engine::Entity entity)
    {
        if (!m_Context->OwnsEntity(entity) || m_Context->IsPendingDestruction(entity))
            return;
        const auto id = entity.GetUUID();
        const std::string label = entity.HasComponent<Engine::TagComponent>()
                                      ? entity.GetName()
                                      : "Entity " + std::to_string(uint64_t(id));
        const auto children = m_Context->GetChildren(entity);
        bool hasChildren = false;
        for (auto child : children)
            hasChildren |= !m_Context->IsPendingDestruction(child);
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
        if (GetSelectedEntity() == entity)
            flags |= ImGuiTreeNodeFlags_Selected;
        if (!hasChildren)
            flags |= ImGuiTreeNodeFlags_Leaf;
        ImGui::PushID(std::to_string(uint64_t(id)).c_str());
        ImGui::SetNextItemOpen(m_Expanded.contains(id), ImGuiCond_Always);
        const bool opened = ImGui::TreeNodeEx("##entity", flags, "%s", label.c_str());
        m_RowHovered |= ImGui::IsItemHovered();
        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen() && !m_LockSelection)
            SetSelectedEntity(entity);
        if (hasChildren)
        {
            if (opened)
                m_Expanded.insert(id);
            else
                m_Expanded.erase(id);
        }

        // These must refer to the current row, before any descendant or popup items.
        if (ImGui::BeginDragDropSource())
        {
            m_DragScene = m_Context;
            m_DragEntityID = id;
            ImGui::SetDragDropPayload(ResourcePayloadTrait<Engine::Entity>::value, &id, sizeof(id));
            ImGui::TextUnformatted(label.c_str());
            ImGui::EndDragDropSource();
        }
        AcceptEntityDrop(id);
        if (ImGui::BeginPopupContextItem())
        {
            auto parent = entity.GetParent();
            const auto siblings = parent ? parent.GetChildren() : m_Context->GetRootEntities();
            const auto index = entity.GetSiblingIndex();
            if (ImGui::MenuItem(entity.HasComponent<Engine::RectTransformComponent>() ? "Create UI Child" : "Create Child"))
                m_PendingActions.push_back({Action::CreateChild, id});
            if (ImGui::MenuItem("Move to Root", nullptr, false, bool(parent)))
                m_PendingActions.push_back({Action::Detach, id});
            if (ImGui::MenuItem("Move Up", nullptr, false, index > 0))
                m_PendingActions.push_back({Action::MoveUp, id});
            if (ImGui::MenuItem("Move Down", nullptr, false, index + 1 < siblings.size()))
                m_PendingActions.push_back({Action::MoveDown, id});
            ImGui::Separator();
            if (ImGui::MenuItem("Duplicate Subtree"))
                m_PendingActions.push_back({Action::Duplicate, id});
            if (ImGui::MenuItem("Delete Subtree"))
                m_PendingActions.push_back({Action::Delete, id});
            ImGui::EndPopup();
        }
        if (opened)
        {
            for (auto child : children)
                DrawEntityNode(child);
            ImGui::TreePop();
        }
        ImGui::PopID();
    }

    void SceneHierarchyPanel::ApplyPendingActions()
    {
        auto actions = std::move(m_PendingActions);
        m_PendingActions.clear();
        if (!m_Context)
            return;
        for (const auto &action : actions)
        {
            m_HierarchyMessage.clear();
            auto entity = Resolve(action.EntityID);
            if (action.Type != Action::CreateRoot && action.Type != Action::CreateCanvas &&
                (!entity || m_Context->IsPendingDestruction(entity)))
            {
                m_HierarchyMessage = "The entity is no longer available.";
                continue;
            }
            switch (action.Type)
            {
            case Action::CreateRoot:
            case Action::CreateCanvas:
            {
                auto created = action.Type == Action::CreateCanvas ? m_Context->CreateCanvas() : m_Context->CreateEntity("Empty Entity");
                created.SetSiblingIndex(UINT32_MAX);
                SetSelectedEntity(created);
                break;
            }
            case Action::CreateChild:
            {
                Engine::Entity created;
                if (entity.HasComponent<Engine::RectTransformComponent>())
                    created = m_Context->CreateUIEntity(entity);
                else
                {
                    created = m_Context->CreateEntity("Empty Entity");
                    if (!m_Context->SetParent(created, entity, &m_HierarchyMessage))
                    {
                        created.Destroy();
                        created = {};
                    }
                }
                if (created)
                {
                    created.SetSiblingIndex(UINT32_MAX);
                    SetSelectedEntity(created);
                }
                else if (m_HierarchyMessage.empty())
                    m_HierarchyMessage = "UI child creation requires a valid root Canvas.";
                break;
            }
            case Action::Reparent:
            case Action::Detach:
            {
                auto target = action.Type == Action::Detach ? Engine::Entity{} : Resolve(action.TargetID);
                if (action.Type == Action::Reparent && !target)
                {
                    m_HierarchyMessage = "The target entity is no longer available.";
                    break;
                }
                if (m_Context->SetParent(entity, target, &m_HierarchyMessage))
                {
                    entity.SetSiblingIndex(UINT32_MAX);
                    SetSelectedEntity(entity);
                    if (!target && entity.HasComponent<Engine::RectTransformComponent>() && !entity.HasComponent<Engine::CanvasComponent>())
                        m_HierarchyMessage = "This UI subtree will not render until attached to a Canvas hierarchy.";
                }
                break;
            }
            case Action::MoveUp:
            case Action::MoveDown:
            {
                const auto index = entity.GetSiblingIndex();
                if (action.Type == Action::MoveDown || index > 0)
                    entity.SetSiblingIndex(action.Type == Action::MoveUp ? index - 1 : index + 1);
                SetSelectedEntity(entity);
                break;
            }
            case Action::Duplicate:
            {
                const auto index = entity.GetSiblingIndex();
                auto copy = m_Context->DuplicateSubtree(entity);
                if (copy)
                {
                    // Duplication may insert before the source on equal sort keys.
                    copy.SetSiblingIndex(UINT32_MAX);
                    copy.SetSiblingIndex(index + 1);
                    SetSelectedEntity(copy);
                }
                else
                    m_HierarchyMessage = "Cannot duplicate this subtree.";
                break;
            }
            case Action::Delete:
                m_Context->DestroyEntity(entity);
                if (!GetSelectedEntity())
                    SetSelectedEntity({});
                break;
            }
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

            if (m_LockIcon && m_UnlockIcon)
            { // Lock Entity
                ImGui::SameLine();
                auto unlock = ImTextureRef((void *)(uint64_t)m_UnlockIcon->GetRendererID());
                auto lock = ImTextureRef((void *)(uint64_t)m_LockIcon->GetRendererID());
                if (ImGui::ImageButton("Lock", m_LockSelection ? lock : unlock, ImVec2(25, 25), ImVec2(1, 1), ImVec2(0, 0), ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1)))
                {
                    m_LockSelection = !m_LockSelection;
                }
            }
        }

        UIUtils::DrawComponent<Engine::RectTransformComponent>("Rect Transform", entity, false, false, m_Context);

        UIUtils::DrawComponent<Engine::TransformComponent>("Transform", entity, false, false, m_Context);

        UIUtils::DrawComponent<Engine::SpriteComponent>("Sprite Renderer", entity, true, true, m_Context);

        UIUtils::DrawComponent<Engine::SpriteAnimationComponent>("Sprite Animation", entity, true, true, m_Context);

        UIUtils::DrawComponent<Engine::CameraComponent>("Camera", entity, true, true, m_Context);

        UIUtils::DrawComponent<Engine::CircleComponent>("Circle", entity, true, true, m_Context);

        UIUtils::DrawComponent<Engine::Rigidbody2DComponent>("Rigidbody 2D", entity, true, true, m_Context);

        UIUtils::DrawComponent<Engine::BoxCollider2DComponent>("Box Collider 2D", entity, true, true, m_Context);
        UIUtils::DrawComponent<Engine::CircleCollider2DComponent>("Circle Collider 2D", entity, true, true, m_Context);

        UIUtils::DrawComponent<Engine::UIImageComponent>("Image", entity, true, true, m_Context);

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
                    UIUtils::DrawScriptInstance(fieldPair.second, instance,scene);
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
                            UIUtils::DrawScriptField(field, fieldInstance,scene);
                        }
                        else
                        {
                            ImGui::Text("Field '%s' not found in script field map", field.Name.c_str());
                            auto &scriptFieldInstance = scriptFieldMap[fieldName];
                            scriptFieldInstance.Field = field;
                            ENGINE_INFO("Creating new script field instance for field '{}' with size {} value {}", 
                                field.Name, sizeof(field.DefaultValue), *(int32_t *)field.DefaultValue);
                            scriptFieldInstance.CopyValueToBuffer(field.DefaultValue, sizeof(field.DefaultValue));
                            UIUtils::DrawScriptField(field, scriptFieldInstance, scene);
                        }
                        if (!exists)
                        {
                            ImGui::PopStyleColor();
                        }
                    }}
                } });
    }
}
