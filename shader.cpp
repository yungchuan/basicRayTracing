#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include "algebra3.h"
#include "shader.h"

bool addTriangle(triangleList *tL, Triangle tri)
{
    if(tL->size == 0){
        tL->aTriangle = (Triangle*)malloc(sizeof(Triangle));
        if(tL->aTriangle == NULL) return false;
        else tL->size = 1;
    }
    else if(tL->num_of_tri >= tL->size){
        tL->aTriangle = (Triangle*)realloc(tL->aTriangle, tL->size*2*sizeof(Triangle));
        if(tL->aTriangle == NULL) return false;
        else tL->size*=2;
    }
    tL->aTriangle[tL->num_of_tri] = tri;
    tL->num_of_tri++;
    return true;
}

float traceTriangle(Triangle T, Ray ray){
    mat3 Mat, Mat_T;
    vec3 root;
    Mat[0] = T.vector12;
    Mat[1] = T.vector13;
    Mat[2] = -ray.direction;
    Mat_T = Mat.transpose();
    if(Mat_T.determinant() == 0) return -1;
    root = Mat_T.inverse()*(ray.resource - T.vertex1);
    if(root[0]>=0 && root[1]>=0 && root[2]>=0 && root[0]+root[1]<=1) return root[2];
    else return -1;
}

bool addSphere(sphereList *sL, Sphere sph)
{
    if(sL->size == 0){
        sL->aSphere = (Sphere*)malloc(sizeof(Sphere));
        if(sL->aSphere == NULL) return false;
        else sL->size = 1;
    }
    else if(sL->num_of_sph >= sL->size){
        sL->aSphere = (Sphere*)realloc(sL->aSphere, sL->size*2*sizeof(Sphere));
        if(sL->aSphere == NULL) return false;
        else sL->size*=2;
    }
    sL->aSphere[sL->num_of_sph] = sph;
    sL->num_of_sph++;
    return true;
}

float traceSphere(Sphere S, Ray ray){
    float a,b,c,D,t;///a*t^2 + b*t + c = 0; D = b^2 - 4*a*c
    a = ray.direction.length2();
    b = 2*(ray.direction*(ray.resource-S.center));
    c = (ray.resource-S.center).length2() - pow(S.radius,2);
    D = b*b - 4*a*c;
    if(D >= 0){
        t = (0-b-sqrt(D))/(2*a);
        if(t < 0.01) t = (0-b+sqrt(D))/(2*a);
        return t;
    }
    return -1;
}

bool Shader::readFile(const char filename[])
{
    FILE *fp = fopen(filename, "r");
    if(fp == NULL) return 0;
    Material mat;
    char Input = '\0';
    while(!feof(fp)){
        fscanf(fp, " %c", &Input);
        switch(Input){
        case 'E':{
            fscanf(fp, "%f %f %f", &eyes_position[0], &eyes_position[1], &eyes_position[2]);
        }break;
        case 'V':{
            fscanf(fp, "%f %f %f", &view_direction[0], &view_direction[1], &view_direction[2]);
        }break;
        case 'L':{
            fscanf(fp, "%f %f %f", &light_position[0], &light_position[1], &light_position[2]);
        }break;
        case 'F':{
            fscanf(fp, "%lf", &field_of_view);
        }break;
        case 'R':{
            fscanf(fp, "%d %d", &resolution_width, &resolution_height);
        }break;
        case 'S':{
            Sphere stmp;
            fscanf(fp, "%f %f %f %f", &stmp.center[0], &stmp.center[1], &stmp.center[2], &stmp.radius);
            stmp.material = mat;
            addSphere(&sphere_list, stmp);
        }break;
        case 'T':{
            Triangle ttmp;
            fscanf(fp, "%f %f %f", &ttmp.vertex1[0], &ttmp.vertex1[1], &ttmp.vertex1[2]);
            fscanf(fp, "%f %f %f", &ttmp.vertex2[0], &ttmp.vertex2[1], &ttmp.vertex2[2]);
            fscanf(fp, "%f %f %f", &ttmp.vertex3[0], &ttmp.vertex3[1], &ttmp.vertex3[2]);
            ttmp.material = mat;
            ttmp.vector12 = ttmp.vertex2 - ttmp.vertex1;
            ttmp.vector13 = ttmp.vertex3 - ttmp.vertex1;
            addTriangle(&triangle_list, ttmp);
        }break;
        case 'M':{
            fscanf(fp, "%f %f %f", &mat.r, &mat.g, &mat.b);
            fscanf(fp, "%f %f %f %f", &mat.Ka, &mat.Kd, &mat.Ks, &mat.exp);
            fscanf(fp, "%f %f %f", &mat.Reflect, &mat.Refract, &mat.Nr);
        }break;
        }
    }
    fclose(fp);
    return true;
}

