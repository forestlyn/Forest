#pragma once
#include "Engine/Core/Core.h"
#include "Texture.h"
#include <glm/glm.hpp>
namespace Engine::Renderer
{
    class SubTexture2D
    {
    public:
        SubTexture2D(const Ref<Texture2D> &texture, const glm::vec2 &min, const glm::vec2 &max);

        const Ref<Texture2D> &GetTexture() const { return m_Texture; }
        const glm::vec2 *GetTexCoords() const { return m_TexCoords; }
        uint32_t GetRendererID() const { return m_Texture->GetRendererID(); }

        static Ref<SubTexture2D> CreateFromCoords(const Ref<Texture2D> &texture, const glm::vec2 &cellSize, const glm::vec2 &coord, const glm::vec2 &spriteSize = {1, 1});

    private:
        Ref<Texture2D> m_Texture;
        glm::vec2 m_TexCoords[4];
    };

} // namespace Engine::Renderer