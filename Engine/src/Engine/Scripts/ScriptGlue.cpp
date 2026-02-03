#include "ScriptGlue.h"
#include "Engine/Core/Log.h"
#include <iostream>
#include <glm/glm.hpp>
#include <mono/metadata/object.h>
namespace Engine
{
#define ADD_ENGINE_INTERNAL_CALL(name) mono_add_internal_call("Engine.InternalCalls::" #name, name)

    static void NativeLog(MonoString *string, int parameter)
    {
        char *cStr = mono_string_to_utf8(string);
        std::string str(cStr);
        mono_free(cStr);
        std::cout << str << ", " << parameter << std::endl;
    }

    static void NativeLog_Vector(glm::vec3 *parameter, glm::vec3 *outResult)
    {
        ENGINE_WARN("Value: {0} {1} {2}", parameter->x, parameter->y, parameter->z);
        *outResult = glm::normalize(*parameter);
    }

    static float NativeLog_VectorDot(glm::vec3 *parameter)
    {
        ENGINE_WARN("Value: {0} {1} {2}", parameter->x, parameter->y, parameter->z);
        return glm::dot(*parameter, *parameter);
    }

    void ScriptGlue::RegisterFuncs()
    {
        // Registration logic for all scriptable classes goes here.
        ADD_ENGINE_INTERNAL_CALL(NativeLog);
        ADD_ENGINE_INTERNAL_CALL(NativeLog_Vector);
        ADD_ENGINE_INTERNAL_CALL(NativeLog_VectorDot);
    }
}