void Shader::setScreen(double dis, vec3 up)
{
    distance = dis;
    up_direction = up;

    screen_lift = up_direction^view_direction;
    screen_up = view_direction^screen_lift;
    up_direction.normalize();
    screen_lift.normalize();
    screen_up.normalize();

    screen_width = 2*distance*tan(field_of_view*M_PI/2/180);
    pixel_length = screen_width/resolution_width;
    screen_height = pixel_length*resolution_height;
}

void Shader::Coloring(Material material, Pixel *color, vec3 normal_vector, Ray shape_to_light, vec3 ray, float ratio)
{
    vec3 half_vector = (ray+shape_to_light.direction)/2 - ray;
    half_vector.normalize();
    float coef_a = material.Ka, coef_d = material.Kd*(normal_vector*shape_to_light.direction), coef_s = material.Ks*pow((normal_vector*half_vector), material.exp);
    if(coef_d < 0) coef_d = 0;
    if(coef_s < 0) coef_s = 0;
    float coef = 255*ratio*(coef_a + coef_d + coef_s);
    ///                     ambient  diffuse  specular

    for(int k = 0; k < triangle_list.num_of_tri; k++)
        if(traceTriangle(triangle_list.aTriangle[k], shape_to_light)>0.01){
            coef = 255*ratio*coef_a;
            break;
        }
    for(int k = 0; k < sphere_list.num_of_sph; k++)
        if(traceSphere(sphere_list.aSphere[k], shape_to_light)>0.01){
            coef = 255*ratio*coef_a;
            break;
        }

    int R = color->R + coef*material.r;
    int G = color->G + coef*material.g;
    int B = color->B + coef*material.b;
    if(R > 255) color->R = 255;
    else color->R = R;
    if(G > 255) color->G = 255;
    else color->G = G;
    if(B > 255) color->B = 255;
    else color->B = B;
}

void Shader::Tracing(Ray ray, Pixel *color, float ratio)
{
    if(ratio > 0){
    float t, t_min = INT_MAX;
    bool istrace = false;
    Material material;
    vec3 normal_vector;
    for(int k = 0; k < triangle_list.num_of_tri; k++){
        t = traceTriangle(triangle_list.aTriangle[k], ray);
        if(t > 0 && t < t_min){
            material = triangle_list.aTriangle[k].material;
            t_min = t;
            normal_vector = triangle_list.aTriangle[k].vector12^triangle_list.aTriangle[k].vector13;
            if(ray.direction*normal_vector > 0) normal_vector = -normal_vector;
            istrace = true;
        }
    }
    for(int k = 0; k < sphere_list.num_of_sph; k++){
        t = traceSphere(sphere_list.aSphere[k], ray);
        if(t > 0.01 && t < t_min){
            material = sphere_list.aSphere[k].material;
            t_min = t;
            normal_vector = ray.resource + t*ray.direction - sphere_list.aSphere[k].center;
            if(ray.direction*normal_vector > 0) normal_vector = -normal_vector;
            istrace = true;
        }
    }
    if(istrace){
        vec3 tan_vector, object_position;
        Ray reflect_ray, refract_ray, shape_to_light;
        object_position = ray.resource + t_min*ray.direction;
        shape_to_light.direction = light_position - (object_position);
        shape_to_light.direction.normalize();
        shape_to_light.resource = object_position;
        normal_vector.normalize();
        Coloring(material, color, normal_vector, shape_to_light, ray.direction, ratio);

        reflect_ray.direction = ray.direction - 2*(ray.direction*normal_vector)*normal_vector;
        reflect_ray.direction.normalize();
        reflect_ray.resource = object_position;

        Tracing(reflect_ray, color, ratio*material.Reflect);
    }
    }
}

void Shader::outputPPMformat(const char filename[])
{
    Ray ray;
    ray.Nr = 1;  ray.resource = eyes_position;
    Pixel color = {0,0,0};
    image.init(resolution_width, resolution_height);
    for(int i = 0; i < resolution_height; i++){
        for(int j = 0; j < resolution_width; j++){
            ray.direction = view_direction + (screen_width/2-(2*j+1)*pixel_length/2)*screen_lift + (screen_height/2-(2*i+1)*pixel_length/2)*screen_up;
            ray.direction.normalize();
            Tracing(ray, &color, 1);
            image.writePixel(j,i,color);
            color = {0,0,0};
        }
    }
    image.outputPPM(filename);
}
