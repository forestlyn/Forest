#pragma once
#include "Engine/Core/Core.h"
#include "Engine/Core/Timestep.h"
#include "ScriptEntity.h"
namespace Engine
{
    class NativeScriptComponent
    {
    public:
        ScriptEntity *Instance = nullptr;

        using InstantiateFn = ScriptEntity *(*)();
        using DestoryFn = void (*)(NativeScriptComponent *);

        InstantiateFn Instantiate = nullptr;
        DestoryFn Destory = nullptr;

        template <typename T>
        void Bind()
        {
            Instantiate = []()
            {
                return static_cast<ScriptEntity *>(new T());
            };
            Destory = [](NativeScriptComponent *nsc)
            {
                delete nsc->Instance;
                nsc->Instance = nullptr;
            };
        }
    };
}