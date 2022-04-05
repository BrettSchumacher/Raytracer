#include "Image.h"

Image::Image()
{
    width = 0;
    height = 0;
    pixels = nullptr;
}

Image::Image(int width, int height)
{
    this->width = width;
    this->height = height;
    pixels = new Vector3[width * height];
}

Image::~Image()
{
    if (pixels != nullptr)
    {
        delete[] pixels;
    }
}


void Image::SetDimensions(int width, int height)
{
    this->width = width;
    this->height = height;
    if (pixels != nullptr)
    {
        delete[] pixels;
    }
    pixels = new Vector3[width * height];
}

void Image::SetPixel(int x, int y, Vector3 color)
{
    if (y * width + x < width * height)
    {
        pixels[y * width + x] = color;
    }
}

Vector3 Image::GetPixel(int x, int y)
{
    if (y * width + x < width * height)
    {
        return pixels[y * width + x];
    }
    else
    {
        return Vector3(0, 0, 0);
    }
}

Vector3 Image::GetColorUV(Float u, Float v)
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
    Vector3 p1 = GetPixel(x1, y1);
    Vector3 p2 = GetPixel(x2, y1);
    Vector3 p3 = GetPixel(x1, y2);
    Vector3 p4 = GetPixel(x2, y2);

    // first lerp the top two and bottom two pixels
    Vector3 bottom = p1 * (1 - t) + p2 * t;
    Vector3 top = p3 * (1 - t) + p4 * t;

    // then lerp the top and bottom
    return top * s + bottom * (1 - s);
}

Vector3 Image::GetBumpUV(Float u, Float v)
{
    // first read in color
    Vector3 color = GetColorUV(u, v);

    // now convert color to bump
    Vector3 bump = Vector3(0, 0, 0);

    // map x,y from 0-1 to -1 to 1
    bump.x = (color.x - 0.5) * 2;
    bump.y = (color.y - 0.5) * 2;

    // z stays the same
    bump.z = color.z;

    return bump;
}

int Image::SaveToFilePPM(string fileName)
{
    string output;
    ostringstream oss(output);
    ofstream file;

    // write header
    oss << "P3\n" << width << " " << height << "\n255\n";

    // write pixels, make sure to flip y axis
    for (int y = height - 1; y >= 0; y--)
    {
        // unroll loop by 3 to write 3 pixels at a time
        int x;
        for (x = 0; x < width - 2; x+=3)
        {
            int ind = y * width + x;
            oss << pixels[ind].PrintRGB() << " ";
            oss << pixels[ind + 1].PrintRGB() << " ";
            oss << pixels[ind + 2].PrintRGB() << endl;
        }

        // write remaining pixels
        for (; x < width; x++)
        {
            int ind = y * width + x;
            oss << pixels[ind].PrintRGB() << " ";
        }
        oss << endl;
    }
    
    file.open(fileName.c_str());

    if (!file.is_open())
    {
        cout << "Error: Could not open file " << fileName << endl;
        return 1;
    }

    file << oss.str();

    file.close();
    return 0;
}

int Image::LoadFromFilePPM(string fileName, Image& image)
{
    ifstream file;
    string line, token;
    vector<string> args;
    istringstream iss;

    int width = -1, height = -1, max = -1;
    bool p3found = false, widthSet = false, heightSet = false, maxSet = false;
    file.open(fileName.c_str());

    if (!file.is_open())
    {
        cout << "Error: Could not open file " << fileName << endl;
        return 1;
    }

    // go through lines of file until we're past the header
    while (!p3found || !widthSet || !heightSet || !maxSet)
    {
        // first turn the line into a vector of strings ignoring comments
        if (!getline(file, line))
        {
            cout << "Error: File " << fileName << " is not a valid PPM file" << endl;
            return 1;
        }
        int comment = line.find_first_of("#");
        if (comment != string::npos)
        {
            line = line.substr(0, comment);
        }

        iss = istringstream(line);
        while (iss >> token)
        {
            args.push_back(token);
        }

        // now go through each token
        // first should be P3, then the width, height, max
        while (args.size() > 0)
        {
            if (!p3found)
            {
                if (args[0] == "P3")
                {
                    p3found = true;
                }
                else
                {
                    cout << "Error: File " << fileName << " is not a PPM P3 file" << endl;
                    return 1;
                }
            }
            else if (!widthSet)
            {
                width = atoi(args[0].c_str());
                widthSet = true;
            }
            else if (!heightSet)
            {
                height = atoi(args[0].c_str());
                heightSet = true;
            }
            else if (!maxSet)
            {
                max = atoi(args[0].c_str());
                maxSet = true;
            }
            else
            {
                cout << "Error: File " << fileName << " is not a PPM P3 file" << endl;
                return 1;
            }

            args.erase(args.begin());
        }
        
        args.clear();
    }

    // now validate the values from the header
    if (width <= 0 || height <= 0)
    {
        cout << "Error: Image " << fileName << " has invalid dimensions" << endl;
        return 1;
    }

    if (max < 0)
    {
        cout << "Error: Image " << fileName << " has invalid max value" << endl;
        return 1;
    }

    // read pixels
    image.SetDimensions(width, height);
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            Float r, g, b;
            if (!(file >> r >> g >> b))
            {
                cout << "Error: Image " << fileName << " has invalid pixel data" << endl;
                return 1;
            }
            int ind = y * width + x;
            image[ind] = Vector3(r / max, g / max, b / max);
        }
    }

    file.close();
    return 0;
}

