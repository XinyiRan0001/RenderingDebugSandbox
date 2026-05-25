#pragma once

enum class ModelRenderMode
{
    Wireframe,
    Normal,
    Lambert,
    BlinnPhong,
    UV,
    Texture,
    TextureLinear
};

class ModelRendererDemo
{
public:
    void run();

private:
    ModelRenderMode renderMode = ModelRenderMode::Wireframe;
};