/*
This glTF reader is adapted from a project I did last semester in 5611 to read glTF files into
OpenGL. I followed this video tutorial: https://www.youtube.com/watch?v=45MIykWJ-C4 but have 
made a few changes to get it to work with Blender's glTF format and to get it working with
the raytracer.
*/

#include "InputReader.h"

int InputReader::loadGLTF(string filename, Scene& scene)
{
    this->filename = filename;

    ifstream fileStream(filename);
    if (!fileStream.is_open())
    {
        cout << "Error: Failed to open file " << filename << endl;
        return 1;
    }

    stringstream buffer;
    buffer << fileStream.rdbuf();
    fileStream.close();

    file = json::parse(buffer.str());

    if (getData(filename) != 0)
    {
        return 1;
    }

    json sceneGlft = file["scenes"][0];
    for (int i = 0; i < sceneGlft["nodes"].size(); i++)
    {
        traverseNode(sceneGlft["nodes"][i], Transform::identity, scene);
    }

	return 0;
}

int InputReader::loadMesh(int indMesh, Transform transform, Scene& scene)
{
	int posAccInd = -1, normAccInd = -1, texAccInd = -1, indAccind = -1, matInd = -1;
	json attr = file["meshes"][indMesh]["primitives"][0]["attributes"];
	json primitives = file["meshes"][indMesh]["primitives"][0];

	if (attr.find("POSITION") != attr.end()) { posAccInd = attr["POSITION"]; }
	if (attr.find("NORMAL") != attr.end()) { normAccInd = attr["NORMAL"]; }
	if (attr.find("TEXCOORD_0") != attr.end()) { texAccInd = attr["TEXCOORD_0"]; }
	if (primitives.find("indices") != primitives.end()) { indAccind = primitives["indices"]; }
	if (primitives.find("material") != primitives.end()) { matInd = primitives["material"]; }

	vector<Vector3> positions, normals;
	vector<UV> texUVs;
	vector<int> indices;

	if (posAccInd != -1) 
	{
		vector<float> poses = getFloats(file["accessors"][posAccInd]);
		positions = groupFloatsVec3(poses); 
	}
	else
	{
		cout << "Error: No position data found" << endl;
		return 1;
	}

	if (normAccInd != -1) 
	{
		vector<float> norms = getFloats(file["accessors"][normAccInd]);
		normals = groupFloatsVec3(norms); 
	}
	if (texAccInd != -1) 
	{
		vector<float> uvs = getFloats(file["accessors"][texAccInd]);
		texUVs = groupFloatsUV(uvs); 
	}
	if (indAccind != -1) 
	{
		vector<int> inds = getIndices(file["accessors"][indAccind]);
		indices = inds; 
	}
	if (matInd != -1) 
	{ 
		getTextures(file["materials"][matInd], scene); 
	}

	for (int i = 0; i < positions.size(); i++)
	{
		positions[i] = transform.transformPoint(positions[i]);
		scene.verts.push_back(positions[i]);
	}

	for (int i = 0; i < normals.size(); i++)
	{
		normals[i] = transform.transformDirection(normals[i]);
		scene.norms.push_back(normals[i]);
	}

	for (int i = 0; i < texUVs.size(); i++)
	{
		scene.uvs.push_back(texUVs[i]);
	}

	// I guess I don't need to store vertices in the scene anymore if I'm loading in the entire mesh at once
	// but I don't feel like changing it right now
	shared_ptr<Shape> tri;
	for (int i = 0; i < indices.size(); i+=3)
	{
		if (normAccInd == -1 && texAccInd == -1)
		{
			tri = make_shared<Triangle>( positions[indices[i]], positions[indices[i+1]], positions[indices[i+2]], scene.GetNumMaterials() - 1);
		}
		else if (normAccInd == -1)
		{
			tri = make_shared<Triangle>( positions[indices[i]], positions[indices[i+1]], positions[indices[i+2]],
									 texUVs[indices[i]], texUVs[indices[i+1]], texUVs[indices[i+2]], scene.GetNumMaterials() - 1);
		}
		else if (texAccInd == -1)
		{
			tri = make_shared<Triangle>( positions[indices[i]], positions[indices[i+1]], positions[indices[i+2]],
									 normals[indices[i]], normals[indices[i+1]], normals[indices[i+2]], scene.GetNumMaterials() - 1);
		}
		else
		{
			tri = make_shared<Triangle>( positions[indices[i]], positions[indices[i+1]], positions[indices[i+2]],
									 normals[indices[i]], normals[indices[i+1]], normals[indices[i+2]],
									 texUVs[indices[i]], texUVs[indices[i+1]], texUVs[indices[i+2]], scene.GetNumMaterials() - 1);
		}
		scene.AddShape(tri);
	}

	return 0;

}

