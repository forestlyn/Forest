#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Entity.h"
namespace EngineEditor
{
    class SceneHierarchyPanel
    {
    public:
        SceneHierarchyPanel() = default;
        SceneHierarchyPanel(const Engine::Ref<Engine::Scene> &context);
        ~SceneHierarchyPanel() = default;

        void SetContext(const Engine::Ref<Engine::Scene> &context)
        {
            m_Context = context;

            auto view = m_Context->GetRegistry().view<Engine::IDComponent>();
            for (auto entity : view)
            {
                if (view.get<Engine::IDComponent>(entity).ID == m_SelectionEntityID)
                {
                    m_SelectionEntity = Engine::Entity(entity, m_Context.get());
                    return;
                }
            }
            m_SelectionEntity = {};
        }
        void OnImGuiRender();

        Engine::Entity GetSelectedEntity() const { return m_SelectionEntity; }
        void SetSelectedEntity(Engine::Entity entity)
        {
            m_SelectionEntity = entity;
            if (entity)
            {
                m_SelectionEntityID = entity.GetUUID();
            }
            else
            {
                m_SelectionEntityID = {};
            }
        }

    private:
        void DrawEntityNode(Engine::Entity entity);
        void DrawComponents(Engine::Entity entity);

    private:
        Engine::Ref<Engine::Scene> m_Context;
        Engine::Entity m_SelectionEntity;
        Engine::UUID m_SelectionEntityID;
    };
};