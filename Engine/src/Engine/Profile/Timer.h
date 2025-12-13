#pragma once
#include <chrono>
#include <string>
#include <thread>
#include "Engine/Core/Log.h"

namespace Engine::Profile
{

    struct ProfileResult
    {
        std::string Name;
        float Time;
    };

    template <typename Fn>
    class Timer
    {
    public:
        Timer(const std::string &name, Fn &&Fn)
            : m_Name(name), m_Func(Fn), isStopped(false)
        {
            m_StartTimepoint = std::chrono::high_resolution_clock::now();
        }

        ~Timer()
        {
            if (!isStopped)
            {
                Stop();
            }
        }

        void Stop()
        {
            isStopped = true;
            auto endTimepoint = std::chrono::high_resolution_clock::now();
            float duration = std::chrono::duration<float, std::micro>(endTimepoint - m_StartTimepoint).count();
            m_Func({m_Name, duration * 0.001f});
        }

    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;
        std::string m_Name;
        bool isStopped = false;
        Fn m_Func;
    };
}