#ifndef MATERIAL_H
#define MATERIAL_H

#include "math/Vector3.h"
#include "Ray.h"
#include "Image.h"
#include "BWImage.h"

#include <vector>
#include <memory>

struct Material
{
    public:
        Material();
        Material(Vector3 diffuse, Vector3 specular, Float k_a, Float k_d, Float k_s, Float spec_falloff, Float alpha, Float ior);
        Material(const Material& other);
        Material& operator=(const Material& other);

        void SetDiffuse(Vector3 diffuse);
        void SetSpecular(Vector3 specular);
        void SetK_D(Float k_d);
        void SetK_S(Float k_s);
        void SetK_A(Float k_a);
        void SetSpecFalloff(Float spec_falloff);
        void SetNormalStrength(Float normal_strength);
        void SetTexture(int tex_ind);
        void SetBumpMap(int tex_ind);
        void SetSpecMap(int tex_ind);
        void SetAlpha(Float alpha);
        void SetIOR(Float ior);

        Vector3 GetDiffuse();
        Vector3 GetSpecular();
        Float GetK_D();
        Float GetK_S();
        Float GetK_A();
        Float GetSpecFalloff();
        Float GetNormalStrength();
        Float GetAlpha();
        Float GetIOR();
        int GetTexture();
        int GetBumpMap();
        int GetSpecMap();

        Vector3 CalculateNormal(RayHit& hitInfo, vector<shared_ptr<Image>>& bumpMaps);
        Vector3 GetColor(RayHit& hitInfo, vector<shared_ptr<Image>>& textures, vector<shared_ptr<Image>>& bumpMaps, vector<shared_ptr<BWImage>>& specMaps,
                         Float diffuseAmt, Float specularAmt, Vector3 LightColor);
        Vector3 GetColorNoAmbient(RayHit& hitInfo, vector<shared_ptr<Image>>& textures, vector<shared_ptr<Image>>& bumpMaps, vector<shared_ptr<BWImage>>& specMaps, 
                                  Float diffuseAmt, Float specularAmt, Vector3 LightColor, Vector3& diffuse, Vector3& specular);
        Vector3 GetAmbient();
        Vector3 GetUnlit(RayHit& hitInfo, vector<shared_ptr<Image>>& textures, vector<shared_ptr<Image>>& bumpMaps);
        Vector3 GetAmbient(RayHit& hitInfo, vector<shared_ptr<Image>>& textures, vector<shared_ptr<Image>>& bumpMaps);


    private:
        Vector3 diffuse;
        Vector3 specular;
        
        Float k_diffuse;
        Float k_specular;
        Float k_ambient;
        Float spec_falloff;     // exponent for specular falloff
        Float normal_strength;
        Float alpha; // opacity of each channel
        Float ior; // index of refraction

        bool has_texture = false;
        bool has_bump_map = false;
        bool has_spec_map = false;
        int texture_index;
        int bump_map_index;
        int spec_map_index;
};

#endif