#include "ScriptEngine.h"
#include "Engine/pcheader.h"
#include "Engine/Core/Core.h"
#include "Engine/Core/Application.h"
#include "ScriptGlue.h"
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/object.h>
#include <mono/metadata/attrdefs.h>
#include <fstream>
namespace Engine
{
    struct ScriptEngineData
    {
        MonoDomain *RootDomain = nullptr;
        MonoDomain *AppDomain = nullptr;

        MonoAssembly *CoreAssembly = nullptr;
    };
    static ScriptEngineData *m_ScriptEngineData = nullptr;

#pragma region Utils
    char *ReadBytes(const std::string &filepath, uint32_t *outSize)
    {
        std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

        if (!stream)
        {
            // Failed to open the file
            return nullptr;
        }

        std::streampos end = stream.tellg();
        stream.seekg(0, std::ios::beg);
        uint32_t size = end - stream.tellg();

        if (size == 0)
        {
            // File is empty
            return nullptr;
        }

        char *buffer = new char[size];
        stream.read((char *)buffer, size);
        stream.close();

        *outSize = size;
        return buffer;
    }
    MonoAssembly *LoadCSharpAssembly(const std::string &assemblyPath)
    {
        uint32_t fileSize = 0;
        char *fileData = ReadBytes(assemblyPath, &fileSize);

        // NOTE: We can't use this image for anything other than loading the assembly
        // because this image doesn't have a reference to the assembly
        MonoImageOpenStatus status;
        MonoImage *image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

        if (status != MONO_IMAGE_OK)
        {
            const char *errorMessage = mono_image_strerror(status);
            ENGINE_ERROR("Failed: {}", errorMessage);
            return nullptr;
        }

        MonoAssembly *assembly = mono_assembly_load_from_full(image, assemblyPath.c_str(), &status, 0);
        mono_image_close(image);

        // Don't forget to free the file data
        delete[] fileData;

        return assembly;
    }

    void PrintAssemblyTypes(MonoAssembly *assembly)
    {
        MonoImage *image = mono_assembly_get_image(assembly);
        const MonoTableInfo *typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
        int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

        for (int32_t i = 0; i < numTypes; i++)
        {
            uint32_t cols[MONO_TYPEDEF_SIZE];
            mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

            const char *nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
            const char *name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

            printf("%s.%s\n", nameSpace, name);
        }
    }

    MonoClass *GetClassFromAssembly(MonoAssembly *assembly, const std::string &namespaceName, const std::string &className)
    {
        MonoImage *image = mono_assembly_get_image(assembly);
        MonoClass *monoClass = mono_class_from_name(image, namespaceName.c_str(), className.c_str());
        if (!monoClass)
        {
            ENGINE_ERROR("Failed to find class {} in namespace {}", className, namespaceName);
            return nullptr;
        }
        return monoClass;
    }

    MonoObject *InitializeClass(MonoClass *monoClass)
    {
        if (!monoClass)
            return nullptr;

        MonoObject *classInstance = mono_object_new(m_ScriptEngineData->AppDomain, monoClass);
        mono_runtime_object_init(classInstance);
        if (!classInstance)
        {
            ENGINE_ERROR("Failed to create instance of class");
            return nullptr;
        }
        return classInstance;
    }

    MonoMethod *GetMethodFromClass(MonoClass *monoClass, const std::string &methodName, int paramCount)
    {
        MonoMethod *method = mono_class_get_method_from_name(monoClass, methodName.c_str(), paramCount);
        if (!method)
        {
            ENGINE_ERROR("Failed to find method {} with {} parameters", methodName, paramCount);
            return nullptr;
        }
        return method;
    }

    MonoProperty *GetPropertyFromClass(MonoClass *monoClass, const std::string &propertyName)
    {
        MonoProperty *property = mono_class_get_property_from_name(monoClass, propertyName.c_str());
        if (!property)
        {
            ENGINE_ERROR("Failed to find property {}", propertyName);
            return nullptr;
        }
        return property;
    }

    uint8_t GetFieldAccessibility(MonoClassField *field)
    {
        uint8_t accessibility = (uint8_t)Accessibility::None;
        uint32_t accessFlag = mono_field_get_flags(field) & MONO_FIELD_ATTR_FIELD_ACCESS_MASK;

        switch (accessFlag)
        {
        case MONO_FIELD_ATTR_PRIVATE:
        {
            accessibility = (uint8_t)Accessibility::Private;
            break;
        }
        case MONO_FIELD_ATTR_FAM_AND_ASSEM:
        {
            accessibility |= (uint8_t)Accessibility::Protected;
            accessibility |= (uint8_t)Accessibility::Internal;
            break;
        }
        case MONO_FIELD_ATTR_ASSEMBLY:
        {
            accessibility = (uint8_t)Accessibility::Internal;
            break;
        }
        case MONO_FIELD_ATTR_FAMILY:
        {
            accessibility = (uint8_t)Accessibility::Protected;
            break;
        }
        case MONO_FIELD_ATTR_FAM_OR_ASSEM:
        {
            accessibility |= (uint8_t)Accessibility::Private;
            accessibility |= (uint8_t)Accessibility::Protected;
            break;
        }
        case MONO_FIELD_ATTR_PUBLIC:
        {
            accessibility = (uint8_t)Accessibility::Public;
            break;
        }
        }
        return accessibility;
    }

