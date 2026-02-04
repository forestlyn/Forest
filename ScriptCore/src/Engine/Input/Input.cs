using System.Runtime.CompilerServices;
namespace Engine
{
    public static class Input
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool IsKeyPressed(int keyCode);
    }
}