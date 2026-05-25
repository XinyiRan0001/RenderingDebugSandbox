#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Texture.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>

Texture::Texture(const std::string& filename)
{
    unsigned char* imageData =
        stbi_load(
            filename.c_str(),
            &width,
            &height,
            &channels,
            3
        );

    if (!imageData)
    {
        std::cout << "Failed to load texture: "
            << filename << std::endl;
        return;
    }

    data.assign(
        imageData,
        imageData + width * height * 3
    );

    stbi_image_free(imageData);

    std::cout << "Texture loaded: "
        << filename
        << " (" << width << "x" << height << ")"
        << std::endl;
}

TextureColor Texture::getPixel(int x, int y) const
{
    x = std::max(0, std::min(x, width - 1));
    y = std::max(0, std::min(y, height - 1));

    int index = (y * width + x) * 3;

    return {
        (float)data[index + 0],
        (float)data[index + 1],
        (float)data[index + 2]
    };
}

TextureColor Texture::getColor(float u, float v) const
{
    if (data.empty())
        return { 255.0f, 0.0f, 255.0f };

    u = std::max(0.0f, std::min(u, 1.0f));
    v = std::max(0.0f, std::min(v, 1.0f));

    int x = (int)(u * (width - 1));
    int y = (int)((1.0f - v) * (height - 1));

    return getPixel(x, y);
}

TextureColor Texture::getColorBilinear(float u, float v) const
{
    if (data.empty())
        return { 255.0f, 0.0f, 255.0f };

    u = std::max(0.0f, std::min(u, 1.0f));
    v = std::max(0.0f, std::min(v, 1.0f));

    float x = u * (width - 1);
    float y = (1.0f - v) * (height - 1);

    int x0 = (int)std::floor(x);
    int y0 = (int)std::floor(y);

    int x1 = x0 + 1;
    int y1 = y0 + 1;

    float tx = x - x0;
    float ty = y - y0;

    TextureColor c00 = getPixel(x0, y0);
    TextureColor c10 = getPixel(x1, y0);
    TextureColor c01 = getPixel(x0, y1);
    TextureColor c11 = getPixel(x1, y1);

    TextureColor c0 = {
        c00.x * (1.0f - tx) + c10.x * tx,
        c00.y * (1.0f - tx) + c10.y * tx,
        c00.z * (1.0f - tx) + c10.z * tx
    };

    TextureColor c1 = {
        c01.x * (1.0f - tx) + c11.x * tx,
        c01.y * (1.0f - tx) + c11.y * tx,
        c01.z * (1.0f - tx) + c11.z * tx
    };

    return {
        c0.x * (1.0f - ty) + c1.x * ty,
        c0.y * (1.0f - ty) + c1.y * ty,
        c0.z * (1.0f - ty) + c1.z * ty
    };
}