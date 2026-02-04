namespace Engine
{
    public class Entity
    {
        public readonly ulong ID;

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
            ComponentInternalCall.HasComponent(ID, typeof(T));
            return true;
        }

        public void AddComponent<T>() where T : Component, new()
        {
            ComponentInternalCall.AddComponent(ID, typeof(T));
        }

        public void RemoveComponent<T>() where T : Component, new()
        {
            ComponentInternalCall.RemoveComponent(ID, typeof(T));
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



    }
}