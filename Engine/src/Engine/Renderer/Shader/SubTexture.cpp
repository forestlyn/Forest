#include "SubTexture.h"

namespace Engine::Renderer
{
    SubTexture2D::SubTexture2D(const Ref<Texture2D> &texture, const glm::vec2 &min, const glm::vec2 &max)
        : m_Texture(texture)
    {
        m_TexCoords[0] = {min.x, min.y};
        m_TexCoords[1] = {max.x, min.y};
        m_TexCoords[2] = {max.x, max.y};
        m_TexCoords[3] = {min.x, max.y};
    }

    Ref<SubTexture2D> Engine::Renderer::SubTexture2D::CreateFromCoords(const Ref<Texture2D> &texture, const glm::vec2 &cellSize, const glm::vec2 &coord, const glm::vec2 &spriteSize)
    {
        auto textureHeight = texture->GetHeight();
        auto textureWidth = texture->GetWidth();
        glm::vec2 min = {cellSize.x * coord.x / textureWidth, cellSize.y * coord.y / textureHeight};
        glm::vec2 max = {(cellSize.x * (coord.x + spriteSize.x)) / textureWidth, (cellSize.y * (coord.y + spriteSize.y)) / textureHeight};
        ENGINE_INFO("SubTexture2D::CreateFromCoords: min({}, {}), max({}, {})", min.x, min.y, max.x, max.y);
        return CreateRef<SubTexture2D>(texture, min, max);
    }
} // namespace Engine::Renderer
