#pragma once
#include <string>
#include <glm/glm.hpp>
#include <functional>
#include "Engine/Scene/Entity.h"
#include <imgui.h>
#include "Engine/Scripts/ScriptEngine.h"
#include "Engine/Reflection/MetaStruct.h"
#include "Engine/Scene/Component.h"
namespace EngineEditor
{
    using REFLECT_UI_TYPE = Engine::ComponentGroup<
        Engine::TagComponent,
        Engine::TransformComponent,
        Engine::CameraComponent,
        Engine::SpriteComponent,
        Engine::CircleComponent,
        Engine::Rigidbody2DComponent,
        Engine::BoxCollider2DComponent,
        Engine::CircleCollider2DComponent,
        Engine::ScriptComponent>;

    template <typename T>
    concept IsUIComponent = Engine::IsInComponentGroup<T, REFLECT_UI_TYPE>;

    class UIUtils
    {
    public:
        static bool DrawValueEdit(const std::string &label, void *value, const Engine::MetaType &type);
        static bool DrawMetaType(const std::string &label, void *obj, const Engine::MetaType &type);

        static bool DrawVector3Control(const std::string &label, glm::vec3 &values, float resetValue = 0.0f, float columnWidth = 100.0f);

        template <typename T>
        static void DrawComponent(const std::string &name, Engine::Entity entity, const std::function<void(T &)> &uiFunction, bool removeable = true);

        template <IsUIComponent T>
        static void DrawComponent(const std::string &name, Engine::Entity entity, bool removeable = true);

        template <typename... T>
        static void DrawAddComponents(Engine::Entity entity)
        {
            ([&]<typename T>()
             {
                if (!entity.HasComponent<T>())
                {
                    if (ImGui::MenuItem(typeid(T).name()))
                    {
                        entity.AddComponent<T>();
                        ImGui::CloseCurrentPopup();
                    }
                } }.template operator()<T>(),
             ...);
        }

        template <typename... T>
        static void DrawAddComponents(Engine::ComponentGroup<T...> components, Engine::Entity entity)
        {
            DrawAddComponents<T...>(entity);
        }

        // draw in Editor
        static void DrawScriptField(const Engine::ScriptField &field, Engine::ScriptFieldInstance &scriptFieldInstance);
        // draw in Running
        static void DrawScriptInstance(const Engine::ScriptField &field, Engine::Ref<Engine::ScriptInstance> scriptInstance);
    };
}
