#pragma once

#define MYICON 101 // Unique integer value define macro
#define MYSOUND 102


//LINK WITH AUDIO LIBRARY
#pragma comment(lib,"winmm.lib")

#include "TextureMacros.h"
#include "LinkedList.h"

BOOL bScene0Complete = FALSE;
BOOL bScene1Complete = FALSE;
BOOL bScene2Complete = FALSE;

GLfloat intro_timer = 0.0f;
GLfloat intro_timer2 = 0.0f;
GLfloat fTextMoveYAxis = -1.0f;

PNODE HEAD = NULL;
BOOL bListResult = FALSE;

extern void Scene0Packaging();
extern void Scene1Vasana();
extern void Scene2Packaging();
