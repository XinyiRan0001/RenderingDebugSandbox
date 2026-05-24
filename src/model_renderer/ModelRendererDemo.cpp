#include "ModelRendererDemo.h"

#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <utility>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "OBJ_Loader.h"

void ModelRendererDemo::run()
{
    const std::string modelPath =
        "assets/models/spot/spot_triangulated_good.obj";

    objl::Loader loader;
    bool loaded = loader.LoadFile(modelPath);

    if (!loaded)
    {
        std::cout << "Failed to load OBJ: " << modelPath << std::endl;
        return;
    }

    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return;
    }

    GLFWwindow* window = glfwCreateWindow(
        1280,
        720,
        "Model Renderer Debugger",
        nullptr,
        nullptr
    );

    if (!window)
    {
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(-1, 1, -1, 1, -1, 1);

    glMatrixMode(GL_MODELVIEW);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    float rotationY = 0.0f;
    float rotationX = 0.0f;
    float modelScale = 0.5f;

    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
            renderMode = ModelRenderMode::Wireframe;

        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
            renderMode = ModelRenderMode::Normal;

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            rotationY -= 0.02f;

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            rotationY += 0.02f;

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            rotationX += 0.02f;

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            rotationX -= 0.02f;

        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            modelScale += 0.01f;

        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            modelScale -= 0.01f;
            if (modelScale < 0.1f)
                modelScale = 0.1f;
        }

        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Model Renderer Debugger");

        glLoadIdentity();

        glColor3f(1.0f, 1.0f, 1.0f);

        glBegin(GL_LINES);

        auto& mesh = loader.LoadedMeshes[0];

        for (size_t i = 0;
            i < mesh.Indices.size();
            i += 3)
        {
            auto v0 =
                mesh.Vertices[
                    mesh.Indices[i]
                ];

            auto v1 =
                mesh.Vertices[
                    mesh.Indices[i + 1]
                ];

            auto v2 =
                mesh.Vertices[
                    mesh.Indices[i + 2]
                ];

            float scale = 0.5f;

            auto transformVertex =
                [&](float x, float y, float z)
                {
                    float y1 =
                        y * cos(rotationX) -
                        z * sin(rotationX);

                    float z1 =
                        y * sin(rotationX) +
                        z * cos(rotationX);

                    float x2 =
                        x * cos(rotationY) +
                        z1 * sin(rotationY);

                    return std::pair<float, float>(
                        x2 * modelScale,
                        y1 * modelScale
                    );
                };

            auto p0 = transformVertex(v0.Position.X, v0.Position.Y, v0.Position.Z);
            auto p1 = transformVertex(v1.Position.X, v1.Position.Y, v1.Position.Z);
            auto p2 = transformVertex(v2.Position.X, v2.Position.Y, v2.Position.Z);

            glVertex2f(p0.first, p0.second);
            glVertex2f(p1.first, p1.second);

            glVertex2f(p1.first, p1.second);
            glVertex2f(p2.first, p2.second);

            glVertex2f(p2.first, p2.second);
            glVertex2f(p0.first, p0.second);
        }

        glEnd();

        ImGui::Text("OBJ Loaded Successfully");
        ImGui::Separator();

        ImGui::Text("Model Path:");
        ImGui::Text("%s", modelPath.c_str());

        ImGui::Separator();

        ImGui::Text("Mesh Count: %d", (int)loader.LoadedMeshes.size());

        if (!loader.LoadedMeshes.empty())
        {
            ImGui::Text(
                "Vertex Count: %d",
                (int)loader.LoadedMeshes[0].Vertices.size()
            );

            ImGui::Text(
                "Index Count: %d",
                (int)loader.LoadedMeshes[0].Indices.size()
            );
        }

        ImGui::Separator();

        ImGui::Text("Next Step:");
        ImGui::Text("Render Mode: Wireframe");
        ImGui::Text("Model: spot_triangulated_good.obj");
        ImGui::Text("A / D - Rotate Y");
        ImGui::Text("W / S - Rotate X");
        ImGui::Text("Up / Down - Zoom");
        ImGui::Text("Scale: %.2f", modelScale);
        ImGui::Text("Rotation X: %.2f", rotationX);
        ImGui::Text("Rotation Y: %.2f", rotationY);


        ImGui::End();

        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(
            ImGui::GetDrawData()
        );

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
}