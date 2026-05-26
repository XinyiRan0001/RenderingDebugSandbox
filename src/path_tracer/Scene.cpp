//
// Created by Göksu Güvendiren on 2019-05-14.
//

#include "Scene.hpp"


void Scene::buildBVH() {
    printf(" - Generating BVH...\n\n");
    this->bvh = new BVHAccel(objects, 1, BVHAccel::SplitMethod::NAIVE);
}

Intersection Scene::intersect(const Ray& ray) const
{
    if (useBVH)
    {
        return this->bvh->Intersect(ray);
    }

    Intersection closest;
    closest.distance = std::numeric_limits<float>::infinity();

    for (auto obj : objects)
    {
        Intersection hit = obj->getIntersection(ray);

        if (hit.happened && hit.distance < closest.distance)
        {
            closest = hit;
        }
    }

    return closest;
}

void Scene::sampleLight(Intersection &pos, float &pdf) const
{
    float emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
        }
    }
    float p = get_random_float() * emit_area_sum;
    emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
            if (p <= emit_area_sum){
                objects[k]->Sample(pos, pdf);
                break;
            }
        }
    }
}

bool Scene::trace(
        const Ray &ray,
        const std::vector<Object*> &objects,
        float &tNear, uint32_t &index, Object **hitObject)
{
    *hitObject = nullptr;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        float tNearK = kInfinity;
        uint32_t indexK;
        Vector2f uvK;
        if (objects[k]->intersect(ray, tNearK, indexK) && tNearK < tNear) {
            *hitObject = objects[k];
            tNear = tNearK;
            index = indexK;
        }
    }


    return (*hitObject != nullptr);
}

// Implementation of Path Tracing
Vector3f Scene::castRay(const Ray& ray, int depth) const
{
    Intersection inter = intersect(ray);

    if (!inter.happened)
        return Vector3f(0.0f);

    if (inter.m->hasEmission())
        return inter.m->getEmission();

    Vector3f hitPoint = inter.coords;
    Vector3f N = inter.normal;
    Vector3f wo = -ray.direction;

    Vector3f L_dir(0.0f);
    Vector3f L_indir(0.0f);

    // Direct lighting
    Intersection lightInter;
    float pdf_light = 0.0f;
    sampleLight(lightInter, pdf_light);

    Vector3f lightPos = lightInter.coords;
    Vector3f lightNormal = lightInter.normal;
    Vector3f emit = lightInter.emit;

    Vector3f lightDir = normalize(lightPos - hitPoint);
    float lightDistance2 = dotProduct(lightPos - hitPoint, lightPos - hitPoint);

    Ray shadowRay(hitPoint + N * 0.001f, lightDir);
    Intersection shadowInter = intersect(shadowRay);

    if (shadowInter.happened &&
        (shadowInter.coords - lightPos).norm() < 0.01f)
    {
        float cosTheta = std::max(0.0f, dotProduct(lightDir, N));
        float cosThetaLight = std::max(0.0f, dotProduct(-lightDir, lightNormal));

        L_dir = emit *
            inter.m->eval(wo, lightDir, N) *
            cosTheta *
            cosThetaLight /
            lightDistance2 /
            pdf_light;
    }

    // Indirect lighting
    if (get_random_float() < RussianRoulette)
    {
        Vector3f wi = inter.m->sample(wo, N).normalized();
        Ray nextRay(hitPoint + N * 0.001f, wi);
        Intersection nextInter = intersect(nextRay);

        if (nextInter.happened && !nextInter.m->hasEmission())
        {
            float pdf = inter.m->pdf(wo, wi, N);

            if (pdf > 0.0001f)
            {
                float cosTheta = std::max(0.0f, dotProduct(wi, N));

                L_indir =
                    castRay(nextRay, depth + 1) *
                    inter.m->eval(wo, wi, N) *
                    cosTheta /
                    pdf /
                    RussianRoulette;
            }
        }
    }

    return L_dir + L_indir;
}