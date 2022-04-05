#include "Scene.h"

using namespace std;

Scene::Scene()
{
    backgroundColor = Vector3(0, 0, 0);
    shapes = vector<shared_ptr<Shape>>();
    lights = vector<shared_ptr<Light>>();
}

Scene::~Scene()
{
    ClearShapes(); 
    ClearLights(); 
    ClearMaterials(); 
    if (useBVH)
    {
        delete rootBV;
    }
    textures.clear();
    bumpMaps.clear();
    specMaps.clear();
}

void Scene::AddShape(shared_ptr<Shape> shape)
{
    if (shape != NULL)
    {
        shape->id = shapes.size();
        shapes.push_back(shape);
    }
}

bool Scene::RemoveShape(shared_ptr<Shape> shape)
{
    for (int i = 0; i < shapes.size(); i++)
    {
        if (shapes[i] == shape)
        {
            shapes.erase(shapes.begin() + i);
            return true;
        }
    }
    return false;
}

bool Scene::RemoveShapeAt(int index)
{
    if (index >= 0 && index < shapes.size())
    {
        shapes.erase(shapes.begin() + index);
        return true;
    }
    return false;
}

void Scene::ClearShapes()
{
    shapes.clear();
}

bool Scene::ValidVerts(int v1, int v2, int v3)
{
    if (v1 > 0 && v1 <= verts.size() && v2 > 0 && v2 <= verts.size() && v3 > 0 && v3 <= verts.size())
    {
        return true;
    }
    return false;
}

bool Scene::ValidNorms(int n1, int n2, int n3)
{
    if (n1 > 0 && n1 <= norms.size() && n2 > 0 && n2 <= norms.size() && n3 > 0 && n3 <= norms.size())
    {
        return true;
    }
    return false;
}

bool Scene::ValidUVs(int u1, int u2, int u3)
{
    if (u1 > 0 && u1 <= uvs.size() && u2 > 0 && u2 <= uvs.size() && u3 > 0 && u3 <= uvs.size())
    {
        return true;
    }
    return false;
}

bool Scene::ValidUVs(int u1, int u2, int u3, int u4)
{
    if (ValidUVs(u1, u2, u3) && u4 > 0 && u4 <= uvs.size())
    {
        return true;
    }
    return false;
}

bool Scene::ValidVerts(int v1, int v2, int v3, int v4)
{
    if (ValidVerts(v1, v2, v3) && v4 > 0 && v4 <= verts.size())
    {
        return true;
    }
    return false;
}

bool Scene::ValidNorms(int n1, int n2, int n3, int n4)
{
    if (ValidNorms(n1, n2, n3) && n4 > 0 && n4 <= norms.size())
    {
        return true;
    }
    return false;
}

void Scene::AddLight(shared_ptr<Light> light)
{
    if (light != NULL)
    {
        lights.push_back(light);
    }
}

bool Scene::RemoveLight(shared_ptr<Light> light)
{
    for (int i = 0; i < lights.size(); i++)
    {
        if (lights[i] == light)
        {
            lights.erase(lights.begin() + i);
            return true;
        }
    }
    return false;
}

bool Scene::RemoveLightAt(int index)
{
    if (index >= 0 && index < lights.size())
    {
        lights.erase(lights.begin() + index);
        return true;
    }
    return false;
}

void Scene::ClearLights()
{
    lights.clear();
}

void Scene::AddTexture(shared_ptr<Image> texture)
{
    textures.push_back(texture);
    ApplyTexture(textures.size() - 1);
}

void Scene::ApplyTexture(int index)
{
    if (index < 0 || index >= textures.size())
    {
        return;
    }
    
    Material newMat(materials.back());
    newMat.SetTexture(index);
    materials.push_back(newMat);
}

void Scene::AddBumpMap(shared_ptr<Image> bumpMap)
{
    bumpMaps.push_back(bumpMap);
    ApplyBumpMap(bumpMaps.size() - 1);
}