    uint8_t GetPropertyAccessbility(MonoProperty *property)
    {
        uint8_t accessibility = (uint8_t)Accessibility::None;

        // Get a reference to the property's getter method
        MonoMethod *propertyGetter = mono_property_get_get_method(property);
        if (propertyGetter != nullptr)
        {
            // Extract the access flags from the getters flags
            uint32_t accessFlag = mono_method_get_flags(propertyGetter, nullptr) & MONO_METHOD_ATTR_ACCESS_MASK;

            switch (accessFlag)
            {
            case MONO_FIELD_ATTR_PRIVATE:
            {
                accessibility = (uint8_t)Accessibility::Private;
                break;
            }
            case MONO_FIELD_ATTR_FAM_AND_ASSEM:
            {
                accessibility |= (uint8_t)Accessibility::Protected;
                accessibility |= (uint8_t)Accessibility::Internal;
                break;
            }
            case MONO_FIELD_ATTR_ASSEMBLY:
            {
                accessibility = (uint8_t)Accessibility::Internal;
                break;
            }
            case MONO_FIELD_ATTR_FAMILY:
            {
                accessibility = (uint8_t)Accessibility::Protected;
                break;
            }
            case MONO_FIELD_ATTR_FAM_OR_ASSEM:
            {
                accessibility |= (uint8_t)Accessibility::Private;
                accessibility |= (uint8_t)Accessibility::Protected;
                break;
            }
            case MONO_FIELD_ATTR_PUBLIC:
            {
                accessibility = (uint8_t)Accessibility::Public;
                break;
            }
            }
        }
        // Get a reference to the property's setter method
        MonoMethod *propertySetter = mono_property_get_set_method(property);
        if (propertySetter != nullptr)
        {
            // Extract the access flags from the setters flags
            uint32_t accessFlag = mono_method_get_flags(propertySetter, nullptr) & MONO_METHOD_ATTR_ACCESS_MASK;
            if (accessFlag != MONO_FIELD_ATTR_PUBLIC)
                accessibility = (uint8_t)Accessibility::Private;
        }
        else
        {
            accessibility = (uint8_t)Accessibility::Private;
        }

        return accessibility;
    }

    void CheckException(MonoObject *exception)
    {
        if (exception)
        {
            MonoString *exceptionMessage = mono_object_to_string(exception, nullptr);
            char *messageCStr = mono_string_to_utf8(exceptionMessage);
            ENGINE_ERROR("Mono Exception: {}", messageCStr);
            mono_free(messageCStr);
        }
    }
    bool CheckMonoError(MonoError &error)
    {
        bool hasError = !mono_error_ok(&error);
        if (hasError)
        {
            unsigned short errorCode = mono_error_get_error_code(&error);
            const char *errorMessage = mono_error_get_message(&error);
            printf("Mono Error!\n");
            printf("\tError Code: %hu\n", errorCode);
            printf("\tError Message: %s\n", errorMessage);
            mono_error_cleanup(&error);
        }
        return hasError;
    }
    std::string MonoStringToUTF8(MonoString *monoString)
    {
        if (monoString == nullptr || mono_string_length(monoString) == 0)
            return "";

        MonoError error;
        char *utf8 = mono_string_to_utf8_checked(monoString, &error);
        if (CheckMonoError(error))
            return "";
        std::string result(utf8);
        mono_free(utf8);
        return result;
    }
#pragma endregion

