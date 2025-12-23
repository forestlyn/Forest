#include "ParticleSystem.h"
#include "Engine/Renderer/Renderer2D.h"
#include "Engine/Math/Random.h"
#include <glm/gtc/constants.hpp>
ParticleSystem::ParticleSystem()
{
    m_ParticlePool.resize(MaxParticles);
}

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::OnUpdate(Engine::Core::Timestep ts)
{
    for (auto &particle : m_ParticlePool)
    {
        if (!particle.Active)
            continue;

        if (particle.LifeRemaining <= 0.0f)
        {
            particle.Active = false;
            continue;
        }

        particle.LifeRemaining -= ts.GetSeconds();
        particle.Position += particle.Velocity * (float)ts;
    }
}

void ParticleSystem::OnRender()
{
    for (auto &particle : m_ParticlePool)
    {
        if (!particle.Active)
            continue;

        float life = particle.LifeRemaining / particle.LifeTime;
        glm::vec4 color = glm::mix(particle.ColorEnd, particle.ColorBegin, life);
        float size = glm::mix(particle.SizeEnd, particle.SizeBegin, life);
        Engine::Renderer::Renderer2D::DrawRotateQuad(particle.Position, {size, size}, color, particle.Rotation);
    }
}

void ParticleSystem::Emit(const ParticleProperties &properties)
{
    Particle &particle = m_ParticlePool[m_PoolIndex];
    particle.Active = true;
    particle.Position = properties.Position;
    particle.Rotation = Engine::Math::Random::Float() * 360.0f;

    // Velocity
    particle.Velocity = properties.Velocity;
    particle.Velocity.x += properties.VelocityVariation.x * (Engine::Math::Random::Float() - 0.5f);
    particle.Velocity.y += properties.VelocityVariation.y * (Engine::Math::Random::Float() - 0.5f);

    // Color
    particle.ColorBegin = properties.ColorBegin;
    particle.ColorEnd = properties.ColorEnd;

    particle.LifeTime = properties.LifeTime;
    particle.LifeRemaining = properties.LifeTime;
    particle.SizeBegin = properties.SizeBegin + properties.SizeVariation * (Engine::Math::Random::Float() - 0.5f);
    particle.SizeEnd = properties.SizeEnd;

    m_PoolIndex = (m_PoolIndex - 1 + MaxParticles) % MaxParticles;
}
