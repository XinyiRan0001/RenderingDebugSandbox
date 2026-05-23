#include "Framebuffer.h"

Framebuffer::Framebuffer(int width, int height)
    : width(width), height(height)
{
    colorBuffer.resize(width * height * 3);
    depthBuffer.resize(width * height);
}

void Framebuffer::clear(Color color)
{
    for (int i = 0; i < width * height; i++)
    {
        colorBuffer[i * 3 + 0] = color.r;
        colorBuffer[i * 3 + 1] = color.g;
        colorBuffer[i * 3 + 2] = color.b;
    }
}

void Framebuffer::clearDepth(float depth)
{
    for (int i = 0; i < width * height; i++)
    {
        depthBuffer[i] = depth;
    }
}

void Framebuffer::setPixel(int x, int y, Color color)
{
    if (x < 0 || x >= width || y < 0 || y >= height)
        return;

    int index = (y * width + x) * 3;

    colorBuffer[index + 0] = color.r;
    colorBuffer[index + 1] = color.g;
    colorBuffer[index + 2] = color.b;
}

float Framebuffer::getDepth(int x, int y) const
{
    int index = y * width + x;
    return depthBuffer[index];
}

void Framebuffer::setDepth(int x, int y, float depth)
{
    int index = y * width + x;
    depthBuffer[index] = depth;
}

int Framebuffer::getWidth() const
{
    return width;
}

int Framebuffer::getHeight() const
{
    return height;
}

const unsigned char* Framebuffer::getColorBufferData() const
{
    return colorBuffer.data();
}