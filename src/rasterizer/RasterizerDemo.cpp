#include "RasterizerDemo.h"
#include "Framebuffer.h"
#include "Triangle.h"

#include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

static float edgeFunction(
    const Vec3& a,
    const Vec3& b,
    float x,
    float y
)
{
    return (b.x - a.x) * (y - a.y) -
        (b.y - a.y) * (x - a.x);
}

static void rasterizeTriangle(
    Framebuffer& framebuffer,
    const Triangle& triangle,
    RasterizerViewMode viewMode
)

{
    float area =
        edgeFunction(
            triangle.v0,
            triangle.v1,
            triangle.v2.x,
            triangle.v2.y
        );

    for (int y = 0; y < framebuffer.getHeight(); y++)
    {
        for (int x = 0; x < framebuffer.getWidth(); x++)
        {
            float px = x + 0.5f;
            float py = y + 0.5f;

            float w0 =
                edgeFunction(
                    triangle.v1,
                    triangle.v2,
                    px,
                    py
                ) / area;

            float w1 =
                edgeFunction(
                    triangle.v2,
                    triangle.v0,
                    px,
                    py
                ) / area;

            float w2 =
                edgeFunction(
                    triangle.v0,
                    triangle.v1,
                    px,
                    py
                ) / area;

            bool inside =
                w0 >= 0.0f &&
                w1 >= 0.0f &&
                w2 >= 0.0f;

            if (!inside)
                continue;

            float depth =
                w0 * triangle.v0.z +
                w1 * triangle.v1.z +
                w2 * triangle.v2.z;

            if (depth < framebuffer.getDepth(x, y))
            {
                framebuffer.setDepth(x, y, depth);

                if (viewMode ==
                    RasterizerViewMode::FinalColor)
                {
                    framebuffer.setPixel(
                        x,
                        y,
                        triangle.color
                    );
                }
                else if (viewMode ==
                    RasterizerViewMode::Depth)
                {
                    unsigned char depthColor =
                        (unsigned char)(
                            (1.0f - depth) * 255.0f
                            );

                    framebuffer.setPixel(
                        x,
                        y,
                        {
                            depthColor,
                            depthColor,
                            depthColor
                        }
                    );
                }
                else if (viewMode ==
                    RasterizerViewMode::Wireframe)
                {
                    float edgeThreshold = 0.02f;

                    bool nearEdge =
                        w0 < edgeThreshold ||
                        w1 < edgeThreshold ||
                        w2 < edgeThreshold;

                    if (nearEdge)
                    {
                        framebuffer.setPixel(
                            x,
                            y,
                            { 255, 255, 255 }
                        );
                    }
                }

                else if (viewMode ==
                    RasterizerViewMode::MSAA)
                {
                    int coveredSamples = 0;

                    float sampleOffsets[4][2] =
                    {
                        {0.25f, 0.25f},
                        {0.75f, 0.25f},
                        {0.25f, 0.75f},
                        {0.75f, 0.75f}
                    };

                    for (int i = 0; i < 4; i++)
                    {
                        float sx = x + sampleOffsets[i][0];
                        float sy = y + sampleOffsets[i][1];

                        float sw0 =
                            edgeFunction(
                                triangle.v1,
                                triangle.v2,
                                sx,
                                sy
                            ) / area;

                        float sw1 =
                            edgeFunction(
                                triangle.v2,
                                triangle.v0,
                                sx,
                                sy
                            ) / area;

                        float sw2 =
                            edgeFunction(
                                triangle.v0,
                                triangle.v1,
                                sx,
                                sy
                            ) / area;

                        bool sampleInside =
                            sw0 >= 0.0f &&
                            sw1 >= 0.0f &&
                            sw2 >= 0.0f;

                        if (sampleInside)
                        {
                            coveredSamples++;
                        }
                    }

                    float coverage = coveredSamples / 4.0f;

                    framebuffer.setPixel(
                        x,
                        y,
                        {
                            (unsigned char)(triangle.color.r * coverage),
                            (unsigned char)(triangle.color.g * coverage),
                            (unsigned char)(triangle.color.b * coverage)
                        }
                    );
                }
            }
        }
    }
}