int InputReader::traverseNode(int nodeInd, Transform transform, Scene& scene)
{
    json node = file["nodes"][nodeInd];

    Vector3 translation = Vector3::zero;
    if (node.find("translation") != node.end())
	{
		//bc blender weird
		translation.x = node["translation"][0];
		translation.y = node["translation"][1];
		translation.z = node["translation"][2];
		
	}

	Quaternion rotation = Quaternion::identity;
	if (node.find("rotation") != node.end())
	{
		Float w = node["rotation"][3];
		Float x = node["rotation"][0];
		Float y = node["rotation"][1];
		Float z = node["rotation"][2];
		rotation = Quaternion(w, x, y, z);
	}

	Vector3 scale = Vector3::one;
	if (node.find("scale") != node.end())
	{
		scale.x = node["scale"][0];
        scale.y = node["scale"][1];
        scale.z = node["scale"][2];
	}

	Transform nodeTransform = Transform(translation, rotation, scale);  
	Transform globalTransform = transform * nodeTransform;

	if (node.find("mesh") != node.end())
	{
		translations.push_back(translation);
		rotations.push_back(rotation);
		scales.push_back(scale);
		transforms.push_back(globalTransform);

		loadMesh(node["mesh"], globalTransform, scene);
	}

	if (node.find("children") != node.end())
	{
		for (unsigned int i = 0; i < node["children"].size(); i++)
			traverseNode(node["children"][i], globalTransform, scene);
	}

	return 0;
}

int InputReader::getData(string filename)
{
    string bytes;
    string uri = file["buffers"][0]["uri"];

    string filePath = filename.substr(0, filename.find_last_of("/") + 1);
    ifstream fileStream(filePath + uri);
    if (!fileStream.is_open())
    {
        cout << "Error: Failed to open file " << filePath + uri << endl;
        return 1;
    }

    stringstream buffer;
    buffer << fileStream.rdbuf();
    fileStream.close();

    bytes = buffer.str();
    data = vector<char>(bytes.begin(), bytes.end());

	return 0;
}

vector<float> InputReader::getFloats(json accessor)
{
    vector<float> floatVec;

	unsigned int buffViewInd = accessor.value("bufferView", 1);
	unsigned int count = accessor["count"];
	unsigned int accByteOffset = accessor.value("byteOffset", 0);
	string type = accessor["type"];

	json bufferView = file["bufferViews"][buffViewInd];
	unsigned int byteOffset = bufferView["byteOffset"];

	unsigned int numPerVert;
	if (type == "SCALAR") numPerVert = 1;
	else if (type == "VEC2") numPerVert = 2;
	else if (type == "VEC3") numPerVert = 3;
	else if (type == "VEC4") numPerVert = 4;
	else throw invalid_argument("Type is invalid (not SCALAR, VEC2, VEC3, or VEC4)");

	unsigned int beginningOfData = byteOffset + accByteOffset;
	unsigned int lengthOfData = count * 4 * numPerVert;

	for (unsigned int i = beginningOfData; i < beginningOfData + lengthOfData;)
	{
		char bytes[] = { data[i++], data[i++], data[i++], data[i++] };
		float value;
		memcpy(&value, bytes, sizeof(float));
		floatVec.push_back(value);
	}

	return floatVec;
}

vector<int> InputReader::getIndices(json accessor)
{
    vector<int> indices;

	unsigned int buffViewInd = accessor.value("bufferView", 0);
	unsigned int count = accessor["count"];
	unsigned int accByteOffset = accessor.value("byteOffset", 0);
	unsigned int componentType = accessor["componentType"];

	json bufferView = file["bufferViews"][buffViewInd];
	unsigned int byteOffset = bufferView["byteOffset"];

	unsigned int beginningOfData = byteOffset + accByteOffset;
	if (componentType == 5125)
	{
		for (unsigned int i = beginningOfData; i < byteOffset + accByteOffset + count * 4;)
		{
			char bytes[] = { data[i++], data[i++], data[i++], data[i++] };
			unsigned int value;
			memcpy(&value, bytes, sizeof(unsigned int));
			indices.push_back((int)value);
		}
	}
	else if (componentType == 5123)
	{
		for (unsigned int i = beginningOfData; i < byteOffset + accByteOffset + count * 2;)
		{
			char bytes[] = { data[i++], data[i++] };
			unsigned short value;
			memcpy(&value, bytes, sizeof(unsigned short));
			indices.push_back((int)value);
		}
	}
	else if (componentType == 5122)
	{
		for (unsigned int i = beginningOfData; i < byteOffset + accByteOffset + count * 2;)
		{
			char bytes[] = { data[i++], data[i++] };
			short value;
			std::memcpy(&value, bytes, sizeof(short));
			indices.push_back((int)value);
		}
	}
	
	return indices;
}

