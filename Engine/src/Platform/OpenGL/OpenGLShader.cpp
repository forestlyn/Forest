#include "OpenGLShader.h"
#include "glad/glad.h"
#include "Engine/pcheader.h"
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <vector>
#include "Engine/Profile/Instrumentor.h"
namespace Platform::OpenGL
{

    static GLenum ConvertShaderTypeFromString(const std::string &type)
    {
        if (type == "vertex")
            return GL_VERTEX_SHADER;
        if (type == "fragment" || type == "pixel")
            return GL_FRAGMENT_SHADER;

        ENGINE_ASSERT(false, "Unknown shader type!");
        return 0;
    }

    OpenGLShader::OpenGLShader(const std::string &name, const std::string &vertexSrc, const std::string &fragmentSrc)
        : m_Name(name)
    {
        ENGINE_PROFILING_FUNC();

        std::unordered_map<GLenum, std::string> shaderSources;
        shaderSources[GL_VERTEX_SHADER] = vertexSrc;
        shaderSources[GL_FRAGMENT_SHADER] = fragmentSrc;

        CompileShader(shaderSources);
    }

    OpenGLShader::OpenGLShader(const std::string &filepath)
    {
        ENGINE_PROFILING_FUNC();

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
        CompileShader(shaderSources);

        // // test
        // ExtractNameFromFilepath("filepath/shader.glsl");
        // ENGINE_INFO("Shader name extracted: {0}", m_Name);
        // ExtractNameFromFilepath("shader.glsl");
        // ENGINE_INFO("Shader name extracted: {0}", m_Name);
        // ExtractNameFromFilepath("filepath\\shader.glsl");
        // ENGINE_INFO("Shader name extracted: {0}", m_Name);
        // ExtractNameFromFilepath("shader");
        // ENGINE_INFO("Shader name extracted: {0}", m_Name);

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

    void OpenGLShader::SetInt(const std::string &name, int value)
    {
        ENGINE_PROFILING_FUNC();

        UploadUniformInt(name, value);
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

#pragma region Upload Uniforms

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
            ENGINE_ASSERT(ConvertShaderTypeFromString(type), "Invalid shader type specified");

            size_t nextLinePos = source.find_first_not_of("\r\n", eol);
            pos = source.find(typeToken, nextLinePos);
            shaderSources[ConvertShaderTypeFromString(type)] =
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

#pragma endregion

} // namespace Platform::OpenGL