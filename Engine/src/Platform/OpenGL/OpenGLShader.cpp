#include "OpenGLShader.h"
#include "glad/glad.h"
#include "Engine/pcheader.h"
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <vector>
#include "Engine/Profile/Instrumentor.h"
#include "Utils.h"
#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>
#include <filesystem>
#include <yaml-cpp/yaml.h>
namespace Platform::OpenGL
{
    OpenGLShader::OpenGLShader(const std::string &name, const std::string &vertexSrc, const std::string &fragmentSrc)
        : m_Name(name)
    {
        ENGINE_PROFILING_FUNC();

        std::unordered_map<GLenum, std::string> shaderSources;
        shaderSources[GL_VERTEX_SHADER] = vertexSrc;
        shaderSources[GL_FRAGMENT_SHADER] = fragmentSrc;
        m_FilePath = "Unnamed.shader";
        {
            CompileOrGetVulkanSPIRV(shaderSources, true);
            CompileOrGetOpenGLBinaries(true);
            CreateProgram();
        }
    }

    OpenGLShader::OpenGLShader(const std::string &filepath) : m_FilePath(filepath)
    {
        ENGINE_PROFILING_FUNC();

        std::string cacheFilepath = Utils::GetCacheDirectory();
        Utils::CreateCacheDirIfNotExists(cacheFilepath);

        std::string source = "";
        {
            std::ifstream in(filepath, std::ios::in | std::ios::binary);
            if (in)
            {
                in.seekg(0, std::ios::end);
                size_t size = in.tellg();
                if (size != -1)
                {
                    source.resize(size);
                    in.seekg(0, std::ios::beg);
                    in.read(&source[0], size);
                }
                else
                {
                    ENGINE_ERROR("Could not read from file '{0}'", filepath);
                }
                in.close();
            }
            else
            {
                ENGINE_ERROR("Could not open file '{0}'", filepath);
            }
        }

        auto shaderSources = PreProcess(source);
        std::string hashValue = CalculateHash(shaderSources);
        std::string oldHashValue = LoadHashValue(filepath);
        bool needsRecompile = hashValue != oldHashValue;
        if (needsRecompile)
        {
            SaveHashValue(filepath, hashValue);
        }
        CompileOrGetVulkanSPIRV(shaderSources, needsRecompile);
        CompileOrGetOpenGLBinaries(needsRecompile);
        CreateProgram();
        // CompileShader(shaderSources);

        ExtractNameFromFilepath(filepath);
    }

    OpenGLShader::~OpenGLShader()
    {
        ENGINE_PROFILING_FUNC();

        glDeleteProgram(m_RendererID);
    }

    void OpenGLShader::Bind() const
    {
        ENGINE_PROFILING_FUNC();

        glUseProgram(m_RendererID);
    }

    void OpenGLShader::Unbind() const
    {
        ENGINE_PROFILING_FUNC();

        glUseProgram(0);
    }

#pragma region Upload Uniforms
    void OpenGLShader::SetInt(const std::string &name, int value)
    {
        ENGINE_PROFILING_FUNC();

        UploadUniformInt(name, value);
    }

    void OpenGLShader::SetIntArray(const std::string &name, int *values, uint32_t count)
    {
        ENGINE_PROFILING_FUNC();

        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform1iv(location, count, values);
    }

    void OpenGLShader::SetFloat(const std::string &name, float value)
    {
        ENGINE_PROFILING_FUNC();

        UploadUniformFloat(name, value);
    }

    void OpenGLShader::SetFloat3(const std::string &name, const glm::vec3 &value)
    {
        ENGINE_PROFILING_FUNC();

        UploadUniformFloat3(name, value);
    }

    void OpenGLShader::SetFloat4(const std::string &name, const glm::vec4 &value)
    {
        ENGINE_PROFILING_FUNC();

        UploadUniformFloat4(name, value);
    }

    void OpenGLShader::SetMat4(const std::string &name, const glm::mat4 &matrix)
    {
        ENGINE_PROFILING_FUNC();

        UploadUniformMat4(name, matrix);
    }

    void OpenGLShader::UploadUniformInt(const std::string &name, int value)
    {
        ENGINE_PROFILING_FUNC();

        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform1i(location, value);
    }

    void OpenGLShader::UploadUniformFloat(const std::string &name, float value)
    {
        ENGINE_PROFILING_FUNC();

        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform1f(location, value);
    }

