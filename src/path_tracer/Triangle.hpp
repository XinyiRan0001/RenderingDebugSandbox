#pragma once

#include "Intersection.hpp"
#include "Material.hpp"
#include "OBJ_Loader.hpp"
#include "Object.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <limits>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

inline bool rayTriangleIntersect(
    const Vector3f& v0,
    const Vector3f& v1,
    const Vector3f& v2,
    const Vector3f& orig,
    const Vector3f& dir,
    float& tnear,
    float& u,
    float& v)
{
    Vector3f edge1 = v1 - v0;
    Vector3f edge2 = v2 - v0;
    Vector3f pvec = crossProduct(dir, edge2);
    float det = dotProduct(edge1, pvec);

    if (std::fabs(det) < EPSILON)
        return false;

    float invDet = 1.0f / det;

    Vector3f tvec = orig - v0;
    u = dotProduct(tvec, pvec) * invDet;

    if (u < 0.0f || u > 1.0f)
        return false;

    Vector3f qvec = crossProduct(tvec, edge1);
    v = dotProduct(dir, qvec) * invDet;

    if (v < 0.0f || u + v > 1.0f)
        return false;

    tnear = dotProduct(edge2, qvec) * invDet;

    return tnear > EPSILON;
}

class Triangle : public Object
{
public:
    Vector3f v0, v1, v2;
    Vector3f e1, e2;
    Vector3f normal;
    float area = 0.0f;
    Material* m = nullptr;

    Triangle(Vector3f _v0, Vector3f _v1, Vector3f _v2, Material* _m = nullptr)
        : v0(_v0), v1(_v1), v2(_v2), m(_m)
    {
        e1 = v1 - v0;
        e2 = v2 - v0;
        normal = normalize(crossProduct(e1, e2));
        area = crossProduct(e1, e2).norm() * 0.5f;
    }

    bool intersect(const Ray& ray) override
    {
        return true;
    }

    bool intersect(const Ray& ray, float& tnear, uint32_t& index) const override
    {
        float u = 0.0f;
        float v = 0.0f;
        return rayTriangleIntersect(v0, v1, v2, ray.origin, ray.direction, tnear, u, v);
    }

    Bounds3 getBounds() override
    {
        return Union(Bounds3(v0, v1), v2);
    }

    Intersection getIntersection(Ray ray) override
    {
        Intersection inter;

        float t = 0.0f;
        float u = 0.0f;
        float v = 0.0f;

        if (!rayTriangleIntersect(v0, v1, v2, ray.origin, ray.direction, t, u, v))
        {
            return inter;
        }

        inter.happened = true;
        inter.coords = ray.origin + ray.direction * t;
        inter.normal = dotProduct(ray.direction, normal) < 0 ? normal : -normal;
        inter.distance = t;
        inter.obj = this;
        inter.m = m;

        return inter;
    }

    void getSurfaceProperties(
        const Vector3f& P,
        const Vector3f& I,
        const uint32_t& index,
        const Vector2f& uv,
        Vector3f& N,
        Vector2f& st) const override
    {
        N = normal;
        st = Vector2f(0.0f, 0.0f);
    }

    Vector3f evalDiffuseColor(const Vector2f&) const override
    {
        if (m)
            return m->Kd;

        return Vector3f(0.5f, 0.5f, 0.5f);
    }

    void Sample(Intersection& pos, float& pdf) override
    {
        float x = std::sqrt(get_random_float());
        float y = get_random_float();

        pos.coords =
            v0 * (1.0f - x)
            + v1 * (x * (1.0f - y))
            + v2 * (x * y);

        pos.normal = normal;
        pos.emit = m ? m->getEmission() : Vector3f(0.0f);
        pos.m = m;
        pos.obj = this;

        pdf = 1.0f / area;
    }

    float getArea() override
    {
        return area;
    }

    bool hasEmit() override
    {
        return m && m->hasEmission();
    }
};

class MeshTriangle : public Object
{
public:
    Bounds3 bounding_box;
    std::vector<Triangle> triangles;
    float area = 0.0f;
    Material* m = nullptr;

