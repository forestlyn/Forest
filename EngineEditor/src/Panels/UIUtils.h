#pragma once
#include <string>
#include <glm/glm.hpp>
#include <functional>
#include "Engine/Scene/Entity.h"
#include <imgui.h>
#include "Engine/Scripts/ScriptEngine.h"
#include "Engine/Reflection/MetaStruct.h"
#include "Engine/Scene/Component.h"
#include "Engine/Resource/ResourceRef.h"
#include "Payload/DragDropPayload.h"
#include "Engine/Project/Project.h"
#include <filesystem>
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
        static bool DrawValueEdit(const std::string &label, void *value, const Engine::MetaType &type, Engine::UIKind uiKind = Engine::UIKind::UITYPE_Default);
        static bool DrawMetaType(const std::string &label, void *obj, const Engine::MetaType &type, Engine::UIKind uiKind = Engine::UIKind::UITYPE_Default);

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

        template <typename T>
        static void DrawResourceRefField(const char *label, Engine::ResourceRef<T> &ref)
        {
            ImGui::Text("%s", label);
            ImGui::SameLine();
            ImGui::Button(ref.path.empty() ? "None" : ref.path.c_str(), ImVec2(-1, 0));

            if (ImGui::BeginDragDropTarget())
            {
                const char *expectedPayload = ResourcePayloadTrait<T>::value;

                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload(expectedPayload))
                {
                    const wchar_t *path = (const wchar_t *)payload->Data;
                    std::filesystem::path filepath(path);
                    // ENGINE_INFO("Accepted payload of type '{}'", expectedPayload);
                    // ENGINE_INFO("Payload data: {}", filepath.string());
                    std::string newPath = filepath.string();
                    ref.SetPath(newPath);
                }
                ImGui::EndDragDropTarget();
            }
        }

        // template <typename T>
        // static void DrawObjectRefField(const char *label, UUID &uuid, const std::function<T *(const UUID &)> &resolver)
        // {
        //     ImGui::Text("%s", label);
        //     ImGui::SameLine();
        //     ImGui::Button(resolver(uuid) ? "Set" : "None", ImVec2(-1, 0));

        //     if (ImGui::BeginDragDropTarget())
        //     {
        //         const char *expectedPayload = typeid(T).name();

        //         if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload(expectedPayload))
        //         {
        //             T *object = *(T **)payload->Data;
        //             ref = object;
        //         }
        //         ImGui::EndDragDropTarget();
        //     }
        // }

        // draw in Editor
        static void DrawScriptField(const Engine::ScriptField &field, Engine::ScriptFieldInstance &scriptFieldInstance);
        // draw in Running
        static void DrawScriptInstance(const Engine::ScriptField &field, Engine::Ref<Engine::ScriptInstance> scriptInstance);
    };
}
