#include "Math.h"
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
namespace Engine::Math
{
    bool DecomposeTransform(const glm::mat4 &transform, glm::vec3 &outTranslation, glm::vec3 &outRotation, glm::vec3 &outScale)
    {
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::quat rotation;
        if (!glm::decompose(transform, outScale, rotation, outTranslation, skew, perspective))
        {
            return false;
        }
        outRotation = glm::eulerAngles(rotation);
        outRotation = glm::degrees(outRotation);
        return true;
    }
}
