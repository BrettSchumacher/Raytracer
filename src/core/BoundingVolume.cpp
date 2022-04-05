#include "BoundingVolume.h"

BoundingVolume::BoundingVolume(vector<shared_ptr<Shape>> shapes, int depth, int maxDepth, int idealShapes)
{
    this->shapes = shapes;
    this->depth = depth;
    this->maxDepth = maxDepth;
    this->idealShapes = idealShapes;
    CalculateBounds();
    if (depth < maxDepth)
    {
        ConstructSubVolumes();
    }
}

void BoundingVolume::CalculateBounds()
{
    WorldBounds bounds = GetShapesWorldBounds(shapes);
    minBounds = bounds.min;
    maxBounds = bounds.max;
}

bool BoundingVolume::Intersect(Ray ray, RayHit& hitInfo, vector<int>& ignoreList)
{
    RayHit tempHitInfo;

    // first check that ray hits the bounding box
    if (!IsPointInside(ray.origin) && IntersectBoundingBox(ray) < 0)
    {
        return false;
    }

    // first check collisions with shapes in this BV
    for (shared_ptr<Shape> shape : shapes)
    {
        if (count(ignoreList.begin(), ignoreList.end(), shape->id) != 0)
        {
            continue;
        }
        if (shape->Intersect(ray, tempHitInfo))
        {
            if (!hitInfo || tempHitInfo.t < hitInfo.t)
            {
                hitInfo = tempHitInfo;
                hitInfo.shapeIndex = shape->id;
            }
        }
    }

    // then check collisions with sub-BVs
    for (shared_ptr<BoundingVolume> subVolume : subVolumes)
    {
        if (subVolume->Intersect(ray, tempHitInfo, ignoreList))
        {
            if (!hitInfo || tempHitInfo.t < hitInfo.t)
            {
                hitInfo = tempHitInfo;
            }
        }
    }

    return hitInfo.hit;
}

double BoundingVolume::IntersectBoundingBox(Ray ray)
{
    Float t = -1;
    Float hit;
    Vector3 testPoint;
    // first check yz planes
    if (ray.direction.x != 0)
    {      
        hit = -1;  
        if (ray.origin.x < minBounds.x)
        {
            hit = (minBounds.x - ray.origin.x) / ray.direction.x;

        }
        else if (ray.origin.x > maxBounds.x)
        {
            hit = (maxBounds.x - ray.origin.x) / ray.direction.x;
        }
        if (hit > 0 && (t < 0 || hit < t))
        {
            testPoint = ray.origin + ray.direction * hit;
            if (testPoint.y >= minBounds.y && testPoint.y <= maxBounds.y &&
                testPoint.z >= minBounds.z && testPoint.z <= maxBounds.z)
            {
                t = hit;
            }
        }
    }

    // then xz planes
    if (ray.direction.y != 0)
    {
        hit = -1;
        if (ray.origin.y < minBounds.y)
        {
            hit = (minBounds.y - ray.origin.y) / ray.direction.y;
        }
        else if (ray.origin.y > maxBounds.y)
        {
            hit = (maxBounds.y - ray.origin.y) / ray.direction.y;
        }
        if (hit > 0 && (t < 0 || hit < t))
        {
            testPoint = ray.origin + ray.direction * hit;
            if (testPoint.x >= minBounds.x && testPoint.x <= maxBounds.x &&
                testPoint.z >= minBounds.z && testPoint.z <= maxBounds.z)
            {
                t = hit;
            }
        }
    }

    // then xy planes
    if (ray.direction.z != 0)
    {
        hit = -1;
        if (ray.origin.z < minBounds.z)
        {
            hit = (minBounds.z - ray.origin.z) / ray.direction.z;
        }
        else if (ray.origin.z > maxBounds.z)
        {
            hit = (maxBounds.z - ray.origin.z) / ray.direction.z;
        }
        if (hit > 0 && (t < 0 || hit < t))
        {
            testPoint = ray.origin + ray.direction * hit;
            if (testPoint.x >= minBounds.x && testPoint.x <= maxBounds.x &&
                testPoint.y >= minBounds.y && testPoint.y <= maxBounds.y)
            {
                t = hit;
            }
        }
    }

    return t;
}

bool BoundingVolume::IsPointInside(Vector3 point)
{
    return point.x >= minBounds.x && point.x <= maxBounds.x &&
           point.y >= minBounds.y && point.y <= maxBounds.y &&
           point.z >= minBounds.z && point.z <= maxBounds.z;
}

void BoundingVolume::ConstructSubVolumes()
{
    if (shapes.size() <= idealShapes)
    {
        return;
    }

    vector<pair<shared_ptr<Shape>, long>> orderedShapes; // vector to store ordered shapes based on morton code
    // the morton code will be calculate with the bottom left of the bounding box as the origin
    // also will use a scale factor so bunched up geometry can be better distributed
    Vector3 range = maxBounds - minBounds;
    Float scale = 10000 / max(range.x, max(range.y, range.z));
    for (int i = 0; i < shapes.size(); i++)
    {
        orderedShapes.push_back(make_pair(shapes[i], shapes[i]->GetWorldBounds().GetMortonCode(minBounds, scale)));
    }

    sort(orderedShapes.begin(), orderedShapes.end(), [](const pair<shared_ptr<Shape>, long>& a, const pair<shared_ptr<Shape>, long>& b) {
        return a.second < b.second;
    });

    vector<shared_ptr<Shape>> leftShapes;
    vector<shared_ptr<Shape>> rightShapes;

    int mid = shapes.size() / 2;
    for (int i = 0; i < mid; i++)
    {
        leftShapes.push_back(orderedShapes[i].first);
    }

    for (int i = mid; i < shapes.size(); i++)
    {
        rightShapes.push_back(orderedShapes[i].first);
    }

    subVolumes.push_back(make_shared<BoundingVolume>(leftShapes, depth + 1, maxDepth, idealShapes));
    subVolumes.push_back(make_shared<BoundingVolume>(rightShapes, depth + 1, maxDepth, idealShapes));
    shapes.clear();
}

WorldBounds BoundingVolume::GetShapesWorldBounds(vector<shared_ptr<Shape>> shapes)
{
    if (shapes.size() == 0)
    {
        return WorldBounds();
    }
    WorldBounds bounds = shapes[0]->GetWorldBounds();
    for (auto shape : shapes)
    {
        WorldBounds shapeBounds = shape->GetWorldBounds();
        bounds.min = Vector3::Min(bounds.min, shapeBounds.min);
        bounds.max = Vector3::Max(bounds.max, shapeBounds.max);
    }
    return bounds;
}