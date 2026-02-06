using System;
using System.Runtime.CompilerServices;

namespace Engine
{
    public static class Log
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void NativeLog(string text);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void NativeLogWarn(string text);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void NativeLogError(string text);
    }
}