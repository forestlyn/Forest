#include "UIUtils.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>
#include <functional>
#include "Engine/Scene/Component.h"
namespace EngineEditor
{
    bool UIUtils::DrawVector3Control(const std::string &label, glm::vec3 &values, float resetValue, float columnWidth)
    {
        bool changed = false;

        ImGuiIO &io = ImGui::GetIO();

        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::TextUnformatted(label.c_str());
        ImGui::NextColumn();
        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});
        float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
        ImVec2 buttonSize = {lineHeight + 3.0f, lineHeight};
        // X
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});

        ImGui::PushFont(io.Fonts->Fonts[0]);
        if (ImGui::Button("X", buttonSize))
        {
            values.x = resetValue;
            changed = true;
        }
        ImGui::PopFont();

        ImGui::PopStyleColor();
        ImGui::SameLine();
        if (ImGui::DragFloat("##X", &values.x, 0.1f))
        {
            changed = true;
        }
        ImGui::PopItemWidth();
        ImGui::SameLine();
        // Y
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
        ImGui::PushFont(io.Fonts->Fonts[0]);
        if (ImGui::Button("Y", buttonSize))
        {
            values.y = resetValue;
            changed = true;
        }
        ImGui::PopFont();
        ImGui::PopStyleColor();
        ImGui::SameLine();
        if (ImGui::DragFloat("##Y", &values.y, 0.1f))
        {
            changed = true;
        }
        ImGui::PopItemWidth();
        ImGui::SameLine();
        // Z
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.15f, 0.25f, 0.8f, 1.0f});
        ImGui::PushFont(io.Fonts->Fonts[0]);
        if (ImGui::Button("Z", buttonSize))
        {
            values.z = resetValue;
            changed = true;
        }
        ImGui::PopFont();
        ImGui::PopStyleColor();
        ImGui::SameLine();
        if (ImGui::DragFloat("##Z", &values.z, 0.1f))
        {
            changed = true;
        }
        ImGui::PopItemWidth();
        ImGui::PopStyleVar();
        ImGui::Columns(1);
        ImGui::PopID();
        return changed;
    }
    template <typename T>
    void UIUtils::DrawComponent(const std::string &name, Engine::Entity entity, const std::function<void(T &)> &uiFunction, bool removeable)
    {
        ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_AllowOverlap | ImGuiTreeNodeFlags_SpanAvailWidth;
        if (!entity.HasComponent<T>())
            return;
        auto &component = entity.GetComponent<T>();
        ImGui::Separator();
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 4));

        ImVec2 contentRegion = ImGui::GetContentRegionAvail();
        float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;

        bool opened = ImGui::TreeNodeEx(name.c_str(), treeNodeFlags);
        if (removeable)
        {
            ImGui::SameLine(contentRegion.x - lineHeight * 0.5f);
            if (ImGui::Button("+", ImVec2{lineHeight, lineHeight}))
            {
                ImGui::OpenPopup("RemoveComponent");
            }
            if (ImGui::BeginPopup("RemoveComponent"))
            {
                if (ImGui::MenuItem("Remove Component"))
                {
                    entity.GetComponent<T>().SetRemove(true);
                }
                ImGui::EndPopup();
            }
        }
        ImGui::PopStyleVar();

        if (opened)
        {
            ImGui::Separator();
            uiFunction(component);
            ImGui::TreePop();
        }

        if (entity.GetComponent<T>().IsRemove())
        {
            entity.RemoveComponent<T>();
        }
    }

    template void UIUtils::DrawComponent<Engine::TransformComponent>(const std::string &name, Engine::Entity entity, const std::function<void(Engine::TransformComponent &)> &uiFunction, bool removeable);
    template void UIUtils::DrawComponent<Engine::SpriteComponent>(const std::string &name, Engine::Entity entity, const std::function<void(Engine::SpriteComponent &)> &uiFunction, bool removeable);
    template void UIUtils::DrawComponent<Engine::CameraComponent>(const std::string &name, Engine::Entity entity, const std::function<void(Engine::CameraComponent &)> &uiFunction, bool removeable);
    template void UIUtils::DrawComponent<Engine::Rigidbody2DComponent>(const std::string &name, Engine::Entity entity, const std::function<void(Engine::Rigidbody2DComponent &)> &uiFunction, bool removeable);
    template void UIUtils::DrawComponent<Engine::BoxCollider2DComponent>(const std::string &name, Engine::Entity entity, const std::function<void(Engine::BoxCollider2DComponent &)> &uiFunction, bool removeable);
}