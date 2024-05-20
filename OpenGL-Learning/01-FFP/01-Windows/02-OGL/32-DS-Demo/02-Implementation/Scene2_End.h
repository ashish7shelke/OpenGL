#pragma once
#include <windows.h>
#include <stdio.h> //For File IO
#include <stdlib.h> //For exit()
#include <wingdi.h>
#include <math.h>

// OPENGL Header Files
#include<gl/GL.h>
#include<gl/GLU.h>

#define Scene2 2

extern GLuint nFontListMarathi;
extern GLuint nFontListEnglish;

extern BOOL loadGlTexture(GLuint*, TCHAR[]);
extern FILE* gpFile;
extern GLfloat intro_timer2;

extern BOOL bScene0Complete;
extern BOOL bScene1Complete;
extern BOOL bScene2Complete;

extern GLfloat fCamx;
extern GLfloat fCamy;
extern GLfloat fCamz;
extern int setup3DMarathiTextRC(HDC hDC);

extern int setup3DEnglishTextRC(HDC hDC);
extern void printString(char* s, GLfloat Red, GLfloat Greeen, GLfloat Blue, GLfloat xPos, GLfloat yPos, GLfloat zPos, GLuint fontList);
