using System.Runtime.CompilerServices;
namespace Engine
{
    public static class Input
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool IsKeyPressed(KeyCode keyCode);
    }
}