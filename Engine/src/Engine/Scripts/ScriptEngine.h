#pragma once
namespace Engine
{
    enum class Accessibility : uint8_t
    {
        None = 0,
        Private = (1 << 0),
        Internal = (1 << 1),
        Protected = (1 << 2),
        Public = (1 << 3)
    };
    class ScriptEngine
    {
    public:
        static void Init();
        static void Shutdown();

    private:
        static void InitMono();
        static void ShutdownMono();
    };
}