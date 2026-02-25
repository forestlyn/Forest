namespace Engine
{
    public static class EntityInternalCalls
    {
        [System.Runtime.CompilerServices.MethodImplAttribute(System.Runtime.CompilerServices.MethodImplOptions.InternalCall)]
        internal extern static ulong FindEntityByName(string name);
        [System.Runtime.CompilerServices.MethodImplAttribute(System.Runtime.CompilerServices.MethodImplOptions.InternalCall)]
        internal extern static object GetScriptInstance(ulong entityID);
    }
    public class Entity
    {
        public readonly ulong ID;

        public Entity()
        {
            ID = 0;
        }
        public Entity(ulong id)
        {
            ID = id;
        }

        public override string ToString()
        {
            return $"Entity({ID})";
        }

        public bool HasComponent<T>() where T : Component, new()
        {
            return ComponentInternalCalls.HasComponent(ID, typeof(T));
        }

        public T AddComponent<T>() where T : Component, new()
        {
            ComponentInternalCalls.AddComponent(ID, typeof(T));
            T component = new T();
            component.Entity = this;
            return component;
        }

        public void RemoveComponent<T>() where T : Component, new()
        {
            ComponentInternalCalls.RemoveComponent(ID, typeof(T));
        }

        public T GetComponent<T>() where T : Component, new()
        {
            if (HasComponent<T>())
            {
                T component = new T();
                component.Entity = this;
                return component;
            }
            return null;
        }

        public static Entity Find(string name)
        {
            ulong id = EntityInternalCalls.FindEntityByName(name);
            if (id != 0)
                return new Entity(id);
            return null;
        }

        public T As<T>() where T : Entity, new()
        {
            if (ID == 0)
                return null;
            Log.NativeLog($"Attempting to cast Entity({ID}) to {typeof(T).Name}");
            object scriptInstance = EntityInternalCalls.GetScriptInstance(ID);
            if (scriptInstance != null && scriptInstance is T)
                return (T)scriptInstance;
            return null;
        }

        public virtual void OnCreate()
        {
        }

        public virtual void OnUpdate(float deltaTime)
        {

        }

    }
}