#include "TxtReader.h"

// f command can be of form f <v1> <v2> <v3>, f <v1>/<vt1> <v2>/<vt2> <v3>/<vt3>,
// f <v1>//<vn1> <v2>//<vn2> <v3>//<vn3>, or f <v1>/<vt1>/<vn1> <v2>/<vt2>/<vn2> <v3>/<vt3>/<vn3>
int TxtReader::parseFace(const char* line, int matID, Scene& scene, int linenum)
{
    int v1, v2, v3, vt1, vt2, vt3, vn1, vn2, vn3;
    shared_ptr<Shape> tri;

    if (sscanf(line, "f %d %d %d", &v1, &v2, &v3) == 3)
    {
        // f <v1> <v2> <v3>
        if (!scene.ValidVerts(v1, v2, v3))
        {
            cout << "Error: Invalid vertex index on line " << linenum << endl;
            return 1;
        }
        tri = make_shared<Triangle>(scene.verts[v1-1], scene.verts[v2-1], scene.verts[v3-1], matID);
    }
    else if (sscanf(line, "f %d/%d %d/%d %d/%d", &v1, &vt1, &v2, &vt2, &v3, &vt3) == 6)
    {
        // f <v1>/<vt1> <v2>/<vt2> <v3>/<vt3>
        if (!scene.ValidVerts(v1, v2, v3) || !scene.ValidUVs(vt1, vt2, vt3))
        {
            cout << "Error: Invalid vertex/uv index on line " << linenum << endl;
            return 1;
        }

        tri = make_shared<Triangle>(scene.verts[v1-1], scene.verts[v2-1], scene.verts[v3-1],
                                    scene.uvs[vt1-1], scene.uvs[vt2-1], scene.uvs[vt3-1], matID);

    }
    else if (sscanf(line, "f %d//%d %d//%d %d//%d", &v1, &vn1, &v2, &vn2, &v3, &vn3) == 6)
    {
        // f <v1>//<vn1> <v2>//<vn2> <v3>//<vn3>
        if (!scene.ValidVerts(v1, v2, v3) || !scene.ValidNorms(vn1, vn2, vn3))
        {
            cout << "Error: Invalid vertex/normal index on line " << linenum << endl;
            return 1;
        }
        tri = make_shared<Triangle>(scene.verts[v1-1], scene.verts[v2-1], scene.verts[v3-1],
                                    scene.norms[vn1-1], scene.norms[vn2-1], scene.norms[vn3-1], matID);
    }
    else if (sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d", &v1, &vt1, &vn1, &v2, &vt2, &vn2, &v3, &vt3, &vn3) == 9)
    {
        // f <v1>/<vt1>/<vn1> <v2>/<vt2>/<vn2> <v3>/<vt3>/<vn3>
        if (!scene.ValidVerts(v1, v2, v3) || !scene.ValidUVs(vt1, vt2, vt3) || !scene.ValidNorms(vn1, vn2, vn3))
        {
            cout << "Error: Invalid vertex/uv/normal index on line " << linenum << endl;
            return 1;
        }
        tri = make_shared<Triangle>(scene.verts[v1-1], scene.verts[v2-1], scene.verts[v3-1],
                                    scene.norms[vn1-1], scene.norms[vn2-1], scene.norms[vn3-1],
                                    scene.uvs[vt1-1], scene.uvs[vt2-1], scene.uvs[vt3-1], matID);
    }
    else
    {
        cout << "Error: Could not parse face on line " << linenum << endl;
        return 1;
    }

    scene.AddShape(tri);
    return 0;
}

// long ParseInput function
int TxtReader::parseInput(string filename, Scene& scene, Camera& camera)
{
    // now parse the file
    ifstream inputFile(filename);
    if (!inputFile.is_open())
    {
        cout << "Error: Could not open file " << filename << endl;
        return 1;
    }

    string line, command, endCheck, token;
    shared_ptr<Shape> shape;
    shared_ptr<Light> light;
    Float x, y, z, w, r, g, b, h, dx, dy, dz, c1, c2, c3, ks, kd, ka, n, alpha, ior, theta_in, theta_out;
    int line_num = 0;
    vector<string> args;
    istringstream iss;
    while (getline(inputFile, line))
    {
        line_num++;
        fillArgs(line, args);

        if (args.size() == 0)
        {
            continue;
        }

        command = args[0];
        args.erase(args.begin());

        // now check the command
        if (command == "imsize")
        {
            int width, height;
            if (args.size() != 2)
            {
                cout << "ERROR on line " << line_num << ": Improper imsize usage: imsize <width> <height>\n";
                return 1;
            }

            width = stoi(args[0]);
            height = stoi(args[1]);

            camera.SetAspectRatio(width, height);
        }
        else if (command == "eye")
        {
            if (args.size() != 3)
            {
                cout << "ERROR on line " << line_num << ": Improper viewdir usage: viewdir <x> <y> <z>\n";
                return 1;
            }

            x = stof(args[0]);
            y = stof(args[1]);
            z = stof(args[2]);

            camera.SetPosition(Vector3(x, y, z));
        }
        else if (command == "viewdir")
        {
            if (args.size() != 3)
            {
                cout << "ERROR on line " << line_num << ": Improper viewdir usage: viewdir <x> <y> <z>\n";
                return 1;
            }

            x = stof(args[0]);
            y = stof(args[1]);
            z = stof(args[2]);

            camera.SetForward(Vector3(x, y, z));
        }
        else if (command == "updir")
        {
            if (args.size() != 3)
            {
                cout << "ERROR on line " << line_num << ": Improper updir usage: updir <x> <y> <z>\n";
                return 1;
            }

            x = stof(args[0]);
            y = stof(args[1]);
            z = stof(args[2]);

            camera.SetUp(Vector3(x, y, z));
        }
        else if (command == "vfov")
        {
            if (args.size() != 1)
            {
                cout << "ERROR on line " << line_num << ": Improper vfov usage: vfov <degrees>\n";
                return 1;
            }

            x = stof(args[0]);

            camera.SetVFOV(x);
        }
        else if (command == "bkgcolor")
        {
            if (args.size() != 3)
            {
                cout << "ERROR on line " << line_num << ": Improper bkgcolor usage: bkgcolor <r> <g> <b>\n";
                return 1;
            }

            r = stof(args[0]);
            g = stof(args[1]);
            b = stof(args[2]);

            scene.SetBackgroundColor(Vector3(r, g, b));
        }
        else if (command == "mtlcolor")
        {
            if (args.size() != 12)
            {
                cout << "ERROR on line " << line_num << ": Improper material usage: material <diff_r> <diff_g> <diff_b> <spec_r> <spec_g> " <<
                                                           "<spec_b> <ambient> <diffuse> <specular> <spec_falloff> <alpha> <ior>\n";
                return 1;
            }

            x = stof(args[0]); y = stof(args[1]); z = stof(args[2]);
            dx = stof(args[3]); dy = stof(args[4]); dz = stof(args[5]);
            ka = stof(args[6]); kd = stof(args[7]); ks = stof(args[8]);
            n = stof(args[9]); alpha = stof(args[10]); ior = stof(args[11]);

            Material mat = Material(Vector3(x, y, z), Vector3(dx, dy, dz), ka, kd, ks, n, alpha, ior);

            scene.AddMaterial(mat);
        }
        else if (command == "sphere")
        {
            if (args.size() != 4)
            {
                cout << "ERROR on line " << line_num << ": Improper sphere usage: sphere <x> <y> <z> <radius>\n";
                return 1;
            }

            x = stof(args[0]); y = stof(args[1]); z = stof(args[2]);
            r = stof(args[3]);

            shape = make_shared<Sphere>(Vector3(x, y, z), scene.GetNumMaterials() - 1, r);
            scene.AddShape(shape);
        }
        else if (command == "cylinder")
        {
            if (args.size() != 8)
            {
                cout << "ERROR on line " << line_num << ": Improper cylinder usage: cylinder <x> <y> <z> <dx> <dy> <dz> <radius> <height> \n";
                return 1;
            }

            x = stof(args[0]); y = stof(args[1]); z = stof(args[2]);
            dx = stof(args[3]); dy = stof(args[4]); dz = stof(args[5]);
            r = stof(args[6]); h = stof(args[7]);

            // user is passing in centered (x,y,z), but cylinder takes bottom center
            Vector3 center(x, y, z);
            center -= Vector3(dx, dy, dz).normalized() * h / 2; // move to bottom center

            shape = make_shared<Cylinder>(center, Vector3(dx, dy, dz), scene.GetNumMaterials() - 1, r, h);

            scene.AddShape(shape);
        }
        else if (command == "v")
        {
            if (args.size() != 3)
            {
                cout << "ERROR on line " << line_num << ": Improper vertex usage: v <x> <y> <z>\n";
                return 1;
            }

            x = stof(args[0]); y = stof(args[1]); z = stof(args[2]);
            scene.verts.push_back(Vector3(x, y, z));
        }
        else if (command == "vn")
        {
            if (args.size() != 3)
            {
                cout << "ERROR on line " << line_num << ": Improper vertex normal usage: vn <x> <y> <z>\n";
                return 1;
            }

            x = stof(args[0]); y = stof(args[1]); z = stof(args[2]);
            scene.norms.push_back(Vector3(x, y, z).normalized());
        }
        else if (command == "vt")
        {
            if (args.size() != 2)
            {
                cout << "ERROR on line " << line_num << ": Improper vertex texture usage: vt <u> <v>\n";
                return 1;
            }

            x = stof(args[0]); y = stof(args[1]);
            scene.uvs.push_back(UV(x, y));
        }
        else if (command == "f")
        {
            if (args.size() != 3)
            {
                cout << "ERROR on line " << line_num << ": Improper face usage: f <v1> <v2> <v3>\n";
                return 1;
            }

            if (parseFace(line.c_str(), scene.GetNumMaterials() - 1, scene, line_num) == 1)
            {
                return 1;
            }
        }
        else if (command == "light")
        {
            if (args.size() != 7 && args.size() != 12)
            {
                cout << "ERROR on line " << line_num << ": Improper light usage: light <x> <y> <z> <w> [<dx> <dy> <dz> <theta_in> <theta_out>] <r> <g> <b>\n";
                return 1;
            }

            x = stof(args[0]); y = stof(args[1]); z = stof(args[2]);
            w = stof(args[3]);

            // 0 for w means directional light
            if (w == 0)
            {
                if (args.size() != 7)
                {
                    cout << "ERROR on line " << line_num << ": directional lights should not have a specified position" << endl;
                    return 1;
                }

                r = stof(args[4]); g = stof(args[5]); b = stof(args[6]);

                light = make_shared<DirectionalLight>(Vector3(x, y, z), Vector3(r, g, b));
            }
            else if (w == 1) // 1 for point light
            {
                if (args.size() != 7)
                {
                    cout << "ERROR on line " << line_num << ": point lights should not have a specified direction" << endl;
                    return 1;
                }

                r = stof(args[4]); g = stof(args[5]); b = stof(args[6]);

                light = make_shared<PointLight>(Vector3(x, y, z), Vector3(r, g, b));
            }
            else if (w == 2) // 2 for spot light
            {
                if (args.size() != 12)
                {
                    cout << "ERROR on line " << line_num << ": spot lights should have specified position, direction, and angles" << endl;
                    return 1;
                }

                dx = stof(args[4]); dy = stof(args[5]); dz = stof(args[6]);
                theta_in = stof(args[7]); theta_out = stof(args[8]);
                r = stof(args[9]); g = stof(args[10]); b = stof(args[11]);

                light = make_shared<SpotLight>(Vector3(x, y, z), Vector3(dx, dy, dz), Vector3(r, g, b), theta_in, theta_out);
            }

            scene.AddLight(light);
        }
        else if (command == "attlight")
        {
            if (args.size() != 10 && args.size() != 15)
            {
                cout << "ERROR on line " << line_num << ": Improper attlight usage: attlight <x> <y> <z> <w> [<dx> <dy> <dz> <theta_in> <theta_out>] <r> <g> <b> <c1> <c2> <c3>\n";
                return 1;
            }

            x = stof(args[0]); y = stof(args[1]); z = stof(args[2]);
            w = stof(args[3]);

            if (w == 0)
            {
                cout << "ERROR on line " << line_num << ": Directional lights should not be attenuated" << endl;
                return 1;
            }
            else if (w == 1) // 1 for point light
            {
                if (args.size() != 10)
                {
                    cout << "ERROR on line " << line_num << ": point lights should not have a specified direction" << endl;
                    return 1;
                }

                r = stof(args[4]); g = stof(args[5]); b = stof(args[6]);
                c1 = stof(args[7]); c2 = stof(args[8]); c3 = stof(args[9]);

                light = make_shared<PointLight>(Vector3(x, y, z), Vector3(r, g, b), c1, c2, c3);
            }
            else if (w == 2) // 2 for spot light
            {
                if (args.size() != 15)
                {
                    cout << "ERROR on line " << line_num << ": spot lights should have specified position, direction, and angles" << endl;
                    return 1;
                }

                dx = stof(args[4]); dy = stof(args[5]); dz = stof(args[6]);
                theta_in = stof(args[7]); theta_out = stof(args[8]);
                r = stof(args[9]); g = stof(args[10]); b = stof(args[11]);
                c1 = stof(args[12]); c2 = stof(args[13]); c3 = stof(args[14]);

                light = make_shared<SpotLight>(Vector3(x, y, z), Vector3(dx, dy, dz), Vector3(r, g, b), theta_in, theta_out, c1, c2, c3);
            }

            // idk what w is far, since it doesn't make sense to have a directional attenuated light :/
            scene.AddLight(light);
        }
        else if (command == "depthcueing")
        {
            if (args.size() != 7)
            {
                cout << "ERROR on line " << line_num << ": Improper depthcueing usage: depthcueing <r> <g> <b> <a_max> <a_min> <dist_max> <dist_min>\n";
                return 1;
            }

            r = stof(args[0]); g = stof(args[1]); b = stof(args[2]);
            x = stof(args[3]); y = stof(args[4]);
            dx = stof(args[5]); dy = stof(args[6]);

            scene.SetDepthcueing(Vector3(r, g, b), dy, dx, y, x);
        }
        else if (command == "shadowSamples")
        {
            if (args.size() != 1)
            {
                cout << "ERROR on line " << line_num << ": Improper shadowSamples usage: shadowSamples <num_samples>\n";
                return 1;
            }

            x = stof(args[0]);

            scene.SetSoftShadows(x);
        }
        else if (command == "unlit")
        {
            if (args.size() != 0)
            {
                cout << "ERROR on line " << line_num << ": Improper unlit usage: unlit\n";
                return 1;
            }

            scene.SetUnlit(true);
        }
        else if (command == "gamma")
        {
            if (args.size() != 1)
            {
                cout << endCheck << endCheck.size() << " ERROR on line " << line_num << ": Improper gamma usage: gamma <gamma>\n";
                return 1;
            }

            x = stof(args[0]);

            camera.SetGamma(x);
        }
        else if (command == "threads")
        {
            if (args.size() != 1)
            {
                cout << "ERROR on line " << line_num << ": Improper threads usage: threads <num_threads>\n";
                return 1;
            }

            x = stof(args[0]);

            camera.SetThreads(x);
        }
        else if (command == "samples")
        {
            if (args.size() != 1)
            {
                cout << "ERROR on line " << line_num << ": Improper samples usage: samples <num_samples>\n";
                return 1;
            }

            x = stof(args[0]);

            if (x < 1)
            {
                cout << "WARNING: samples must be greater than 0, setting to 1\n";
                x = 1;
            }

            camera.SetNumSamples((unsigned int)x);
        }
        else if (command == "bounces")
        {
            if (args.size() != 1)
            {
                cout << "ERROR on line " << line_num << ": Improper bounces usage: bounces <num_bounces>\n";
                return 1;
            }

            x = stof(args[0]);

            if (x < 0)
            {
                cout << "WARNING: bounces must be greater than 0, setting to 1\n";
                x = 1;
            }

            camera.SetNumBounces((unsigned int)x);
        }
        else if (command == "bvh")
        {
            if (args.size() != 1)
            {
                cout << "ERROR on line " << line_num << ": Improper bvh usage: bvh <max_depth>\n";
                return 1;
            }

            x = stof(args[0]);
            scene.SetBVHMaxDepth(x);
            scene.SetBVHIdealShapesPerBV(4); // should maybe make this a parameter as well...
            scene.SetUseBVH(true);
        }
        else if (command == "texture")
        {
            if (args.size() != 1)
            {
                cout << "ERROR on line " << line_num << ": Improper texture usage: texture <filename>\n";
                return 1;
            }

            shared_ptr<Image> tex = make_shared<Image>();

            string filetype = args[0].substr(args[0].find_last_of(".") + 1);
            // first load ppm files with my ppm loader, otherwise use stb_image for jpg/png
            if (filetype == "ppm")
            {
                if (Image::LoadFromFilePPM(args[0], tex) != 0)
                {
                    cout << "ERROR on line " << line_num << ": Could not load texture file " << args[0] << endl;
                    return 1;
                }
            }
            else 
            {
                if (Image::LoadFromFile(args[0], tex) != 0)
                {
                    cout << "ERROR on line " << line_num << ": Could not load texture file " << args[0] << endl;
                    return 1;
                }
            }
            
            scene.AddTexture(tex);
        }
        else if (command == "bump")
        {
            if (args.size() != 1)
            {
                cout << "ERROR on line " << line_num << ": Improper bump usage: bump <filename>\n";
                return 1;
            }

            shared_ptr<Image> tex = make_shared<Image>();
            string filetype = args[0].substr(args[0].find_last_of(".") + 1);
            if (filetype == "ppm")
            {
                if (Image::LoadFromFilePPM(args[0], tex) != 0)
                {
                    cout << "ERROR on line " << line_num << ": Could not load texture file " << args[0] << endl;
                    return 1;
                }
            }
            else
            {
                if (Image::LoadFromFile(args[0], tex) != 0)
                {
                    cout << "ERROR on line " << line_num << ": Could not load texture file " << args[0] << endl;
                    return 1;
                }
            }

            scene.AddBumpMap(tex);
        }
        else if (command == "obj")
        {
            if (args.size() != 1)
            {
                cout << "ERROR on line " << line_num << ": Improper obj usage: obj <filename>\n";
                return 1;
            }

            if (loadOBJ(args[0], scene, camera) != 0)
            {
                cout << "ERROR on line " << line_num << ": Could not load obj file " << args[0] << endl;
                return 1;
            }
        }
        else if (command == "gltf")
        {
            if (args.size() != 1)
            {
                cout << "ERROR on line " << line_num << ": Improper gltf usage: gltf <filename>\n";
                return 1;
            }

            if (InputReader::loadGLTF(args[0], scene) != 0)
            {
                cout << "ERROR on line " << line_num << ": Could not load gltf file " << args[0] << endl;
                return 1;
            }
        }
        else if (command == "ior")
        {
            if (args.size() != 1)
            {
                cout << "ERROR on line " << line_num << ": Improper ior usage: ior <ior>\n";
                return 1;
            }

            x = stof(args[0]);
            if (x < 1)
            {
                cout << "WARNING: ior must be greater than 1, setting to 1\n";
                x = 1;
            }

            camera.SetIOR(x);
        }
        else if (command == "hdri")
        {
            if (args.size() != 1)
            {
                cout << "ERROR on line " << line_num << ": Improper hdri usage: hdri <filename>\n";
                return 1;
            }

            shared_ptr<Image> hdri = make_shared<Image>();
            string filetype = args[0].substr(args[0].find_last_of(".") + 1);
            if (filetype == "ppm")
            {
                if (Image::LoadFromFilePPM(args[0], hdri) != 0)
                {
                    cout << "ERROR on line " << line_num << ": Could not load hdri file " << args[0] << endl;
                    return 1;
                }
            }
            else
            {
                if (Image::LoadFromFile(args[0], hdri) != 0)
                {
                    cout << "ERROR on line " << line_num << ": Could not load hdri file " << args[0] << endl;
                    return 1;
                }
            }

            scene.SetHDRI(hdri);
        }
        else if (command != "")
        {
            cout << "ERROR on line " << line_num << ": Unknown command: " << command << "\n";
            return 1;
        }

    }

    return 0;
}

void TxtReader::fillArgs(string line, vector<string> &args)
{
    args.clear();

    int comment = line.find_first_of('#');
    if (comment != string::npos)
    {
        line = line.substr(0, comment);
    }

    istringstream iss(line);
    string arg;
    while (iss >> arg)
    {
        args.push_back(arg);
    }
}

int TxtReader::loadOBJ(string filename, Scene& scene, Camera& camera)
{
    // ignore all lines that don't start with v, vt, vn, or f
    ifstream file(filename);
    if (!file.is_open())
    {
        return 1;
    }

    vector<string> args;
    string line;
    int line_num = 0;
    while (getline(file, line))
    {
        line_num++;
        fillArgs(line, args);

        if (args.size() == 0)
        {
            continue;
        }

        string command = args[0];

        if (command == "v")
        {
            if (args.size() != 4)
            {
                cout << "ERROR: Improper v usage: v <x> <y> <z>\n";
                return 1;
            }

            float x = stof(args[1]);
            float y = stof(args[2]);
            float z = stof(args[3]);

            scene.verts.push_back(Vector3(x, y, z));
        }
        else if (command == "vt")
        {
            if (args.size() != 3)
            {
                cout << "ERROR: Improper vt usage: vt <u> <v>\n";
                return 1;
            }

            float u = stof(args[1]);
            float v = stof(args[2]);

            scene.uvs.push_back(UV(u, v));
        }
        else if (command == "vn")
        {
            if (args.size() != 4)
            {
                cout << "ERROR: Improper vn usage: vn <x> <y> <z>\n";
                return 1;
            }

            float x = stof(args[1]);
            float y = stof(args[2]);
            float z = stof(args[3]);

            scene.norms.push_back(Vector3(x, y, z));
        }
        else if (command == "f")
        {
            if (args.size() != 4)
            {
                cout << "ERROR: Improper f usage: f <v1> <v2> <v3>\n";
                return 1;
            }

            if (parseFace(line.c_str(), scene.GetNumMaterials() - 1, scene, line_num) != 0)
            {
                return 1;
            }
        }

    }

    return 0;
}

bool TxtReader::isValidFiletype(string filename)
{
    string ext = filename.substr(filename.find_last_of('.') + 1);
    if (ext == "txt")
    {
        return true;
    }
    else
    {
        return false;
    }
}

