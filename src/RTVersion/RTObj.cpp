/**************************************************
Obj is subclass class of Geometry
that loads an obj file.
 This obj file loader only supports vertex positions and normals:
 v   x y z
 vn nx ny nz
 f 123//456
 i.e. there is no texture.
*****************************************************/
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include "../../include/RTVersion/RTObj.h"
#include "../../include/RTVersion/Triangle.h"

void RTObj::init(const char * filename){
    std::vector< glm::vec3 > temp_vertices, vertices;
    std::vector< glm::vec3 > temp_normals, normals;
    std::vector< unsigned int > temp_vertexIndices, indices;
    std::vector< unsigned int > temp_normalIndices;
    
    // load obj file
    FILE * file = fopen( filename , "r" );
    if( file == NULL ){
        std::cerr << "Cannot open file: " << filename << std::endl;
        exit(-1);
    }
    std::cout << "Loading " << filename << "...";
    while (!feof(file)){
        char lineHeader[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.

        // else : parse lineHeader
        if ( strcmp( lineHeader, "v" ) == 0 ){
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
            temp_vertices.push_back(vertex);
        }else if ( strcmp( lineHeader, "vn" ) == 0 ){
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
            temp_normals.push_back(normal);
        }else if ( strcmp( lineHeader, "f" ) == 0 ){
            //std::string vertex1, vertex2, vertex3;
            unsigned int vertexIndex[3], normalIndex[3];
            fscanf(file, "%d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2] );
            temp_vertexIndices.push_back(vertexIndex[0]);
            temp_vertexIndices.push_back(vertexIndex[1]);
            temp_vertexIndices.push_back(vertexIndex[2]);
            temp_normalIndices.push_back(normalIndex[0]);
            temp_normalIndices.push_back(normalIndex[1]);
            temp_normalIndices.push_back(normalIndex[2]);
        }
    }
    std::cout << "done." << std::endl;
    
    // post processing
    std::cout << "Processing data...";
    unsigned int n = temp_vertexIndices.size(); // #(triangles)*3
    vertices.resize(n);
    normals.resize(n);
    indices.resize(n);
    for (unsigned int i = 0; i<n; i++){
        indices[i] = i;
        vertices[i] = temp_vertices[ temp_vertexIndices[i] - 1 ];
        normals[i] = temp_normals[ temp_normalIndices[i] - 1 ];
    }
    std::cout << "done." << std::endl;
    
    // fill in triangles
    std::cout << "Setting up triangles...";
    count = n/3;
    int pos_index = 0;
    int nor_index = 0;
    count = 24 / 3;
    for (unsigned int i=0; i<count; i++){
        Triangle toAddTriangle;

        int index = pos_index;
        toAddTriangle.P.push_back(glm::vec3(vertices[indices[index]][0],vertices[indices[index]][1],vertices[indices[index]][2]));
        index = pos_index++;
        toAddTriangle.P.push_back(glm::vec3(vertices[indices[index]][0],vertices[indices[index]][1],vertices[indices[index]][2]));
        index = pos_index++;
        toAddTriangle.P.push_back(glm::vec3(vertices[indices[index]][0],vertices[indices[index]][1],vertices[indices[index]][2]));

        index = nor_index;
        toAddTriangle.N.push_back(glm::vec3(normals[indices[index]][0],normals[indices[index]][1],normals[indices[index]][2]));
        index = nor_index++;
        toAddTriangle.N.push_back(glm::vec3(normals[indices[index]][0],normals[indices[index]][1],normals[indices[index]][2]));
        index = nor_index++;
        toAddTriangle.N.push_back(glm::vec3(normals[indices[index]][0],normals[indices[index]][1],normals[indices[index]][2]));

        elements.push_back(toAddTriangle);
    }
    std::cout << "done.\n";
}


