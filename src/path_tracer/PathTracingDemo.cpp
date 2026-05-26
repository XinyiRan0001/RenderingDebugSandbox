#include "PathTracingDemo.h"

#include "Renderer.hpp"
#include "Scene.hpp"
#include "Triangle.hpp"
#include "Sphere.hpp"
#include "Vector.hpp"
#include "global.hpp"

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <string>

static void openImage(const std::string& path)
{
    std::string command = "start \"\" \"" + path + "\"";
    system(command.c_str());
}

static std::string makeOutputPath(const std::string& benchmarkName)
{
    std::filesystem::create_directories("outputs/path_tracer");

    std::string outputPath = "outputs/path_tracer/" + benchmarkName + ".ppm";

    for (char& c : outputPath)
    {
        if (c == ' ')
        {
            c = '_';
        }
    }

    return outputPath;
}

static void printBenchmarkConfig(
    const std::string& name,
    int width,
    int height,
    int spp,
    int threads)
{
    std::cout << "\n=== " << name << " ===\n\n";
    std::cout << "Renderer: CPU Path Tracer\n";
    std::cout << "Scene: Cornell Box\n";
    std::cout << "Resolution: " << width << " x " << height << "\n";
    std::cout << "SPP: " << spp << "\n";
    std::cout << "Threads: " << threads << "\n";
    std::cout << "BVH: Enabled\n";
    std::cout << "Microfacet BRDF: Enabled\n";
    std::cout << "Render Type: Offline Monte Carlo Path Tracing\n";

    std::cout << "\n--------------------------------\n";
    std::cout << "Press [R] to start rendering\n";
    std::cout << "Press [B] to go back\n";
    std::cout << "Selection: ";
}

static void runRenderBenchmark(
    const std::string& benchmarkName,
    int width,
    int height,
    int spp,
    int threads)
{
    printBenchmarkConfig(benchmarkName, width, height, spp, threads);

    char confirm;
    std::cin >> confirm;

    if (confirm == 'B' || confirm == 'b')
    {
        std::cout << "Back to Path Tracing menu.\n";
        return;
    }

    if (confirm != 'R' && confirm != 'r')
    {
        std::cout << "Cancelled.\n";
        return;
    }

    std::string outputPath = makeOutputPath(benchmarkName);

    std::cout << "\nStarting benchmark...\n";

    auto start = std::chrono::high_resolution_clock::now();

    Scene scene(width, height);

    Material* red = new Material(DIFFUSE, Vector3f(0.0f));
    red->Kd = Vector3f(0.63f, 0.065f, 0.05f);

    Material* green = new Material(DIFFUSE, Vector3f(0.0f));
    green->Kd = Vector3f(0.14f, 0.45f, 0.091f);

    Material* white = new Material(DIFFUSE, Vector3f(0.0f));
    white->Kd = Vector3f(0.725f, 0.71f, 0.68f);

    Material* microfacet = new Material(DIFFUSE, Vector3f(0.0f));
    microfacet->Kd = Vector3f(0.2f, 0.2f, 0.2f);
    microfacet->Ks = Vector3f(0.9f, 0.9f, 0.9f);

    Vector3f lightEmission =
        8.0f * Vector3f(0.747f + 0.058f, 0.747f + 0.258f, 0.747f)
        + 15.6f * Vector3f(0.740f + 0.287f, 0.740f + 0.160f, 0.740f)
        + 18.4f * Vector3f(0.737f + 0.642f, 0.737f + 0.159f, 0.737f);

    Material* light = new Material(DIFFUSE, lightEmission);
    light->Kd = Vector3f(0.65f);

    std::cout << "[Debug] Loading Cornell Box meshes...\n";

    std::cout << "[Debug] Loading floor...\n";
    auto* floor = new MeshTriangle("assets/path_tracer/cornellbox/floor.obj", white);

    std::cout << "[Debug] Loading shortbox...\n";
    auto* shortbox = new MeshTriangle("assets/path_tracer/cornellbox/shortbox.obj", microfacet);

    std::cout << "[Debug] Loading tallbox...\n";
    auto* tallbox = new MeshTriangle("assets/path_tracer/cornellbox/tallbox.obj", microfacet);

    std::cout << "[Debug] Loading left...\n";
    auto* left = new MeshTriangle("assets/path_tracer/cornellbox/left.obj", red);

    std::cout << "[Debug] Loading right...\n";
    auto* right = new MeshTriangle("assets/path_tracer/cornellbox/right.obj", green);

    std::cout << "[Debug] Loading light...\n";
    auto* lightMesh = new MeshTriangle("assets/path_tracer/cornellbox/light.obj", light);

    scene.Add(floor);
    scene.Add(shortbox);
    scene.Add(tallbox);
    scene.Add(left);
    scene.Add(right);
    scene.Add(lightMesh);

    std::cout << "[Debug] Calling scene.buildBVH()...\n";
    scene.buildBVH();
    std::cout << "[Debug] BVH build finished.\n";
    
    Renderer renderer;

    std::cout << "[Debug] Calling renderer.Render...\n";
    renderer.Render(scene, spp, threads, outputPath);
    std::cout << "[Debug] renderer.Render finished.\n";

    auto end = std::chrono::high_resolution_clock::now();

    double seconds =
        std::chrono::duration<double>(end - start).count();

    std::cout << "\n=== Benchmark Result ===\n";
    std::cout << "Mode: " << benchmarkName << "\n";
    std::cout << "Resolution: " << width << " x " << height << "\n";
    std::cout << "SPP: " << spp << "\n";
    std::cout << "Threads: " << threads << "\n";
    std::cout << "BVH: Enabled\n";
    std::cout << "Microfacet BRDF: Enabled\n";
    std::cout << "Output: " << outputPath << "\n";

    std::cout << "Time Taken: "
        << std::fixed
        << std::setprecision(2)
        << seconds
        << " sec\n";

    std::cout << "Minutes: "
        << seconds / 60.0
        << "\n";

    std::cout << "Hours: "
        << seconds / 3600.0
        << "\n";

    delete red;
    delete green;
    delete white;
    delete microfacet;
    delete light;
}

