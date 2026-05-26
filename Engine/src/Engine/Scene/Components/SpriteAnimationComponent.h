#pragma once
#include "Engine/Animation/SpriteFrame.h"

namespace Engine
{
    struct SpriteAnimationComponent
    {
        std::vector<SpriteFrame> Frames; // 动画序列帧
        float FrameDuration = 0.1f;      // 每帧持续时间（秒）

        float ElapsedTime = 0.0f;     // 累计时间
        size_t CurrentFrameIndex = 0; // 当前播放到第几帧
        bool IsLooping = true;
        bool IsPlaying = true;
    };

    REFLECT_TYPE_BEGIN(SpriteAnimationComponent)
    REFLECT_FIELD(FrameDuration);
    REFLECT_FIELD(ElapsedTime).Flags(PropertyFlags::Property_Transient).UIPROPERTY(UIProperty::ReadOnly);
    REFLECT_FIELD(CurrentFrameIndex).Flags(PropertyFlags::Property_Transient).UIPROPERTY(UIProperty::ReadOnly);
    REFLECT_FIELD(IsLooping);
    REFLECT_FIELD(IsPlaying);
    REFLECT_TYPE_END(SpriteAnimationComponent)
}