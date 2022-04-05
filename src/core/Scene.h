#ifndef SCENE_H
#define SCENE_H

#include "math/Vector3.h"
#include "shapes/Shape.h"
#include "lights/Light.h"
#include "Material.h"
#include "BoundingVolume.h"
#include "Image.h"

#include <vector>
#include <math.h>
#include <memory>
#include <algorithm>

using namespace std;

class Scene
{
    public:
        vector<Vector3> verts;
        vector<Vector3> norms;
        vector<UV> uvs;

        Scene();
        ~Scene();

        void AddShape(shared_ptr<Shape> shape);
        bool RemoveShape(shared_ptr<Shape> shape);
        bool RemoveShapeAt(int index);
        void ClearShapes();

        bool ValidVerts(int v1, int v2, int v3);
        bool ValidNorms(int n1, int n2, int n3);
        bool ValidUVs(int u1, int u2, int u3);
        bool ValidVerts(int v1, int v2, int v3, int v4);
        bool ValidNorms(int n1, int n2, int n3, int n4);
        bool ValidUVs(int u1, int u2, int u3, int u4);

        void AddLight(shared_ptr<Light> light);
        bool RemoveLight(shared_ptr<Light> light);
        bool RemoveLightAt(int index);
        void ClearLights();

        void AddTexture(shared_ptr<Image> texture);
        int GetNumTextures() { return textures.size(); }
        shared_ptr<Image> GetTexture(int index) { return textures[index]; }
        void ApplyTexture(int index);

        void AddBumpMap(shared_ptr<Image> bumpMap);
        int GetNumBumpMaps() { return bumpMaps.size(); }
        shared_ptr<Image> GetBumpMap(int index) { return bumpMaps[index]; }
        void ApplyBumpMap(int index);

        void AddSpecMap(shared_ptr<BWImage> specMap);
        int GetNumSpecMaps() { return specMaps.size(); }
        shared_ptr<BWImage> GetSpecMap(int index) { return specMaps[index]; }
        void ApplySpecMap(int index);

        void SetUnlit(bool unlit);
        bool GetUnlit();
        void SetDepthcueing(Vector3 color, Float near, Float far, Float startAlpha, Float endAlpha);
        void SetSoftShadows(int numSamples);
        void SetUseBVH(bool useBVH);
        bool GetUseBVH();
        void SetBVHMaxDepth(int maxDepth);
        int GetBVHMaxDepth();
        void SetBVHIdealShapesPerBV(int idealShapesPerBV);
        int GetBVHIdealShapesPerBV();
        void InitializeBVH();
        void SetHDRI(shared_ptr<Image> hdri);
        shared_ptr<Image> GetHDRI();

        void AddMaterial(Material material);
        void ClearMaterials();
        int GetNumMaterials();

        bool Intersect(Ray ray, RayHit& hitInfo, vector<int>& ignoreList);

        Vector3 GetBackgroundColor();
        void SetBackgroundColor(Vector3 color);

        int GetNumShapes();
        shared_ptr<Shape> GetShape(int index);

        Vector3 TraceRay(Ray ray, int depth, Float& dist);
        Vector3 ShadeRay(Ray ray, RayHit hitInfo, int depth);

    private:
        vector<shared_ptr<Shape>> shapes;
        vector<shared_ptr<Light>> lights;
        vector<Material> materials;
        vector<shared_ptr<Image>> textures;
        vector<shared_ptr<Image>> bumpMaps;
        vector<shared_ptr<BWImage>> specMaps;
        shared_ptr<Image> hdri;

        BoundingVolume *rootBV;
        Vector3 backgroundColor;
        bool unlit = false;
        bool depthcueing = false;
        bool useBVH = false;
        bool useHDRI = false;
        Vector3 depthColor = Vector3(0, 0, 0);
        Float minDepth, maxDepth, alphaMin, alphaMax;
        int shadowSamples = 1;
        int maxBVDepth = 5;
        int idealShapesPerBV = 4;

        Vector3 GetColorFromLight(int lightInd, Vector3 reflect, Vector3 viewDir, Vector3 normal, Ray ray, RayHit hitInfo, Vector3& diffuse, Vector3& specular);
        Vector3 GetFresnelColor(Ray ray, RayHit hitInfo, Vector3 reflct, Vector3 viewDir, Vector3 normal, Vector3 diffuse, int depth);
        void ApplyDepthCueing(Vector3 &color, RayHit &hitInfo);
        Vector3 SampleHDRI(Vector3 dir);
        Vector3 ShadowTrace(Ray ray, Float maxDist, vector<int>& ignoreList);
};

#endif