void InputReader::getTextures(json material, Scene& scene)
{
	string filePath;
	string fileDirectory = filename.substr(0, filename.find_last_of('/') + 1);
	bool skip = false;
	shared_ptr<Image> curTexture;
	shared_ptr<BWImage> curBWTexture;

	// first generate a default material to build upon
	Material mat = Material(Vector3(0.5,0.5,0.5), Vector3::one, 0.1, 0.8, 0.5, 8, 1, 1);

	if (material.find("normalTexture") != material.end())
	{
		filePath = file["images"][(int)material["normalTexture"]["index"]]["uri"];
		// first check if it's already loaded
		for (int i = 0; i < scene.GetNumBumpMaps(); i++)
		{
			if (scene.GetBumpMap(i)->filepath == filePath)
			{
				mat.SetBumpMap(i);
				skip = true;
				break;
			}
		}

		if (!skip)
		{
			curTexture = make_shared<Image>();
			Image::LoadFromFile(filePath, curTexture);
			scene.AddBumpMap(curTexture);
			mat.SetBumpMap(scene.GetNumBumpMaps() - 1);
		}

		if (material["normalTexture"].find("scale") != material["normalTexture"].end())
		{
			mat.SetNormalStrength(material["normalTexture"]["scale"]);
		}
	}
	if (material.find("pbrMetallicRoughness") != material.end())
	{
		material = material["pbrMetallicRoughness"];
		if (material.find("baseColorTexture") != material.end())
		{
			filePath = file["images"][(int)material["baseColorTexture"]["index"]]["uri"];
			//see if it's already loaded
			for (unsigned int i = 0; i < scene.GetNumTextures(); i++)
			{
				if (scene.GetTexture(i)->filepath == filePath)
				{
					// get scene to reapply texture to current material
					mat.SetTexture(i);
					skip = true;
					break;
				}
			}
			if (!skip)
			{
				//printf("Diffuse texture loaded: %s\n", filePath.c_str());
				curTexture = make_shared<Image>();
				Image::LoadFromFile(filePath, curTexture);
				scene.AddTexture(curTexture);
				mat.SetTexture(scene.GetNumTextures() - 1);
			}
		}
		else if (material.find("baseColorFactor") != material.end())
		{
			Float r = material["baseColorFactor"][0];
			Float g = material["baseColorFactor"][1];
			Float b = material["baseColorFactor"][2];

			mat.SetDiffuse(Vector3(r, g, b));
		}

		if (material.find("metallicRoughnessTexture") != material.end())
		{
			filePath = file["images"][(int)material["metallicRoughnessTexture"]["index"]]["uri"];
			//see if it's already loaded
			for (unsigned int i = 0; i < scene.GetNumSpecMaps(); i++)
			{
				if (scene.GetSpecMap(i)->filepath == filePath)
				{
					//printf("Specular texture already loaded: %d\n", i);
					mat.SetSpecMap(i);
					skip = true;
					break;
				}
			}
			if (!skip)
			{
				//printf("Specular texture loaded: %s\n", filePath.c_str());
				curBWTexture = make_shared<BWImage>();
				BWImage::LoadFromFile(filePath, curBWTexture);
				scene.AddSpecMap(curBWTexture);
				mat.SetSpecMap(scene.GetNumSpecMaps() - 1);
			}
		}

		// roughness factor to spec exponent is 900 * (roughness - 1)^2
		// figured this out by exporting a bunch at various values and graphing them in desmos lol
		if (material.find("roughnessFactor") != material.end())
		{
			Float n = material["roughnessFactor"];
			n = 100 * (n - 1) * (n - 1);
			mat.SetSpecFalloff(n);
		}

		if (material.find("metallicFactor") != material.end())
		{
			// gonna treat metallic factor as a lerp for the specular color between white and diffuse
			Float t = material["metallicFactor"];
			Vector3 spec = mat.GetDiffuse() * t + Vector3(1, 1, 1) * (1 - t);
			mat.SetSpecular(spec);
		}

	}

	// apply the material to the scene
	scene.AddMaterial(mat);
}

vector<UV> InputReader::groupFloatsUV(vector<float>& floatVec)
{
	std::vector<UV> uvs;
	for (int i = 0; i < floatVec.size();)
	{
		uvs.push_back(UV(floatVec[i], floatVec[i+1]));
		i += 2;
	}
	return uvs;
}

vector<Vector3> InputReader::groupFloatsVec3(vector<float>& floatVec)
{
	vector<Vector3> vectors;
	for (int i = 0; i < floatVec.size();)
	{
		vectors.push_back(Vector3(floatVec[i], floatVec[i+1], floatVec[i+2]));
		i += 3;
	}
	return vectors;
}


