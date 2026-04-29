#include "UIUtils.h"
#include <imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>
#include <functional>
#include "Engine/Core/UUID.h"
#include "Engine/Reflection/ReflectMacro.h"

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

    bool UIUtils::DrawValueEdit(const std::string &label, void *value, const Engine::MetaType &type)
    {
        switch (type.kind)
        {
        case Engine::MetaKind::Bool:
        {
            bool *boolValue = (bool *)value;
            if (ImGui::Checkbox(label.c_str(), boolValue))
            {
                return true;
            }
            break;
        }
        case Engine::MetaKind::Int:
        {
            int *intValue = (int *)value;
            if (ImGui::DragInt(label.c_str(), intValue))
            {
                return true;
            }
            break;
        }
        case Engine::MetaKind::Int2:
        {
            glm::ivec2 *vec2Value = (glm::ivec2 *)value;
            if (ImGui::DragInt2(label.c_str(), glm::value_ptr(*vec2Value)))
            {
                return true;
            }
            break;
        }
        case Engine::MetaKind::Int3:
        {
            glm::ivec3 *vec3Value = (glm::ivec3 *)value;
            if (ImGui::DragInt3(label.c_str(), glm::value_ptr(*vec3Value)))
            {
                return true;
            }
            break;
        }
        case Engine::MetaKind::Int4:
        {
            glm::ivec4 *vec4Value = (glm::ivec4 *)value;
            if (ImGui::DragInt4(label.c_str(), glm::value_ptr(*vec4Value)))
            {
                return true;
            }
            break;
        }
        case Engine::MetaKind::Float:
        {
            float *floatValue = (float *)value;
            if (ImGui::DragFloat(label.c_str(), floatValue))
            {
                return true;
            }
            break;
        }
        case Engine::MetaKind::Float2:
        {
            glm::vec2 *vec2Value = (glm::vec2 *)value;
            if (ImGui::DragFloat2(label.c_str(), glm::value_ptr(*vec2Value), 0.1f))
            {
                return true;
            }
            break;
        }
        case Engine::MetaKind::Float3:
        {
            glm::vec3 *vec3Value = (glm::vec3 *)value;
            DrawVector3Control(label, *vec3Value);
            break;
        }
        case Engine::MetaKind::Float4:
        {
            glm::vec4 *vec4Value = (glm::vec4 *)value;
            if (ImGui::DragFloat4(label.c_str(), glm::value_ptr(*vec4Value), 0.1f))
            {
                return true;
            }
            break;
        }
        case Engine::MetaKind::String:
        {
            std::string *stringValue = (std::string *)value;
            char buffer[256];
            strncpy(buffer, stringValue->c_str(), sizeof(buffer));
            if (ImGui::InputText(label.c_str(), buffer, sizeof(buffer)))
            {
                *stringValue = buffer;
                return true;
            }
            break;
        }
        case Engine::MetaKind::Enum:
        {
            if (!type.enumValues || !type.EnumToInt || !type.EnumFromInt)
            {
                ImGui::Text("Invalid enum metadata");
                break;
            }

            const int64_t currentValue = type.EnumToInt(value);
            const char *currentName = "Unknown";
            for (const auto &enumValue : *type.enumValues)
            {
                if (enumValue.Value == currentValue)
                {
                    currentName = enumValue.Name;
                    break;
                }
            }

            bool changed = false;
            if (ImGui::BeginCombo(label.c_str(), currentName))
            {
                for (const auto &enumValue : *type.enumValues)
                {
                    const bool selected = enumValue.Value == currentValue;
                    if (ImGui::Selectable(enumValue.Name, selected))
                    {
                        type.EnumFromInt(value, enumValue.Value);
                        changed = true;
                    }
                    if (selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            if (changed)
            {
                return true;
            }
            break;
        }
        case Engine::MetaKind::UUID:
        {
            Engine::UUID *uuidValue = (Engine::UUID *)value;
            ImGui::Text("UUID: %s", std::to_string((uint64_t)(*uuidValue)).c_str());
            break;
        }
        default:
            ImGui::Text("Unsupported type");
            break;
        }
        return false;
    }

    bool UIUtils::DrawMetaType(const std::string &label, void *obj, const Engine::MetaType &type)
    {
        switch (type.kind)
        {
        case Engine::MetaKind::Float:
        case Engine::MetaKind::Float2:
        case Engine::MetaKind::Float3:
        case Engine::MetaKind::Float4:
        case Engine::MetaKind::Int:
        case Engine::MetaKind::Int2:
        case Engine::MetaKind::Int3:
        case Engine::MetaKind::Int4:
        case Engine::MetaKind::Bool:
        case Engine::MetaKind::String:
        case Engine::MetaKind::Enum:
        case Engine::MetaKind::UUID:
            return DrawValueEdit(label, obj, type);
        case Engine::MetaKind::Struct:
        {
            if (!type.fields)
            {
                ImGui::Text("No fields");
                return false;
            }
            for (const auto &field : *type.fields)
            {
                if (DrawMetaType(field.name, field.get(obj), *field.type))
                {
                    return true;
                }
            }
            break;
        }
        default:
            ImGui::Text("Unsupported type: %s %d", label.c_str(), static_cast<int>(type.kind));
            break;
        }
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
            Engine::UUID value = scriptInstance->GetFieldValue<Engine::UUID>(field.Name);
            ImGui::Text("Entity:%s", std::to_string((uint64_t)value).c_str());
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
            Engine::UUID value = scriptFieldInstance.GetValue<Engine::UUID>();
            ImGui::Text("Entity:%s", std::to_string((uint64_t)value).c_str());
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

    template <IsUIComponent T>
    void UIUtils::DrawComponent(const std::string &name, Engine::Entity entity, bool removeable)
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
            DrawMetaType(name, &component, Engine::Reflect<T>());
            ImGui::TreePop();
        }

        if (entity.GetComponent<T>().IsRemove())
        {
            entity.RemoveComponent<T>();
        }
    }

    // Explicit template instantiations
    template void UIUtils::DrawComponent<Engine::TransformComponent>(const std::string &name, Engine::Entity entity, bool removeable);
    template void UIUtils::DrawComponent<Engine::SpriteComponent>(const std::string &name, Engine::Entity entity, bool removeable);
    template void UIUtils::DrawComponent<Engine::CameraComponent>(const std::string &name, Engine::Entity entity, bool removeable);
    template void UIUtils::DrawComponent<Engine::Rigidbody2DComponent>(const std::string &name, Engine::Entity entity, bool removeable);
    template void UIUtils::DrawComponent<Engine::BoxCollider2DComponent>(const std::string &name, Engine::Entity entity, bool removeable);
    template void UIUtils::DrawComponent<Engine::CircleComponent>(const std::string &name, Engine::Entity entity, bool removeable);
    template void UIUtils::DrawComponent<Engine::CircleCollider2DComponent>(const std::string &name, Engine::Entity entity, bool removeable);
    template void UIUtils::DrawComponent<Engine::ScriptComponent>(const std::string &name, Engine::Entity entity, const std::function<void(Engine::ScriptComponent &)> &uiFunction, bool removeable);
}
