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
            if (m_SelectionEntity)
            {
                Engine::UUID uuid = m_SelectionEntity.GetUUID();
                auto view = m_Context->GetRegistry().view<Engine::IDComponent>();
                for (auto entity : view)
                {
                    if (view.get<Engine::IDComponent>(entity).ID == uuid)
                    {
                        m_SelectionEntity = Engine::Entity(entity, m_Context.get());
                        return;
                    }
                }
                m_SelectionEntity = {};
            }
        }
        void OnImGuiRender();

        Engine::Entity GetSelectedEntity() const { return m_SelectionEntity; }
        void SetSelectedEntity(Engine::Entity entity) { m_SelectionEntity = entity; }

    private:
        void DrawEntityNode(Engine::Entity entity);
        void DrawComponents(Engine::Entity entity);

    private:
        Engine::Ref<Engine::Scene> m_Context;
        Engine::Entity m_SelectionEntity;
    };
};