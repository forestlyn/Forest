#include "Utils.h"
#include <filesystem>
#include <fstream>
#include <shaderc/shaderc.hpp>
namespace Platform::OpenGL::Utils
{
#pragma region Vertex Array Utility
    bool IsInteger(Engine::Renderer::ShaderDataType type)
    {
        switch (type)
        {
        case Engine::Renderer::ShaderDataType::Int:
        case Engine::Renderer::ShaderDataType::Int2:
        case Engine::Renderer::ShaderDataType::Int3:
        case Engine::Renderer::ShaderDataType::Int4:
            return true;
        default:
            break;
        }
        return false;
    }

    GLenum GetGLenumFromShaderDataType(Engine::Renderer::ShaderDataType type)
    {
        switch (type)
        {
        case Engine::Renderer::ShaderDataType::Float:
        case Engine::Renderer::ShaderDataType::Float2:
        case Engine::Renderer::ShaderDataType::Float3:
        case Engine::Renderer::ShaderDataType::Float4:
        case Engine::Renderer::ShaderDataType::Mat3:
        case Engine::Renderer::ShaderDataType::Mat4:
            return GL_FLOAT;
        case Engine::Renderer::ShaderDataType::Int:
        case Engine::Renderer::ShaderDataType::Int2:
        case Engine::Renderer::ShaderDataType::Int3:
        case Engine::Renderer::ShaderDataType::Int4:
            return GL_INT;
        case Engine::Renderer::ShaderDataType::Bool:
            return GL_BOOL;
        default:
            ENGINE_ASSERT(false, "Unknown ShaderDataType!");
            return 0;
        }
    }
#pragma endregion
#pragma region Framebuffer Utility
    GLenum TextureTarget(bool multisample)
    {
        return multisample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
    }

    GLenum TextureFormatToGLFormat(Engine::Renderer::TextureFormat format)
    {
        switch (format)
        {
        case Engine::Renderer::TextureFormat::RGBA8:
            return GL_RGBA8;
        case Engine::Renderer::TextureFormat::RED_INTEGER_32:
            return GL_R32I;
        case Engine::Renderer::TextureFormat::DEPTH24STENCIL8:
            return GL_DEPTH24_STENCIL8;
        default:
            ENGINE_ASSERT(false, "Unknown TextureFormat!");
            return 0;
        }
    }

    GLenum TextureFormatToGLBaseFormat(Engine::Renderer::TextureFormat format)
    {
        switch (format)
        {
        case Engine::Renderer::TextureFormat::RGBA8:
            return GL_RGBA;
        case Engine::Renderer::TextureFormat::RED_INTEGER_32:
            return GL_RED_INTEGER;
        case Engine::Renderer::TextureFormat::DEPTH24STENCIL8:
            return GL_DEPTH_STENCIL;
        default:
            ENGINE_ASSERT(false, "Unknown TextureFormat!");
            return 0;
        }
    }

    bool IsDepthFormat(Engine::Renderer::TextureFormat format)
    {
        switch (format)
        {
        case Engine::Renderer::TextureFormat::DEPTH24STENCIL8:
            return true;
        default:
            return false;
        }
        return false;
    }

    void GLCreateTexture(bool multisample, uint32_t &outID, uint32_t count)
    {
        glCreateTextures(TextureTarget(multisample), count, &outID);
    }

    void GLBindTexture(bool multisample, uint32_t id)
    {
        glBindTexture(TextureTarget(multisample), id);
    }

    void GLAttachColorTexture(uint32_t id, int samples, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, int index)
    {
        if (samples > 1)
        {
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
        }
        else
        {
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);

            GLint filter = (format == GL_RED_INTEGER) ? GL_NEAREST : GL_LINEAR;
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(samples > 1), id, 0);
    }
    void GLAttachDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height)
    {
        if (samples > 1)
        {
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
        }
        else
        {
            glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(samples > 1), id, 0);
    }
#pragma endregion
#pragma region Compile Shader Utility
    GLenum ConvertShaderTypeFromString(const std::string &type)
    {
        if (type == "vertex")
            return GL_VERTEX_SHADER;
        if (type == "fragment" || type == "pixel")
            return GL_FRAGMENT_SHADER;

        ENGINE_ASSERT(false, "Unknown shader type!");
        return 0;
    }
    std::string GLShaderStageToString(GLenum type)
    {
        switch (type)
        {
        case GL_VERTEX_SHADER:
            return "GL_VERTEX_SHADER";
        case GL_FRAGMENT_SHADER:
            return "GL_FRAGMENT_SHADER";
        default:
            ENGINE_ASSERT(false, "Unknown GL shader type!");
            return "";
        }
    }

    shaderc_shader_kind GLShaderTypeToShaderC(GLenum type)
    {
        switch (type)
        {
        case GL_VERTEX_SHADER:
            return shaderc_vertex_shader;
        case GL_FRAGMENT_SHADER:
            return shaderc_fragment_shader;
        default:
            ENGINE_ASSERT(false, "Unknown GL shader type!");
            return static_cast<shaderc_shader_kind>(0);
        }
    }

    std::string GetCacheDirectory()
    {
        return "assets/cache/shaders/opengl/";
    }
    void CreateCacheDirIfNotExists(std::string cacheDir)
    {
        if (std::filesystem::exists(std::filesystem::path(cacheDir)) == false)
        {
            std::filesystem::create_directories(std::filesystem::path(cacheDir));
        }
    }
    std::string GLShaderStageCachedVulkanFileExtension(GLenum type)
    {
        std::string outExtension;
        switch (type)
        {
        case GL_VERTEX_SHADER:
            outExtension = ".vulkan.vert.spv";
            break;
        case GL_FRAGMENT_SHADER:
            outExtension = ".vulkan.frag.spv";
            break;
        default:
            ENGINE_ASSERT(false, "Unsupported shader type for SPIR-V compilation");
            break;
        }
        return outExtension;
    }

    std::string GLShaderStageCachedOpenGLFileExtension(GLenum type)
    {
        std::string outExtension;
        switch (type)
        {
        case GL_VERTEX_SHADER:
            outExtension = ".opengl.vert.spv";
            break;
        case GL_FRAGMENT_SHADER:
            outExtension = ".opengl.frag.spv";
            break;
        default:
            ENGINE_ASSERT(false, "Unsupported shader type for SPIR-V compilation");
            break;
        }
        return outExtension;
    }

    void WriteSPIRVBinaryToCache(const std::string &cacheFile, const std::vector<uint32_t> &spirv)
    {
        std::filesystem::path filePath = cacheFile;
        std::ofstream outFile(filePath, std::ios::binary);
        if (outFile)
        {
            outFile.write(reinterpret_cast<const char *>(spirv.data()), spirv.size() * sizeof(uint32_t));
            outFile.close();
        }
        else
        {
            ENGINE_ERROR("Could not write SPIR-V binary to cache at '{0}'", filePath.string());
        }
    }

#pragma endregion
}