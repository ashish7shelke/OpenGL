#pragma once
#include <windows.h>
#include <stdio.h> //For File IO
#include <stdlib.h> //For exit()
#include <wingdi.h>
#include <math.h>

// OPENGL Header Files
#include<gl/GL.h>
#include<gl/GLU.h>

#include "Particle.h"
#include "TextureMacros.h"
#include "Human.h"

#define Scene1 1

float	xspeed;						// Base X Speed (To Allow Keyboard Direction Of Tail)
float	yspeed;						// Base Y Speed (To Allow Keyboard Direction Of Tail)
float	zspeed;	
float	zoom= 0.0f;				// Used To Zoom Out

float posX = 0.0f; 
float posY = -5.5f; 
float posZ = 0.0f;

float gravX = 0.0f;
float gravY = 0.0f;
float gravZ = 0.0f;

float fTextureTileFraction0 = 0.0f;
float fTextureTileFraction1 = 1.0;

//Texture Object
GLuint texture_floor = 0;
GLuint texture_village_floor = 0;
GLuint texture_water = 0;
GLuint texture_background = 0;
GLuint texture_city_road = 0;
GLuint texture_village = 0;

float camSpeed = 4.2f;//2.5f;
float speed = 5.0f * 4.2f;

BOOL gbWalkEnable = TRUE;

extern BOOL bScene0Complete;
extern BOOL bScene1Complete;
extern BOOL bScene2Complete;

/*float fV1[3] = {1.0f, fHutWallHeight, 1.0f};
float fV2[3] = {-1.0f, fHutWallHeight, 1.0f};
float fV3[3] = {-1.0f, -1.0f, 1.0f};
float fV4[3] = {1.0f, -1.0f, 1.0f};

float bV1[3] = {1.0f, fHutWallHeight, -1.0f};
float bV2[3] = {-1.0f, fHutWallHeight, -1.0f};
float bV3[3] = {-1.0f, -1.0f, -1.0f};
float bV4[3] = {1.0f, -1.0f, -1.0f};*/

extern BOOL loadGlTexture(GLuint*, TCHAR[]);
extern FILE* gpFile;

int Scene1Initialize();
void Scene1Uninitialize();

void Scene1Vasana();
void Scene1Update();

void DrawGrid(void);
void DrawWater(void);

void InitializeParticles();
void DrawWaterParticle();
