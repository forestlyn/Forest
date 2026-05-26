#pragma once
#include "Engine/Core/Core.h"
#include "Engine/Reflection/ReflectMacro.h"
#include <memory>
#include <string>
#include <glm/glm.hpp>
#include "Engine/Renderer/Shader/Texture.h"
namespace Engine
{
    struct SpriteFrame
    {
        // 基础信息
        std::string Name;                                    // 帧名称
        Engine::ResourceRef<Renderer::Texture2D> TextureRef; // 所在的大图集纹理引用

        // 渲染用的 UV 坐标
        glm::vec2 UVMin{0.0f, 0.0f}; // 左下角 UV
        glm::vec2 UVMax{1.0f, 1.0f}; // 右上角 UV

        // 变换中心（锚点），默认居中 (0.5, 0.5)
        // 范围通常为 [0, 1]，(0,0) 为左下角，(1,1) 为右上角
        glm::vec2 Pivot{0.5f, 0.5f};

        // 尺寸信息（用于处理图集裁剪/Trimming）
        glm::vec2 Size{0.0f, 0.0f};         // 在图集中的实际像素尺寸（裁剪后的宽高）
        glm::vec2 OriginalSize{0.0f, 0.0f}; // 原始未裁剪时的像素尺寸
        glm::vec2 Offset{0.0f, 0.0f};       // 裁剪后，实际图像相对于原始图像左上角的偏移量

        bool IsRotated = false; // 图集打包时是否旋转了 90 度
    };

    REFLECT_TYPE_BEGIN(SpriteFrame)
    REFLECT_FIELD(Name);
    REFLECT_FIELD(TextureRef).Category(FieldCategory::AssetReference).Flags(PropertyFlags::Property_Transient).UIPROPERTY(UIProperty::Hidden);
    REFLECT_FIELD(UVMin);
    REFLECT_FIELD(UVMax);
    REFLECT_FIELD(Pivot);
    REFLECT_FIELD(Size);
    REFLECT_FIELD(OriginalSize);
    REFLECT_FIELD(Offset);
    REFLECT_FIELD(IsRotated);
    REFLECT_TYPE_END(SpriteFrame)
}
