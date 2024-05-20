#include "Scene0_Intro.h"

int Scene0Initialize(HDC hDC)
{
	//Setting up 3D Text
	if (setup3DMarathiTextRC(hDC) != 0)
	{
		fprintf(gpFile, "setup3DTextRC failed ...\n");
		return -30;
	}
	
	//Setting up 3D Text
	if (setup3DEnglishTextRC(hDC) != 0)
	{
		fprintf(gpFile, "setup3DTextRC failed ...\n");
		return -31;
	}

	return 0;
}

void Scene0Uninitialize()
{

}

void Scene0Update()
{
	static BOOL once = FALSE;
	if (intro_timer <= 9.0f)
	{
		intro_timer = intro_timer + 0.001f;
	}
	else
	{
		if(once == FALSE)
		{
			once = TRUE;
			fCamx = -20.0f;
		}
		bScene0Complete = TRUE;
	}
}

void printString(char* s, GLfloat Red, GLfloat Greeen, GLfloat Blue, GLfloat xPos, GLfloat yPos, GLfloat zPos, GLuint fontList)
{
	glPushMatrix();
	glColor3ub(Red, Greeen, Blue);
	glTranslatef(xPos, yPos, zPos);
	glListBase(fontList);
	glCallLists(strlen(s), GL_UNSIGNED_BYTE, (GLubyte*)s);
	glPopMatrix();
}
void Scene0Packaging()
{
	glTranslatef(-2.5f, 0.0f, 0.0f);
	void printString(char* s, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLuint);
	if (intro_timer <= 2.0f)
	{
		//printString("A^sT/aomaoDIka^mp p`stut", 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, nFontListMarathi); //Shivaji font
		//printString("AataM ivaXvaa makoM dovaoM", 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, nFontListMarathi);
		glPushMatrix();	// Tital PASAYDAN
		glTranslatef(0.0f, 0.0f, 0.0f);
		glScalef(1.0f, 1.0f, 1.0f);
		glListBase(nFontListMarathi);
		char Titel[] = "A^sT/aomaoDIka^mp p`stut";
		glCallLists(sizeof(Titel) - 1, GL_UNSIGNED_BYTE, Titel);
		glPopMatrix();
	}
	else if (intro_timer <= 5.0f)
	{
		glPushMatrix();	// Tital PASAYDAN
		glTranslatef(2.0f, 1.0f, 0.0f);
		glScalef(0.4f, 0.4f, 0.4f);
		glListBase(nFontListEnglish);
		char text1[] = "RTR 5";
		glCallLists(sizeof(text1) - 1, GL_UNSIGNED_BYTE, text1);
		glPopMatrix();

		glPushMatrix();	// Tital PASAYDAN
		glTranslatef(1.25f, 0.0f, 0.0f);
		glScalef(1.5f, 1.5f, 1.5f);
		glListBase(nFontListMarathi);
		char text2[] = "hla g`aup ";
		glCallLists(sizeof(text2) - 1, GL_UNSIGNED_BYTE, text2);
		glPopMatrix();

		glPushMatrix();	// Tital PASAYDAN
		glTranslatef(0.5f, -1.0f, 0.0f);
		glScalef(1.0f, 1.0f, 1.0f);
		glListBase(nFontListMarathi);
		char text3[] = "saadr krIt Aaho ";
		glCallLists(sizeof(text3) - 1, GL_UNSIGNED_BYTE, text3);
		glPopMatrix();
	}
	else if(intro_timer <= 8.0f)
	{
		glPushMatrix();	// Tital PASAYDAN
		glTranslatef(-1.25f, -1.0f, 0.0f);
		glScalef(1.5f, 1.5f, 1.5f);
		glListBase(nFontListMarathi);
		char text3[] = "vaasanaa sauKacyaa rMgaI";
		glCallLists(sizeof(text3) - 1, GL_UNSIGNED_BYTE, text3);
		glPopMatrix();
	}
}





