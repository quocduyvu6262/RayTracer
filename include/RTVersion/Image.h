/************************************
Image class that contains the arrays 
of pixels where we store the colors
*************************************/

#include <vector>
#include <glm/glm.hpp>
#include <OpenGL/gl3.h>
#ifndef __IMAGE_H__
#define __IMAGE_H__

unsigned int fbo; // frame buffer object
unsigned int tbo; // texture buffer object

class Image {
public:
    // Instance variables
    int width;
    int height;
    std::vector<glm::vec3> pixels;

    /**
     * @brief Construct a new Image object
     * @param width 
     * @param height 
     */
    Image(int w, int h){
        this -> width = w;
        this -> height = h;
    }

    /**
     * @brief Initializes the size of the array 
     * pixels to width times height 
     */
    void init(){
        this -> pixels.resize(this -> width * this -> height, glm::vec3(0.0));
        glGenFramebuffers(1,&fbo);
        glGenTextures(1,&tbo);
    }

    /**
     * @brief Pour the data of the pixels into the texture buffer,
     * and attach the texture to the framebuffer
     */
    void draw(){
        glBindTexture(GL_TEXTURE_2D, tbo);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height, // load texture
                    0,GL_RGB,GL_FLOAT, &pixels[0][0]);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_READ_FRAMEBUFFER,GL_COLOR_ATTACHMENT0, 
                                GL_TEXTURE_2D, tbo, 0);// attach texture and the read frame
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // if not already so
        glBlitFramebuffer(0,0,width,height,0,0,width,height,GL_COLOR_BUFFER_BIT,
                            GL_NEAREST); // copy framebuffer from read to write
    }
};

#endif