void Scene::ApplyBumpMap(int index)
{
    if (index < 0 || index >= bumpMaps.size())
    {
        return;
    }

    Material newMat(materials.back());
    newMat.SetBumpMap(index);
    materials.push_back(newMat);
}

void Scene::AddSpecMap(shared_ptr<BWImage> specMap)
{
    specMaps.push_back(specMap);
    ApplySpecMap(specMaps.size() - 1);
}

void Scene::ApplySpecMap(int index)
{
    if (index < 0 || index >= specMaps.size())
    {
        return;
    }

    Material newMat(materials.back());
    newMat.SetSpecMap(index);
    materials.push_back(newMat);
}

void Scene::SetUnlit(bool unlit)
{
    this->unlit = unlit;
}

bool Scene::GetUnlit()
{
    return unlit;
}

void Scene::SetDepthcueing(Vector3 color, Float near, Float far, Float startAlpha, Float endAlpha)
{
    depthColor = color;
    minDepth = near;
    maxDepth = far;
    alphaMin = startAlpha;
    alphaMax = endAlpha;
    depthcueing = true;
}

void Scene::SetSoftShadows(int numSamples)
{
    shadowSamples = numSamples;
}

void Scene::SetUseBVH(bool useBVH)
{
    this->useBVH = useBVH;
}

bool Scene::GetUseBVH()
{
    return useBVH;
}

void Scene::SetBVHMaxDepth(int maxDepth)
{
    this->maxBVDepth = maxDepth;
}

int Scene::GetBVHMaxDepth()
{
    return maxBVDepth;
}

void Scene::SetBVHIdealShapesPerBV(int idealShapes)
{
    this->idealShapesPerBV = idealShapes;
}

int Scene::GetBVHIdealShapesPerBV()
{
    return idealShapesPerBV;
}

void Scene::SetHDRI(shared_ptr<Image> hdri)
{
    this->hdri = hdri;
    this->useHDRI = true;
}

shared_ptr<Image> Scene::GetHDRI()
{
    return hdri;
}

void Scene::AddMaterial(Material material)
{
    materials.push_back(material);
}

void Scene::ClearMaterials()
{
    materials.clear();
}

int Scene::GetNumMaterials()
{
    return materials.size();
}

// since TraceRay() goes directly into ShadeRay(), I made Intersect a separate
// function so I could find intersections without worrying about shading
bool Scene::Intersect(Ray ray, RayHit& rayInfo, vector<int>& ignoreList)
{
    RayHit bestHit;
    bestHit.t = INFINITY;
    bestHit.hit = false;

    RayHit hitInfo;
    if (useBVH)
    {   
        rootBV->Intersect(ray, hitInfo, ignoreList);
        if (hitInfo.hit)
        {
            bestHit = hitInfo;
        }
    }
    else
    {
        for (int i = 0; i < shapes.size(); i++)
        {
            if (count(ignoreList.begin(), ignoreList.end(), shapes[i]->id) != 0)
            {
                continue;
            }
            if (shapes[i]->Intersect(ray, hitInfo))
            {
                if (hitInfo < bestHit)
                {
                    bestHit = hitInfo;
                    bestHit.shapeIndex = shapes[i]->id;
                }
            }
        }
    }
    

    if (bestHit) // overrode the equality operator for RayHits
    {
        rayInfo = bestHit;
        return true;
    }
    
    return false;
}

Vector3 Scene::GetBackgroundColor()
{
    return backgroundColor;
}

void Scene::SetBackgroundColor(Vector3 color)
{
    backgroundColor = color;
}

int Scene::GetNumShapes()
{
    return shapes.size();
}

shared_ptr<Shape> Scene::GetShape(int index)
{
    return shapes[index];
}