    void OpenGLShader::UploadUniformFloat2(const std::string &name, const glm::vec2 &value)
    {
        ENGINE_PROFILING_FUNC();

        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform2f(location, value.x, value.y);
    }

    void OpenGLShader::UploadUniformFloat3(const std::string &name, const glm::vec3 &value)
    {
        ENGINE_PROFILING_FUNC();

        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform3f(location, value.x, value.y, value.z);
    }

    void OpenGLShader::UploadUniformFloat4(const std::string &name, const glm::vec4 &value)
    {
        ENGINE_PROFILING_FUNC();

        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform4f(location, value.x, value.y, value.z, value.w);
    }

    void OpenGLShader::UploadUniformMat3(const std::string &name, const glm::mat3 &matrix)
    {
        ENGINE_PROFILING_FUNC();

        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    }

    void OpenGLShader::UploadUniformMat4(const std::string &name, const glm::mat4 &matrix)
    {
        ENGINE_PROFILING_FUNC();

        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    }

#pragma endregion

#pragma region Compile Shader
    std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string &source)
    {
        ENGINE_PROFILING_FUNC();

        auto shaderSources = std::unordered_map<GLenum, std::string>();

        const char *typeToken = "#type";

        size_t pos = source.find(typeToken, 0);
        while (pos != std::string::npos)
        {
            size_t eol = source.find_first_of("\r\n", pos);
            ENGINE_ASSERT(eol != std::string::npos, "Syntax error");

            size_t begin = pos + strlen(typeToken) + 1;
            std::string type = source.substr(begin, eol - begin);
            ENGINE_ASSERT(Utils::ConvertShaderTypeFromString(type), "Invalid shader type specified");

            size_t nextLinePos = source.find_first_not_of("\r\n", eol);
            pos = source.find(typeToken, nextLinePos);
            shaderSources[Utils::ConvertShaderTypeFromString(type)] =
                (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
        }

        return shaderSources;
    }
    void OpenGLShader::CompileShader(std::unordered_map<GLenum, std::string> &shaderSources)
    {
        ENGINE_PROFILING_FUNC();

        ENGINE_ASSERT(shaderSources.size() <= 2, "We only support 2 shaders for now (vertex and fragment)");
        std::array<GLuint, 2> shaderIDs;
        int shaderIndex = 0;

        for (const auto &kv : shaderSources)
        {
            GLenum type = kv.first;
            const std::string &sourceStr = kv.second;

            GLuint shader = glCreateShader(type);

            // Send the vertex shader source code to GL
            // Note that std::string's .c_str is NULL character terminated.
            const GLchar *sourceCStr = (const GLchar *)sourceStr.c_str();
            glShaderSource(shader, 1, &sourceCStr, 0);

            // Compile the vertex shader
            glCompileShader(shader);

            GLint isCompiled = 0;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);

            if (isCompiled == GL_FALSE)
            {
                GLint maxLength = 0;
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

                // The maxLength includes the NULL character
                std::vector<GLchar> infoLog(maxLength);
                glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);
                // We don't need the shader anymore.
                glDeleteShader(shader);

                // Use the infoLog as you see fit.
                ENGINE_ERROR("Vertex shader compilation failed:\n{0}", infoLog.data());
                // In this simple program, we'll just leave
                return;
            }

            shaderIDs[shaderIndex++] = shader;
        }

        // Vertex and fragment shaders are successfully compiled.
        // Now time to link them together into a program.
        // Get a program object.
        m_RendererID = glCreateProgram();
        GLuint program = m_RendererID;

        // Attach our shaders to our program
        for (GLuint shader : shaderIDs)
        {
            glAttachShader(program, shader);
        }
        // Link our program
        glLinkProgram(program);

        // Note the different functions here: glGetProgram* instead of glGetShader*.
        GLint isLinked = 0;
        glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
        if (isLinked == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::vector<GLchar> infoLog(maxLength);
            glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

            // We don't need the program anymore.
            glDeleteProgram(program);
            // Don't leak shaders either.
            for (GLuint shader : shaderIDs)
            {
                glDeleteShader(shader);
            }

            // Use the infoLog as you see fit.
            ENGINE_ERROR("Shader linking failed:\n{0}", infoLog.data());
            // In this simple program, we'll just leave
            return;
        }

        // Always detach shaders after a successful link.
        for (GLuint shader : shaderIDs)
        {
            glDetachShader(program, shader);
        }
    }

    void OpenGLShader::ExtractNameFromFilepath(const std::string &filepath)
    {
        size_t lastSlash = filepath.find_last_of("/\\");
        lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
        size_t lastDot = filepath.rfind('.');
        lastDot = lastDot == std::string::npos ? filepath.size() : lastDot;
        m_Name = filepath.substr(lastSlash, lastDot - lastSlash);
    }

    void OpenGLShader::CompileOrGetVulkanSPIRV(const std::unordered_map<GLenum, std::string> &shaderSources, bool forceRecompile)
    {
        ENGINE_PROFILING_FUNC();

        std::string hashValue = CalculateHash(shaderSources);

        GLuint program = glCreateProgram();
        shaderc::Compiler compiler;
        shaderc::CompileOptions options;
        options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
        const bool optimize = true;
        if (optimize)
            options.SetOptimizationLevel(shaderc_optimization_level_performance);

        std::filesystem::path cacheDir = Utils::GetCacheDirectory();

        auto &shaderData = m_VulkanSPIRV;
        shaderData.clear();

        for (const auto &[type, sourceStr] : shaderSources)
        {
            std::filesystem::path shaderFilePath = m_FilePath;
            std::filesystem::path cachedPath = cacheDir / (shaderFilePath.filename().string() + Utils::GLShaderStageCachedVulkanFileExtension(type));

            std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
            if (in.is_open() && !forceRecompile)
            {
                in.seekg(0, std::ios::end);
                auto size = in.tellg();
                in.seekg(0, std::ios::beg);

                auto &data = shaderData[type];
                data.resize(size / sizeof(uint32_t));
                in.read((char *)data.data(), size);
            }
            else
            {
                shaderc_shader_kind kind = Utils::GLShaderTypeToShaderC(type);

                shaderc::SpvCompilationResult module =
                    compiler.CompileGlslToSpv(sourceStr, kind, m_FilePath.c_str(), "main", options);

                if (module.GetCompilationStatus() != shaderc_compilation_status_success)
                {
                    ENGINE_ERROR("Shader compilation error: {0}", module.GetErrorMessage());
                    continue;
                }

                m_VulkanSPIRV[type] = std::vector<uint32_t>(module.cbegin(), module.cend());

                Utils::WriteSPIRVBinaryToCache(cachedPath.string(), m_VulkanSPIRV[type]);
            }
        }

        for (auto &&[stage, data] : shaderData)
            Reflect(stage, data);
    }

    void OpenGLShader::CompileOrGetOpenGLBinaries(bool forceRecompile)
    {
        ENGINE_PROFILING_FUNC();

        shaderc::Compiler compiler;
        shaderc::CompileOptions options;
        options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
        const bool optimize = false;
        if (optimize)
            options.SetOptimizationLevel(shaderc_optimization_level_performance);

        auto &shaderData = m_OpenGLSPIRV;
        shaderData.clear();
        std::filesystem::path cacheDir = Utils::GetCacheDirectory();
        m_OpenGLSourceCode.clear();
        for (auto &&[type, spirv] : m_VulkanSPIRV)
        {
            std::filesystem::path shaderFilePath = m_FilePath;
            std::filesystem::path cachedPath = cacheDir / (shaderFilePath.filename().string() + Utils::GLShaderStageCachedOpenGLFileExtension(type));

            std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
            if (in.is_open() && !forceRecompile)
            {
                in.seekg(0, std::ios::end);
                auto size = in.tellg();
                in.seekg(0, std::ios::beg);

                auto &data = shaderData[type];
                data.resize(size / sizeof(uint32_t));
                in.read((char *)data.data(), size);
            }
            else
            {
                spirv_cross::CompilerGLSL glslCompiler(spirv);
                m_OpenGLSourceCode[type] = glslCompiler.compile();
                auto &source = m_OpenGLSourceCode[type];
                shaderc_shader_kind kind = Utils::GLShaderTypeToShaderC(type);

                shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, kind, m_FilePath.c_str());
                if (module.GetCompilationStatus() != shaderc_compilation_status_success)
                {
                    ENGINE_ERROR(module.GetErrorMessage());
                    ENGINE_ASSERT(false);
                }

                shaderData[type] = std::vector<uint32_t>(module.cbegin(), module.cend());

                Utils::WriteSPIRVBinaryToCache(cachedPath.string(), shaderData[type]);
            }
        }
    }

    void OpenGLShader::Reflect(GLenum stage, const std::vector<uint32_t> &spirv)
    {
        ENGINE_PROFILING_FUNC();
        spirv_cross::Compiler compiler(spirv);
        spirv_cross::ShaderResources resources = compiler.get_shader_resources();
        ENGINE_TRACE("OpenGLShader::Reflect - {0} {1}", Utils::GLShaderStageToString(stage), m_FilePath);
        ENGINE_TRACE("    {0} uniform buffers", resources.uniform_buffers.size());
        ENGINE_TRACE("    {0} resources", resources.sampled_images.size());
        for (const auto &resource : resources.uniform_buffers)
        {
            const auto &type = compiler.get_type(resource.type_id);
            size_t size = compiler.get_declared_struct_size(type);
            uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
            int memberCount = type.member_types.size();
            ENGINE_TRACE("  {0}", resource.name);
            ENGINE_TRACE("    Size = {0}", size);
            ENGINE_TRACE("    Binding = {0}", binding);
            ENGINE_TRACE("    Members = {0}", memberCount);
        }
    }

    void OpenGLShader::CreateProgram()
    {
        ENGINE_PROFILING_FUNC();

        std::array<GLuint, 2> shaderIDs;
        int shaderIndex = 0;

        for (const auto &[type, spirv] : m_OpenGLSPIRV)
        {
            GLuint shader = glCreateShader(type);

            glShaderBinary(1, &shader, GL_SHADER_BINARY_FORMAT_SPIR_V, spirv.data(), spirv.size() * sizeof(uint32_t));
            glSpecializeShader(shader, "main", 0, nullptr, nullptr);

            shaderIDs[shaderIndex++] = shader;
        }

        // Vertex and fragment shaders are successfully compiled.
        // Now time to link them together into a program.
        // Get a program object.
        GLuint program = glCreateProgram();
        m_RendererID = program;
        // Attach our shaders to our program
        for (GLuint shader : shaderIDs)
        {
            glAttachShader(program, shader);
        }
        // Link our program
        glLinkProgram(program);

        // Note the different functions here: glGetProgram* instead of glGetShader*.
        GLint isLinked = 0;
        glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
        if (isLinked == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::vector<GLchar> infoLog(maxLength);
            glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

            // We don't need the program anymore.
            glDeleteProgram(program);
            // Don't leak shaders either.
            for (GLuint shader : shaderIDs)
            {
                glDeleteShader(shader);
            }

            // Use the infoLog as you see fit.
            ENGINE_ERROR("Shader linking failed:\n{0}", infoLog.data());
            // In this simple program, we'll just leave
            return;
        }

        // Always detach shaders after a successful link.
        for (GLuint shader : shaderIDs)
        {
            glDetachShader(program, shader);
        }
    }

