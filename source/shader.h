#ifndef SHADER__
#define SHADER__
#include "algebra3.h"
#include "output.h"

struct Material
{
    float r = 0, g = 0, b = 0;
    float Ka = 0, Kd = 0, Ks = 0, exp = 0;
    float Reflect = 0, Refract = 0, Nr = 0;
};

struct Triangle
{
    vec3 vertex1, vertex2, vertex3;
    vec3 vector12, vector13;
    Material material;
};

struct triangleList
{
    Triangle *aTriangle = NULL;
    int size = 0, num_of_tri = 0;
};

struct Sphere
{
    vec3 center;
    float radius;
    Material material;
};

struct sphereList
{
    Sphere *aSphere = NULL;
    int size = 0, num_of_sph = 0;
};

struct Ray
{
    vec3 direction, resource;
    float Nr = 1;
};

class Shader
{
private:
    vec3 light_position, eyes_position, view_direction, up_direction, screen_up, screen_lift;
    double field_of_view, distance, pixel_length, screen_width, screen_height;
    int resolution_width, resolution_height;
    triangleList triangle_list;
    sphereList sphere_list;
    ColorImage image;
public:
    bool readFile(const char filename[]);
    void setScreen(double dis, vec3 up);
    void Coloring(Material material, Pixel *color, vec3 normal_vector, Ray shape_to_light, vec3 ray, float ratio);
    void Tracing(Ray ray, Pixel *color, float ratio);
    void outputPPMformat(const char filename[]);
};

#endif // SHADER__
