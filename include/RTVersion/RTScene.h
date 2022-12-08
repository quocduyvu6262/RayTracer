/**************************************************
Ray tracing version of Scene
*****************************************************/
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_normalized_axis.hpp>
#include <math.h>

#include <stdlib.h>
#include <vector>
#include <string>
#include <map>
#include <stack>

#include "Camera.h"
#include "SurfaceShader.h"
#include "Light.h"
#include "RTGeometry.h"
#include "Material.h"
#include "RTModel.h"
#include "Triangle.h"

#ifndef __RTSCENE_H__
#define __RTSCENE_H__

class RTNode {
public:
    std::vector< RTNode* > childnodes;
    std::vector< glm::mat4 > childtransforms;
    std::vector< RTModel* > models;
    std::vector< glm::mat4 > modeltransforms;
};

class RTScene {
public:
    Camera* camera;
    SurfaceShader* shader;
    // The following are containers of objects serving as the object palettes.
    // The containers store pointers so that they can also store derived class objects.
    std::map< std::string, RTGeometry* > geometry;
    std::map< std::string, Material* > material;
    std::map< std::string, RTModel* > model;
    std::map< std::string, Light* > light;

    // list of triangles in world or camera coordinates
    std::vector<Triangle> triangle_soup;
    // The container of nodes will be the scene graph after we connect the nodes by setting the child_nodes.
    std::map< std::string, RTNode* > node;
    
    RTScene(){
        // the default scene graph already has one node named "world."
        node["world"] = new RTNode;
    }
    
    void init( void );
    void buildTriangleSoup();
    
    // destructor
    ~RTScene(){
        // The containers of pointers own the object pointed to by the pointers.
        // All the objects should be deleted when the object palette is destructed.
        // light
        for(std::pair<std::string,Light*> entry : light ){
            delete entry.second;
        }
        // geometry
        for(std::pair<std::string,RTGeometry*> entry : geometry ){
            delete entry.second;
        }
        // material
        for(std::pair<std::string,Material*> entry : material ){
            delete entry.second;
        }
        // model
        for(std::pair<std::string,RTModel*> entry : model ){
            delete entry.second;
        }
        // model
        for(std::pair<std::string,RTNode*> entry : node ){
            delete entry.second;
        }
        delete camera;
        delete shader;
    }
};

#endif 
