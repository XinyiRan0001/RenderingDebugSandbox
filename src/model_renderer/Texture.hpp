#pragma once

#include <string>
#include <vector>

struct TextureColor
{
    float x;
    float y;
    float z;
};

class Texture
{
public:
    Texture(const std::string& filename);

    TextureColor getColor(float u, float v) const;
    TextureColor getColorBilinear(float u, float v) const;

private:
    int width = 0;
    int height = 0;
    int channels = 0;

    std::vector<unsigned char> data;

    TextureColor getPixel(int x, int y) const;
};
