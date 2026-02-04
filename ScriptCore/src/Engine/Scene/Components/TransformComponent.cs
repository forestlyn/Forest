using System.Runtime.CompilerServices;

namespace Engine
{
    public class TransformComponentInternalCall
    {
        [MethodImplAttribute(System.Runtime.CompilerServices.MethodImplOptions.InternalCall)]
        internal extern static void SetPosition(ulong entityID, Vector3 position);
        [MethodImplAttribute(System.Runtime.CompilerServices.MethodImplOptions.InternalCall)]
        internal extern static Vector3 GetPosition(ulong entityID);
        [MethodImplAttribute(System.Runtime.CompilerServices.MethodImplOptions.InternalCall)]
        internal extern static void SetRotation(ulong entityID, Vector3 rotation);
        [MethodImplAttribute(System.Runtime.CompilerServices.MethodImplOptions.InternalCall)]
        internal extern static Vector3 GetRotation(ulong entityID);
        [MethodImplAttribute(System.Runtime.CompilerServices.MethodImplOptions.InternalCall)]
        internal extern static void SetScale(ulong entityID, Vector3 scale);
        [MethodImplAttribute(System.Runtime.CompilerServices.MethodImplOptions.InternalCall)]
        internal extern static Vector3 GetScale(ulong entityID);
    }


    public class TransformComponent : Component
    {
        public Vector3 Position
        {
            get => TransformComponentInternalCall.GetPosition(Entity.ID);
            set => TransformComponentInternalCall.SetPosition(Entity.ID, value);
        }

        public Vector3 Rotation
        {
            get => TransformComponentInternalCall.GetRotation(Entity.ID);
            set => TransformComponentInternalCall.SetRotation(Entity.ID, value);
        }

        public Vector3 Scale
        {
            get => TransformComponentInternalCall.GetScale(Entity.ID);
            set => TransformComponentInternalCall.SetScale(Entity.ID, value);
        }


        public TransformComponent()
        {
        }
    }
}