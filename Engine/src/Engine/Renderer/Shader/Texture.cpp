#include "Texture.h"
#include "Engine/pcheader.h"
#include "../Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"
#include "../Utils.h"
namespace Engine::Renderer
{
    Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
    {
        switch (Renderer::Renderer::GetAPI())
        {
        case RendererAPI::API::None:
            ENGINE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL:
        {
            Ref<Platform::OpenGL::OpenGLTexture2D> textureRef = Ref<Platform::OpenGL::OpenGLTexture2D>(new Platform::OpenGL::OpenGLTexture2D(width, height));
#ifdef ENGINE_ENABLE_RENDERTHREAD
            Core::Application::Get().SubmitRendererCommand([textureRef]()
                                                           { textureRef->Init(); });
#else
            {
                textureRef->Init();
            }
#endif
            return textureRef;
        }

        default:
            break;
        }
        ENGINE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }

    Ref<Texture2D> Texture2D::Create(const std::string &path)
    {
        switch (Renderer::Renderer::GetAPI())
        {
        case RendererAPI::API::None:
            ENGINE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL:
        {
            int width, height, channels;
            void *textureData = LoadImg(path, width, height, channels);
            ENGINE_ASSERT(textureData);
            Ref<Platform::OpenGL::OpenGLTexture2D> textureRef = Ref<Platform::OpenGL::OpenGLTexture2D>(new Platform::OpenGL::OpenGLTexture2D(width, height, channels, textureData));

#ifdef ENGINE_ENABLE_RENDERTHREAD
            Core::Application::Get().SubmitRendererCommand([textureRef]()
                                                           { textureRef->Init(); });
#else
            {
                textureRef->Init();
            }
#endif

#ifndef ENGINE_ENABLE_RENDERTHREAD
            // 单线程模式下，直接在外部释放
            delete[] (unsigned char *)textureData; // 请确保这里跟 LoadImg 分配内存的方式匹配
#endif
            return textureRef;
        }
        default:
            break;
        }
        ENGINE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
} // namespace Engine::Renderer