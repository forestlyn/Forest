using System;
using System.Runtime.CompilerServices;

namespace Engine
{
    public static class Log
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void NativeLog(string text);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void NativeLogWarn(string text);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void NativeLogError(string text);
    }
}