    void ScriptEngine::Init()
    {
        m_ScriptEngineData = new ScriptEngineData();

        InitMono();

        ScriptGlue::RegisterFuncs();

        // // Example usage
        // // 1. get class
        // MonoClass *monoClass = GetClassFromAssembly(m_ScriptEngineData->CoreAssembly, "Engine", "CSharpTesting");
        // ENGINE_ASSERT(monoClass);
        // // 2. create instance
        // MonoObject *instance = InitializeClass(monoClass);
        // ENGINE_ASSERT(instance);

        // // 3. call function
        // MonoObject *exception = nullptr;
        // MonoMethod *printMessageFunc = GetMethodFromClass(monoClass, "PrintFloatVar", 0);
        // mono_runtime_invoke(printMessageFunc, instance, nullptr, &exception);
        // CheckException(exception);

        // // 4. call function with param
        // MonoMethod *incrementFunc = GetMethodFromClass(monoClass, "IncrementFloatVar", 1);
        // float value = 5;
        // void *param = &value;

        // mono_runtime_invoke(incrementFunc, instance, &param, &exception);
        // CheckException(exception);

        // mono_runtime_invoke(printMessageFunc, instance, nullptr, &exception);
        // CheckException(exception);

        // // 5. get property
        // MonoProperty *floatVarProperty = GetPropertyFromClass(monoClass, "MyPublicFloatVar");
        // ENGINE_ASSERT(floatVarProperty);

        // uint32_t flags = mono_property_get_flags(floatVarProperty);
        // ENGINE_INFO("Property flags: {}", flags);

        // MonoMethod *getter = mono_property_get_get_method(floatVarProperty);
        // MonoObject *result = mono_runtime_invoke(getter, instance, nullptr, &exception);
        // CheckException(exception);

        // float *floatVarValue = (float *)mono_object_unbox(result);
        // ENGINE_INFO("FloatVar property value: {}", *floatVarValue);
        // MonoMethod *setter = mono_property_get_set_method(floatVarProperty);
        // float newValue = 42.0f;
        // void *setterParam = &newValue;
        // // mono_runtime_invoke(setter, instance, &setterParam, &exception);
        // mono_property_set_value(floatVarProperty, instance, &setterParam, &exception);
        // CheckException(exception);

        // result = mono_runtime_invoke(getter, instance, nullptr, &exception);
        // floatVarValue = (float *)mono_object_unbox(result);
        // ENGINE_INFO("FloatVar property new value: {}", *floatVarValue);
        // CheckException(exception);

        // // 6. get field
        // MonoClassField *field = mono_class_get_field_from_name(monoClass, "MyPublicIntVar");
        // ENGINE_ASSERT(field);
        // uint8_t accessibility = GetFieldAccessibility(field);
        // ENGINE_INFO("Field accessibility: {}", accessibility);

        // MonoObject *fieldValueObject = mono_field_get_value_object(m_ScriptEngineData->AppDomain, field, instance);
        // int *fieldValue = (int *)mono_object_unbox(fieldValueObject);
        // ENGINE_INFO("Field value: {}", *fieldValue);

        // MonoClassField *stringField = mono_class_get_field_from_name(monoClass, "MyPublicStringVar");
        // ENGINE_ASSERT(stringField);
        // MonoObject *stringFieldValueObject = mono_field_get_value_object(m_ScriptEngineData->AppDomain, stringField, instance);
        // MonoString *stringFieldValue = (MonoString *)stringFieldValueObject;
        // std::string stringValue = MonoStringToUTF8(stringFieldValue);
        // ENGINE_INFO("String Field value: {}", stringValue);

        MonoClass *monoClass = GetClassFromAssembly(m_ScriptEngineData->CoreAssembly, "Engine", "Entity");
        ENGINE_ASSERT(monoClass);
        MonoObject *entityInstance = InitializeClass(monoClass);
        ENGINE_ASSERT(entityInstance);

        MonoMethod *printMessageMethod = GetMethodFromClass(monoClass, "PrintMessage", 1);
        MonoObject *exception = nullptr;
        const char *message = "Hello from C++!";
        MonoString *monoMessage = mono_string_new(m_ScriptEngineData->AppDomain, message);
        void *param = monoMessage;
        mono_runtime_invoke(printMessageMethod, entityInstance, &param, &exception);
        CheckException(exception);
    }

    void ScriptEngine::Shutdown()
    {
        ShutdownMono();
        delete m_ScriptEngineData;
    }

    void ScriptEngine::InitMono()
    {
        Core::Application &app = Core::Application::Get();
        mono_set_assemblies_path(app.GetSpecification().MonoAssemblyPath.c_str());

        MonoDomain *rootDomain = mono_jit_init("EngineJITRuntime");
        ENGINE_ASSERT(rootDomain);
        m_ScriptEngineData->RootDomain = rootDomain;

        char domainName[] = "EngineScriptRuntime";
        MonoDomain *appDomain = mono_domain_create_appdomain(domainName, nullptr);
        ENGINE_ASSERT(appDomain);
        mono_domain_set(appDomain, true);
        m_ScriptEngineData->AppDomain = appDomain;

        m_ScriptEngineData->CoreAssembly = LoadCSharpAssembly("scripts/bin/Engine-ScriptCore.dll");
        PrintAssemblyTypes(m_ScriptEngineData->CoreAssembly);
    }

    void ScriptEngine::ShutdownMono()
    {
        mono_assemblies_cleanup();
        mono_domain_unload(m_ScriptEngineData->AppDomain);
        mono_domain_unload(m_ScriptEngineData->RootDomain);
    }
}