// depth is currently ignored
// intersect function makes traceray a little short for now
Vector3 Scene::TraceRay(Ray ray, int depth, Float& dist)
{
    RayHit hit;
    vector<int> ignoreList;

    Intersect(ray, hit, ignoreList);

    dist = hit ? hit.t : -1;

    return ShadeRay(ray, hit, depth);
}

// for now shadeRay will just return the color of the material plus
// basic shading with ambient light and single directional light
Vector3 Scene::ShadeRay(Ray ray, RayHit hitInfo, int depth)
{
    if (!hitInfo)
    {
        return SampleHDRI(ray.direction);
    }

    // if no lights provided, render scene unlit
    if (unlit)
    {
        return materials[hitInfo.materialIndex].GetUnlit(hitInfo, textures, bumpMaps);
    }

    // now calculate lighting for each light
    Vector3 normal = materials[hitInfo.materialIndex].CalculateNormal(hitInfo, bumpMaps);
    Vector3 viewDir = (ray.origin - hitInfo.position).normalized();
    Vector3 reflect = 2.0 * Vector3::Project(viewDir, normal) - viewDir;
    reflect.Normalize();
    Vector3 diffuse = Vector3::zero, specular = Vector3::zero;
    for (int i = 0; i < lights.size(); i++)
    {
        GetColorFromLight(i, reflect, viewDir, normal, ray, hitInfo, diffuse, specular);
    }

    // add ambient light
    Vector3 ambient = materials[hitInfo.materialIndex].GetAmbient(hitInfo, textures, bumpMaps);

    if (depth <= 0)
    {
        // if depth is negative, don't do reflection/refraction
        return ambient + diffuse + specular;
    }

    // add reflection/refraction
    Vector3 fresnel = GetFresnelColor(ray, hitInfo, reflect, viewDir, normal, diffuse, depth);
    Vector3 col = ambient + specular + fresnel; // diffuse gets included in fresnel

    if (depthcueing)
    {
        ApplyDepthCueing(col, hitInfo);
    }

    // clamp each color component to [0, 1]
    col.x = col.x > 1 ? 1 : col.x;
    col.x = col.x < 0 ? 0 : col.x;

    col.y = col.y > 1 ? 1 : col.y;
    col.y = col.y < 0 ? 0 : col.y;

    col.z = col.z > 1 ? 1 : col.z;
    col.z = col.z < 0 ? 0 : col.z;

    return col;
}

// helper function for ShadeRay()
Vector3 Scene::GetColorFromLight(int lightInd, Vector3 reflect, Vector3 viewDir, Vector3 normal, Ray ray, RayHit hitInfo, Vector3& diffuse, Vector3& specular)
{
    vector<int> ignoreList;
    if (shapes[hitInfo.shapeIndex]->IgnoreSelfShadowing())
    {
        ignoreList.push_back(hitInfo.shapeIndex);
    }

    // first get relatvent info about light and the collision
    shared_ptr<Light> light = lights[lightInd];
    Vector3 point = hitInfo.position + hitInfo.normal * 0.01;

    Vector3 lightDir = -light->GetDirectionAt(point);
    Float dist = light->GetDistanceFrom(point);
    Vector3 lightCol = light->color * light->GetIntensityAt(point);

    // if color is black, don't bother shading
    if (lightCol.sqrMagnitude() < 0.0001)
    {
        return Vector3(0, 0, 0);
    }

    Ray shadowRay;
    RayHit shadowHit;

    // now determine shadow value
    Vector3 shadowCol = Vector3::zero;
    if (shadowSamples < 2) // if samples is set to 1 or less, just do hard shadows
    {
        shadowRay = Ray(point, lightDir);
        shadowCol += ShadowTrace(shadowRay, dist, ignoreList);
    }
    else
    {
        // otherwise, do soft shadows by offsetting the light source randomly and shooting out a bunch of rays
        // it might be less noisy if I did a uniform distribution of offsets in a grid around the light source
        // but I'll try that later
        for (int i = 0; i < shadowSamples; i++)
        {
            Vector3 lightOffset = Vector3((rand() / (Float) RAND_MAX) - 0.5, (rand() / (Float) RAND_MAX) - 0.5, (rand() / (Float) RAND_MAX) - 0.5);
            Vector3 lightPoint = lightDir * dist + 2.0 * lightOffset;
            shadowRay = Ray(point, lightPoint.normalized());
            shadowCol += ShadowTrace(shadowRay, dist, ignoreList);
        }
    }

    if (shadowCol.magnitude() < 0.0001)
    {
        return Vector3(0, 0, 0);
    }

    shadowCol /= (Float) shadowSamples;

    // now use phong illumination to calculate color
    // i'm using r*l instead of n*h for specular because i think it looks better
    // plus we'll need r for reflections/refractions later
    Float diffuseAmt = normal.dot(lightDir);
    Float specAmt = lightDir.dot(reflect);

    Vector3 tempDiff, tempSpec;
    materials[hitInfo.materialIndex].GetColorNoAmbient(hitInfo, textures, bumpMaps, specMaps, diffuseAmt, specAmt, lightCol, tempDiff, tempSpec);
    tempDiff *= shadowCol;
    tempSpec *= shadowCol;
    diffuse += tempDiff;
    specular += tempSpec;

    return tempDiff + tempSpec;
}

