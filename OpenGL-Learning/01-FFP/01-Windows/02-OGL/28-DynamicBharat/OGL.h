#pragma once

#define MYICON 101 // Unique integer value define macro

#define MY_BACKGROUND 102
#define MY_FIGHTER_JET 103

#define MY_MUSIC 104

void DrawCube()
{
	float fScaleFactorX = 1.0f;
	float fScaleFactorY = 1.0f;
	float fScaleFactorZ = 1.0f;

	float fV1[3] = {1.0f, 1.0f, 1.0f};
	float fV2[3] = {-1.0f, 1.0f, fScaleFactorZ * 1.0f};
	float fV3[3] = {-1.0f, -1.0f, fScaleFactorZ * 1.0f};
	float fV4[3] = {1.0f, -1.0f, 1.0f};

	float bV1[3] = {1.0f, 1.0f, -1.0f};
	float bV2[3] = {-1.0f, 1.0f, fScaleFactorZ * -1.0f};
	float bV3[3] = {-1.0f, -1.0f, fScaleFactorZ * -1.0f};
	float bV4[3] = {1.0f, -1.0f, -1.0f};

	glPushMatrix();
  
	glBegin(GL_QUADS);

	//Front Face
	glVertex3fv(fV1);
	glVertex3fv(fV2);
	glVertex3fv(fV3);
	glVertex3fv(fV4);

	//Right Side Face
	glVertex3fv(bV1);
	glVertex3fv(fV1);
	glVertex3fv(fV4);
	glVertex3fv(bV4);

	//Back Face
	glVertex3fv(bV1);
	glVertex3fv(bV2);
	glVertex3fv(bV3);
	glVertex3fv(bV4);

	//Left Side Face
	glVertex3fv(fV2);
	glVertex3fv(bV2);
	glVertex3fv(bV3);
	glVertex3fv(fV3);

	//Top Side Face
	glVertex3fv(bV1);
	glVertex3fv(bV2);
	glVertex3fv(fV2);
	glVertex3fv(fV1);

	//Bottom Side Face
	glVertex3fv(bV4);
	glVertex3fv(bV3);
	glVertex3fv(fV3);
	glVertex3fv(fV4);

	glEnd();

	glPopMatrix();
}
