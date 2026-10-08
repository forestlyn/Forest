#pragma once
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Renderer/Shader/Texture.h"
namespace EngineEditor
{
    class SceneHierarchyPanel
    {
    public:
        SceneHierarchyPanel();
        SceneHierarchyPanel(const Engine::Ref<Engine::Scene> &context);
        ~SceneHierarchyPanel() = default;

        void SetContext(const Engine::Ref<Engine::Scene> &context);
        void OnImGuiRender();

        Engine::Entity GetSelectedEntity() const;
        void SetSelectedEntity(Engine::Entity entity);

    private:
        void DrawEntityNode(Engine::Entity entity);
        void DrawComponents(Engine::Entity entity);
        void LoadIcons();
        enum class Action { CreateRoot, CreateCanvas, CreateChild, Reparent, Detach, MoveUp, MoveDown, Duplicate, Delete };
        struct PendingAction
        {
            Action Type;
            Engine::UUID EntityID{0};
            Engine::UUID TargetID{0};
        };
        void ApplyPendingActions();
        void RevealSelection();
        Engine::Entity Resolve(Engine::UUID id) const;
        bool AcceptEntityDrop(Engine::UUID target);
        friend struct HierarchyPanelTestAccess;


    private:
        Engine::Ref<Engine::Scene> m_Context;
        Engine::Entity m_SelectionEntity;
        Engine::UUID m_SelectionEntityID{0};
        std::vector<PendingAction> m_PendingActions;
        std::unordered_set<Engine::UUID> m_Expanded;
        std::weak_ptr<Engine::Scene> m_DragScene;
        Engine::UUID m_DragEntityID{0};
        std::string m_HierarchyMessage;
        bool m_RowHovered = false;

        bool m_LockSelection = false;

        Engine::Ref<Engine::Renderer::Texture2D> m_LockIcon;
        Engine::Ref<Engine::Renderer::Texture2D> m_UnlockIcon;
    };
};