Vector3 Scene::GetFresnelColor(Ray ray, RayHit hitInfo, Vector3 reflect, Vector3 viewDir, Vector3 normal, Vector3 diffuse, int depth)
{
    if (viewDir.dot(normal) < 0)
    {
        normal = -normal;
    }

    // first find reflection color
    Ray reflRay = Ray(hitInfo.position + normal * 0.01, reflect);
    Float reflDist;
    Vector3 reflColor = TraceRay(reflRay, depth - 1, reflDist);

    // next calculate the fresnel coefficient
    Float eta_i, eta_t; // first get the indices of refraction
    if (hitInfo.inside)
    {
        // we are leaving the material
        eta_i = ray.iors.back();
        ray.iors.pop_back();
        if (ray.iors.size() == 0)
        {
            ray.iors.push_back(1.0);
        }
        eta_t = ray.iors.back();
    }
    else
    {
        // we are entering the material
        eta_i = ray.iors.back();
        eta_t = materials[hitInfo.materialIndex].GetIOR();
        ray.iors.push_back(eta_t);
    }
    Float f0 = pow((eta_i - eta_t) / (eta_i + eta_t), 2);
    Float cos_i = viewDir.dot(normal);
    Float cos_t2 = 1 - (eta_i * eta_i) / (eta_t * eta_t) * (1 - cos_i * cos_i);
    Float fr = f0 + (1 - f0) * pow(1 - cos_i, 5);
    fr *= materials[hitInfo.materialIndex].GetK_S();

    if (cos_t2 < 0)
    {
        // total internal reflection, only do reflection
        return reflColor;
    }

    // otherwise we now need to calculate the refraction color
    Float cos_t = sqrt(cos_t2);
    Vector3 refr = -normal * cos_t + eta_i / eta_t * (cos_i * normal - viewDir);
    Ray refrRay = Ray(hitInfo.position - normal * 0.01, refr);
    refrRay.iors = vector<Float>(ray.iors);
    Float refrDist;
    Vector3 refrColor = TraceRay(refrRay, depth - 1, refrDist);

    if (!hitInfo.inside)
    {
        // account for opacity of material
        Float alpha = materials[hitInfo.materialIndex].GetAlpha();
        Float opacity; // opacity of 1 means that material is opaque, 0 means that it is fully transparent
        if (refrDist < 0)
        {
            // if we did not hit anything, then opacity should be 0 unless alpha is 0 then 1
            opacity = alpha == 0 ? 1 : 0;
        }
        else
        {
            opacity = exp(-alpha * refrDist);
        }
        refrColor = opacity * refrColor + (1 - opacity) * diffuse;
    }

    return fr * reflColor + (1 - fr) * refrColor;
}


