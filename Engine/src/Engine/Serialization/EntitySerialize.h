#include "ComponentSerialize.h"
#include "Engine/Scene/Entity.h"
namespace Engine::Serialization
{
    void SerilizaEntity(YAML::Emitter &out, Entity entity);
    bool DeserializeEntity(const YAML::Node &entityNode, Entity &entity);
} // namespace Engine::Serialization