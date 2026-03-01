#pragma once
#include "Event.h"
namespace Engine::Event
{
    class WindowCloseEvent : public Event
    {
    public:
        WindowCloseEvent() {}

        EVENT_CLASS_TYPE(WindowClose)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)

    private:
    };

    class WindowResizeEvent : public Event
    {
    public:
        WindowResizeEvent(int width, int height)
            : m_Width(width), m_Height(height) {}

        inline int GetWidth() const { return m_Width; }
        inline int GetHeight() const { return m_Height; }

        std::string ToString() const override
        {
            return std::string(GetName()) + ": " + std::to_string(m_Width) + ", " + std::to_string(m_Height);
        }

        EVENT_CLASS_TYPE(WindowResize)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)

    private:
        int m_Width, m_Height;
    };

    class WindowFocusEvent : public Event
    {
    public:
        WindowFocusEvent() {}
        std::string ToString() const override
        {
            return std::string(GetName()) + ": Focused";
        }
        EVENT_CLASS_TYPE(WindowFocus)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };

    class WindowLostFocusEvent : public Event
    {
    public:
        WindowLostFocusEvent() {}
        std::string ToString() const override
        {
            return std::string(GetName()) + ": Lost Focus";
        }

        EVENT_CLASS_TYPE(WindowLostFocus)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };
}