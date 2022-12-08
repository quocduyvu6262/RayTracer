/**************************************************
RTCube is subclass class of RTGeometry
that represents a ray tracing 3D cube.
*****************************************************/
#include "RTGeometry.h"
#include "Triangle.h"
#ifndef __CUBE_H__
#define __CUBE_H__

class RTCube : public RTGeometry {
public:

    void init(void){
        // vertex positions
        const GLfloat positions[24][3] ={
            // Front face
            { -0.5f, -0.5f, 0.5f },{ -0.5f, 0.5f, 0.5f },{ 0.5f, 0.5f, 0.5f },{ 0.5f, -0.5f, 0.5f },
            // Back face
            { -0.5f, -0.5f, -0.5f },{ -0.5f, 0.5f, -0.5f },{ 0.5f, 0.5f, -0.5f },{ 0.5f, -0.5f, -0.5f },
            // Left face
            { -0.5f, -0.5f, 0.5f },{ -0.5f, 0.5f, 0.5f },{ -0.5f, 0.5f, -0.5f },{ -0.5f, -0.5f, -0.5f },
            // Right face
            { 0.5f, -0.5f, 0.5f },{ 0.5f, 0.5f, 0.5f },{ 0.5f, 0.5f, -0.5f },{ 0.5f, -0.5f, -0.5f },
            // Top face
            { 0.5f, 0.5f, 0.5f },{ -0.5f, 0.5f, 0.5f },{ -0.5f, 0.5f, -0.5f },{ 0.5f, 0.5f, -0.5f },
            // Bottom face
            { 0.5f, -0.5f, 0.5f },{ -0.5f, -0.5f, 0.5f },{ -0.5f, -0.5f, -0.5f },{ 0.5f, -0.5f, -0.5f }
        };
        // vertex normals
        const GLfloat normals[24][3] = {
            // Front face
            { 0.0f, 0.0f, 1.0f },{ 0.0f, 0.0f, 1.0f },{ 0.0f, 0.0f, 1.0f },{ 0.0f, 0.0f, 1.0f },
            // Back face
            { 0.0f, 0.0f, -1.0f },{ 0.0f, 0.0f, -1.0f },{ 0.0f, 0.0f, -1.0f },{ 0.0f, 0.0f, -1.0f },
            // Left face
            { -1.0f, 0.0f, 0.0f },{ -1.0f, 0.0f, 0.0f },{ -1.0f, 0.0f, 0.0f },{ -1.0f, 0.0f, 0.0f },
            // Right face
            { 1.0f, 0.0f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 1.0f, 0.0f, 0.0f },
            // Top face
            { 0.0f, 1.0f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 1.0f, 0.0f },
            // Bottom face
            { 0.0f, -1.0f, 0.0f },{ 0.0f, -1.0f, 0.0f },{ 0.0f, -1.0f, 0.0f },{ 0.0f, -1.0f, 0.0f }
        };
        // Cube indices
        const GLuint indices[36] = {
            0, 1, 2, 0, 2, 3, // Front face
            4, 5, 6, 4, 6, 7, // Back face
            8, 9, 10, 8, 10, 11, // Left face
            12, 13, 14, 12, 14, 15, // Right face
            16, 17, 18, 16, 18, 19, // Top face
            20, 21, 22, 20, 22, 23 // Bottom face
        };

        // fill in triangles
        int pos_index = 0;
        int nor_index = 0;
        count = sizeof(indices) / sizeof(indices[0]);
        for (unsigned int i=0; i<count/3; i++){
            Triangle toAddTriangle; 
            toAddTriangle.P.push_back(glm::vec3(positions[indices[pos_index]][0],positions[indices[pos_index]][1],positions[indices[pos_index]][2]));
            pos_index += 1;
            toAddTriangle.P.push_back(glm::vec3(positions[indices[pos_index]][0],positions[indices[pos_index]][1],positions[indices[pos_index]][2]));
            pos_index += 1;
            toAddTriangle.P.push_back(glm::vec3(positions[indices[pos_index]][0],positions[indices[pos_index]][1],positions[indices[pos_index]][2]));
            pos_index += 1;

            toAddTriangle.N.push_back(glm::vec3(normals[indices[nor_index]][0], normals[indices[nor_index]][1], normals[indices[nor_index]][2]));
            nor_index += 1;
            toAddTriangle.N.push_back(glm::vec3(normals[indices[nor_index]][0], normals[indices[nor_index]][1], normals[indices[nor_index]][2]));
            nor_index += 1;
            toAddTriangle.N.push_back(glm::vec3(normals[indices[nor_index]][0], normals[indices[nor_index]][1], normals[indices[nor_index]][2]));
            nor_index += 1;
            
            elements.push_back(toAddTriangle);
        }

        
    }
};

#endif 
