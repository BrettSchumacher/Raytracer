#include "Material.h"

Material::Material()
{
    diffuse = Vector3(0.0, 0.0, 0.0);
    specular = Vector3(1.0, 1.0, 1.0);
    k_diffuse = 0.8;
    k_specular = 0.4;
    k_ambient = 0.2;
    spec_falloff = 2.0;
    normal_strength = 1.0;
    alpha = 1;
    ior = 1.0;
}

Material::Material(Vector3 diffuse, Vector3 specular, Float k_a, Float k_d, Float k_s, Float spec_falloff, Float alpha, Float ior)
{
    this->diffuse = diffuse;
    this->specular = specular;
    this->k_diffuse = k_d;
    this->k_specular = k_s;
    this->k_ambient = k_a;
    this->spec_falloff = spec_falloff;
    this->normal_strength = 1.0;
    this->alpha = alpha;
    this->ior = ior;
}

Material::Material(const Material& other)
{
    this->diffuse = other.diffuse;
    this->specular = other.specular;
    this->k_diffuse = other.k_diffuse;
    this->k_specular = other.k_specular;
    this->k_ambient = other.k_ambient;
    this->spec_falloff = other.spec_falloff;
    this->has_texture = other.has_texture;
    this->has_bump_map = other.has_bump_map;
    this->has_spec_map = other.has_spec_map;
    this->texture_index = other.texture_index;
    this->bump_map_index = other.bump_map_index;
    this->spec_map_index = other.spec_map_index;
    this->normal_strength = other.normal_strength;
    this->alpha = other.alpha;
    this->ior = other.ior;
}

Material& Material::operator=(const Material& other) 
{
    this->diffuse = other.diffuse;
    this->specular = other.specular;
    this->k_diffuse = other.k_diffuse;
    this->k_specular = other.k_specular;
    this->k_ambient = other.k_ambient;
    this->spec_falloff = other.spec_falloff;
    this->has_texture = other.has_texture;
    this->has_bump_map = other.has_bump_map;
    this->has_spec_map = other.has_spec_map;
    this->texture_index = other.texture_index;
    this->bump_map_index = other.bump_map_index;
    this->spec_map_index = other.spec_map_index;
    this->normal_strength = other.normal_strength;
    this->alpha = other.alpha;
    this->ior = other.ior;
    return *this;
}

void Material::SetDiffuse(Vector3 diffuse)
{
    this->diffuse = diffuse;
}

void Material::SetSpecular(Vector3 specular)
{
    this->specular = specular;
}

void Material::SetK_D(Float k_d)
{
    this->k_diffuse = k_d;
}

void Material::SetK_S(Float k_s)
{
    this->k_specular = k_s;
}

void Material::SetK_A(Float k_a)
{
    this->k_ambient = k_a;
}

void Material::SetSpecFalloff(Float spec_falloff)
{
    this->spec_falloff = spec_falloff;
}

void Material::SetAlpha(Float alpha)
{
    this->alpha = alpha;
}

void Material::SetIOR(Float ior)
{
    this->ior = ior;
}

void Material::SetNormalStrength(Float normal_strength)
{
    this->normal_strength = normal_strength;
}

void Material::SetTexture(int tex_ind)
{
    this->texture_index = tex_ind;
    has_texture = true;
}

void Material::SetBumpMap(int tex_ind)
{
    this->bump_map_index = tex_ind;
    has_bump_map = true;
}

void Material::SetSpecMap(int tex_ind)
{
    this->spec_map_index = tex_ind;
    has_spec_map = true;
}

Vector3 Material::GetDiffuse()
{
    return diffuse;
}

Vector3 Material::GetSpecular()
{
    return specular;
}

Float Material::GetK_D()
{
    return k_diffuse;
}

Float Material::GetK_S()
{
    return k_specular;
}

Float Material::GetK_A()
{
    return k_ambient;
}

Float Material::GetSpecFalloff()
{
    return spec_falloff;
}

Float Material::GetAlpha()
{
    return alpha;
}

Float Material::GetIOR()
{
    return ior;
}

Float Material::GetNormalStrength()
{
    return normal_strength;
}

