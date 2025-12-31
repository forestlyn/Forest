#pragma once
#include "Engine/Core/Core.h"
#include "Engine/Core/Timestep.h"

namespace Engine
{
    class NativeScriptComponent
    {
    public:
        void *Instance = nullptr;

        using InstantiateFn = void *(*)();
        using OnCreateFn = void (*)(NativeScriptComponent *);
        using OnDestroyScriptFn = void (*)(NativeScriptComponent *);
        using OnUpdateFn = void (*)(NativeScriptComponent *, Core::Timestep);

        InstantiateFn Instantiate = nullptr;
        OnDestroyScriptFn OnDestroy = nullptr;
        OnCreateFn OnCreate = nullptr;
        OnUpdateFn OnUpdate = nullptr;

        template <typename T>
        void Bind()
        {
            Instantiate = []()
            { return static_cast<void *>(new T()); };
            OnCreate = [](NativeScriptComponent *nsc)
            {
                static_cast<T *>(nsc->Instance)->OnCreate();
            };
            OnDestroy = [](NativeScriptComponent *nsc)
            {
                delete static_cast<T *>(nsc->Instance);
                nsc->Instance = nullptr;
            };
            OnUpdate = [](NativeScriptComponent *nsc, Core::Timestep ts)
            {
                static_cast<T *>(nsc->Instance)->OnUpdate(ts);
            };
        }
    };
}