static void runBVHBenchmark()
{
    std::cout << "\n=== BVH On/Off Benchmark ===\n";

    std::cout << "This benchmark compares scene traversal with BVH enabled and disabled.\n";
    std::cout << "Press [R] to start, [B] to go back: ";

    char confirm;
    std::cin >> confirm;

    if (confirm == 'B' || confirm == 'b')
        return;

    if (confirm != 'R' && confirm != 'r')
        return;

    const int width = 384;
    const int height = 384;
    const int spp = 4;
    const int threads = 2;

    auto runOnce = [&](bool useBVH, const std::string& name)
        {
            std::string outputPath = makeOutputPath(name);

            Scene scene(width, height);
            scene.useBVH = useBVH;

            Material* red = new Material(DIFFUSE, Vector3f(0.0f));
            red->Kd = Vector3f(0.63f, 0.065f, 0.05f);

            Material* green = new Material(DIFFUSE, Vector3f(0.0f));
            green->Kd = Vector3f(0.14f, 0.45f, 0.091f);

            Material* white = new Material(DIFFUSE, Vector3f(0.0f));
            white->Kd = Vector3f(0.725f, 0.71f, 0.68f);

            Material* microfacet = new Material(DIFFUSE, Vector3f(0.0f));
            microfacet->Kd = Vector3f(0.2f, 0.2f, 0.2f);
            microfacet->Ks = Vector3f(0.9f, 0.9f, 0.9f);

            Vector3f lightEmission =
                8.0f * Vector3f(0.747f + 0.058f, 0.747f + 0.258f, 0.747f)
                + 15.6f * Vector3f(0.740f + 0.287f, 0.740f + 0.160f, 0.740f)
                + 18.4f * Vector3f(0.737f + 0.642f, 0.737f + 0.159f, 0.737f);

            Material* light = new Material(DIFFUSE, lightEmission);
            light->Kd = Vector3f(0.65f);

            auto* floor = new MeshTriangle("assets/path_tracer/cornellbox/floor.obj", white);
            auto* shortbox = new MeshTriangle("assets/path_tracer/cornellbox/shortbox.obj", microfacet);
            auto* tallbox = new MeshTriangle("assets/path_tracer/cornellbox/tallbox.obj", microfacet);
            auto* left = new MeshTriangle("assets/path_tracer/cornellbox/left.obj", red);
            auto* right = new MeshTriangle("assets/path_tracer/cornellbox/right.obj", green);
            auto* lightMesh = new MeshTriangle("assets/path_tracer/cornellbox/light.obj", light);

            scene.Add(floor);
            scene.Add(shortbox);
            scene.Add(tallbox);
            scene.Add(left);
            scene.Add(right);
            scene.Add(lightMesh);

            scene.buildBVH();

            Renderer renderer;

            auto start = std::chrono::high_resolution_clock::now();

            renderer.Render(scene, spp, threads, outputPath);

            auto end = std::chrono::high_resolution_clock::now();

            double seconds =
                std::chrono::duration<double>(end - start).count();

            std::cout << "\n" << name << "\n";
            std::cout << "BVH: " << (useBVH ? "ON" : "OFF") << "\n";
            std::cout << "Time: " << seconds << " sec\n";
            std::cout << "Output: " << outputPath << "\n";

            return seconds;
        };

    double bvhOnTime = runOnce(true, "BVH_ON");
    double bvhOffTime = runOnce(false, "BVH_OFF");

    std::cout << "\n=== BVH Benchmark Result ===\n";
    std::cout << "BVH ON : " << bvhOnTime << " sec\n";
    std::cout << "BVH OFF: " << bvhOffTime << " sec\n";

    if (bvhOnTime > 0.0)
    {
        std::cout << "Speedup: " << bvhOffTime / bvhOnTime << "x\n";
    }
}

