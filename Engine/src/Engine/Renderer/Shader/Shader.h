#pragma once
#include <string>
#include <glm/glm.hpp>
namespace Engine::Renderer
{

    class Shader
    {
    public:
        virtual ~Shader() {}

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual void SetInt(const std::string &name, int value) = 0;
        virtual void SetFloat3(const std::string &name, const glm::vec3 &value) = 0;
        virtual void SetFloat4(const std::string &name, const glm::vec4 &value) = 0;
        virtual void SetMat4(const std::string &name, const glm::mat4 &matrix) = 0;

        virtual const std::string &GetName() const = 0;

        static Ref<Shader> Create(const std::string &name, const std::string &vertexSrc, const std::string &fragmentSrc);
        static Ref<Shader> Create(const std::string &filepath);
    };

} // namespace Engine::Renderer