#include "BWImage.h"

#define STB_IMAGE_IMPLEMENTATION
#include "ext/stb_image.h"

BWImage::BWImage()
{
    width = 0;
    height = 0;
    pixels = nullptr;
}

BWImage::BWImage(int width, int height)
{
    this->width = width;
    this->height = height;
    pixels = new float[width * height];
}

BWImage::~BWImage()
{
    if (pixels != nullptr)
    {
        delete[] pixels;
    }
}

void BWImage::SetDimensions(int width, int height)
{
    this->width = width;
    this->height = height;
    if (pixels != nullptr)
    {
        delete[] pixels;
    }
    pixels = new float[width * height];
}

void BWImage::SetPixel(int x, int y, Float value)
{
    if (y * width + x < width * height)
    {
        pixels[y * width + x] = value;
    }
}

Float BWImage::GetPixel(int x, int y)
{
    if (y * width + x < width * height)
    {
        return pixels[y * width + x];
    }
    else
    {
        return 0;
    }
}

Float BWImage::GetColorUV(Float u, Float v)
{
    if (u < 0)
    {
        u = 0;
    }
    else if (u > 1)
    {
        u = 1;
    }

    if (v < 0)
    {
        v = 0;
    }
    else if (v > 1)
    {
        v = 1;
    }

    // interpolate between 4 closest pixels to u and v
    int x1 = (int)(u * width);
    int x2 = x1 + 1;
    int y1 = (int)(v * height);
    int y2 = y1 + 1;

    // modulo into range of image
    x1 = x1 % width;
    x2 = x2 % width;
    y1 = y1 % height;
    y2 = y2 % height;

    Float t = u - (int)u; // lerp factor for u
    Float s = v - (int)v; // lerp factor for v

    // get color of 4 closest pixels
    Float p1 = GetPixel(x1, y1);
    Float p2 = GetPixel(x2, y1);
    Float p3 = GetPixel(x1, y2);
    Float p4 = GetPixel(x2, y2);

    // first lerp the top two and bottom two pixels
    Float bottom = p1 * (1 - t) + p2 * t;
    Float top = p3 * (1 - t) + p4 * t;

    // then lerp the top and bottom
    return top * s + bottom * (1 - s);
}

int BWImage::LoadFromFile(string fileName, shared_ptr<BWImage> image)
{
    // load image
    float* data = stbi_loadf(fileName.c_str(), &image->width, &image->height, nullptr, 1);
    if (data == nullptr)
    {
        return -1;
    }

    image->SetDimensions(image->width, image->height);

    // copy data into image
    // flip y axis
    int index = 0;
    for (int y = 0; y < image->height; y++) // (int y = image->height - 1; y >= 0; y--)
    {
        for (int x = 0; x < image->width; x++)
        {
            image->SetPixel(x, y, data[index]);
            index++;
        }
    }

    image->filepath = fileName;

    return 0;
}

float& BWImage::operator[](int index)
{
    return pixels[index];
}