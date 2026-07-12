#pragma once
#include <unordered_map>
#include <vector>
#include "SpriteFrame.h"
namespace Engine
{
    class TextureAtlas
    {
    public:
        // Load frame data from a JSON config file and associate it with the given texture
        static Ref<TextureAtlas> Create(const std::string &configPath);

        const SpriteFrame *GetFrame(const std::string &name) const
        {
            auto it = m_Frames.find(name);
            if (it != m_Frames.end())
            {
                return &it->second;
            }
            return nullptr;
        }

        const std::vector<std::string> &GetFrameNames() const { return m_FrameNames; }
        const Ref<Renderer::Texture2D> &GetTexture() const { return m_Texture; }

    private:
        Ref<Renderer::Texture2D> m_Texture;
        std::unordered_map<std::string, SpriteFrame> m_Frames;
        std::vector<std::string> m_FrameNames;
    };
}