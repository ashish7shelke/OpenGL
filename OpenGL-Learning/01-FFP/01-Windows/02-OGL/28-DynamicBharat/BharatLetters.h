#pragma once

extern GLfloat fBaseDepth;
extern void DrawPartialCircle(float fXCenterPos, float fYCenterPos, float fRadius, int firstPos, int secondPos, float* fColorRgbV, float* fColorRgbV2, int);
extern void DrawPartialTirangaCircle(float fXCenterPos, float fYCenterPos, float fRadius, int firstPos, int secondPos, float* fColorRgbV, float* fColorRgbV2, int);
extern GLfloat fAlphaFactor;

//Saffron 255, 103, 31 / 0, 60, 88, 0
//White 255, 255, 255 / 0, 0, 0, 0
//Green 4, 106, 56 / 96, 0, 47, 58
//Blue 6, 3, 141 / 96, 98, 0, 45

BOOL bBletterDisplayed = FALSE;
BOOL bHletterDisplayed = FALSE;
BOOL bA1letterDisplayed = FALSE;
BOOL bRletterDisplayed = FALSE;
BOOL bA2letterDisplayed = FALSE;
BOOL bTletterDisplayed = FALSE;

BOOL bBletterTirangaDisplayed = FALSE;
BOOL bHletterTirangaDisplayed = FALSE;
BOOL bA1letterTirangaDisplayed = FALSE;
BOOL bRletterTirangaDisplayed = FALSE;
BOOL bA2letterTirangaDisplayed = FALSE;
BOOL bTletterTirangaDisplayed = FALSE;

GLfloat Saffron[4] = { 255.0f / 255.0f, 103.0f / 255.0f, 31.0f / 255.0f };
GLfloat White[4] = { 255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f };
GLfloat Green[4] = { 4.0f / 255.0f, 106.0f / 255.0f, 56.0f / 255.0f };
GLfloat Blue[4] = { 6.0f / 255.0f, 3.0f / 255.0f, 141.0f / 255.0f };

GLfloat SaffronSmoke[4] = { 255.0f / 255.0f, 103.0f / 255.0f, 31.0f / 255.0f, 0.2f };

GLfloat LetterColor[4] = { 128.0/255.0,128.0/255.0,128.0/255.0,1.0};

float fLetterStickHalfWidth = 0.5f;
float fLetterStickHalfHeight = 5.0f;
float fLetterHalfWidth = 2.0f;

float fTirangaLetterHeight = 1.0f;

