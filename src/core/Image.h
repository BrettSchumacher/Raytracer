#ifndef IMAGE_H
#define IMAGE_H

#include "math/Vector3.h"
#include "ext/stb_image.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <memory>

using namespace std;

class Image
{
    public:
        string filepath;

        Image();
        Image(int width, int height);
        ~Image();

        void SetDimensions(int width, int height);

        void SetPixel(int x, int y, Vector3 color);
        Vector3 GetPixel(int x, int y);
        Vector3 GetColorUV(Float u, Float v);
        Vector3 GetBumpUV(Float u, Float v);

        int SaveToFilePPM(string fileName);

        static int LoadFromFilePPM(string fileName, Image& image);
        static int LoadFromFilePPM(string fileName, shared_ptr<Image> image);
        static int LoadFromFile(string fileName, shared_ptr<Image> image); // for jpg and png using stb_image

        Vector3& operator[](int);

    private:
        int width;
        int height;
        Vector3* pixels;

};

#endif