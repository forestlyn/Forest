#pragma once
#include "Engine/pcheader.h"
namespace Engine::Renderer
{
    void *LoadImg(const std::string &path, int &width, int &height, int &channels);
}