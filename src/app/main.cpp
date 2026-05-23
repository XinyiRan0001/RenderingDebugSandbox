#include <iostream>

#include "RopeVisualDemo.h"
#include "../rasterizer/RasterizerDemo.h"

int main()
{
    std::cout << "=== Rendering Debug Sandbox ===" << std::endl;
    std::cout << "1 - Rope Simulation" << std::endl;
    std::cout << "2 - Rasterizer Debugger" << std::endl;

    int choice;
    std::cin >> choice;

    if (choice == 1)
    {
        RopeVisualDemo demo;
        demo.run();
    }
    else if (choice == 2)
    {
        RasterizerDemo demo;
        demo.run();
    }

    return 0;
}