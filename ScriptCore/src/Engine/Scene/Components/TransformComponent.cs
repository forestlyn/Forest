using System.Runtime.CompilerServices;

namespace Engine
{
    public class TransformComponentInternalCalls
    {
        [MethodImplAttribute(System.Runtime.CompilerServices.MethodImplOptions.InternalCall)]
        internal extern static bool SetPosition(ulong entityID, ref Vector3 position);
        [MethodImplAttribute(System.Runtime.CompilerServices.MethodImplOptions.InternalCall)]
        internal extern static bool GetPosition(ulong entityID, out Vector3 position);
        [MethodImplAttribute(System.Runtime.CompilerServices.MethodImplOptions.InternalCall)]
        internal extern static bool SetRotation(ulong entityID, ref Vector3 rotation);
        [MethodImplAttribute(System.Runtime.CompilerServices.MethodImplOptions.InternalCall)]
        internal extern static bool GetRotation(ulong entityID, out Vector3 rotation);
        [MethodImplAttribute(System.Runtime.CompilerServices.MethodImplOptions.InternalCall)]
        internal extern static bool SetScale(ulong entityID, ref Vector3 scale);
        [MethodImplAttribute(System.Runtime.CompilerServices.MethodImplOptions.InternalCall)]
        internal extern static bool GetScale(ulong entityID, out Vector3 scale);
    }


    public class TransformComponent : Component
    {
        public Vector3 Position
        {
            get
            {
                EnsureAvailable(TransformComponentInternalCalls.GetPosition(Entity.ID, out Vector3 position));
                return position;
            }
            set => EnsureAvailable(TransformComponentInternalCalls.SetPosition(Entity.ID, ref value));
        }

        public Vector3 Rotation
        {
            get
            {
                EnsureAvailable(TransformComponentInternalCalls.GetRotation(Entity.ID, out Vector3 rotation));
                return rotation;
            }
            set => EnsureAvailable(TransformComponentInternalCalls.SetRotation(Entity.ID, ref value));
        }

        public Vector3 Scale
        {
            get
            {
                EnsureAvailable(TransformComponentInternalCalls.GetScale(Entity.ID, out Vector3 scale));
                return scale;
            }
            set => EnsureAvailable(TransformComponentInternalCalls.SetScale(Entity.ID, ref value));
        }


        private static void EnsureAvailable(bool available)
        {
            if (!available)
                throw new System.InvalidOperationException("Entity no longer has a TransformComponent. UI entities use RectTransform.");
        }

        public TransformComponent()
        {
        }
    }
}