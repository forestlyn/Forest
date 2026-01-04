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
            m_SelectionContext = {};
        }
        void OnImGuiRender();

    private:
        void DrawEntityNode(Engine::Entity entity);
        void DrawComponents(Engine::Entity entity);

    private:
        Engine::Ref<Engine::Scene> m_Context;
        Engine::Entity m_SelectionContext;
    };
};