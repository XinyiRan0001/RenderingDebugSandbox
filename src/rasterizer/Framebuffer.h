#pragma once

#include <vector>

struct Color
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

class Framebuffer
{
public:
    Framebuffer(int width, int height);

    void clear(Color color);
    void setPixel(int x, int y, Color color);

    int getWidth() const;
    int getHeight() const;

    const unsigned char* getColorBufferData() const;

    void clearDepth(float depth);

    float getDepth(int x, int y) const;

    void setDepth(int x, int y, float depth);

private:
    int width;
    int height;

    std::vector<unsigned char> colorBuffer;
    std::vector<float> depthBuffer;
};