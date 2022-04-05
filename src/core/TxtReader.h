#ifndef TXTREADER_H
#define TXTREADER_H

#include "InputReader.h"

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <map>
#include <ext/json.h> // thank you nlohmann

using namespace std;

// class to handle reading in txt input files for the program
class TxtReader : public InputReader
{
    public:
        TxtReader() {}
        int parseInput(string filename, Scene& scene, Camera& camera);
        bool isValidFiletype(string filename);

    private:
        int parseFace(const char* line, int matID, Scene& scene, int linenum);
        int loadOBJ(string filename, Scene& scene, Camera& camera);
        void fillArgs(string line, vector<string>& args);

        map<string, int> matMap;
};

#endif