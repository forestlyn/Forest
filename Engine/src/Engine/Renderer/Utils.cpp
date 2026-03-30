#include "Utils.h"
#include "stb_image.h"
#include "Engine/Core/Application.h"
namespace Engine::Renderer
{
    void *LoadImg(const std::string &path, int &width, int &height, int &channels)
    {
        stbi_set_flip_vertically_on_load(1);
        unsigned char *data = stbi_load(path.c_str(), &width, &height, &channels, 0);

        if (!data)
        {
            // 加载失败时直接返回，防止发生未初始化的内存拷贝与崩溃
            return nullptr;
        }

        int size = width * height * channels;
        void *imgData = nullptr;
#ifdef ENGINE_ENABLE_RENDERTHREAD
        imgData = Core::Application::Get().RendererAllocator(size);
        ENGINE_INFO("Allocate imgdata");
#else
        imgData = new char[size];
#endif
        ENGINE_ASSERT(imgData);
        memcpy(imgData, data, size);
        stbi_image_free(data);
        return imgData;
    }
}