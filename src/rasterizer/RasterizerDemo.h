#pragma once

enum class RasterizerViewMode
{
    FinalColor,
    Depth,
    Wireframe,
    MSAA
};

class RasterizerDemo
{
public:
    void run();

private:
    RasterizerViewMode viewMode =
        RasterizerViewMode::FinalColor;
};