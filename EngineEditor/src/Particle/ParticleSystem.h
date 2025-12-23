#pragma once
#include "Engine/pcheader.h"
#include "Engine/Core/Timestep.h"
#include "Engine/Renderer/Camera/OrthographicCamera.h"
#include <glm/glm.hpp>

struct ParticleProperties
{
    glm::vec3 Position = glm::vec3(0.0f);
    glm::vec3 Velocity = glm::vec3(0.0f);
    glm::vec3 VelocityVariation = glm::vec3(0.0f);
    glm::vec4 ColorBegin = glm::vec4(1.0f);
    glm::vec4 ColorEnd = glm::vec4(0.0f);
    float SizeBegin = 0.3f;
    float SizeEnd = 0.1f;
    float SizeVariation = 0.1f;
    float LifeTime = 1.0f;
    float Rotation = 0.0f;
};

class ParticleSystem
{
public:
    ParticleSystem();
    ~ParticleSystem();

    void OnUpdate(Engine::Core::Timestep ts);
    void OnRender();

    void Emit(const ParticleProperties &properties);

private:
    struct Particle
    {
        glm::vec3 Position;
        glm::vec3 Velocity;
        glm::vec4 ColorBegin;
        glm::vec4 ColorEnd;
        float SizeBegin, SizeEnd;
        float LifeTime = 0.0f;
        float LifeRemaining = 0.0f;
        float Rotation = 0.0f;

        bool Active = false;
    };

    Particle m_Particle;
    std::vector<Particle> m_ParticlePool;
    const int MaxParticles = 1000;
    uint32_t m_PoolIndex = MaxParticles - 1;
};
