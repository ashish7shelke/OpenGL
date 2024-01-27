#pragma once

#define MAX_PARTICLES 10000	//Number of particles

typedef struct                      // Create A Structure For Particle
{
    BOOL    active;                 // Active (Yes/No)
    float   life;                   // Particle Life
    float   fade;                   // Fade Speed
    float   r;                  // Red Value
    float   g;                  // Green Value
    float   b;                  // Blue Value
    float   x;                  // X Position
    float   y;                  // Y Position
    float   z;                  // Z Position
    float   xi;                 // X Direction
    float   yi;                 // Y Direction
    float   zi;                 // Z Direction
    float   xg;                 // X Gravity
    float   yg;                 // Y Gravity
    float   zg;                 // Z Gravity
}particles;                      // Particles Structure

particles particle[MAX_PARTICLES];          // Particle Array (Room For Particle Info)

static float colors[12][3]=               // Rainbow Of Colors
{
    {1.0f,0.5f,0.5f},{1.0f,0.75f,0.5f},{1.0f,1.0f,0.5f},{0.75f,1.0f,0.5f},
    {0.5f,1.0f,0.5f},{0.5f,1.0f,0.75f},{0.5f,1.0f,1.0f},{0.5f,0.75f,1.0f},
    {0.5f,0.5f,1.0f},{0.75f,0.5f,1.0f},{1.0f,0.5f,1.0f},{1.0f,0.5f,0.75f}
};

static float white[3] =	{1.0f,1.0f,1.0f};
static float blue[3] =	{0.0f,0.0f,1.0f};
static float yellow[3] =	{1.0f,1.0f,0.0f};
static float orange[3] =	{1.0f,0.5f,0.0f};
static float red[3] =		{1.0f,0.1f,0.0f};