static void runCornellStressTest()
{
    std::cout << "\n=== Cornell Box Stress Test ===\n";
    std::cout << "Cornell Box + additional sphere objects.\n";
    std::cout << "Press [R] to start, [B] to go back: ";

    char confirm;
    std::cin >> confirm;

    if (confirm == 'B' || confirm == 'b')
        return;

    if (confirm != 'R' && confirm != 'r')
        return;

    const int width = 384;
    const int height = 384;
    const int spp = 8;
    const int threads = 4;

    Scene scene(width, height);

    Material* red = new Material(DIFFUSE, Vector3f(0.0f));
    red->Kd = Vector3f(0.63f, 0.065f, 0.05f);

    Material* green = new Material(DIFFUSE, Vector3f(0.0f));
    green->Kd = Vector3f(0.14f, 0.45f, 0.091f);

    Material* white = new Material(DIFFUSE, Vector3f(0.0f));
    white->Kd = Vector3f(0.725f, 0.71f, 0.68f);

    Material* microfacet = new Material(DIFFUSE, Vector3f(0.0f));
    microfacet->Kd = Vector3f(0.2f, 0.2f, 0.2f);
    microfacet->Ks = Vector3f(0.9f, 0.9f, 0.9f);

    Vector3f lightEmission =
        8.0f * Vector3f(0.747f + 0.058f, 0.747f + 0.258f, 0.747f)
        + 15.6f * Vector3f(0.740f + 0.287f, 0.740f + 0.160f, 0.740f)
        + 18.4f * Vector3f(0.737f + 0.642f, 0.737f + 0.159f, 0.737f);

    Material* light = new Material(DIFFUSE, lightEmission);
    light->Kd = Vector3f(0.65f);

    std::cout << "[Stress] Loading Cornell Box OBJ scene...\n";

    auto* floor = new MeshTriangle("assets/path_tracer/cornellbox/floor.obj", white);
    auto* shortbox = new MeshTriangle("assets/path_tracer/cornellbox/shortbox.obj", microfacet);
    auto* tallbox = new MeshTriangle("assets/path_tracer/cornellbox/tallbox.obj", microfacet);
    auto* left = new MeshTriangle("assets/path_tracer/cornellbox/left.obj", red);
    auto* right = new MeshTriangle("assets/path_tracer/cornellbox/right.obj", green);
    auto* lightMesh = new MeshTriangle("assets/path_tracer/cornellbox/light.obj", light);

    scene.Add(floor);
    scene.Add(shortbox);
    scene.Add(tallbox);
    scene.Add(left);
    scene.Add(right);
    scene.Add(lightMesh);

    const int gridX = 3;
    const int gridZ = 2;
    const float radius = 28.0f;
    const float spacingX = 70.0f;
    const float spacingZ = 70.0f;

    int sphereCount = 0;

    std::cout << "[Stress] Adding extra spheres...\n";

    for (int x = 0; x < gridX; ++x)
    {
        for (int z = 0; z < gridZ; ++z)
        {
            float px = 300.0f + x * spacingX;
            float pz = 80.0f + z * spacingZ;

            auto* sphere = new Sphere(
                Vector3f(px, radius, pz),
                radius,
                white
            );

            scene.Add(sphere);
            sphereCount++;
        }
    }

    std::cout << "[Stress] Extra spheres: " << sphereCount << "\n";

    std::cout << "[Stress] Building BVH...\n";
    scene.buildBVH();

    Renderer renderer;

    std::string outputPath = makeOutputPath("Cornell_Stress_Test");

    auto start = std::chrono::high_resolution_clock::now();

    renderer.Render(scene, spp, threads, outputPath);

    auto end = std::chrono::high_resolution_clock::now();

    double seconds =
        std::chrono::duration<double>(end - start).count();

    std::cout << "\n=== Cornell Stress Test Result ===\n";
    std::cout << "Scene: Cornell Box + Sphere Grid\n";
    std::cout << "Base OBJ objects: 6\n";
    std::cout << "Extra spheres: " << sphereCount << "\n";
    std::cout << "Resolution: " << width << " x " << height << "\n";
    std::cout << "SPP: " << spp << "\n";
    std::cout << "Threads: " << threads << "\n";
    std::cout << "Render Time: " << seconds << " sec\n";
    std::cout << "Output: " << outputPath << "\n";

    openImage(outputPath);
}

