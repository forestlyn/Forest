#pragma once
#include <string>
#include <vector>
#include "SpriteFrame.h"
namespace Engine
{
    struct AnimFrame
    {
        SpriteFrame Frame;
        float Duration; // 持续时间，单位为秒
    };

    struct AnimationClip2D
    {
        std::string name;
        std::vector<AnimFrame> frames; // 每帧的图像数据

        bool loop = true; // 是否循环播放

        // 获取动画总时长
        float GetTotalDuration() const
        {
            float totalDuration = 0.0f;
            for (const auto &frame : frames)
            {
                totalDuration += frame.Duration;
            }
            return totalDuration;
        }

        // 获取当前时间点对应的帧索引
        int GetFrameIndexAtTime(float time) const
        {
            if (frames.empty() || time < 0.0f)
                return -1; // 没有帧或持续时间，返回无效索引
            if (loop)
            {
                float totalDuration = GetTotalDuration();
                if (totalDuration <= 0.0f)
                    return -1;                    // 总时长为零，返回无效索引
                time = fmod(time, totalDuration); // 循环时间
            }
            float accumulatedTime = 0.0f;
            for (size_t i = 0; i < frames.size(); ++i)
            {
                accumulatedTime += frames[i].Duration;
                if (time < accumulatedTime)
                {
                    return static_cast<int>(i);
                }
            }
            return static_cast<int>(frames.size() - 1); // 如果时间超过总时长，返回最后一帧
        }

        SpriteFrame GetCurrentFrame(float time) const
        {
            int frameIndex = GetFrameIndexAtTime(time);
            if (frameIndex >= 0 && frameIndex < frames.size())
            {
                return frames[frameIndex].Frame;
            }
            return SpriteFrame(); // 返回默认帧
        }
    };
}