#pragma once

enum class ModelRenderMode
{
    Wireframe,
    Normal
};

class ModelRendererDemo
{
public:
    void run();

private:
    ModelRenderMode renderMode = ModelRenderMode::Wireframe;
};