void PathTracingDemo::run()
{
    while (true)
    {
        std::cout << "\n=== Path Tracing Renderer ===\n";
        std::cout << "1 - View Render Gallery\n";
        std::cout << "2 - Baseline Benchmark\n";
        std::cout << "3 - Progressive Preview\n";
        std::cout << "4 - High Quality Render\n";
        std::cout << "5 - Benchmark Comparison\n";
        std::cout << "6 - BVH On/Off Benchmark\n";
        std::cout << "7 - Sphere Stress Test\n";
        std::cout << "8 - Back\n";;
        std::cout << "Select mode: ";

        int mode;
        std::cin >> mode;

        if (mode == 1)
        {
            std::cout << "Opening render gallery...\n";
            openImage("outputs/path_tracer/Progressive_Preview.ppm");
            openImage("outputs/path_tracer/High_Quality_Render.ppm");
        }
        else if (mode == 2)
        {
            runRenderBenchmark("Baseline Benchmark", 384, 384, 8, 1);
        }
        else if (mode == 3)
        {
            runRenderBenchmark("Progressive Preview", 384, 384, 64, 2);
        }
        else if (mode == 4)
        {
            runRenderBenchmark("High Quality Render", 512, 512, 16, 4);
        }
        else if (mode == 5)
        {
            std::cout << "\n=== Benchmark Comparison ===\n";

            const int width = 384;
            const int height = 384;
            const int spp = 8;

            std::vector<int> testThreads = { 1, 2, 4, 8 };

            double baseline = 0.0;

            for (int t : testThreads)
            {
                auto start = std::chrono::high_resolution_clock::now();

                runRenderBenchmark(
                    "Benchmark_" + std::to_string(t) + "Threads",
                    width,
                    height,
                    spp,
                    t
                );

                auto end = std::chrono::high_resolution_clock::now();

                double seconds =
                    std::chrono::duration<double>(end - start).count();

                if (t == 1)
                {
                    baseline = seconds;
                }

                double speedup = baseline / seconds;

                std::cout << "\nThreads: " << t
                    << "\nTime: " << seconds << " sec"
                    << "\nSpeedup: " << speedup << "x\n";
            }

            break;
        }
        else if (mode == 6)
        {
            runBVHBenchmark();
        }

        else if (mode == 7)
        {
            runCornellStressTest();
        }
        
        else if (mode == 8)
        {
            return;
        }

        else
        {
            std::cout << "Invalid mode.\n";
        }
    }
}