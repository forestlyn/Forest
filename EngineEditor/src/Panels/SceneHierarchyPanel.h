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

        void SetContext(const Engine::Ref<Engine::Scene> &context) { m_Context = context; }
        void OnImGuiRender();

    private:
        static bool DrawVector3Control(const std::string &label, glm::vec3 &values, float resetValue = 0.0f, float columnWidth = 100.0f);

        void DrawEntityNode(Engine::Entity entity);
        void DrawComponents(Engine::Entity entity);

    private:
        Engine::Ref<Engine::Scene> m_Context;
        Engine::Entity m_SelectionContext;
    };
};