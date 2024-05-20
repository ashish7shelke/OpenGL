#pragma once
#include <windows.h>
#include <stdio.h> //For File IO
#include <stdlib.h> //For exit()
#include <wingdi.h>
#include <math.h>

// OPENGL Header Files
#include<gl/GL.h>
#include<gl/GLU.h>

#define Scene0 0

GLuint nFontListMarathi = 0;
GLuint nFontListEnglish = 0;

GLuint nFontList;
GLuint nFontList1;

extern BOOL loadGlTexture(GLuint*, TCHAR[]);
extern FILE* gpFile;
extern GLfloat intro_timer;
extern GLfloat fTextMoveYAxis;

extern BOOL bScene0Complete;
extern BOOL bScene1Complete;
extern BOOL bScene2Complete;

extern GLfloat fCamx;
extern GLfloat fCamy;
extern GLfloat fCamz;

int setup3DMarathiTextRC(HDC hDC)
{
	// Setup the font characteristics
	HFONT hFont;
	GLYPHMETRICSFLOAT agmf[128];
	LOGFONT logfont;

	logfont.lfHeight = -5.0f;
	logfont.lfWidth = 0.0f;
	logfont.lfEscapement = 0.0f;
	logfont.lfOrientation = 0.0f;
	logfont.lfWeight = FW_BOLD;
	logfont.lfItalic = FALSE;
	logfont.lfUnderline = FALSE;
	logfont.lfStrikeOut = FALSE;
	logfont.lfCharSet = ANSI_CHARSET;
	logfont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	logfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logfont.lfQuality = DEFAULT_QUALITY;
	logfont.lfPitchAndFamily = DEFAULT_PITCH;

	//strcpy(logfont.lfFaceName, "Kruti Dev 010");
	strcpy(logfont.lfFaceName, "Shivaji01");


	// Create the font and display list
	hFont = CreateFontIndirect(&logfont);
	SelectObject(hDC, hFont);

	// Create display lists for glyphs 0 through 128 with 0.1 extrusion and default deviation
	// Display list numbering starts at 1000 (It could be any number)
	nFontListMarathi = glGenLists(128);

	if((GL_INVALID_OPERATION == nFontListMarathi) || (GL_INVALID_VALUE == nFontListMarathi))
		return -1;

	wglUseFontOutlines(hDC, 0, 128, nFontListMarathi, 0.0f, 0.05f, WGL_FONT_POLYGONS, agmf);

	DeleteObject(hFont);

	return 0;
}


int setup3DEnglishTextRC(HDC hDC)
{
	// Setup the font characteristics
	HFONT hFont;
	GLYPHMETRICSFLOAT agmf[128];
	LOGFONT logfont;

	logfont.lfHeight = -5.0f;
	logfont.lfWidth = 0.0f;
	logfont.lfEscapement = 0.0f;
	logfont.lfOrientation = 0.0f;
	logfont.lfWeight = FW_BOLD;
	logfont.lfItalic = FALSE;
	logfont.lfUnderline = FALSE;
	logfont.lfStrikeOut = FALSE;
	logfont.lfCharSet = ANSI_CHARSET;
	logfont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	logfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logfont.lfQuality = DEFAULT_QUALITY;
	logfont.lfPitchAndFamily = DEFAULT_PITCH;

	//strcpy(logfont.lfFaceName, "Kruti Dev 010");
	strcpy(logfont.lfFaceName, "Arial");


	// Create the font and display list
	hFont = CreateFontIndirect(&logfont);
	SelectObject(hDC, hFont);

	// Create display lists for glyphs 0 through 128 with 0.1 extrusion and default deviation
	// Display list numbering starts at 1000 (It could be any number)
	nFontListEnglish = glGenLists(128);

	if((GL_INVALID_OPERATION == nFontListEnglish) || (GL_INVALID_VALUE == nFontListEnglish))
		return -1;

	wglUseFontOutlines(hDC, 0, 128, nFontListEnglish, 0.0f, 0.05f, WGL_FONT_POLYGONS, agmf);

	DeleteObject(hFont);

	return 0;
}

void printString(char* s, GLfloat Red, GLfloat Greeen, GLfloat Blue, GLfloat xPos, GLfloat yPos, GLfloat zPos, GLuint fontList);
void Scene0Update();
void Scene0Packaging();
