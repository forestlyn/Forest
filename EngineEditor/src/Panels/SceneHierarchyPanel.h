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
            if (m_SelectionContext)
            {
                Engine::UUID uuid = m_SelectionContext.GetUUID();
                auto view = m_Context->GetRegistry().view<Engine::IDComponent>();
                for (auto entity : view)
                {
                    if (view.get<Engine::IDComponent>(entity).ID == uuid)
                    {
                        m_SelectionContext = Engine::Entity(entity, m_Context.get());
                        return;
                    }
                }
                m_SelectionContext = {};
            }
        }
        void OnImGuiRender();

        Engine::Entity GetSelectedEntity() const { return m_SelectionContext; }
        void SetSelectedEntity(Engine::Entity entity) { m_SelectionContext = entity; }

    private:
        void DrawEntityNode(Engine::Entity entity);
        void DrawComponents(Engine::Entity entity);

    private:
        Engine::Ref<Engine::Scene> m_Context;
        Engine::Entity m_SelectionContext;
    };
};