using System.Runtime.CompilerServices;

namespace Engine
{
    public class TransformComponentInternalCall
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
                TransformComponentInternalCall.GetPosition(Entity.ID, out Vector3 position);
                return position;
            }
            set => TransformComponentInternalCall.SetPosition(Entity.ID, ref value);
        }

        public Vector3 Rotation
        {
            get
            {
                TransformComponentInternalCall.GetRotation(Entity.ID, out Vector3 rotation);
                return rotation;
            }
            set => TransformComponentInternalCall.SetRotation(Entity.ID, ref value);
        }

        public Vector3 Scale
        {
            get
            {
                TransformComponentInternalCall.GetScale(Entity.ID, out Vector3 scale);
                return scale;
            }
            set => TransformComponentInternalCall.SetScale(Entity.ID, ref value);
        }


        public TransformComponent()
        {
        }
    }
}