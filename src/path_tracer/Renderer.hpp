#pragma once

#include "Scene.hpp"
#include <string>

struct hit_payload
{
    float tNear;
    uint32_t index;
    Vector2f uv;
    Object* hit_obj;
};

class Renderer
{
public:
    void Render(
        const Scene& scene,
        int spp,
        int threadCount,
        const std::string& outputPath
    );

private:
};