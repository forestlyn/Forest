#include "Engine/pcheader.h"
#include "Utils.h"
#include "Engine/Core/Log.h"
#include <fstream>
#include "ScriptEngine.h"
namespace Engine
{
    static std::unordered_map<std::string, ScriptFieldType> s_ScriptFieldTypeMap = {
        {"System.None", ScriptFieldType::None},
        {"System.Single", ScriptFieldType::Float},
        {"System.Double", ScriptFieldType::Double},
        {"System.Boolean", ScriptFieldType::Bool},
        {"System.Char", ScriptFieldType::Char},
        {"System.Byte", ScriptFieldType::Byte},
        {"System.Int16", ScriptFieldType::Short},
        {"System.UInt16", ScriptFieldType::UShort},
        {"System.Int32", ScriptFieldType::Int},
        {"System.UInt32", ScriptFieldType::UInt},
        {"System.Int64", ScriptFieldType::Long},
        {"System.UInt64", ScriptFieldType::ULong},
        {"System.String", ScriptFieldType::String},
        {"Engine.Vector2", ScriptFieldType::Vector2},
        {"Engine.Vector3", ScriptFieldType::Vector3},
        {"Engine.Vector4", ScriptFieldType::Vector4},
        {"Engine.Entity", ScriptFieldType::Entity}};

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

    ScriptFieldType MonoTypeToScriptFieldType(MonoType *monoType)
    {
        std::string typeName = mono_type_get_name(monoType);

        auto it = s_ScriptFieldTypeMap.find(typeName);
        if (it == s_ScriptFieldTypeMap.end())
        {
            ENGINE_ERROR("Unknown type: {}", typeName);
            return ScriptFieldType::None;
        }

        return it->second;
    }

    const char *ScriptFieldTypeToString(ScriptFieldType type)
    {
        switch (type)
        {
        case ScriptFieldType::Float:
            return "Float";
        case ScriptFieldType::Double:
            return "Double";
        case ScriptFieldType::Bool:
            return "Bool";
        case ScriptFieldType::Char:
            return "Char";
        case ScriptFieldType::Byte:
            return "Byte";
        case ScriptFieldType::Short:
            return "Short";
        case ScriptFieldType::Int:
            return "Int";
        case ScriptFieldType::Long:
            return "Long";
        case ScriptFieldType::UByte:
            return "UByte";
        case ScriptFieldType::UShort:
            return "UShort";
        case ScriptFieldType::UInt:
            return "UInt";
        case ScriptFieldType::ULong:
            return "ULong";
        case ScriptFieldType::Vector2:
            return "Vector2";
        case ScriptFieldType::Vector3:
            return "Vector3";
        case ScriptFieldType::Vector4:
            return "Vector4";
        case ScriptFieldType::Entity:
            return "Entity";
        }
        return "<Invalid>";
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

    MonoObject *InitializeClass(MonoDomain *domain, MonoClass *monoClass)
    {
        if (!monoClass)
            return nullptr;

        MonoObject *classInstance = mono_object_new(domain, monoClass);
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

    uint8_t GetPropertyAccessibility(MonoProperty *property)
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

}