#include "Engine/Core/Input.h"
namespace Engine
{
    static bool IsKeyPressed(int keycode)
    {
        return Core::Input::IsKeyPressed(keycode);
    }
}