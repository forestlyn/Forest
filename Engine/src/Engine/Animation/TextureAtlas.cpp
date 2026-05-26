#include "TextureAtlas.h"

namespace Engine
{
    Ref<TextureAtlas> TextureAtlas::Create(const std::string &configPath, const Ref<Renderer::Texture2D> &texture)
    {
        auto atlas = Ref<TextureAtlas>();
        atlas->m_Texture = texture;
        // TODO: 解析 JSON 配置文件，填充 m_Frames 和 m_FrameNames

        return atlas;
    }
}