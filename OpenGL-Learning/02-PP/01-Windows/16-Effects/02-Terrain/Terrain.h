#pragma once

//Windows Header Files
#include <windows.h>
#include <stdio.h> //For File IO
#include <stdlib.h> //For exit()
#include <wingdi.h>
#include <vector>

// OPENGL Header Files
#include<gl/GLEW.h> //This must be before GL.h

#include "vmath.h"
#include "Transform.h"
#include "NoiseGenerator.h"
#include "OGL.h"

//Link with OpenGL library
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib")

using namespace vmath;

class Terrain
{
    
    private:
        Transform modelTransform;
        NoiseGenerator *noiseGenerator;
    
        unsigned int vao;
        unsigned int vbo;
        unsigned int vbo_normal;
        unsigned int vbo_index;

        float startX;
        float startY;
        float endX;
        float endY;

        unsigned int subX;
        unsigned int subY;

        float *vertices = NULL;
        float *indices = NULL;
        float *normals = NULL;

        unsigned int numberIndices;
        unsigned int numberVertices;

        std::vector<std::vector<float> > heightMap;
        std::vector<std::vector<std::vector<float> > > heightMapNormals;

};