void DrawB()
{
	//Draw B 
	glPushMatrix();

	glTranslatef(-fLetterHalfWidth, 0.0f, 0.0f);

	//0.1f adjusted to remove top extra top corner
	glBegin(GL_QUADS);
	glColor4fv(LetterColor);
	glVertex3f(fLetterStickHalfWidth, fLetterStickHalfHeight, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f(-fLetterStickHalfWidth, fLetterStickHalfHeight, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f(-fLetterStickHalfWidth, 0.0f, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f(fLetterStickHalfWidth, 0.0f, 0.0f);
	glEnd();

	glBegin(GL_QUADS);
	glColor4fv(LetterColor);
	glVertex3f(fLetterStickHalfWidth, 0.0f, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f(-fLetterStickHalfWidth, 0.0f, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f(-fLetterStickHalfWidth, -fLetterStickHalfHeight, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f(fLetterStickHalfWidth, -fLetterStickHalfHeight, 0.0f);//0.1f adjusted to remove top extra top corners
	glEnd();

	if(bBletterTirangaDisplayed)
	{
		glBegin(GL_QUADS);
		glColor4fv(Saffron);
		glVertex3f(fLetterStickHalfWidth, fTirangaLetterHeight, 0.0f);
		glColor4fv(Saffron);
		glVertex3f(-fLetterStickHalfWidth, fTirangaLetterHeight, 0.0f);
		glColor4fv(White);
		glVertex3f(-fLetterStickHalfWidth, 0.0f, 0.0f);
		glColor4fv(White);
		glVertex3f(fLetterStickHalfWidth, 0.0f, 0.0f);
		glEnd();

		glBegin(GL_QUADS);
		glColor4fv(White);
		glVertex3f(fLetterStickHalfWidth, 0.0f, 0.0f);
		glColor4fv(White);
		glVertex3f(-fLetterStickHalfWidth, 0.0f, 0.0f);
		glColor4fv(Green);
		glVertex3f(-fLetterStickHalfWidth, -fTirangaLetterHeight, 0.0f);
		glColor4fv(Green);
		glVertex3f(fLetterStickHalfWidth, -fTirangaLetterHeight, 0.0f);//0.1f adjusted to remove top extra top corners
		glEnd();		
	}


	glPushMatrix();
	glTranslatef(0.0f, (fLetterStickHalfHeight / 2.0f), 0.0f);
	DrawPartialCircle(0.0f, 0.0f, (fLetterStickHalfHeight / 2.0f) - fLetterStickHalfWidth+ 0.05f, 283.0f, 78.0f, White, Saffron, 4);
	if(bBletterTirangaDisplayed)
	{
		DrawPartialTirangaCircle(0.0f, 0.0f, (fLetterStickHalfHeight / 2.0f) - fLetterStickHalfWidth+ 0.05f, 283.0f, 315.0f, White, Saffron, 4);
	}

	glPopMatrix();
	glTranslatef(0.0f, -fLetterStickHalfHeight / 2.0f, 0.0f);
	DrawPartialCircle(0.0f, 0.0f, (fLetterStickHalfHeight / 2.0f) - fLetterStickHalfWidth+ 0.05f, 283.0f, 78.0f, Green, White, 3);
	if(bBletterTirangaDisplayed)
	{
		DrawPartialTirangaCircle(0.0f, 0.0f, (fLetterStickHalfHeight / 2.0f) - fLetterStickHalfWidth+ 0.05f, 45.0f, 78.0f, Green, White, 3);
	}
	glPopMatrix();
}

void DrawH()
{
	//Draw B 
	glPushMatrix();

	glTranslatef(-fLetterHalfWidth, 0.0f, 0.0f);

	float fColorGradient = 0.75f;
	float fColor[4];
	fColor[0] = (Saffron[0] * (1.0f - fColorGradient)) + (White[0] * fColorGradient);
	fColor[1] = (Saffron[1] * (1.0f - fColorGradient)) + (White[1] * fColorGradient);
	fColor[2] = (Saffron[2] * (1.0f - fColorGradient)) + (White[2] * fColorGradient);
	fColor[3] = 1.0f;

	glBegin(GL_QUADS);
	glColor4fv(LetterColor);
	glVertex3f(0.0f + fLetterHalfWidth*2.0, fLetterStickHalfHeight / 4.0f, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f(-0.0f, fLetterStickHalfHeight / 4.0f, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f(-0.0f, 0.0f, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f(0.0 + fLetterHalfWidth*2.0, 0.0f, 0.0f);
	glEnd();

	if(bHletterTirangaDisplayed)
	{
		glBegin(GL_QUADS);
		glColor4fv(Saffron);
		glVertex3f(0.0f + fLetterHalfWidth*2.0, fTirangaLetterHeight, 0.0f);
		glColor4fv(Saffron);
		glVertex3f(-0.0f, fTirangaLetterHeight, 0.0f);
		glColor4fv(White);
		glVertex3f(-0.0f, 0.0f, 0.0f);
		glColor4fv(White);
		glVertex3f(0.0 + fLetterHalfWidth*2.0, 0.0f, 0.0f);
		glEnd();		
	}

	fColor[0] = (Green[0] * (1.0f - fColorGradient)) + (White[0] * fColorGradient);
	fColor[1] = (Green[1] * (1.0f - fColorGradient)) + (White[1] * fColorGradient);
	fColor[2] = (Green[2] * (1.0f - fColorGradient)) + (White[2] * fColorGradient);
	fColor[3] = 1.0f;

	glBegin(GL_QUADS);
	glColor4fv(LetterColor);
	glVertex3f(0.0f + fLetterHalfWidth*2.0, 0.0f, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f(-0.0f, 0.0f, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f(-0.0f, -fLetterStickHalfHeight / 4.0f, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f(0.0f + fLetterHalfWidth*2.0, -fLetterStickHalfHeight / 4.0f, 0.0f);
	glEnd();

	if(bHletterTirangaDisplayed)
	{
		glBegin(GL_QUADS);
		glColor4fv(White);
		glVertex3f(0.0f + fLetterHalfWidth*2.0, 0.0f, 0.0f);
		glColor4fv(White);
		glVertex3f(-0.0f, 0.0f, 0.0f);
		glColor4fv(Green);
		glVertex3f(-0.0f, -fTirangaLetterHeight, 0.0f);
		glColor4fv(Green);
		glVertex3f(0.0f + fLetterHalfWidth*2.0, -fTirangaLetterHeight, 0.0f);
		glEnd();	
	}	

	glBegin(GL_QUADS);
	glColor4fv(LetterColor);
	glVertex3f(fLetterStickHalfWidth, fLetterStickHalfHeight, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f(-fLetterStickHalfWidth, fLetterStickHalfHeight, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f(-fLetterStickHalfWidth, 0.0f, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f(fLetterStickHalfWidth, 0.0f, 0.0f);
	glEnd();

	if(bHletterTirangaDisplayed)
	{
		glBegin(GL_QUADS);
		glColor4fv(Saffron);
		glVertex3f(fLetterStickHalfWidth, fTirangaLetterHeight, 0.0f);
		glColor4fv(Saffron);
		glVertex3f(-fLetterStickHalfWidth, fTirangaLetterHeight, 0.0f);
		glColor4fv(White);
		glVertex3f(-fLetterStickHalfWidth, 0.0f, 0.0f);
		glColor4fv(White);
		glVertex3f(fLetterStickHalfWidth, 0.0f, 0.0f);
		glEnd();
	}

	glBegin(GL_QUADS);
	glColor4fv(LetterColor);
	glVertex3f(fLetterStickHalfWidth, 0.0f, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f(-fLetterStickHalfWidth, 0.0f, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f(-fLetterStickHalfWidth, -fLetterStickHalfHeight, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f(fLetterStickHalfWidth, -fLetterStickHalfHeight, 0.0f);
	glEnd();

	if(bHletterTirangaDisplayed)
	{
		glBegin(GL_QUADS);
		glColor4fv(White);
		glVertex3f(fLetterStickHalfWidth, 0.0f, 0.0f);
		glColor4fv(White);
		glVertex3f(-fLetterStickHalfWidth, 0.0f, 0.0f);
		glColor4fv(Green);
		glVertex3f(-fLetterStickHalfWidth, -fTirangaLetterHeight, 0.0f);
		glColor4fv(Green);
		glVertex3f(fLetterStickHalfWidth, -fTirangaLetterHeight, 0.0f);
		glEnd();
	}

	glPushMatrix();

	glTranslatef(fLetterHalfWidth*2, 0.0f, 0.0f);
	glBegin(GL_QUADS);
	glColor4fv(LetterColor);
	glVertex3f(fLetterStickHalfWidth, fLetterStickHalfHeight, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f(-fLetterStickHalfWidth, fLetterStickHalfHeight, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f(-fLetterStickHalfWidth, 0.0f, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f(fLetterStickHalfWidth, 0.0f, 0.0f);
	glEnd();

	if(bHletterTirangaDisplayed)
	{
		glBegin(GL_QUADS);
		glColor4fv(Saffron);
		glVertex3f(fLetterStickHalfWidth, fTirangaLetterHeight, 0.0f);
		glColor4fv(Saffron);
		glVertex3f(-fLetterStickHalfWidth, fTirangaLetterHeight, 0.0f);
		glColor4fv(White);
		glVertex3f(-fLetterStickHalfWidth, 0.0f, 0.0f);
		glColor4fv(White);
		glVertex3f(fLetterStickHalfWidth, 0.0f, 0.0f);
		glEnd();
	}	

	glBegin(GL_QUADS);
	glColor4fv(LetterColor);
	glVertex3f(fLetterStickHalfWidth, 0.0f, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f(-fLetterStickHalfWidth, 0.0f, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f(-fLetterStickHalfWidth, -fLetterStickHalfHeight, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f(fLetterStickHalfWidth, -fLetterStickHalfHeight, 0.0f);
	glEnd();

	if(bHletterTirangaDisplayed)
	{
		glBegin(GL_QUADS);
		glColor4fv(White);
		glVertex3f(fLetterStickHalfWidth, 0.0f, 0.0f);
		glColor4fv(White);
		glVertex3f(-fLetterStickHalfWidth, 0.0f, 0.0f);
		glColor4fv(Green);
		glVertex3f(-fLetterStickHalfWidth, -fTirangaLetterHeight, 0.0f);
		glColor4fv(Green);
		glVertex3f(fLetterStickHalfWidth, -fTirangaLetterHeight, 0.0f);
		glEnd();
	}
	//
	glPopMatrix();

	glPopMatrix();

}

void DrawA(BOOL bEnableTiranga)
{
	//Draw B 
	glPushMatrix();

	glTranslatef(-fLetterHalfWidth, 0.0f, 0.0f);

	glBegin(GL_QUADS);
	glColor4fv(LetterColor);
	glVertex3f(fLetterHalfWidth + fLetterStickHalfWidth, fLetterStickHalfHeight, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f(fLetterHalfWidth - fLetterStickHalfWidth, fLetterStickHalfHeight, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f((fLetterHalfWidth - fLetterStickHalfWidth)/2.0, 0.0f, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f((fLetterHalfWidth + fLetterStickHalfWidth) / 2.0, 0.0f, 0.0f);

	glColor4fv(LetterColor);
	glVertex3f((fLetterHalfWidth + fLetterStickHalfWidth) / 2.0, 0.0f, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f((fLetterHalfWidth - fLetterStickHalfWidth) / 2.0, 0.0f, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f(-fLetterStickHalfWidth, -fLetterStickHalfHeight, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f(fLetterStickHalfWidth, -fLetterStickHalfHeight, 0.0f);
	glEnd();
	
	glPushMatrix();

	glTranslatef(fLetterHalfWidth*2.0f, 0.0f, 0.0f);
	glRotatef(180.0, 0.0f, 1.0f, 0.0f);
	glBegin(GL_QUADS);
	glColor4fv(LetterColor);
	glVertex3f(fLetterHalfWidth + fLetterStickHalfWidth, fLetterStickHalfHeight, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f(fLetterHalfWidth - fLetterStickHalfWidth, fLetterStickHalfHeight, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f((fLetterHalfWidth - fLetterStickHalfWidth) / 2.0, 0.0f, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f((fLetterHalfWidth + fLetterStickHalfWidth) / 2.0, 0.0f, 0.0f);

	glColor4fv(LetterColor);
	glVertex3f((fLetterHalfWidth + fLetterStickHalfWidth) / 2.0, 0.0f, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f((fLetterHalfWidth - fLetterStickHalfWidth) / 2.0, 0.0f, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f(-fLetterStickHalfWidth, -fLetterStickHalfHeight, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f(fLetterStickHalfWidth, -fLetterStickHalfHeight, 0.0f);
	glEnd();

	float fColorGradient = 0.85f;
	float fColor[4];
	fColor[0] = (Saffron[0] * (1.0f - fColorGradient)) + (White[0] * fColorGradient);
	fColor[1] = (Saffron[1] * (1.0f - fColorGradient)) + (White[1] * fColorGradient);
	fColor[2] = (Saffron[2] * (1.0f - fColorGradient)) + (White[2] * fColorGradient);
	fColor[3] = 1.0f;

	glPopMatrix();
	//0.25f adjusted to remove top corners
	glBegin(GL_QUADS);
	glColor4fv(LetterColor);
	glVertex3f((-0.25f + fLetterHalfWidth + fLetterStickHalfWidth + fLetterHalfWidth*2.0f) / 2.0 , fLetterStickHalfHeight / 10.0f, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f((0.25f + fLetterHalfWidth - fLetterStickHalfWidth) / 2.0, fLetterStickHalfHeight / 10.0f, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f((fLetterHalfWidth - fLetterStickHalfWidth) / 2.0, 0.0f, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f((fLetterHalfWidth + fLetterStickHalfWidth + fLetterHalfWidth * 2.0f) / 2.0, 0.0f, 0.0f);
	glEnd();
	if(bEnableTiranga)
	{
		glBegin(GL_QUADS);
		glColor4fv(Saffron);
		glVertex3f((-0.25f + fLetterHalfWidth + fLetterStickHalfWidth + fLetterHalfWidth*2.0f) / 2.0 , fTirangaLetterHeight, 0.0f);
		glColor4fv(Saffron);
		glVertex3f((0.25f + fLetterHalfWidth - fLetterStickHalfWidth) / 2.0, fTirangaLetterHeight, 0.0f);
		glColor4fv(White);
		glVertex3f((fLetterHalfWidth - fLetterStickHalfWidth) / 2.0, 0.0f, 0.0f);
		glColor4fv(White);
		glVertex3f((fLetterHalfWidth + fLetterStickHalfWidth + fLetterHalfWidth * 2.0f) / 2.0, 0.0f, 0.0f);
		glEnd();		
	}

	fColor[0] = (Green[0] * (1.0f - fColorGradient)) + (White[0] * fColorGradient);
	fColor[1] = (Green[1] * (1.0f - fColorGradient)) + (White[1] * fColorGradient);
	fColor[2] = (Green[2] * (1.0f - fColorGradient)) + (White[2] * fColorGradient);
	fColor[3] = 1.0f;

	glBegin(GL_QUADS);
	glColor4fv(LetterColor);
	glVertex3f((fLetterHalfWidth + fLetterStickHalfWidth + fLetterHalfWidth * 2.0f) / 2.0, 0.0f, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f((fLetterHalfWidth - fLetterStickHalfWidth) / 2.0, 0.0f, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f((fLetterHalfWidth - fLetterStickHalfWidth) / 2.0, -fTirangaLetterHeight, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f((fLetterHalfWidth + fLetterStickHalfWidth + fLetterHalfWidth * 2.0f) / 2.0, -fTirangaLetterHeight, 0.0f);
	glEnd();
	if(bEnableTiranga)
	{
		glBegin(GL_QUADS);
		glColor4fv(White);
		glVertex3f((fLetterHalfWidth + fLetterStickHalfWidth + fLetterHalfWidth * 2.0f) / 2.0, 0.0f, 0.0f);
		glColor4fv(White);
		glVertex3f((fLetterHalfWidth - fLetterStickHalfWidth) / 2.0, 0.0f, 0.0f);
		glColor4fv(Green);
		glVertex3f((fLetterHalfWidth - fLetterStickHalfWidth - 0.5f) / 2.0, -fTirangaLetterHeight, 0.0f);
		glColor4fv(Green);
		glVertex3f((fLetterHalfWidth + fLetterStickHalfWidth + 0.5f + fLetterHalfWidth * 2.0f) / 2.0, -fTirangaLetterHeight, 0.0f);
		glEnd();		
	}
	glPopMatrix();

	//Added intensionally As A is taking extra half char width
	//glTranslatef(fLetterHalfWidth, 0.0f, 0.0f);
}

void DrawR()
{
	//Draw B 
	glPushMatrix();

	glTranslatef(-fLetterHalfWidth, 0.0f, 0.0f);

	glBegin(GL_QUADS);
	glColor4fv(LetterColor);
	glVertex3f(fLetterStickHalfWidth, fLetterStickHalfHeight, 0.0f);//0.1f adjusted to remove top extra top corners
	glColor4fv(LetterColor);
	glVertex3f(-fLetterStickHalfWidth, fLetterStickHalfHeight, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f(-fLetterStickHalfWidth, 0.0f, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f(fLetterStickHalfWidth, 0.0f, 0.0f);
	glEnd();

	glBegin(GL_QUADS);
	glColor4fv(LetterColor);
	glVertex3f(fLetterStickHalfWidth, 0.0f, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f(-fLetterStickHalfWidth, 0.0f, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f(-fLetterStickHalfWidth, -fLetterStickHalfHeight, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f(fLetterStickHalfWidth, -fLetterStickHalfHeight, 0.0f);
	glEnd();

	if(bRletterTirangaDisplayed)
	{
		glBegin(GL_QUADS);
		glColor4fv(Saffron);
		glVertex3f(fLetterStickHalfWidth, fTirangaLetterHeight, 0.0f); //0.1f adjusted to remove top extra top corners
		glColor4fv(Saffron);
		glVertex3f(-fLetterStickHalfWidth, fTirangaLetterHeight, 0.0f);
		glColor4fv(White);
		glVertex3f(-fLetterStickHalfWidth, 0.0f, 0.0f);
		glColor4fv(White);
		glVertex3f(fLetterStickHalfWidth, 0.0f, 0.0f);
		glEnd();

		glBegin(GL_QUADS);
		glColor4fv(White);
		glVertex3f(fLetterStickHalfWidth, 0.0f, 0.0f);
		glColor4fv(White);
		glVertex3f(-fLetterStickHalfWidth, 0.0f, 0.0f);
		glColor4fv(Green);
		glVertex3f(-fLetterStickHalfWidth, -fTirangaLetterHeight, 0.0f);
		glColor4fv(Green);
		glVertex3f(fLetterStickHalfWidth, -fTirangaLetterHeight, 0.0f);
		glEnd();		
	}	

	glPushMatrix();
	glTranslatef(fLetterHalfWidth * 0.75f, 0.0f, 0.0f);
	glRotatef(180.0, 0.0f, 1.0f, 0.0f);
	glBegin(GL_QUADS);
	glColor4fv(LetterColor);
	glVertex3f((fLetterHalfWidth + fLetterStickHalfWidth) / 2.0, 0.1f, 0.0f);//0.15f adjusted to lift line up
	glColor4fv(LetterColor);
	glVertex3f((fLetterHalfWidth - fLetterStickHalfWidth) / 2.0, 0.1f, 0.0f);//0.15f adjusted to lift line up
	glColor4fv(LetterColor);
	glVertex3f((- fLetterStickHalfWidth - 0.5f)- fLetterHalfWidth, -fLetterStickHalfHeight, 0.0f);//0.5f adjusted to pull line to right
	glColor4fv(LetterColor);
	glVertex3f((fLetterStickHalfWidth - 0.5f)- fLetterHalfWidth, -fLetterStickHalfHeight, 0.0f);//0.5f adjusted to pull line to right
	glEnd();
	if(bRletterTirangaDisplayed)
	{
		glBegin(GL_QUADS);
		glColor4fv(White);
		glVertex3f((fLetterHalfWidth + fLetterStickHalfWidth) / 2.0, 0.1f, 0.0f);//0.15f adjusted to lift line up
		glColor4fv(White);
		glVertex3f((fLetterHalfWidth - fLetterStickHalfWidth) / 2.0, 0.1f, 0.0f);//0.15f adjusted to lift line up
		glColor4fv(Green);
		glVertex3f(-(fLetterStickHalfWidth - 0.4f), -fTirangaLetterHeight, 0.0f);//0.5f adjusted to pull line to right
		glColor4fv(Green);
		glVertex3f((fLetterStickHalfWidth + 0.08f), -fTirangaLetterHeight, 0.0f);//0.5f adjusted to pull line to right
		glEnd();
	}	
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, fLetterStickHalfHeight / 2.0f, 0.0f);
	DrawPartialCircle(0.0f, 0.0f, (fLetterStickHalfHeight / 2.0f) - fLetterStickHalfWidth + 0.05f, 283.0f, 78.0f, White, Saffron, 4);
	if(bRletterTirangaDisplayed)
	{
		DrawPartialTirangaCircle(0.0f, 0.0f, (fLetterStickHalfHeight / 2.0f) - fLetterStickHalfWidth + 0.05f, 283.0f, 310.0f, White, Saffron, 3);
	}	
	glPopMatrix();	

	glPopMatrix();
}

void DrawT()
{
	glPushMatrix();

	glTranslatef(-fLetterHalfWidth, 0.0f, 0.0f);

	glBegin(GL_QUADS);
	glColor4fv(LetterColor);
	glVertex3f(fLetterStickHalfWidth + fLetterHalfWidth*2.0, fLetterStickHalfHeight, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f(-fLetterStickHalfWidth, fLetterStickHalfHeight, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f(-fLetterStickHalfWidth, fLetterStickHalfHeight - fLetterStickHalfWidth*2.0f, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f(fLetterStickHalfWidth + fLetterHalfWidth*2.0, fLetterStickHalfHeight - fLetterStickHalfWidth*2.0f, 0.0f);
	glEnd();

	glTranslatef(fLetterHalfWidth, 0.0f, 0.0f);

	glBegin(GL_QUADS);
	glColor4fv(LetterColor);
	glVertex3f(fLetterStickHalfWidth, fLetterStickHalfHeight - fLetterStickHalfWidth*2.0f, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f(-fLetterStickHalfWidth, fLetterStickHalfHeight - fLetterStickHalfWidth*2.0f, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f(-fLetterStickHalfWidth, 0.0f, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f(fLetterStickHalfWidth, 0.0f, 0.0f);
	glEnd();

	if(bTletterTirangaDisplayed)
	{
		glBegin(GL_QUADS);
		glColor4fv(Saffron);
		glVertex3f(fLetterStickHalfWidth, fTirangaLetterHeight, 0.0f);
		glColor4fv(Saffron);
		glVertex3f(-fLetterStickHalfWidth, fTirangaLetterHeight, 0.0f);
		glColor4fv(White);
		glVertex3f(-fLetterStickHalfWidth, 0.0f, 0.0f);
		glColor4fv(White);
		glVertex3f(fLetterStickHalfWidth, 0.0f, 0.0f);
		glEnd();	
	}

	glBegin(GL_QUADS);
	glColor4fv(LetterColor);
	glVertex3f(fLetterStickHalfWidth, 0.0f, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f(-fLetterStickHalfWidth, 0.0f, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f(-fLetterStickHalfWidth, -fLetterStickHalfHeight, 0.0f);
	glColor4fv(LetterColor);
	glVertex3f(fLetterStickHalfWidth, -fLetterStickHalfHeight, 0.0f);
	glEnd();

	if(bTletterTirangaDisplayed)
	{
		glBegin(GL_QUADS);
		glColor4fv(White);
		glVertex3f(fLetterStickHalfWidth, 0.0f, 0.0f);
		glColor4fv(White);
		glVertex3f(-fLetterStickHalfWidth, 0.0f, 0.0f);
		glColor4fv(Green);
		glVertex3f(-fLetterStickHalfWidth, -fTirangaLetterHeight, 0.0f);
		glColor4fv(Green);
		glVertex3f(fLetterStickHalfWidth, -fTirangaLetterHeight, 0.0f);
		glEnd();	
	}	

	glPopMatrix();

}
