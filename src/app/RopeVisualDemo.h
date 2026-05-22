#pragma once

#include "rope.h"

class RopeVisualDemo
{
public:
    void run();

private:

    void resetRope();

    bool useVerlet = true;

    CGL::Rope* rope = nullptr;
};