// ShadowTrace gets the shadow value for a given ray taking into account alpha transparency
Vector3 Scene::ShadowTrace(Ray ray, Float maxDist, vector<int>& ignoreList)
{
    RayHit hit, lastHit;
    Float totalDist = 0;
    Vector3 shadowCol = Vector3::one;
    while (totalDist < maxDist)
    {
        if (Intersect(ray, hit, ignoreList))
        {
            if (hit.inside)
            {
                Float alpha = materials[hit.materialIndex].GetAlpha();
                Float dist = min(maxDist - totalDist, hit.t);
                Float absorbtion = 1 - exp(-alpha * dist);
                shadowCol -= absorbtion * (Vector3::one - materials[hit.materialIndex].GetDiffuse());
                shadowCol.x = max(shadowCol.x, (Float)0);
                shadowCol.y = max(shadowCol.y, (Float)0);
                shadowCol.z = max(shadowCol.z, (Float)0);
            }
            
            // now exit if we have absorbed all light
            if (shadowCol.sqrMagnitude() < 0.001)
            {
                return shadowCol;
            }

            // also exit if we have shot past the max distance
            if (hit.t > maxDist - totalDist)
            {
                return shadowCol;
            }

            // otherwise, we need to continue tracing (without accounting for refraction)
            ray.origin = hit.position + ray.direction * 0.01;
            totalDist += hit.t;
            lastHit = hit;
        }
        else
        {
            // if we don't hit anything, then we just need to account for opacity of material we're currently in
            if (lastHit && !lastHit.inside)
            {
                Float alpha = materials[lastHit.materialIndex].GetAlpha();
                Float dist = maxDist - totalDist;
                Float absorbtion = exp(-alpha * dist);
                shadowCol -= absorbtion * (Vector3::one - materials[lastHit.materialIndex].GetDiffuse());
                shadowCol.x = max(shadowCol.x, (Float)0);
                shadowCol.y = max(shadowCol.y, (Float)0);
                shadowCol.z = max(shadowCol.z, (Float)0);
            }

            return shadowCol;
        }
    }
    return shadowCol;
}

// depthcueing function that takes in a color and applies the depth cueing
void Scene::ApplyDepthCueing(Vector3& col, RayHit &hitInfo)
{
    // first calculate lerp amount
    Float depth = hitInfo.t;
    Float lerpAmt = (depth - minDepth) / (maxDepth - minDepth);
    
    // clamp between 0 and 1
    lerpAmt = lerpAmt > 1 ? 1 : lerpAmt;
    lerpAmt = lerpAmt < 0 ? 0 : lerpAmt;

    // now lerp the color
    Float alpha = (alphaMax - alphaMin) * lerpAmt + alphaMin;
    col = col * (1 - alpha) + depthColor * alpha;
}

Vector3 Scene::SampleHDRI(Vector3 dir)
{
    if (!useHDRI)
    {
        return backgroundColor;
    }

    // convert to uv coordinates, with u = phi / 2pi, v = theta / pi
    Float u = atan2(dir.z, dir.x) * 0.5 * M_1_PI + 0.5;
    Float v = acos(dir.y) * M_1_PI;

    Vector3 col = hdri->GetColorUV(u, v);    
    // clamp each color component to [0, 1]
    col.x = col.x > 1 ? 1 : col.x;
    col.x = col.x < 0 ? 0 : col.x;

    col.y = col.y > 1 ? 1 : col.y;
    col.y = col.y < 0 ? 0 : col.y;

    col.z = col.z > 1 ? 1 : col.z;
    col.z = col.z < 0 ? 0 : col.z;

    return col;
}

void Scene::InitializeBVH()
{
    rootBV = new BoundingVolume(shapes, 0, maxBVDepth, idealShapesPerBV);
}



