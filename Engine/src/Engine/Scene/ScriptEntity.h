#pragma once
#include "Entity.h"
#include "Engine/Core/Timestep.h"
namespace Engine
{
    class ScriptEntity
    {
    private:
        Entity m_Entity;
        friend class Scene;

    public:
        ScriptEntity();
        virtual ~ScriptEntity() = default;

        template <typename T>
        T &GetComponent()
        {
            return m_Entity.GetComponent<T>();
        }

        template <typename T>
        bool HasComponent()
        {
            return m_Entity.HasComponent<T>();
        }
        template <typename T, typename... Args>
        T &AddComponent(Args &&...args)
        {
            return m_Entity.AddComponent<T>(std::forward<Args>(args)...);
        }
        template <typename T>
        void RemoveComponent()
        {
            m_Entity.RemoveComponent<T>();
        }

    protected:
        virtual void OnCreate() {}
        virtual void OnDestroy() {}
        virtual void OnUpdate(Core::Timestep ts) {}
    };
}