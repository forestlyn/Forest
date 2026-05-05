#pragma once
#include "Engine/Core/Core.h"
#include <string>

namespace Engine
{
    template <typename T>
    struct ResourceRef
    {
        std::string path = "";

        Ref<T> instance = nullptr;

        bool IsValid() const { return !path.empty(); }
        bool IsLoaded() const { return instance != nullptr; }

        void Clear()
        {
            path = "";
            instance = nullptr;
        }

        void SetPath(const std::string &newPath)
        {
            if (path != newPath)
            {
                path = newPath;
                instance = nullptr; // 强制刷新
            }
        }
    };
}