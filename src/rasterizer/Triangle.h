#pragma once
#include "Framebuffer.h"

struct Vec3
{
    float x;
    float y;
    float z;
};

class Triangle
{
public:

    Vec3 v0;
    Vec3 v1;
    Vec3 v2;

    Color color;
};