int Image::LoadFromFilePPM(string fileName, shared_ptr<Image> image)
{
    ifstream file;
    string line, token;
    vector<string> args;
    istringstream iss;

    int width = -1, height = -1, max = -1;
    bool p3found = false, widthSet = false, heightSet = false, maxSet = false;
    file.open(fileName.c_str());

    if (!file.is_open())
    {
        cout << "Error: Could not open file " << fileName << endl;
        return 1;
    }

    // go through lines of file until we're past the header
    while (!p3found || !widthSet || !heightSet || !maxSet)
    {
        // first turn the line into a vector of strings ignoring comments
        if (!getline(file, line))
        {
            cout << "Error: File " << fileName << " is not a valid PPM file" << endl;
            return 1;
        }
        int comment = line.find_first_of("#");
        if (comment != string::npos)
        {
            line = line.substr(0, comment);
        }

        iss = istringstream(line);
        while (iss >> token)
        {
            args.push_back(token);
        }

        // now go through each token
        // first should be P3, then the width, height, max
        while (args.size() > 0)
        {
            if (!p3found)
            {
                if (args[0] == "P3")
                {
                    p3found = true;
                }
                else
                {
                    cout << "Error: File " << fileName << " is not a PPM P3 file" << endl;
                    return 1;
                }
            }
            else if (!widthSet)
            {
                width = atoi(args[0].c_str());
                widthSet = true;
            }
            else if (!heightSet)
            {
                height = atoi(args[0].c_str());
                heightSet = true;
            }
            else if (!maxSet)
            {
                max = atoi(args[0].c_str());
                maxSet = true;
            }
            else
            {
                cout << "Error: File " << fileName << " is not a PPM P3 file" << endl;
                return 1;
            }

            args.erase(args.begin());
        }
        
        args.clear();
    }

    // now validate the values from the header
    if (width <= 0 || height <= 0)
    {
        cout << "Error: Image " << fileName << " has invalid dimensions" << endl;
        return 1;
    }

    if (max <= 0)
    {
        cout << "Error: Image " << fileName << " has invalid max value" << endl;
        return 1;
    }

    // read pixels
    image->SetDimensions(width, height);
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int r, g, b;
            if (!(file >> r >> g >> b))
            {
                cout << "Error: Image " << fileName << " has invalid pixel data" << endl;
                return 1;
            }
            image->SetPixel(x, y, Vector3(r / (Float) max, g / (Float) max, b / (Float) max));
        }
    }

    file.close();
    return 0;
}

int Image::LoadFromFile(string filename, shared_ptr<Image> image)
{
    // load rbg image into pixels using stb_image library
    stbi_ldr_to_hdr_gamma(1.0f);
    float* data = stbi_loadf(filename.c_str(), &image->width, &image->height, nullptr, 3);
    if (!data)
    {
        cout << "Error: Could not load image " << filename << endl;
        return 1;
    }

    image->SetDimensions(image->width, image->height);

    // flip y axis
    int index = 0;
    for (int y = 0; y < image->height; y++) // (int y = image->height - 1; y >= 0; y--) // 
    {
        for (int x = 0; x < image->width; x++) // (int x = image->width - 1; x >= 0; x--) // 
        {
            image->SetPixel(x, y, Vector3(data[index], data[index + 1], data[index + 2]));
            index += 3;
        }
    }

    stbi_image_free(data);

    image->filepath = filename;
    return 0;
}

Vector3& Image::operator[](int ind)
{
    if (ind > 0 && ind < width * height)
    {
        return pixels[ind];
    }
    else
    {
        return pixels[0];
    }
}
