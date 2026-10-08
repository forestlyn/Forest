using System.Runtime.CompilerServices;
using System;
namespace Engine
{
    public static class ComponentInternalCalls
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool HasComponent(ulong entityID, Type componentType);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool AddComponent(ulong entityID, Type componentType);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool RemoveComponent(ulong entityID, Type componentType);
    }

    public class Component
    {
        public Entity Entity
        {
            get; internal set;
        }
    }


}