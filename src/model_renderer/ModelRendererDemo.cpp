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
#include "stb_image.h"

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

    int texWidth = 0;
    int texHeight = 0;
    int texChannels = 0;

    unsigned char* textureData = stbi_load(
        "assets/models/spot/spot_texture.png",
        &texWidth,
        &texHeight,
        &texChannels,
        3
    );

    if (!textureData)
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    else
    {
        std::cout << "Texture loaded: "
            << texWidth << "x" << texHeight
            << std::endl;
    }

    GLuint modelTextureID = 0;

    glGenTextures(1, &modelTextureID);
    glBindTexture(GL_TEXTURE_2D, modelTextureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        texWidth,
        texHeight,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        textureData
    );

    stbi_image_free(textureData);

    glEnable(GL_DEPTH_TEST);

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

    float lightX = 0.3f;
    float lightY = 0.7f;
    float lightZ = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
            renderMode = ModelRenderMode::Wireframe;

        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
            renderMode = ModelRenderMode::Normal;

        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
            renderMode = ModelRenderMode::Lambert;

        if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
            renderMode = ModelRenderMode::BlinnPhong;

        if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
            renderMode = ModelRenderMode::UV;

        if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
            renderMode = ModelRenderMode::Texture;

        if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
            renderMode = ModelRenderMode::TextureLinear;

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

        if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
            lightX -= 0.02f;

        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
            lightX += 0.02f;

        if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
            lightY += 0.02f;

        if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
            lightY -= 0.02f;

        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            modelScale -= 0.01f;
            if (modelScale < 0.1f)
                modelScale = 0.1f;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        

        glLoadIdentity();

        auto& mesh = loader.LoadedMeshes[0];

        struct ScreenVertex
        {
            float x;
            float y;
            float z;
        };

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

                float z2 =
                    -x * sin(rotationY) +
                    z1 * cos(rotationY);

                return ScreenVertex{
                    x2 * modelScale,
                    y1 * modelScale,
                    z2 * modelScale
                };
            };

        for (size_t i = 0; i < mesh.Indices.size(); i += 3)
        {
            auto v0 = mesh.Vertices[mesh.Indices[i]];
            auto v1 = mesh.Vertices[mesh.Indices[i + 1]];
            auto v2 = mesh.Vertices[mesh.Indices[i + 2]];

            auto p0 =
                transformVertex(
                    v0.Position.X,
                    v0.Position.Y,
                    v0.Position.Z
                );

            auto p1 =
                transformVertex(
                    v1.Position.X,
                    v1.Position.Y,
                    v1.Position.Z
                );

            auto p2 =
                transformVertex(
                    v2.Position.X,
                    v2.Position.Y,
                    v2.Position.Z
                );

            if (renderMode ==
                ModelRenderMode::Normal)
            {
                glBegin(GL_TRIANGLES);

                auto setNormalColor =
                    [&](const objl::Vertex& v)
                    {
                        float r =
                            v.Normal.X * 0.5f + 0.5f;

                        float g =
                            v.Normal.Y * 0.5f + 0.5f;

                        float b =
                            v.Normal.Z * 0.5f + 0.5f;

                        glColor3f(r, g, b);
                    };

                setNormalColor(v0);
                glVertex3f(
                    p0.x, p0.y, p0.z
                );

                setNormalColor(v1);
                glVertex3f(
                    p1.x, p1.y, p1.z
                );

                setNormalColor(v2);
                glVertex3f(
                    p2.x, p2.y, p2.z
                );

                glEnd();
            }
            else if (renderMode ==
                ModelRenderMode::Wireframe)
            {
                glBegin(GL_LINES);

                glColor3f(
                    1.0f,
                    1.0f,
                    1.0f
                );

                glVertex3f(
                    p0.x, p0.y, p0.z
                );

                glVertex3f(
                    p1.x, p1.y, p1.z
                );

                glVertex3f(
                    p2.x, p2.y, p2.z
                );

                glVertex3f(
                    p2.x, p2.y, p2.z
                );

                glVertex3f(
                    p2.x, p2.y, p2.z
                );

                glVertex3f(
                    p0.x, p0.y, p0.z
                );

                glEnd();
            }

            else if (renderMode == ModelRenderMode::Lambert)
            {
                glBegin(GL_TRIANGLES);

                auto setLambertColor =
                    [&](const objl::Vertex& v)
                    {
                        float length =
                            sqrt(lightX * lightX +
                                lightY * lightY +
                                lightZ * lightZ);

                        float lx = lightX / length;
                        float ly = lightY / length;
                        float lz = lightZ / length;

                        float brightness =
                            v.Normal.X * lx +
                            v.Normal.Y * ly +
                            v.Normal.Z * lz;

                        if (brightness < 0.0f)
                            brightness = 0.0f;

                        glColor3f(
                            brightness,
                            brightness,
                            brightness
                        );
                    };

                setLambertColor(v0);
                glVertex3f(p0.x, p0.y, p0.z);

                setLambertColor(v1);
                glVertex3f(p1.x, p1.y, p1.z);

                setLambertColor(v2);
                glVertex3f(p2.x, p2.y, p2.z);

                glEnd();
            }

            else if (renderMode == ModelRenderMode::BlinnPhong)
            {
                glBegin(GL_TRIANGLES);

                auto setBlinnPhongColor =
                    [&](const objl::Vertex& v)
                    {
                        float length =
                            sqrt(lightX * lightX +
                                lightY * lightY +
                                lightZ * lightZ);

                        float lx = lightX / length;
                        float ly = lightY / length;
                        float lz = lightZ / length;

                        float vx = 0.0f;
                        float vy = 0.0f;
                        float vz = 1.0f;

                        float hx = lx + vx;
                        float hy = ly + vy;
                        float hz = lz + vz;

                        float hLength =
                            sqrt(hx * hx + hy * hy + hz * hz);

                        hx /= hLength;
                        hy /= hLength;
                        hz /= hLength;

                        float diffuse =
                            v.Normal.X * lx +
                            v.Normal.Y * ly +
                            v.Normal.Z * lz;

                        if (diffuse < 0.0f)
                            diffuse = 0.0f;

                        float specular =
                            v.Normal.X * hx +
                            v.Normal.Y * hy +
                            v.Normal.Z * hz;

                        if (specular < 0.0f)
                            specular = 0.0f;

                        specular = pow(specular, 32.0f);

                        float intensity =
                            0.15f + diffuse * 0.65f + specular * 0.8f;

                        if (intensity > 1.0f)
                            intensity = 1.0f;

                        glColor3f(
                            intensity,
                            intensity,
                            intensity
                        );
                    };



                setBlinnPhongColor(v0);
                glVertex3f(p0.x, p0.y, p0.z);

                setBlinnPhongColor(v1);
                glVertex3f(p1.x, p1.y, p1.z);

                setBlinnPhongColor(v2);
                glVertex3f(p2.x, p2.y, p2.z);

                glEnd();
             }

            else if (renderMode == ModelRenderMode::UV)
            {
                glBegin(GL_TRIANGLES);

                auto setUVColor =
                    [&](const objl::Vertex& v)
                    {
                        float u = v.TextureCoordinate.X;
                        float vcoord = v.TextureCoordinate.Y;

                        glColor3f(
                            u,
                            vcoord,
                            0.2f
                        );
                    };

                setUVColor(v0);
                glVertex3f(p0.x, p0.y, p0.z);

                setUVColor(v1);
                glVertex3f(p1.x, p1.y, p1.z);

                setUVColor(v2);
                glVertex3f(p2.x, p2.y, p2.z);

                glEnd();
            }

            else if (renderMode == ModelRenderMode::Texture)
            {
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, modelTextureID);

                glTexParameteri(
                    GL_TEXTURE_2D,
                    GL_TEXTURE_MIN_FILTER,
                    GL_NEAREST
                );

                glTexParameteri(
                    GL_TEXTURE_2D,
                    GL_TEXTURE_MAG_FILTER,
                    GL_NEAREST
                );

                glBegin(GL_TRIANGLES);

                auto setTexturedBlinnPhongColor =
                    [&](const objl::Vertex& v)
                    {
                        float length =
                            sqrt(lightX * lightX +
                                lightY * lightY +
                                lightZ * lightZ);

                        float lx = lightX / length;
                        float ly = lightY / length;
                        float lz = lightZ / length;

                        float vx = 0.0f;
                        float vy = 0.0f;
                        float vz = 1.0f;

                        float hx = lx + vx;
                        float hy = ly + vy;
                        float hz = lz + vz;

                        float hLength =
                            sqrt(hx * hx + hy * hy + hz * hz);

                        hx /= hLength;
                        hy /= hLength;
                        hz /= hLength;

                        float diffuse =
                            v.Normal.X * lx +
                            v.Normal.Y * ly +
                            v.Normal.Z * lz;

                        if (diffuse < 0.0f)
                            diffuse = 0.0f;

                        float specular =
                            v.Normal.X * hx +
                            v.Normal.Y * hy +
                            v.Normal.Z * hz;

                        if (specular < 0.0f)
                            specular = 0.0f;

                        specular = pow(specular, 32.0f);

                        float intensity =
                            0.25f + diffuse * 0.65f + specular * 1.2f;

                        if (intensity > 1.0f)
                            intensity = 1.0f;

                        glColor3f(
                            intensity,
                            intensity,
                            intensity
                        );
                    };

                setTexturedBlinnPhongColor(v0);
                glTexCoord2f(v0.TextureCoordinate.X, 1.0f - v0.TextureCoordinate.Y);
                glVertex3f(p0.x, p0.y, p0.z);

                setTexturedBlinnPhongColor(v1);
                glTexCoord2f(v1.TextureCoordinate.X, 1.0f - v1.TextureCoordinate.Y);
                glVertex3f(p1.x, p1.y, p1.z);

                setTexturedBlinnPhongColor(v2);
                glTexCoord2f(v2.TextureCoordinate.X, 1.0f - v2.TextureCoordinate.Y);
                glVertex3f(p2.x, p2.y, p2.z);

                glEnd();

                glDisable(GL_TEXTURE_2D);
            }

            else if (renderMode == ModelRenderMode::TextureLinear)
            {
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, modelTextureID);

                glTexParameteri(
                    GL_TEXTURE_2D,
                    GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR
                );

                glTexParameteri(
                    GL_TEXTURE_2D,
                    GL_TEXTURE_MAG_FILTER,
                    GL_LINEAR
                );

                glBegin(GL_TRIANGLES);

                auto setTexturedBlinnPhongColor =
                    [&](const objl::Vertex& v)
                    {
                        float length =
                            sqrt(lightX * lightX +
                                lightY * lightY +
                                lightZ * lightZ);

                        float lx = lightX / length;
                        float ly = lightY / length;
                        float lz = lightZ / length;

                        float vx = 0.0f;
                        float vy = 0.0f;
                        float vz = 1.0f;

                        float hx = lx + vx;
                        float hy = ly + vy;
                        float hz = lz + vz;

                        float hLength =
                            sqrt(hx * hx + hy * hy + hz * hz);

                        hx /= hLength;
                        hy /= hLength;
                        hz /= hLength;

                        float diffuse =
                            v.Normal.X * lx +
                            v.Normal.Y * ly +
                            v.Normal.Z * lz;

                        if (diffuse < 0.0f)
                            diffuse = 0.0f;

                        float specular =
                            v.Normal.X * hx +
                            v.Normal.Y * hy +
                            v.Normal.Z * hz;

                        if (specular < 0.0f)
                            specular = 0.0f;

                        specular = pow(specular, 32.0f);

                        float intensity =
                            0.25f + diffuse * 0.65f + specular * 1.2f;

                        if (intensity > 1.0f)
                            intensity = 1.0f;

                        glColor3f(
                            intensity,
                            intensity,
                            intensity
                        );
                    };

                setTexturedBlinnPhongColor(v0);
                glTexCoord2f(v0.TextureCoordinate.X, 1.0f - v0.TextureCoordinate.Y);
                glVertex3f(p0.x, p0.y, p0.z);

                setTexturedBlinnPhongColor(v1);
                glTexCoord2f(v1.TextureCoordinate.X, 1.0f - v1.TextureCoordinate.Y);
                glVertex3f(p1.x, p1.y, p1.z);

                setTexturedBlinnPhongColor(v2);
                glTexCoord2f(v2.TextureCoordinate.X, 1.0f - v2.TextureCoordinate.Y);
                glVertex3f(p2.x, p2.y, p2.z);

                glEnd();

                glDisable(GL_TEXTURE_2D);
                }
        }
        ImGui::Begin("Model Renderer Debugger");
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
        ImGui::Text("1 - Wireframe");
        ImGui::Text("2 - Normal Visualization");
        ImGui::Text("3 - Lambert Lighting");
        ImGui::Text("4 - Blinn-Phong");
        ImGui::Text("5 - UV Visualization");
        ImGui::Text("6 - Texture Mapping");
        ImGui::Text("7 - Texture Linear");
        ImGui::Text("Render Mode: Wireframe");
        ImGui::Text("Model: spot_triangulated_good.obj");
        ImGui::Text("A / D - Rotate Y");
        ImGui::Text("W / S - Rotate X");
        ImGui::Text("Up / Down - Zoom");
        ImGui::Text("J / L - Light X");
        ImGui::Text("I / K - Light Y");
        ImGui::Text("Light Dir: %.2f %.2f %.2f", lightX, lightY, lightZ);
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