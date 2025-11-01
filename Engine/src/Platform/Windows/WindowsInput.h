#pragma once
#include "Engine/Core/Input.h"

namespace Engine::Platform::Windows
{
    class WindowsInput : public Engine::Core::Input
    {
    protected:
        bool IsKeyPressedImpl(int keycode) override;
        bool IsMouseButtonPressedImpl(int button) override;
        std::pair<float, float> GetMousePositionImpl() override;
        float GetMouseXImpl() override;
        float GetMouseYImpl() override;
    };
}