#pragma endregion

#pragma region Check hash

    std::string OpenGLShader::CalculateHash(const std::unordered_map<GLenum, std::string> &shaderSources)
    {
        ENGINE_PROFILING_FUNC();

        std::string combinedSources;
        for (const auto &[type, source] : shaderSources)
        {
            combinedSources += source;
        }
        return Utils::CalculateHash(combinedSources);
    }

    std::string OpenGLShader::LoadHashValue(std::string path)
    {
        if (!std::filesystem::exists(Utils::GetCacheDirectory() + "/" + "hash.yaml"))
        {
            return "";
        }
        YAML::Node config = YAML::LoadFile(Utils::GetCacheDirectory() + "/" + "hash.yaml");
        if (config[path])
        {
            return config[path].as<std::string>();
        }
        return "";
    }

    void OpenGLShader::SaveHashValue(std::string path, std::string hashValue)
    {
        if (!std::filesystem::exists(Utils::GetCacheDirectory() + "/" + "hash.yaml"))
        {
            std::ofstream fout(Utils::GetCacheDirectory() + "/" + "hash.yaml");
            fout << "";
        }
        YAML::Node config = YAML::LoadFile(Utils::GetCacheDirectory() + "/" + "hash.yaml");
        config[path] = hashValue;
        std::ofstream fout(Utils::GetCacheDirectory() + "/" + "hash.yaml");
        fout << config;
    }
#pragma endregion
} // namespace Platform::OpenGL