#include "Ray.h"
#include "Intersection.h"
#include "Triangle.h"
#include "RTScene.h"
#include <iostream>
namespace RayTracer{

    Ray RayThruPixel(Camera cam, int i, int j, int width, int height) {
        glm::vec3 source = cam.eye;
        float alpha = 2 * (i+0.5)/width - 1;
        float beta = 1 - 2*(j+0.5)/height;

        glm::vec3 w = glm::normalize(cam.eye-cam.target);
        glm::vec3 u = glm::normalize(glm::cross(cam.up,w));
        glm::vec3 v = glm::cross(w,u);

        glm::vec3 direction = glm::normalize(-alpha * cam.aspect * glm::tan(cam.fovy/2) * u
                                            + beta * glm::tan(cam.fovy/2)*v-w);

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
        glm::vec4 lambdas = glm::inverse(matrix) * rayPos;

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
    Intersection Intersect(Ray ray, RTScene* scene) {
        float minDist = INFINITY;
        Intersection hit;
        hit.dist = minDist;
        for(Triangle t : scene -> triangle_soup){
            Intersection hit_temp = Intersect(ray, &t);
            if(hit_temp.dist < minDist) {
                minDist = hit_temp.dist;
                hit = hit_temp;
            }
        }

        return hit;
    }

    glm::vec3 FindColor(Intersection hit, int recursion_depth, RTScene* scene) {
        glm::vec4 color;
        Material* m;
        
        if(hit.dist == INFINITY) { // if color not visible
            color = glm::vec4(0);
        } else { // Shading model
            m = hit.triangle -> material;
            color = m -> emision;
            int visible;
            glm::vec3 specular;
            glm::vec3 diffuse;
            glm::vec3 currentlight;
            glm::vec4 lightposition;
            glm::vec3 lightdirection;
            glm::vec3 halfwaydirection;
            glm::vec3 viewdirection;
            Ray raytolight;
            Intersection checkShadow;            
            int nlights = (scene -> light).size();
            std::vector<glm::vec4> lightpositions;
            std::vector<glm::vec4> lightcolors;
            for (std::pair<std::string, Light*> entry : scene -> light){
                lightpositions.push_back((entry.second) -> position);
                lightcolors.push_back((entry.second) -> color);
            }

            for(int i=0; i<nlights; i++) {
                currentlight = glm::vec3(m->ambient);
                lightposition = lightpositions[i];
                lightdirection = glm::normalize(glm::vec3(lightposition)-lightposition.w*hit.N);
                diffuse = glm::vec3(m->diffuse)*glm::max(glm::dot(hit.N,lightdirection),0.0f);
                currentlight = currentlight + diffuse;

                // max recursion
                if(recursion_depth == 0) {
                    viewdirection = glm::normalize(-hit.N);
                    halfwaydirection = glm::normalize(lightdirection + viewdirection);
                    specular = glm::vec3(m->specular) * glm::pow(glm::max(glm::dot(hit.N,halfwaydirection), 0.0f),m->shininess);
                    currentlight = currentlight + specular;
                }

                // shadow
                raytolight.p0 = hit.P + 0.2f*lightdirection;
                raytolight.dir = lightdirection;
                float distancetolight = glm::sqrt(glm::pow((lightposition.x-hit.P.x),2) +
                                            glm::pow((lightposition.y-hit.P.y),2) +
                                            glm::pow((lightposition.z-hit.P.z),2));
                checkShadow = Intersect(raytolight, scene);

                visible = 1;
                if(checkShadow.dist < distancetolight) visible = 0;
                
                currentlight.x = visible * currentlight.x * lightcolors[i].x;
                currentlight.y = visible * currentlight.y * lightcolors[i].y;
                currentlight.z = visible * currentlight.z * lightcolors[i].z;
                color = color + glm::vec4(currentlight,0);
            }
            
            // Generate minor-reflected ray
            Ray ray2;
            Intersection hit2;
            if(recursion_depth > 0) {
                ray2.dir = 2.0f * glm::dot(hit.N,hit.V)*hit.N - hit.V;
                ray2.p0 = hit.P + 0.1f*ray2.dir;
                hit2 = Intersect(ray2, scene);

                specular = FindColor(hit2, recursion_depth-1, scene);
                color = color + glm::vec4(specular,0);
            }
        }
        return color;
    }
    void Raytrace(Camera cam, RTScene scene, Image &image) {
        int w = image.width; int h = image.height;
        std::cout<<"Processing ray tracing image...\n";
        for (int j=0; j<h; j++){ 
            for (int i=0; i<w; i++){ 
                Ray ray = RayThruPixel(cam, i, j, w, h);
                Intersection hit = Intersect(ray, &scene);
                image.pixels[i+j*w] = FindColor(hit, 3, &scene);
            }
        }
        std::cout<<"done...\n";
    }
}
