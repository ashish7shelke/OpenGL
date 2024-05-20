#include "Scene2_End.h"

int Scene2Initialize(HDC hDC)
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

void Scene2Uninitialize()
{

}

void Scene2Update()
{
	if(bScene1Complete)
	{
		if (intro_timer2 <= 10.0f)
		{
			intro_timer2 = intro_timer2 + 0.001f;
		}
		else
		{
			bScene2Complete = TRUE;
		}
	}
}

void Scene2Packaging()
{
	void Packaging();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	Packaging();
	glPopMatrix();

}

void Packaging()
{
	glTranslatef(fCamx + -2.5f, 0.5f, 0.0f);
	if (intro_timer2 <= 2.0f)
	{
		//printString("A^sT/aomaoDIka^mp p`stut", 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, nFontListMarathi); //Shivaji font
		//printString("AataM ivaXvaa makoM dovaoM", 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, nFontListMarathi);
		glPushMatrix();	// Tital PASAYDAN
		glTranslatef(0.0f, 0.5f, 0.0f);
		glScalef(0.5f, 0.5f, 0.5f);
		glListBase(nFontListMarathi);
		char text1[] = "gaIt        : vaasanaa sauKacyaa rMgaI";
		glCallLists(sizeof(text1) - 1, GL_UNSIGNED_BYTE, text1);
		glPopMatrix();

		glPushMatrix();	// Tital PASAYDAN
		glTranslatef(0.0f, 0.0f, 0.0f);
		glScalef(0.5f, 0.5f, 0.5f);
		glListBase(nFontListMarathi);
		char text2[] = "gaayak      : sauroXa vaaDkr";
		glCallLists(sizeof(text2) - 1, GL_UNSIGNED_BYTE, text2);
		glPopMatrix();		

		glPushMatrix();	// Tital PASAYDAN
		glTranslatef(0.0f, -0.5f, 0.0f);
		glScalef(0.5f, 0.5f, 0.5f);
		glListBase(nFontListMarathi);
		char text3[] = "saadrIkrNa  : AaiSaYa SaoLko";
		glCallLists(sizeof(text3) - 1, GL_UNSIGNED_BYTE, text3);
		glPopMatrix();	

		glPushMatrix();	// Tital PASAYDAN
		glTranslatef(0.0f, -1.0f, 0.0f);
		glScalef(0.5f, 0.5f, 0.5f);
		glListBase(nFontListMarathi);
		char text4[] = "g`aup ilaDr   : Atula fulasauMdr";
		glCallLists(sizeof(text4) - 1, GL_UNSIGNED_BYTE, text4);
		glPopMatrix();	

		glPushMatrix();	// Tital PASAYDAN
		glTranslatef(0.0f, -1.5f, 0.0f);
		glScalef(0.5f, 0.5f, 0.5f);
		glListBase(nFontListMarathi);
		char text5[] = "maaga-dSa-k   : Da^ ivajaya AaiNa rmaa gaaoKlao";
		glCallLists(sizeof(text5) - 1, GL_UNSIGNED_BYTE, text5);
		glPopMatrix();	

		glPushMatrix();	// Tital PASAYDAN
		glTranslatef(0.0f, -2.0f, 0.0f);
		glScalef(0.5f, 0.5f, 0.5f);
		glListBase(nFontListMarathi);
		char text6[] = "saMdBa-       : Asaa[namaoMT";
		glCallLists(sizeof(text6) - 1, GL_UNSIGNED_BYTE, text6);
		glPopMatrix();	
	}
}


