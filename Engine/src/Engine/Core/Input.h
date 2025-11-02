#pragma once
#include "Engine/Core/Core.h"
#include "Engine/pcheader.h"
namespace Engine::Core
{
    class FOREST_API Input
    {
    public:
        static bool IsKeyPressed(int keycode)
        {
            ENGINE_ASSERT(s_Instance, "Input instance not set!");
            return s_Instance->IsKeyPressedImpl(keycode);
        }
        static bool IsMouseButtonPressed(int button)
        {
            ENGINE_ASSERT(s_Instance, "Input instance not set!");
            return s_Instance->IsMouseButtonPressedImpl(button);
        }
        static std::pair<float, float> GetMousePosition()
        {
            ENGINE_ASSERT(s_Instance, "Input instance not set!");
            return s_Instance->GetMousePositionImpl();
        }
        static float GetMouseX()
        {
            ENGINE_ASSERT(s_Instance, "Input instance not set!");
            return s_Instance->GetMouseXImpl();
        }
        static float GetMouseY()
        {
            ENGINE_ASSERT(s_Instance, "Input instance not set!");
            return s_Instance->GetMouseYImpl();
        }

    protected:
        virtual bool IsKeyPressedImpl(int keycode) = 0;
        virtual bool IsMouseButtonPressedImpl(int button) = 0;
        virtual std::pair<float, float> GetMousePositionImpl() = 0;
        virtual float GetMouseXImpl() = 0;
        virtual float GetMouseYImpl() = 0;

    private:
        static Input *s_Instance;
    };
}