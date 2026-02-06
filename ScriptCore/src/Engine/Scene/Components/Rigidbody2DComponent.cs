
namespace Engine
{
    public static class Rigidbody2DComponentInternalCalls
    {
        [System.Runtime.CompilerServices.MethodImplAttribute(System.Runtime.CompilerServices.MethodImplOptions.InternalCall)]
        internal extern static void ApplyLinearImpulse(ulong entityID, ref Vector2 impulse, ref Vector2 worldPosition, bool wake);

        [System.Runtime.CompilerServices.MethodImplAttribute(System.Runtime.CompilerServices.MethodImplOptions.InternalCall)]
        internal extern static void ApplyLinearImpulseToCenter(ulong entityID, ref Vector2 impulse, bool wake);
    }
    public class Rigidbody2DComponent : Component
    {
        public void ApplyLinearImpulse(Vector2 impulse, Vector2 worldPosition, bool wake)
        {
            Rigidbody2DComponentInternalCalls.ApplyLinearImpulse(Entity.ID, ref impulse, ref worldPosition, wake);
        }

        public void ApplyLinearImpulse(Vector2 impulse, bool wake)
        {
            Rigidbody2DComponentInternalCalls.ApplyLinearImpulseToCenter(Entity.ID, ref impulse, wake);
        }
    }
}