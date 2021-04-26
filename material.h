#ifndef TRABALHO3D_MATERIAL_H
#define TRABALHO3D_MATERIAL_H
#include <GL/gl.h>

class Material
{
private:
    float emission[4];
    float ambient[4];
    float diffuse[4];
    float specular[4];
    float shininess;
public:
    Material(
            float emis_r, float emis_g, float emis_b,
            float ambi_r, float ambi_g, float ambi_b,
            float diff_r, float diff_g, float diff_b,
            float spec_r, float spec_g, float spec_b,
            float shin
    );
    ~Material();
    void ativar();
};


#endif //TRABALHO3D_MATERIAL_H
