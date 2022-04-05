#ifndef BOUNDING_VOLUME_H
#define BOUNDING_VOLUME_H

#include "shapes/Shape.h"
#include <vector>
#include <memory>
#include <algorithm>
#include <math.h>

class BoundingVolume
{
    public:
        BoundingVolume(vector<shared_ptr<Shape>> shapes, int depth, int maxDepth, int idealShapes);

        bool Intersect(Ray ray, RayHit& hitInfo, vector<int>& ignoreList);
        double IntersectBoundingBox(Ray ray);
        bool IsPointInside(Vector3 point);

    private:
        vector<shared_ptr<Shape>> shapes;
        vector<shared_ptr<BoundingVolume>> subVolumes;

        Vector3 minBounds;
        Vector3 maxBounds;
        int depth;
        int maxDepth;
        int idealShapes;

        void CalculateBounds();
        void ConstructSubVolumes();
        static WorldBounds GetShapesWorldBounds(vector<shared_ptr<Shape>> shapes);
};

#endif