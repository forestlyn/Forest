#include "UIUtils.h"
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
    void UIUtils::DrawScriptInstance(const Engine::ScriptField &field, Engine::Ref<Engine::ScriptInstance> scriptInstance)
    {
        switch (field.FieldType)
        {
        case Engine::ScriptFieldType::Float:
        {
            float value = scriptInstance->GetFieldValue<float>(field.Name);
            if (ImGui::DragFloat(field.Name.c_str(), &value))
            {
                scriptInstance->SetFieldValue(field.Name, value);
            }
            break;
        }
        case Engine::ScriptFieldType::Double:
        {
            double value = scriptInstance->GetFieldValue<double>(field.Name);
            if (ImGui::DragScalar(field.Name.c_str(),   // 标签
                                  ImGuiDataType_Double, // 数据类型
                                  &value,               // 变量地址
                                  0.1f,                 // 拖拽速度 (注意这里是 float)
                                  nullptr,              // 最小值地址 (必须是 double*)
                                  nullptr,              // 最大值地址 (必须是 double*)
                                  "%.4f"))
            {
                scriptInstance->SetFieldValue(field.Name, value);
            }
            break;
        }
        case Engine::ScriptFieldType::Bool:
        {
            bool value = scriptInstance->GetFieldValue<bool>(field.Name);
            if (ImGui::Checkbox(field.Name.c_str(), &value))
            {
                scriptInstance->SetFieldValue(field.Name, value);
            }
            break;
        }
        case Engine::ScriptFieldType::Char:
        {
            char value = scriptInstance->GetFieldValue<char>(field.Name);
            if (ImGui::InputText(field.Name.c_str(), &value, sizeof(char)))
            {
                scriptInstance->SetFieldValue(field.Name, value);
            }
            break;
        }
        case Engine::ScriptFieldType::UByte:
        {
            uint8_t value = scriptInstance->GetFieldValue<uint8_t>(field.Name);
            if (ImGui::DragInt(field.Name.c_str(), (int *)&value, 1.0f, 0, 255))
            {
                scriptInstance->SetFieldValue(field.Name, value);
            }
            break;
        }
        case Engine::ScriptFieldType::Byte:
        {
            int8_t value = scriptInstance->GetFieldValue<int8_t>(field.Name);
            if (ImGui::DragInt(field.Name.c_str(), (int *)&value, 1.0f, -128, 127))
            {
                scriptInstance->SetFieldValue(field.Name, value);
            }
            break;
        }
        case Engine::ScriptFieldType::Short:
        {
            int16_t value = scriptInstance->GetFieldValue<int16_t>(field.Name);
            if (ImGui::DragInt(field.Name.c_str(), (int *)&value, 1.0f, -32768, 32767))
            {
                scriptInstance->SetFieldValue(field.Name, value);
            }
            break;
        }
        case Engine::ScriptFieldType::UShort:
        {
            uint16_t value = scriptInstance->GetFieldValue<uint16_t>(field.Name);
            if (ImGui::DragInt(field.Name.c_str(), (int *)&value, 1.0f, 0, 65535))
            {
                scriptInstance->SetFieldValue(field.Name, value);
            }
            break;
        }
        case Engine::ScriptFieldType::Int:
        {
            int value = scriptInstance->GetFieldValue<int>(field.Name);
            if (ImGui::DragInt(field.Name.c_str(), &value))
            {
                scriptInstance->SetFieldValue(field.Name, value);
            }
            break;
        }
        case Engine::ScriptFieldType::UInt:
        {
            uint32_t value = scriptInstance->GetFieldValue<uint32_t>(field.Name);
            if (ImGui::DragInt(field.Name.c_str(), (int *)&value))
            {
                scriptInstance->SetFieldValue(field.Name, value);
            }
            break;
        }
        case Engine::ScriptFieldType::Long:
        {
            int64_t value = scriptInstance->GetFieldValue<int64_t>(field.Name);
            if (ImGui::DragScalar(field.Name.c_str(), ImGuiDataType_S64, &value, 1.0f, nullptr, nullptr, "%ld"))
            {
                scriptInstance->SetFieldValue(field.Name, value);
            }
            break;
        }
        case Engine::ScriptFieldType::ULong:
        {
            uint64_t value = scriptInstance->GetFieldValue<uint64_t>(field.Name);
            if (ImGui::DragScalar(field.Name.c_str(), ImGuiDataType_U64, &value, 1.0f, nullptr, nullptr, "%lu"))
            {
                scriptInstance->SetFieldValue(field.Name, value);
            }
            break;
        }
        case Engine::ScriptFieldType::Vector2:
        {
            glm::vec2 value = scriptInstance->GetFieldValue<glm::vec2>(field.Name);
            if (ImGui::DragFloat2(field.Name.c_str(), glm::value_ptr(value), 0.1f))
            {
                scriptInstance->SetFieldValue(field.Name, value);
            }
            break;
        }
        case Engine::ScriptFieldType::Vector3:
        {
            glm::vec3 value = scriptInstance->GetFieldValue<glm::vec3>(field.Name);
            if (ImGui::DragFloat3(field.Name.c_str(), glm::value_ptr(value), 0.1f))
            {
                scriptInstance->SetFieldValue(field.Name, value);
            }
            break;
        }
        case Engine::ScriptFieldType::Vector4:
        {
            glm::vec4 value = scriptInstance->GetFieldValue<glm::vec4>(field.Name);
            if (ImGui::DragFloat4(field.Name.c_str(), glm::value_ptr(value), 0.1f))
            {
                scriptInstance->SetFieldValue(field.Name, value);
            }
            break;
        }
        case Engine::ScriptFieldType::Entity:
        {
            ImGui::Text("Entity field type not supported in editor");
            break;
        }
        default:
        {
            ImGui::Text("Unknown field type");
            break;
        }
        }
    }

    void UIUtils::DrawScriptField(const Engine::ScriptField &field, Engine::ScriptFieldInstance &scriptFieldInstance)
    {
        switch (field.FieldType)
        {
        case Engine::ScriptFieldType::Float:
        {
            float value = scriptFieldInstance.GetValue<float>();
            if (ImGui::DragFloat(field.Name.c_str(), &value))
            {
                scriptFieldInstance.SetValue<float>(value);
            }
            break;
        }
        case Engine::ScriptFieldType::Double:
        {
            double value = scriptFieldInstance.GetValue<double>();
            if (ImGui::DragScalar(field.Name.c_str(),   // 标签
                                  ImGuiDataType_Double, // 数据类型
                                  &value,               // 变量地址
                                  0.1f,                 // 拖拽速度 (注意这里是 float)
                                  nullptr,              // 最小值地址 (必须是 double*)
                                  nullptr,              // 最大值地址 (必须是 double*)
                                  "%.4f"))
            {
                scriptFieldInstance.SetValue<double>(value);
            }
            break;
        }
        case Engine::ScriptFieldType::Bool:
        {
            bool value = scriptFieldInstance.GetValue<bool>();
            if (ImGui::Checkbox(field.Name.c_str(), &value))
            {
                scriptFieldInstance.SetValue<bool>(value);
            }
            break;
        }
        case Engine::ScriptFieldType::Char:
        {
            char value = scriptFieldInstance.GetValue<char>();
            if (ImGui::InputText(field.Name.c_str(), &value, sizeof(char)))
            {
                scriptFieldInstance.SetValue<char>(value);
            }
            break;
        }
        case Engine::ScriptFieldType::UByte:
        {
            uint8_t value = scriptFieldInstance.GetValue<uint8_t>();
            if (ImGui::DragInt(field.Name.c_str(), (int *)&value, 1.0f, 0, 255))
            {
                scriptFieldInstance.SetValue<uint8_t>(value);
            }
            break;
        }
        case Engine::ScriptFieldType::Byte:
        {
            int8_t value = scriptFieldInstance.GetValue<int8_t>();
            if (ImGui::DragInt(field.Name.c_str(), (int *)&value, 1.0f, -128, 127))
            {
                scriptFieldInstance.SetValue<int8_t>(value);
            }
            break;
        }
        case Engine::ScriptFieldType::Short:
        {
            int16_t value = scriptFieldInstance.GetValue<int16_t>();
            if (ImGui::DragInt(field.Name.c_str(), (int *)&value, 1.0f, -32768, 32767))
            {
                scriptFieldInstance.SetValue<int16_t>(value);
            }
            break;
        }
        case Engine::ScriptFieldType::UShort:
        {
            uint16_t value = scriptFieldInstance.GetValue<uint16_t>();
            if (ImGui::DragInt(field.Name.c_str(), (int *)&value, 1.0f, 0, 65535))
            {
                scriptFieldInstance.SetValue<uint16_t>(value);
            }
            break;
        }
        case Engine::ScriptFieldType::Int:
        {
            int value = scriptFieldInstance.GetValue<int>();
            if (ImGui::DragInt(field.Name.c_str(), &value))
            {
                scriptFieldInstance.SetValue<int>(value);
            }
            break;
        }
        case Engine::ScriptFieldType::UInt:
        {
            uint32_t value = scriptFieldInstance.GetValue<uint32_t>();
            if (ImGui::DragInt(field.Name.c_str(), (int *)&value))
            {
                scriptFieldInstance.SetValue<uint32_t>(value);
            }
            break;
        }
        case Engine::ScriptFieldType::Long:
        {
            int64_t value = scriptFieldInstance.GetValue<int64_t>();
            if (ImGui::DragScalar(field.Name.c_str(), ImGuiDataType_S64, &value, 1.0f, nullptr, nullptr, "%lld"))
            {
                scriptFieldInstance.SetValue<int64_t>(value);
            }
            break;
        }
        case Engine::ScriptFieldType::ULong:
        {
            uint64_t value = 0;
            if (ImGui::DragScalar(field.Name.c_str(), ImGuiDataType_U64, &value, 1.0f, nullptr, nullptr, "%llu"))
            {
                scriptFieldInstance.SetValue<uint64_t>(value);
            }
            break;
        }
        case Engine::ScriptFieldType::Vector2:
        {
            glm::vec2 value = scriptFieldInstance.GetValue<glm::vec2>();
            if (ImGui::DragFloat2(field.Name.c_str(), glm::value_ptr(value), 0.1f))
            {
                scriptFieldInstance.SetValue<glm::vec2>(value);
            }
            break;
        }
        case Engine::ScriptFieldType::Vector3:
        {
            glm::vec3 value = scriptFieldInstance.GetValue<glm::vec3>();
            if (ImGui::DragFloat3(field.Name.c_str(), glm::value_ptr(value), 0.1f))
            {
                scriptFieldInstance.SetValue<glm::vec3>(value);
            }
            break;
        }
        case Engine::ScriptFieldType::Vector4:
        {
            glm::vec4 value = scriptFieldInstance.GetValue<glm::vec4>();
            if (ImGui::DragFloat4(field.Name.c_str(), glm::value_ptr(value), 0.1f))
            {
                scriptFieldInstance.SetValue<glm::vec4>(value);
            }
            break;
        }
        case Engine::ScriptFieldType::Entity:
        {
            ImGui::Text("Entity field type not supported in editor");
            break;
        }
        default:
        {
            ImGui::Text("Unknown field type");
            break;
        }
        }
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

    // Explicit template instantiations
    template void UIUtils::DrawComponent<Engine::TransformComponent>(const std::string &name, Engine::Entity entity, const std::function<void(Engine::TransformComponent &)> &uiFunction, bool removeable);
    template void UIUtils::DrawComponent<Engine::SpriteComponent>(const std::string &name, Engine::Entity entity, const std::function<void(Engine::SpriteComponent &)> &uiFunction, bool removeable);
    template void UIUtils::DrawComponent<Engine::CameraComponent>(const std::string &name, Engine::Entity entity, const std::function<void(Engine::CameraComponent &)> &uiFunction, bool removeable);
    template void UIUtils::DrawComponent<Engine::Rigidbody2DComponent>(const std::string &name, Engine::Entity entity, const std::function<void(Engine::Rigidbody2DComponent &)> &uiFunction, bool removeable);
    template void UIUtils::DrawComponent<Engine::BoxCollider2DComponent>(const std::string &name, Engine::Entity entity, const std::function<void(Engine::BoxCollider2DComponent &)> &uiFunction, bool removeable);
    template void UIUtils::DrawComponent<Engine::CircleComponent>(const std::string &name, Engine::Entity entity, const std::function<void(Engine::CircleComponent &)> &uiFunction, bool removeable);
    template void UIUtils::DrawComponent<Engine::CircleCollider2DComponent>(const std::string &name, Engine::Entity entity, const std::function<void(Engine::CircleCollider2DComponent &)> &uiFunction, bool removeable);
    template void UIUtils::DrawComponent<Engine::ScriptComponent>(const std::string &name, Engine::Entity entity, const std::function<void(Engine::ScriptComponent &)> &uiFunction, bool removeable);
}