#include "RopeVisualDemo.h"

#include <chrono>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "rope.h"
#include "../profiling/Timer.h"

void RopeVisualDemo::resetRope()
{
    std::vector<int> pinned_nodes = { 0 };

    rope = new CGL::Rope(
        CGL::Vector2D(500, 620),
        CGL::Vector2D(760, 620),
        16,
        1.0f,
        400.0f,
        pinned_nodes
    );
}

void RopeVisualDemo::run()
{
    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return;
    }

    GLFWwindow* window = glfwCreateWindow(
        1280,
        720,
        "Rendering Debug Sandbox - Rope Simulation",
        nullptr,
        nullptr
    );

    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    glPointSize(8.0f);
    glLineWidth(3.0f);

    resetRope();

    CGL::Vector2D gravity(0, -98.0f);

    Timer frameTimer;
    Timer simulationTimer;

    double frameTimeMs = 0.0;
    double simulationTimeMs = 0.0;
    double displayFrameTimeMs = 0.0;
    double displaySimulationTimeMs = 0.0;
    double displayFPS = 0.0;

    bool showVelocity = false;
    bool vKeyWasPressed = false;

    double statsUpdateTimer = 0.0;

    const double fixedDeltaTime = 1.0 / 120.0;

    double physicsAccumulator = 0.0;

    auto previousTime =
        std::chrono::high_resolution_clock::now();

    while (!glfwWindowShouldClose(window))
    {
        frameTimer.start();
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        {
            useVerlet = false;
            delete rope;
            resetRope();
        }

        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        {
            useVerlet = true;
            delete rope;
            resetRope();
        }

        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        {
            delete rope;
            resetRope();
        }

        bool vKeyIsPressed =
        glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS;

        if (vKeyIsPressed && !vKeyWasPressed)
        {
            showVelocity = !showVelocity;
        }

        vKeyWasPressed = vKeyIsPressed;

        simulationTimer.start();

        auto currentTime =
            std::chrono::high_resolution_clock::now();

        std::chrono::duration<double> elapsed =
            currentTime - previousTime;

        previousTime = currentTime;

        physicsAccumulator += elapsed.count();

        simulationTimer.start();

        while (physicsAccumulator >= fixedDeltaTime)
        {
            if (useVerlet)
            {
                rope->simulateVerlet(
                    (float)fixedDeltaTime,
                    gravity
                );
            }
            else
            {
                rope->simulateEuler(
                    (float)fixedDeltaTime,
                    gravity
                );
            }

            physicsAccumulator -= fixedDeltaTime;
        }

        simulationTimeMs =
            simulationTimer.stopMilliseconds();

        simulationTimeMs = simulationTimer.stopMilliseconds();

        glClear(GL_COLOR_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, 1280, 0, 720, -1, 1);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        if (useVerlet)
        {
            glColor3f(0.2f, 1.0f, 0.2f);
        }
        else
        {
            glColor3f(1.0f, 0.3f, 0.3f);
        }

        glBegin(GL_LINES);
        for (auto spring : rope->springs)
        {
            auto a = spring->m1->position;
            auto b = spring->m2->position;

            glVertex2f(a.x, a.y);
            glVertex2f(b.x, b.y);
        }
        glEnd();

        if (showVelocity)
        {
            glBegin(GL_LINES);

            for (auto mass : rope->masses)
            {
                if (mass->pinned)
                    continue;

                CGL::Vector2D velocity = mass->position - mass->last_position;

                float scale = useVerlet ? 40.0f : 8.0f;
                velocity = velocity * scale;

                float maxLength = 30.0f;
                float length = sqrt(velocity.x * velocity.x + velocity.y * velocity.y);

                if (length > maxLength && length > 0.0001f)
                {
                    velocity = velocity / length * maxLength;
                }

                glColor3f(0.2f, 0.6f, 1.0f);

                glVertex2f(mass->position.x, mass->position.y);
                glVertex2f(
                    mass->position.x + velocity.x,
                    mass->position.y + velocity.y
                );
            }

            glEnd();
        }

        glBegin(GL_POINTS);

        for (auto mass : rope->masses)
        {
            if (mass->pinned)
                glColor3f(1.0f, 0.2f, 0.2f);
            else
                glColor3f(1.0f, 1.0f, 1.0f);

            glVertex2f(mass->position.x, mass->position.y);
        }

        glEnd();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Rope Simulation Debug");
        ImGui::Text("Solver: %s", useVerlet ? "Verlet" : "Euler");
        ImGui::Text("Controls:");
        ImGui::Text("1 - Euler");
        ImGui::Text("2 - Verlet");
        ImGui::Text("R - Reset");
        ImGui::Text("V - Toggle Velocity");
        ImGui::Text("Velocity Debug: %s", showVelocity ? "ON" : "OFF");
        ImGui::Separator();
        ImGui::Text("Nodes: %d", (int)rope->masses.size());
        ImGui::Text("Springs: %d", (int)rope->springs.size());
        ImGui::Text("Substeps: 8");
        ImGui::Text("Blue lines: velocity direction / magnitude");
        ImGui::Text("Fixed Delta Time: %.4f", fixedDeltaTime);
        ImGui::Separator();
        ImGui::Text("Simulation Time: %.4f ms", displaySimulationTimeMs);
        ImGui::Text("Frame Time: %.4f ms", displayFrameTimeMs);
        ImGui::Text("FPS: %.1f", displayFPS);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
        frameTimeMs = frameTimer.stopMilliseconds();
        statsUpdateTimer += frameTimeMs;

        if (statsUpdateTimer >= 250.0)
        {
            displayFrameTimeMs = frameTimeMs;
            displaySimulationTimeMs = simulationTimeMs;
            displayFPS = frameTimeMs > 0.0 ? 1000.0 / frameTimeMs : 0.0;

            statsUpdateTimer = 0.0;
        }
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    delete rope;
    rope = nullptr;

    glfwTerminate();
}