#include "Ray.h"
#include "Intersection.h"
#include "../Camera.h"
#include "RTScene.h"
#include "Image.h"
#include <iostream>

namespace RayTracer{

    Ray RayThruPixel(Camera cam, int i, int j, int width, int height){
        glm::vec3 source = cam.eye;
        float alpha = 2*(i+0.5)/width-1;
        float beta = 1-2*(j+0.5)/height;
        glm::vec3 w = glm::normalize(cam.eye-cam.target);
        glm::vec3 u = glm::normalize(glm::cross(cam.up,w));
        glm::vec3 v = glm::cross(w,u);
        glm::vec3 direction = glm::normalize(-alpha*cam.aspect*glm::tan(cam.fovy/2)*u
            +beta*glm::tan(cam.fovy/2)*v-w);
        Ray result;
        result.p0 = source;
        result.dir = direction;
        return result;
    }

    Intersection Intersect(Ray ray, Triangle* triangle){
        Intersection result;
        result.triangle = triangle;
            
        glm::vec4 rayPos = glm::vec4(ray.p0,1);
        glm::mat4 matrix = {
            glm::vec4(triangle->P[0],1).x, glm::vec4(triangle->P[0],1).y, glm::vec4(triangle->P[0],1).z, 1,
            glm::vec4(triangle->P[1],1).x, glm::vec4(triangle->P[1],1).y, glm::vec4(triangle->P[1],1).z, 1,
            glm::vec4(triangle->P[2],1).x, glm::vec4(triangle->P[2],1).y, glm::vec4(triangle->P[2],1).z, 1,
            glm::vec4(-ray.dir,0).x, glm::vec4(-ray.dir,0).y, glm::vec4(-ray.dir,0).z, 0
        };
        glm::vec4 lambdas = glm::inverse(matrix)*rayPos;

        // there is an intersection if lambdas >= 0
        if(lambdas.x>=0 && lambdas.y>=0 && lambdas.z>=0 && lambdas.w>=0) {
            glm::vec3 q = lambdas.x*triangle->P[0] + lambdas.y*triangle->P[1] + lambdas.z*triangle->P[2];
            glm::vec3 n = glm::normalize(lambdas.x*triangle->N[0] + lambdas.y*triangle->N[1] + lambdas.z*triangle->N[2]);
            result.P = q;
            result.N = n;
            result.V = -ray.dir;
            result.dist = lambdas.w;
        } else { 
            result.dist = INFINITY;
        }
        return result;
    }
    Intersection Intersect(Ray ray, RTScene* scene){
        float minDist = INFINITY;
        Intersection hit;
        for(Triangle obj : scene -> triangle_soup){
            Intersection hit_temp = Intersect(ray, &obj);
            if(hit_temp.dist < minDist){
                minDist = hit_temp.dist;
                hit = hit_temp;
            }
        }
        return hit;
    }
    glm::vec3 FindColor(Intersection hit, int recursion_depth, RTScene* sceneptr){
        glm::vec4 color;
        Material* m;
        
        std::map<std::string, Light* > light = sceneptr -> light;
        int nlights = light.size();
        std::vector<glm::vec4> lightpositions;
        std::vector<glm::vec4> lightcolors;
        for (std::pair<std::string, Light*> entry : light){
            lightpositions.push_back((entry.second) -> position);
            lightcolors.push_back((entry.second) -> color);
        }
        if(hit.dist == INFINITY) {
            color = glm::vec4(0);
        } else {
            m = hit.triangle->material;
            color = m -> emision;
            int visibility;
            glm::vec3 specularComp;
            
            glm::vec3 curLight;
            glm::vec4 lightPos;
            glm::vec3 dirLight;
            glm::vec3 diffuseComp;
            glm::vec3 dirView;
            glm::vec3 halfwayDir;
            Ray toLight;
            Intersection checkShadow;
            for(int i=0; i<nlights; i++) {
                // ambient component
                curLight = glm::vec3(m->ambient);

                // diffuse component
                lightPos = lightpositions[i];
                dirLight = glm::normalize(glm::vec3(lightPos)-lightPos.w*hit.N);
                diffuseComp = glm::vec3(m->diffuse)*glm::max(glm::dot(hit.N,dirLight),(float)0);
                curLight = curLight + diffuseComp;

                // specular component at maximum recursion
                if(recursion_depth == 0) {
                    dirView = glm::normalize(-hit.N);
                    halfwayDir = normalize(dirLight+dirView);
                    specularComp = glm::vec3(m->specular)*glm::pow(glm::max(glm::dot(hit.N,halfwayDir),(float)0),m->shininess);
                    curLight = curLight + specularComp;
                }

                //check for shadow
                toLight.p0 = hit.P + 0.2f*dirLight;
                toLight.dir = dirLight;
                float distToLight = glm::sqrt(glm::pow((lightPos.x-hit.P.x),2) +
                                            glm::pow((lightPos.y-hit.P.y),2) +
                                            glm::pow((lightPos.z-hit.P.z),2));
                checkShadow = Intersect(toLight, sceneptr);
                visibility = 1;
                if(checkShadow.dist < distToLight) {
                    visibility = 0;
                }
                curLight.x = visibility * curLight.x * lightcolors[i].x;
                curLight.y = visibility * curLight.y * lightcolors[i].y;
                curLight.z = visibility * curLight.z * lightcolors[i].z;
                color = color + glm::vec4(curLight,0);
            }
            
            Ray newRay;
            Intersection hit2;
            // specular component (recursive)
            if(recursion_depth > 0) {
                newRay.dir = 2.0f*glm::dot(hit.N,hit.V)*hit.N-hit.V;
                newRay.p0 = hit.P + 0.1f* newRay.dir;
                hit2 = Intersect(newRay, sceneptr);
                specularComp = FindColor(hit2,recursion_depth-1,sceneptr);
                color = color + glm::vec4(specularComp,0);
            }
        }
        return color;
    }

    void Raytrace(Camera cam, RTScene scene, Image &image){
        std::cout << "Processing ray tracing image..."<<std::endl;
        int w = image.width; int h = image.height;
        for (int j=0; j<h; j++){ 
            for (int i=0; i<w; i++){ 
                Ray ray = RayThruPixel(cam, i, j, w, h);
                Intersection hit = Intersect(ray, &scene);
                image.pixels[i+j*w] = FindColor(hit, 3, &scene);
            }
        }
        std::cout <<"done ray tracing."<<std::endl;
    }

};