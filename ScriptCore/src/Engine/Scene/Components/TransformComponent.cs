using System.Runtime.CompilerServices;

namespace Engine
{
    public class TransformComponentInternalCalls
    {
        [MethodImplAttribute(System.Runtime.CompilerServices.MethodImplOptions.InternalCall)]
        internal extern static void SetPosition(ulong entityID, ref Vector3 position);
        [MethodImplAttribute(System.Runtime.CompilerServices.MethodImplOptions.InternalCall)]
        internal extern static void GetPosition(ulong entityID, out Vector3 position);
        [MethodImplAttribute(System.Runtime.CompilerServices.MethodImplOptions.InternalCall)]
        internal extern static void SetRotation(ulong entityID, ref Vector3 rotation);
        [MethodImplAttribute(System.Runtime.CompilerServices.MethodImplOptions.InternalCall)]
        internal extern static void GetRotation(ulong entityID, out Vector3 rotation);
        [MethodImplAttribute(System.Runtime.CompilerServices.MethodImplOptions.InternalCall)]
        internal extern static void SetScale(ulong entityID, ref Vector3 scale);
        [MethodImplAttribute(System.Runtime.CompilerServices.MethodImplOptions.InternalCall)]
        internal extern static void GetScale(ulong entityID, out Vector3 scale);
    }


    public class TransformComponent : Component
    {
        public Vector3 Position
        {
            get
            {
                TransformComponentInternalCalls.GetPosition(Entity.ID, out Vector3 position);
                return position;
            }
            set => TransformComponentInternalCalls.SetPosition(Entity.ID, ref value);
        }

        public Vector3 Rotation
        {
            get
            {
                TransformComponentInternalCalls.GetRotation(Entity.ID, out Vector3 rotation);
                return rotation;
            }
            set => TransformComponentInternalCalls.SetRotation(Entity.ID, ref value);
        }

        public Vector3 Scale
        {
            get
            {
                TransformComponentInternalCalls.GetScale(Entity.ID, out Vector3 scale);
                return scale;
            }
            set => TransformComponentInternalCalls.SetScale(Entity.ID, ref value);
        }


        public TransformComponent()
        {
        }
    }
}