    MeshTriangle(const std::string& filename, Material* mt = new Material())
    {
        m = mt;
        area = 0.0f;

        std::cout << "[MeshTriangle] Loading OBJ manually: "
            << filename
            << std::endl;

        std::ifstream file(filename);

        if (!file.is_open())
        {
            std::cerr << "[MeshTriangle] Failed to open OBJ: "
                << filename
                << std::endl;
            return;
        }

        std::vector<Vector3f> positions;
        std::string line;

        Vector3f min_vert(
            std::numeric_limits<float>::infinity(),
            std::numeric_limits<float>::infinity(),
            std::numeric_limits<float>::infinity()
        );

        Vector3f max_vert(
            -std::numeric_limits<float>::infinity(),
            -std::numeric_limits<float>::infinity(),
            -std::numeric_limits<float>::infinity()
        );

        auto parseVertexIndex = [](const std::string& token) -> int
            {
                std::stringstream ss(token);
                std::string indexString;
                std::getline(ss, indexString, '/');
                return std::stoi(indexString) - 1;
            };

        while (std::getline(file, line))
        {
            if (line.empty())
                continue;

            std::stringstream ss(line);
            std::string type;
            ss >> type;

            if (type == "v")
            {
                float x, y, z;
                ss >> x >> y >> z;

                positions.emplace_back(x, y, z);
            }
            else if (type == "f")
            {
                std::vector<int> faceIndices;
                std::string token;

                while (ss >> token)
                {
                    int index = parseVertexIndex(token);

                    if (index >= 0 && index < positions.size())
                    {
                        faceIndices.push_back(index);
                    }
                }

                if (faceIndices.size() < 3)
                    continue;

                for (int i = 1; i + 1 < faceIndices.size(); ++i)
                {
                    Vector3f p0 = positions[faceIndices[0]];
                    Vector3f p1 = positions[faceIndices[i]];
                    Vector3f p2 = positions[faceIndices[i + 1]];

                    min_vert = Vector3f(
                        std::min(min_vert.x, std::min(p0.x, std::min(p1.x, p2.x))),
                        std::min(min_vert.y, std::min(p0.y, std::min(p1.y, p2.y))),
                        std::min(min_vert.z, std::min(p0.z, std::min(p1.z, p2.z)))
                    );

                    max_vert = Vector3f(
                        std::max(max_vert.x, std::max(p0.x, std::max(p1.x, p2.x))),
                        std::max(max_vert.y, std::max(p0.y, std::max(p1.y, p2.y))),
                        std::max(max_vert.z, std::max(p0.z, std::max(p1.z, p2.z)))
                    );

                    triangles.emplace_back(p0, p1, p2, mt);
                }
            }
        }

        bounding_box = Bounds3(min_vert, max_vert);

        for (auto& tri : triangles)
        {
            area += tri.area;
        }

        std::cout << "[MeshTriangle] Triangles created: "
            << triangles.size()
            << ", Area: "
            << area
            << std::endl;
    }

    bool intersect(const Ray& ray) override
    {
        return true;
    }

    bool intersect(const Ray& ray, float& tnear, uint32_t& index) const override
    {
        bool hitAnything = false;

        for (uint32_t i = 0; i < triangles.size(); ++i)
        {
            float t = tnear;
            uint32_t tempIndex = i;

            if (triangles[i].intersect(ray, t, tempIndex) && t < tnear)
            {
                tnear = t;
                index = i;
                hitAnything = true;
            }
        }

        return hitAnything;
    }

    Bounds3 getBounds() override
    {
        return bounding_box;
    }

    Intersection getIntersection(Ray ray) override
    {
        Intersection closest;
        closest.distance = std::numeric_limits<float>::infinity();

        for (auto& tri : triangles)
        {
            Intersection hit = tri.getIntersection(ray);

            if (hit.happened && hit.distance < closest.distance)
            {
                closest = hit;
            }
        }

        if (closest.happened)
        {
            closest.obj = this;
            closest.m = m;
        }

        return closest;
    }

    void getSurfaceProperties(
        const Vector3f& P,
        const Vector3f& I,
        const uint32_t& index,
        const Vector2f& uv,
        Vector3f& N,
        Vector2f& st) const override
    {
        N = Vector3f(0.0f, 1.0f, 0.0f);
        st = Vector2f(0.0f, 0.0f);
    }

    Vector3f evalDiffuseColor(const Vector2f&) const override
    {
        if (m)
            return m->Kd;

        return Vector3f(0.5f, 0.5f, 0.5f);
    }

    void Sample(Intersection& pos, float& pdf) override
    {
        if (triangles.empty() || area <= 0.0f)
        {
            pdf = 0.0f;
            return;
        }

        float p = get_random_float() * area;
        float accumulated = 0.0f;

        for (auto& tri : triangles)
        {
            accumulated += tri.area;

            if (p <= accumulated)
            {
                tri.Sample(pos, pdf);
                pdf *= tri.area / area;

                pos.emit = m ? m->getEmission() : Vector3f(0.0f);
                pos.m = m;
                pos.obj = this;
                return;
            }
        }

        triangles.back().Sample(pos, pdf);
        pdf *= triangles.back().area / area;

        pos.emit = m ? m->getEmission() : Vector3f(0.0f);
        pos.m = m;
        pos.obj = this;
    }

    float getArea() override
    {
        return area;
    }

    bool hasEmit() override
    {
        return m && m->hasEmission();
    }
};