/**************************************************
Ray tracing Obj is subclass class of RTGeometry
that loads an obj file.
*****************************************************/
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include "Geometry.h"
#ifndef __OBJ_H__
#define __OBJ_H__

class RTObj : public RTGeometry {
public:

    void init(const char * filename);
        
};

#endif 
