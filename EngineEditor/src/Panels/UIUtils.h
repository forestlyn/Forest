#pragma once
#include <string>
#include <glm/glm.hpp>
#include <functional>
#include "Engine/Scene/Entity.h"
#include <imgui.h>

namespace EngineEditor
{
    class UIUtils
    {
    public:
        static bool DrawVector3Control(const std::string &label, glm::vec3 &values, float resetValue = 0.0f, float columnWidth = 100.0f);
        template <typename T>
        static void DrawComponent(const std::string &name, Engine::Entity entity, const std::function<void(T &)> &uiFunction, bool removeable = true);

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
    };
}