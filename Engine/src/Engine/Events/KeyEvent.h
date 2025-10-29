#pragma once
#include "Event.h"
namespace Engine::Event
{
    class KeyEvent : public Event
    {
    public:
        inline int GetKeyCode() const { return m_KeyCode; }

        EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
    protected:
        KeyEvent(int keycode)
            : m_KeyCode(keycode) {}

        int m_KeyCode;
    };

    class KeyPressedEvent : public KeyEvent
    {
    public:
        KeyPressedEvent(int keycode, bool is_repeat)
            : KeyEvent(keycode), m_IsRepeat(is_repeat) {}

        inline bool IsRepeat() const { return m_IsRepeat; }

        std::string ToString() const override
        {
            return std::string(GetName()) + ": " + std::to_string(m_KeyCode) + " (" + (m_IsRepeat ? "repeats" : "does not repeat") + ")";
        }

        EVENT_CLASS_TYPE(KeyPressed)
    private:
        bool m_IsRepeat;
    };

    class KeyReleasedEvent : public KeyEvent
    {
    public:
        KeyReleasedEvent(int keycode)
            : KeyEvent(keycode) {}

        std::string ToString() const override
        {
            return std::string(GetName()) + ": " + std::to_string(m_KeyCode);
        }

        EVENT_CLASS_TYPE(KeyReleased)
    };

    class KeyTypedEvent : public KeyEvent
    {
    public:
        KeyTypedEvent(int keycode)
            : KeyEvent(keycode) {}

        std::string ToString() const override
        {
            return std::string(GetName()) + ": " + std::to_string(m_KeyCode);
        }

        EVENT_CLASS_TYPE(KeyTyped)
    };
}