void RasterizerDemo::run()
{
    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return;
    }

    GLFWwindow* window = glfwCreateWindow(
        1280,
        720,
        "Rasterizer Debugger",
        nullptr,
        nullptr
    );

    if (!window)
    {
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);

    Framebuffer framebuffer(512, 512);

    Triangle triangle;
    triangle.v0 = { 100, 100, 0.9f };
    triangle.v1 = { 400, 120, 0.3f };
    triangle.v2 = { 250, 400, 0.6f };
    triangle.color = { 255, 0, 0 };

    Triangle triangle2;
    triangle2.v0 = { 180, 180, 0.2f };
    triangle2.v1 = { 450, 220, 0.7f };
    triangle2.v2 = { 280, 450, 0.4f };
    triangle2.color = { 0, 255, 0 };
    float rotationAngle = 0.0f;

    GLuint textureID;

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        {
            viewMode = RasterizerViewMode::FinalColor;
        }

        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        {
            viewMode = RasterizerViewMode::Depth;
        }

        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        {
            viewMode = RasterizerViewMode::Wireframe;
        }

        if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
        {
            viewMode = RasterizerViewMode::MSAA;
        }

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            rotationAngle += 0.02f;
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            rotationAngle -= 0.02f;
        }

        glClear(GL_COLOR_BUFFER_BIT);

        framebuffer.clear({ 20, 20, 20 });
        framebuffer.clearDepth(99999.0f);

        Triangle rotatedTriangle = triangle2;

        float centerX =
            (triangle2.v0.x +
                triangle2.v1.x +
                triangle2.v2.x) / 3.0f;

        float centerY =
            (triangle2.v0.y +
                triangle2.v1.y +
                triangle2.v2.y) / 3.0f;

        auto rotateVertex =
            [&](Vec3& v)
            {
                float localX = v.x - centerX;
                float localY = v.y - centerY;

                float rotatedX =
                    localX * cos(rotationAngle) -
                    localY * sin(rotationAngle);

                float rotatedY =
                    localX * sin(rotationAngle) +
                    localY * cos(rotationAngle);

                v.x = rotatedX + centerX;
                v.y = rotatedY + centerY;
            };

        rotateVertex(rotatedTriangle.v0);
        rotateVertex(rotatedTriangle.v1);
        rotateVertex(rotatedTriangle.v2);

        rasterizeTriangle(
            framebuffer,
            triangle,
            viewMode
        );

        rasterizeTriangle(
            framebuffer,
            rotatedTriangle,
            viewMode
        );

        glBindTexture(GL_TEXTURE_2D, textureID);

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGB,
            framebuffer.getWidth(),
            framebuffer.getHeight(),
            0,
            GL_RGB,
            GL_UNSIGNED_BYTE,
            framebuffer.getColorBufferData()
        );

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Rasterizer Debugger");

        ImGui::Text("Software Rasterizer Module");
        ImGui::Separator();

        ImGui::Text("Current Features:");
        ImGui::BulletText("Triangle rasterization");
        ImGui::BulletText("Edge-function inside test");
        ImGui::BulletText("Barycentric depth interpolation");
        ImGui::BulletText("Z-buffer depth testing");
        ImGui::BulletText("Wireframe visualization");
        ImGui::BulletText("2x2 MSAA anti-aliasing");
        ImGui::BulletText("Interactive triangle rotation");
        ImGui::BulletText("CPU framebuffer preview");

        ImGui::Separator();

        ImGui::Text("Depth Test:");
        ImGui::Text("Green triangle depth = 0.2");
        ImGui::Text("Red triangle depth = 0.5");
        ImGui::Text("Smaller depth value is closer.");

        ImGui::Separator();

        ImGui::Text("Debug Views:");
        ImGui::Text("1 - Final Color");
        ImGui::Text("2 - Depth Buffer");
        ImGui::Text("3 - Wireframe");
        ImGui::Text("4 - MSAA 2x2");

        ImGui::Image(
            (ImTextureID)(intptr_t)textureID,
            ImVec2(512, 512)
        );

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