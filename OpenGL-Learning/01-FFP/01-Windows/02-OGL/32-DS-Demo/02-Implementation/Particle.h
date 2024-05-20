#pragma once

#define MAX_PARTICLES 30000	//Number of particles
#define PARTICLE_SIDE 0.5f
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
static float blue[3] =	{0.9f,0.9f,0.94f};
static float blue_dark[3] =	{0.7f,0.7f,0.7f};
static float blue_polluted[3] =	{152.0f/255.0f,135.0f/255.0f,158.0f/255.0f}; //152,133,133
