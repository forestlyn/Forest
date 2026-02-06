#pragma once
#include <mono/metadata/object.h>
#include "Engine/Core/Log.h"
namespace Engine
{
    static void NativeLog(MonoString *string)
    {
        char *cStr = mono_string_to_utf8(string);
        std::string str(cStr);
        mono_free(cStr);
        LOG_INFO("{0}", str);
    }

    static void NativeLogWarn(MonoString *string)
    {
        char *cStr = mono_string_to_utf8(string);
        std::string str(cStr);
        mono_free(cStr);
        LOG_WARN("{0}", str);
    }

    static void NativeLogError(MonoString *string)
    {
        char *cStr = mono_string_to_utf8(string);
        std::string str(cStr);
        mono_free(cStr);
        LOG_ERROR("{0}", str);
    }
}