#pragma once

namespace Engine
{
    struct BaseComponent
    {
    protected:
        bool m_Remove = false;
        bool m_Enabled = true;

    public:
        virtual ~BaseComponent() = default;
        void SetRemove(bool remove) { m_Remove = remove; }
        bool IsRemove() const { return m_Remove; }
        void SetEnabled(bool enabled) { m_Enabled = enabled; }
        bool IsEnabled() const { return m_Enabled; }
    };
} // namespace Engine::Scene