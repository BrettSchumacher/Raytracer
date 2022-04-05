#ifndef INPUTREADER_H
#define INPUTREADER_H

#include "Camera.h"
#include "shapes/Sphere.h"
#include "shapes/Cylinder.h"
#include "shapes/Triangle.h"
#include "lights/DirectionalLight.h"
#include "lights/PointLight.h"
#include "lights/SpotLight.h"
#include "ext/json.h"
#include "math/Transform.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;
using json = nlohmann::json;

class InputReader
{
    public:
        InputReader() { }

        virtual int parseInput(string filename, Scene& scene, Camera& camera) = 0;
        virtual bool isValidFiletype(string filename) = 0;

        int loadGLTF(string filename, Scene& scene);

    private:
        int loadMesh(int indMesh, Transform transform, Scene& scene);
        int traverseNode(int node, Transform transform, Scene& scene);
        int getData(string filename);
        vector<float> getFloats(json accessor);
        vector<int> getIndices(json accessor);
        void getTextures(json material, Scene& scene);
        vector<Vector3> groupFloatsVec3(vector<float>& floats);
        vector<UV> groupFloatsUV(vector<float>& floats);
        
        string filename;
        json file;
        vector<char> data;
        vector<Vector3> translations;
        vector<Quaternion> rotations;
        vector<Vector3> scales;
        vector<Transform> transforms;

};


#endif