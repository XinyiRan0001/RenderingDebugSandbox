#include <fstream>
#include <thread>
#include <atomic>
#include <mutex>
#include <iostream>
#include <cmath>

#include "Scene.hpp"
#include "Renderer.hpp"

inline float deg2rad(const float& deg)
{
    return deg * M_PI / 180.0f;
}

const float EPSILON = 0.00001f;

void Renderer::Render(
    const Scene& scene,
    int spp,
    int threadCount,
    const std::string& outputPath)
{
    std::vector<Vector3f> framebuffer(scene.width * scene.height);

    float scale = tan(deg2rad(scene.fov * 0.5f));
    float imageAspectRatio = scene.width / (float)scene.height;
    Vector3f eye_pos(278, 273, -800);

    if (spp <= 0)
    {
        spp = 1;
    }

    if (threadCount <= 0)
    {
        threadCount = 1;
    }

    if (threadCount > scene.height)
    {
        threadCount = scene.height;
    }

    std::cout << "SPP: " << spp << "\n";
    std::cout << "Threads: " << threadCount << "\n";
    std::cout << "Output: " << outputPath << "\n";

    std::vector<std::thread> threads;
    std::atomic<int> finishedRows(0);
    std::mutex progressMutex;

    auto renderRows = [&](int startY, int endY)
        {
            for (int j = startY; j < endY; ++j)
            {
                for (int i = 0; i < scene.width; ++i)
                {
                    int index = j * scene.width + i;

                    float x =
                        (2 * (i + 0.5f) / (float)scene.width - 1) *
                        imageAspectRatio *
                        scale;

                    float y =
                        (1 - 2 * (j + 0.5f) / (float)scene.height) *
                        scale;

                    Vector3f dir = normalize(Vector3f(-x, y, 1));

                    Vector3f pixelColor(0);

                    for (int k = 0; k < spp; k++)
                    {
                        pixelColor += scene.castRay(Ray(eye_pos, dir), 0) / spp;
                    }

                    framebuffer[index] = pixelColor;
                }

                int rowsDone = ++finishedRows;

                if (rowsDone % 16 == 0 || rowsDone == scene.height)
                {
                    std::lock_guard<std::mutex> lock(progressMutex);
                    UpdateProgress(rowsDone / (float)scene.height);
                }
            }
        };

    int rowsPerThread = scene.height / threadCount;
    int startY = 0;

    for (int t = 0; t < threadCount; ++t)
    {
        int endY =
            (t == threadCount - 1)
            ? scene.height
            : startY + rowsPerThread;

        threads.emplace_back(renderRows, startY, endY);
        startY = endY;
    }

    for (auto& thread : threads)
    {
        thread.join();
    }

    UpdateProgress(1.0f);

    FILE* fp = fopen(outputPath.c_str(), "wb");

    if (!fp)
    {
        std::cerr << "Failed to open output file: "
            << outputPath
            << "\n";
        return;
    }

    fprintf(fp, "P6\n%d %d\n255\n", scene.width, scene.height);

    for (int i = 0; i < scene.height * scene.width; ++i)
    {
        unsigned char color[3];

        color[0] =
            (unsigned char)(255 * std::pow(clamp(0, 1, framebuffer[i].x), 0.6f));

        color[1] =
            (unsigned char)(255 * std::pow(clamp(0, 1, framebuffer[i].y), 0.6f));

        color[2] =
            (unsigned char)(255 * std::pow(clamp(0, 1, framebuffer[i].z), 0.6f));

        fwrite(color, 1, 3, fp);
    }

    fclose(fp);
}