#pragma once
#include "Engine/Animation/SpriteFrame.h"
#include "Engine/Animation/TextureAtlas.h"
#include "Engine/Animation/AnimationClip2D.h"
namespace Engine
{
    struct SpriteAnimationComponent : public BaseComponent
    {
        std::unordered_map<std::string, ResourceRef<AnimationClip2D>> Animations;

        Ref<AnimationClip2D> ActiveClip = nullptr;
        std::string ActiveStateName = "";
        size_t CurrentFrameIndex = 0; // 当前播放到第几帧

        float ElapsedTime = 0.0f; // 当前帧累计播放时间
        float SpeedMultiplier = 1.0f;
        bool IsPlaying = true;

        void Play(const std::string &stateName)
        {
            auto it = Animations.find(stateName);
            if (it != Animations.end())
            {
                ActiveClip = it->second.instance;
                ActiveStateName = stateName;
                CurrentFrameIndex = 0;
                ElapsedTime = 0.0f;
                IsPlaying = true;
            }
            else
            {
                ENGINE_WARN("Animation state '{}' not found.", stateName);
            }
        }

        void Stop()
        {
            IsPlaying = false;
            ElapsedTime = 0.0f;
            CurrentFrameIndex = 0;
        }

        void Update(float deltaTime)
        {
            if (!IsPlaying || !ActiveClip)
                return;

            ElapsedTime += deltaTime * SpeedMultiplier;

            int newFrameIndex = ActiveClip->GetFrameIndexAtTime(ElapsedTime);
            if (newFrameIndex != -1 && newFrameIndex != static_cast<int>(CurrentFrameIndex))
            {
                CurrentFrameIndex = newFrameIndex;
                // 可以在这里触发帧事件，例如播放声音、生成粒子等
            }
        }
    };

    REFLECT_TYPE_BEGIN(SpriteAnimationComponent)
    REFLECT_FIELD(Animations);
    REFLECT_FIELD(ElapsedTime).Flags(PropertyFlags::Property_Transient).UIPROPERTY(UIProperty::ReadOnly);
    REFLECT_FIELD(CurrentFrameIndex).Flags(PropertyFlags::Property_Transient).UIPROPERTY(UIProperty::ReadOnly);
    REFLECT_FIELD(SpeedMultiplier);
    REFLECT_FIELD(IsPlaying).Flags(PropertyFlags::Property_Transient).UIPROPERTY(UIProperty::ReadOnly);
    REFLECT_TYPE_END(SpriteAnimationComponent)
}