#ifndef BWIMAGE_H
#define BWIMAGE_H

#include "math/Vector3.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <memory>

class BWImage
{
    public:
        string filepath;

        BWImage();
        BWImage(int width, int height);
        ~BWImage();

        void SetDimensions(int width, int height);

        void SetPixel(int x, int y, Float value);
        Float GetPixel(int x, int y);
        Float GetColorUV(Float u, Float v);

        static int LoadFromFile(string fileName, shared_ptr<BWImage> image);

        float& operator[](int);

    private:
        int width;
        int height;
        float* pixels; // using float rather than Float bc that's what stb_image uses

};

#endif