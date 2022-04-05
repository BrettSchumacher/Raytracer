#include "Shape.h"

Shape::Shape()
{
    materialIndex = 0;
}

long WorldBounds::GetMortonCode(Vector3 offset, Float scaleFac)
{
    int x = (centroid.x - offset.x) * scaleFac;
    int y = (centroid.y - offset.y) * scaleFac;
    int z = (centroid.z - offset.z) * scaleFac;
    long mortonCode = 0;
    int bits = sizeof(long) * 8;
    
    // interleave bits of x, y, and z
    for (int i = 0; i < bits; i += 3)
    {
        mortonCode |= (x & 1) << i;
        mortonCode |= (y & 1) << (i + 1);
        mortonCode |= (z & 1) << (i + 2);
        x >>= 1;
        y >>= 1;
        z >>= 1;
    }

    return mortonCode;
}