Vector3 Material::CalculateNormal(RayHit& hit, vector<shared_ptr<Image>>& bumpMaps)
{
    if (!has_bump_map)
    {
        return hit.normal;
    }

    Vector3 normal = hit.normal;
    Vector3 tangent = hit.tangent;
    Vector3 bitangent = hit.bitangent;
    Vector3 map_col = bumpMaps[bump_map_index]->GetColorUV(hit.uv.u, hit.uv.v);
    Vector3 bump_normal = map_col * 2.0 - Vector3(1.0, 1.0, 1.0);
    bump_normal.x *= normal_strength;
    bump_normal.y *= normal_strength;
    bump_normal.z /= normal_strength;
    bump_normal.Normalize();

    Vector3 new_normal = Vector3::zero;
    new_normal.x = tangent.x * bump_normal.x + bitangent.x * bump_normal.y + normal.x * bump_normal.z;
    new_normal.y = tangent.y * bump_normal.x + bitangent.y * bump_normal.y + normal.y * bump_normal.z;
    new_normal.z = tangent.z * bump_normal.x + bitangent.z * bump_normal.y + normal.z * bump_normal.z;
    new_normal.Normalize();

    return new_normal;
}

// calculated using the Phong model
Vector3 Material::GetColor(RayHit& hitInfo, vector<shared_ptr<Image>>& textures, vector<shared_ptr<Image>>& bumpMaps, vector<shared_ptr<BWImage>>& specMaps, 
                           Float diffuseAmt, Float specularAmt, Vector3 lightColor)
{
    if (has_spec_map)
    {
        Float n = specMaps[spec_map_index]->GetColorUV(hitInfo.uv.u, hitInfo.uv.v);
        spec_falloff = 100 * (1 - n) * (1 - n);
    }

    diffuseAmt = diffuseAmt < 0.0 ? 0.0 : diffuseAmt;
    specularAmt = specularAmt < 0.0 ? 0.0 : specularAmt;
    specularAmt = pow(specularAmt, spec_falloff);

    Vector3 diffCol = diffuse;

    if (has_texture)
    {
        diffCol = textures[texture_index]->GetColorUV(hitInfo.uv.u, hitInfo.uv.v);
    }

    Vector3 col =  k_diffuse * diffCol * diffuseAmt + k_specular * specular * specularAmt;
    col = col * lightColor; // scale diffuse/specular color by light color
    col += k_ambient * diffuse; // add ambient

    return col;
}

Vector3 Material::GetColorNoAmbient(RayHit& hitInfo, vector<shared_ptr<Image>>& textures, vector<shared_ptr<Image>>& bumpMaps, vector<shared_ptr<BWImage>>& specMaps,
                                    Float diffuseAmt, Float specularAmt, Vector3 lightColor, Vector3& diffuse, Vector3& specular)
{
    if (has_spec_map)
    {
        Float n = specMaps[spec_map_index]->GetColorUV(hitInfo.uv.u, hitInfo.uv.v);
        spec_falloff = 100 * (1 - n) * (1 - n);
    }

    diffuseAmt = diffuseAmt < 0.0 ? 0.0 : diffuseAmt;
    specularAmt = specularAmt < 0.0 ? 0.0 : specularAmt;
    specularAmt = pow(specularAmt, spec_falloff);

    Vector3 diffCol = this->diffuse;

    if (has_texture)
    {
        diffCol = textures[texture_index]->GetColorUV(hitInfo.uv.u, hitInfo.uv.v);
    }
    
    diffuse = k_diffuse * diffCol * diffuseAmt * lightColor;
    specular = k_specular * this->specular * specularAmt * lightColor;

    return diffuse + specular;
}

Vector3 Material::GetAmbient()
{
    return k_ambient * diffuse;
}

Vector3 Material::GetUnlit(RayHit& hitInfo, vector<shared_ptr<Image>>& textures, vector<shared_ptr<Image>>& bumpMaps)
{
    Vector3 diffCol = diffuse;

    if (has_texture)
    {
        diffCol = textures[texture_index]->GetColorUV(hitInfo.uv.u, hitInfo.uv.v);
    }

    return diffCol;
}

Vector3 Material::GetAmbient(RayHit& hitInfo, vector<shared_ptr<Image>>& textures, vector<shared_ptr<Image>>& bumpMaps)
{
    Vector3 diffCol = diffuse;

    if (has_texture)
    {
        diffCol = textures[texture_index]->GetColorUV(hitInfo.uv.u, hitInfo.uv.v);
    }

    return k_ambient * diffCol;
}

int Material::GetTexture()
{
    return has_texture ? texture_index : -1;
}

int Material::GetBumpMap()
{
    return has_bump_map ? bump_map_index : -1;
} 

int Material::GetSpecMap()
{
    return has_spec_map ? spec_map_index : -1;
}
