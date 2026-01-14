#pragma once
#include "Engine/Renderer/FrameBuffer.h"
#include "Engine/Renderer/Shader/Buffer.h"
#include <glad/glad.h>
#include <shaderc/shaderc.hpp>
namespace Platform::OpenGL::Utils
{

    // Vertex Array utility
    bool IsInteger(Engine::Renderer::ShaderDataType type);
    GLenum GetGLenumFromShaderDataType(Engine::Renderer::ShaderDataType type);

    // Utility functions for framebuffer management
    GLenum TextureTarget(bool multisample);
    GLenum TextureFormatToGLFormat(Engine::Renderer::TextureFormat format);
    GLenum TextureFormatToGLBaseFormat(Engine::Renderer::TextureFormat format);

    bool IsDepthFormat(Engine::Renderer::TextureFormat format);

    void GLCreateTexture(bool multisample, uint32_t &outID, uint32_t count);
    void GLBindTexture(bool multisample, uint32_t id);
    void GLAttachColorTexture(uint32_t id, int samples, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, int index);
    void GLAttachDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height);

    // Compile Shader utility functions
    GLenum ConvertShaderTypeFromString(const std::string &type);
    std::string GLShaderStageToString(GLenum type);
    shaderc_shader_kind GLShaderTypeToShaderC(GLenum type);
    std::string GetCacheDirectory();
    void CreateCacheDirIfNotExists(std::string cacheDir);
    std::string GLShaderStageCachedVulkanFileExtension(GLenum type);
    std::string GLShaderStageCachedOpenGLFileExtension(GLenum type);
    void WriteSPIRVBinaryToCache(const std::string &cacheFile, const std::vector<uint32_t> &spirv);
}