/*******************************
Ray class
*******************************/

#include <glm/glm.hpp>
#ifndef __RAY_H__
#define __RAY_H__
class Ray{
public:
    glm::vec3 p0; // base point
    glm::vec3 dir; // direction
};

#endif