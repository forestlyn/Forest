namespace Engine
{
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

        public virtual void OnCreate()
        {
        }

        public virtual void OnUpdate(float deltaTime)
        {

        }

    }
}