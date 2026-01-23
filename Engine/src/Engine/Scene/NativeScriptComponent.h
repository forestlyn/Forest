#pragma once
#include "Engine/Core/Core.h"
#include "Engine/Core/Timestep.h"

namespace Engine
{
    class ScriptEntity;
    class NativeScriptComponent
    {
    public:
        ScriptEntity *Instance = nullptr;

        using InstantiateFn = ScriptEntity *(*)();
        using DestroyFn = void (*)(NativeScriptComponent *);

        InstantiateFn Instantiate = nullptr;
        DestroyFn Destroy = nullptr;

        template <typename T>
        void Bind()
        {
            Instantiate = []()
            {
                return static_cast<ScriptEntity *>(new T());
            };
            Destroy = [](NativeScriptComponent *nsc)
            {
                delete nsc->Instance;
                nsc->Instance = nullptr;
            };
        }
    };
}