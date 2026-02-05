using System.Runtime.CompilerServices;
using System;
namespace Engine
{
    public static class ComponentInternalCall
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool HasComponent(ulong entityID, Type componentType);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AddComponent(ulong entityID, Type componentType);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RemoveComponent(ulong entityID, Type componentType);
    }

    public class Component
    {
        public Entity Entity
        {
            get; internal set;
        }
    }


}