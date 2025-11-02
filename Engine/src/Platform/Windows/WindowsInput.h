#pragma once
#include "Engine/Core/Input.h"

namespace Engine::Platform::Windows
{
    class WindowsInput : public Engine::Core::Input
    {
    protected:
        bool IsKeyPressedImpl(int keycode) override;
        bool IsMouseButtonPressedImpl(int button) override;
        Engine::Math::Vector2 GetMousePositionImpl() override;
        float GetMouseXImpl() override;
        float GetMouseYImpl() override;
    };
}