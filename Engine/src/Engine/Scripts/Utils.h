#pragma once
#include <string>
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/object.h>
#include <mono/metadata/attrdefs.h>
#include <mono/metadata/metadata.h>
#include <filesystem>
namespace Engine
{
    enum class ScriptFieldType;
    enum class Accessibility : uint8_t
    {
        None = 0,
        Private = (1 << 0),
        Internal = (1 << 1),
        Protected = (1 << 2),
        Public = (1 << 3)
    };

    char *ReadBytes(const std::filesystem::path &filepath, uint32_t *outSize);
    MonoAssembly *LoadCSharpAssembly(const std::filesystem::path &assemblyPath);
    void PrintAssemblyTypes(MonoAssembly *assembly);

    MonoClass *GetClassFromAssembly(MonoAssembly *assembly, const std::string &namespaceName, const std::string &className);
    MonoObject *InitializeClass(MonoDomain *domain, MonoClass *monoClass);
    MonoMethod *GetMethodFromClass(MonoClass *monoClass, const std::string &methodName, int paramCount);
    MonoProperty *GetPropertyFromClass(MonoClass *monoClass, const std::string &propertyName);
    uint8_t GetPropertyAccessibility(MonoProperty *property);
    void CheckException(MonoObject *exception);

    uint64_t GetEntityIDFromEntityField(MonoObject *instance, MonoClassField *field);
    const char *ScriptFieldTypeToString(ScriptFieldType type);
    ScriptFieldType MonoTypeToScriptFieldType(MonoType *type, MonoClass *entityClass);
    void GetFieldDefaultValue(MonoObject *instance, MonoClassField *field, ScriptFieldType fieldType, uint8_t *outBuffer);
}