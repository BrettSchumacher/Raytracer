// I saw in pbrt that have an include file that handles all their includes and defines
// so they only have a couple include statements per regular file, might try that later
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>
#include <math.h>
#include <chrono>

#include "core/TxtReader.h"

using namespace std;

void getOutputFilename(int argc, char* argv[], string& outputFilename);

int main(int argc, char* argv[])
{
    Camera camera; // Camera handles ray generation and image creation
    Scene scene; // Scene handles the actual ray tracing and shape storage
    string outputFilename;
    auto start = chrono::high_resolution_clock::now();

    TxtReader txtReader;

    // first check if user provided a filename as only argument
    if (argc < 2 || argc > 3)
    {
        cout << "Usage: " << argv[0] << " <input_filename> [output_filename]\n";
        return 1;
    }

    // get output filename
    getOutputFilename(argc, argv, outputFilename);

    // create default material
    Material defaultMaterial;
    defaultMaterial.SetDiffuse(Vector3(0.8, 0.8, 0.8));
    defaultMaterial.SetSpecular(Vector3(1.0, 1.0, 1.0));
    defaultMaterial.SetK_A(0.4);
    defaultMaterial.SetK_D(0.8);
    defaultMaterial.SetK_S(0.4);
    defaultMaterial.SetSpecFalloff(64.0);

    scene.AddMaterial(defaultMaterial);

    // start by parsing the input file
    if (txtReader.parseInput(argv[1], scene, camera) != 0)
    {
        return 1;
    }

    // make sure the camera is set up correctly
    camera.SetDistToPlane(1);

    // camera.IsValid() will print type of error if it fails
    if (!camera.IsValid())
    {
        return 1;
    }

    // construct BVH
    if (scene.GetUseBVH())
    {
        cout << "Constructing BVH..." << endl;
        scene.InitializeBVH();
    }

    // now that we have a valid scene, we can render it
    Image image;
    cout << "Rendering image..." << endl;
    if (camera.RenderScene(scene, image) != 0)
    {
        return 1;
    }

    // write the image to a file
    cout << "Writing image to file..." << endl;
    if (image.SaveToFilePPM(outputFilename) != 0)
    {
        return 1;
    }
 
    // clean up
    scene.ClearShapes();

    auto end = chrono::high_resolution_clock::now();
    double elapsed = chrono::duration_cast<chrono::milliseconds>(end - start).count() / (double)1000;
    cout << "Render Complete" << endl;
    cout << "Total time: " << elapsed << "s" << endl;

    return 0;
}

void getOutputFilename(int argc, char* argv[], string& outputFilename)
{
    if (argc == 3)
    {
        outputFilename = argv[2];
    }
    else
    {
        outputFilename = argv[1];
    }

    // remove extension from output file name and add .ppm
    int extensionPos = outputFilename.find_last_of('.');
    if (extensionPos == string::npos)
    {
        outputFilename = outputFilename + ".ppm";
    }
    else
    {
        outputFilename = outputFilename.substr(0, extensionPos) + ".ppm";
    }
}

