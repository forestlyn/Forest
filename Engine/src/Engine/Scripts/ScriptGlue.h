#pragma once
struct _MonoType;
typedef struct _MonoType MonoType;
namespace Engine
{
    class UUID;

    class ScriptGlue
    {
    public:
        static void RegisterFuncs();
        static void RegisterComponents();

        static bool EntityHasComponent(UUID entityID, MonoType *componentType);
    };
}