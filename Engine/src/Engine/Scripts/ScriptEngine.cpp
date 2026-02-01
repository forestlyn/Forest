#include "ScriptEngine.h"
#include "Engine/pcheader.h"
#include "Engine/Core/Core.h"
#include "Engine/Core/Application.h"
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/object.h>
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

    void
    ScriptEngine::Init()
    {
        m_ScriptEngineData = new ScriptEngineData();
        InitMono();
    }
    void ScriptEngine::Shutdown()
    {
        ShutdownMono();
        delete m_ScriptEngineData;
    }

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

        MonoImage *assemblyImage = mono_assembly_get_image(m_ScriptEngineData->CoreAssembly);
        MonoClass *monoClass = mono_class_from_name(assemblyImage, "Engine", "CSharpTesting");
        // 1. create an object (and call constructor)
        MonoObject *instance = mono_object_new(m_ScriptEngineData->AppDomain, monoClass);
        mono_runtime_object_init(instance);

        // 2. call function
        MonoMethod *printMessageFunc = mono_class_get_method_from_name(monoClass, "PrintFloatVar", 0);
        mono_runtime_invoke(printMessageFunc, instance, nullptr, nullptr);

        // 3. call function with param
        MonoMethod *incrementFunc = mono_class_get_method_from_name(monoClass, "IncrementFloatVar", 1);

        float value = 5;
        void *param = &value;

        mono_runtime_invoke(incrementFunc, instance, &param, nullptr);
        mono_runtime_invoke(printMessageFunc, instance, nullptr, nullptr);
    }

    void ScriptEngine::ShutdownMono()
    {
        mono_assemblies_cleanup();
        mono_domain_unload(m_ScriptEngineData->AppDomain);
        mono_domain_unload(m_ScriptEngineData->RootDomain);
    }
}
