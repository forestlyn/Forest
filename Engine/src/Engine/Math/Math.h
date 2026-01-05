#include "Random.h"
#include <glm/glm.hpp>
namespace Engine::Math
{
    bool DecomposeTransform(const glm::mat4 &transform, glm::vec3 &outTranslation, glm::vec3 &outRotation, glm::vec3 &outScale);
} // namespace Engine::Math