#pragma once

extern GLuint texture_FighterJet;
extern GLUquadric* quadric;
extern GLfloat cAngle;
extern BOOL bJetSmokeEnabled;

float fJetTranslateXFactor = -95.0f; //-105.0f;
float fJetTranslateYFactor = 100.0f; //95.0f;

//fJetTranslateXFactor = -51.399788 fJetTranslateYFactor = 90.399193.
//fJetTranslateXFactor = 51.399788 fJetTranslateYFactor = 90.399193.

//fJetTranslateXFactor = -103.498993 fJetTranslateYFactor = 90.399193

float fJetTranslateXFactorBegin = -50.0f;
float fJetTranslateYFactorBegin = 100.0f;

float fCurveAngle = 90.0f;

GLfloat BluePowder[4] = { 176.0/255.0, 224.0/255.0, 230.0/255.0, 1.0f };
GLfloat MediumGrey[4] = { 128.0/255.0,128.0/255.0,128.0/255.0,1.0 };
GLfloat LowGrey[4] = { 192.0/255.0,192.0/255.0,192.0/255.0,1.0 };

void DrawFighterJet(float* smokeColor)
{
	void DrawWings();
    {
	    glTranslatef(0.0f, 0.0f, 0.0f);
        //Plane Cylibdrical Body
        glPushMatrix();
        glScalef(1.0f, 1.2f, 8.0f);
        glColor4fv(BluePowder);
        gluSphere(quadric, 1.0f, 30, 30);
        glPopMatrix();
    }

    {
        // Pilot Glass Cover code starts
        glPushMatrix();
        glTranslatef(0.0f, 0.5f, 5.0f);
        glScalef(1.0f, 1.2f, 3.0f);
        glColor4fv(MediumGrey);
        gluSphere(quadric, 0.5f, 30, 30);
        glPopMatrix();
    }// Pilot Glass Cover code ends
    
    /////////////////////////////////////////////////////////////////////////////////////
    // Left Bigger Wing Code Starts here
    /////////////////////////////////////////////////////////////////////////////////////
	//Plane Bigger Left Wings
	{
        glPushMatrix();
        glTranslatef(-6.0f, 0.0f, 0.0f);
        glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
        glScalef(5.0f, 0.2f, 1.0f);
        glColor4fv(LowGrey);
        DrawWings();
        glPopMatrix();

        //Bigger Left Wing's Fan
        glPushMatrix();
        glTranslatef(-4.0f, -1.0f, 0.8f);

        glPushMatrix();
        glScalef(1.0f, 0.8f, 1.25f);
        glColor4fv(BluePowder);
        gluSphere(quadric, 1.0f, 30, 30);
        glPopMatrix();	

        glPushMatrix();
        glRotatef(cAngle*2.0f, 0.0f, 0.0f, 1.0f);
        glTranslatef(-0.7f, 0.0f, 1.0f);
        glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        glScalef(0.5f, 0.05f, 0.2f);
        glColor4fv(MediumGrey);
        DrawWings();
        glPopMatrix();

        glPushMatrix();
        glRotatef(cAngle*2.0f, 0.0f, 0.0f, 1.0f);	
        glTranslatef(0.7f, 0.0f, 1.0f);
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        glScalef(0.5f, 0.05f, 0.2f);
        glColor4fv(MediumGrey);
        DrawWings();
        glPopMatrix();	

        glPushMatrix(); // Additional Fans Push Start
        glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
        glPushMatrix();
        glRotatef(cAngle*2.0f, 0.0f, 0.0f, 1.0f);
        glTranslatef(-0.7f, 0.0f, 1.0f);
        glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        glScalef(0.5f, 0.05f, 0.2f);
        glColor4fv(MediumGrey);
        DrawWings();
        glPopMatrix();

        glPushMatrix();
        glRotatef(cAngle*2.0f, 0.0f, 0.0f, 1.0f);	
        glTranslatef(0.7f, 0.0f, 1.0f);	
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        glScalef(0.5f, 0.05f, 0.2f);
        glColor4fv(MediumGrey);
        DrawWings();
        glPopMatrix();
        glPopMatrix	(); // Additional Fans Push Pop End

        glPopMatrix();
    }
    /////////////////////////////////////////////////////////////////////////////////////
    // Left Bigger Wing Code Finished here
    /////////////////////////////////////////////////////////////////////////////////////


    /////////////////////////////////////////////////////////////////////////////////////
    // Right Bigger Wing Code Starts here
    /////////////////////////////////////////////////////////////////////////////////////
	//Plane Bigger Right Wings
    {
        glPushMatrix();
        glTranslatef(6.0f, 0.0f, 0.0f);
        glScalef(5.0f, 0.2f, 1.0f);
        glColor4fv(LowGrey);
        DrawWings();
        glPopMatrix();

        //Bigger Right Wing's Fan
        glPushMatrix();		// Bigger Right Wing's Fan Push(1) Begin
        glTranslatef(4.0f, -1.0f, 0.8f);

        glPushMatrix();
        glScalef(1.0f, 0.8f, 1.25f);
        glColor4fv(BluePowder);
        gluSphere(quadric, 1.0f, 30, 30);
        glPopMatrix();	

        glPushMatrix();
        glRotatef(cAngle*2.0f, 0.0f, 0.0f, 1.0f);
        glTranslatef(-0.7f, 0.0f, 1.0f);
        glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        glScalef(0.5f, 0.05f, 0.2f);
        glColor4fv(MediumGrey);
        DrawWings();
        glPopMatrix();

        glPushMatrix();
        glRotatef(cAngle*2.0f, 0.0f, 0.0f, 1.0f);	
        glTranslatef(0.7f, 0.0f, 1.0f);
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        glScalef(0.5f, 0.05f, 0.2f);
        glColor4fv(MediumGrey);
        DrawWings();
        glPopMatrix();	

        glPushMatrix(); // Additional Fans Push Start
        glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
        glPushMatrix();
        glRotatef(cAngle*2.0f, 0.0f, 0.0f, 1.0f);
        glTranslatef(-0.7f, 0.0f, 1.0f);
        glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        glScalef(0.5f, 0.05f, 0.2f);
        glColor4fv(MediumGrey);
        DrawWings();
        glPopMatrix();

        glPushMatrix();
        glRotatef(cAngle*2.0f, 0.0f, 0.0f, 1.0f);	
        glTranslatef(0.7f, 0.0f, 1.0f);	
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        glScalef(0.5f, 0.05f, 0.2f);
        glColor4fv(MediumGrey);
        DrawWings();
        glPopMatrix();
        glPopMatrix	(); // Additional Fans Push Pop End

        glPopMatrix(); //Bigger Right Wing's Fan Push(1) End
    }
    /////////////////////////////////////////////////////////////////////////////////////
    // Right Bigger Wing Code Finished here
    /////////////////////////////////////////////////////////////////////////////////////

    {
        //Plane Smaller  Upward Wings
        glPushMatrix();
        glTranslatef(-2.0f, 1.0f, -5.0f);
        glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
        glRotatef(-30.0f, 0.0f, 0.0f, 1.0f);
        glScalef(2.0f, 0.2f, 1.0f);
        glColor4fv(LowGrey);
        DrawWings();
        glPopMatrix();

        glPushMatrix();
        glTranslatef(2.0f, 1.0f, -5.0f);
        glRotatef(30.0f, 0.0f, 0.0f, 1.0f);
        glScalef(2.0f, 0.2f, 1.0f);
        glColor4fv(LowGrey);
        DrawWings();
        glPopMatrix();

        //Plane Smaller  Lower Wings
        glPushMatrix();
        glTranslatef(-1.0f, -1.0f, -5.0f);
        glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
        glRotatef(40.0f, 0.0f, 0.0f, 1.0f);
        glScalef(1.0f, 0.2f, 1.0f);
        glColor4fv(LowGrey);
        DrawWings();
        glPopMatrix();

        glPushMatrix();
        glTranslatef(1.0f, -1.0f, -5.0f);
        glRotatef(-40.0f, 0.0f, 0.0f, 1.0f);
        glScalef(1.0f, 0.2f, 1.0f);
        glColor4fv(LowGrey);
        DrawWings();
        glPopMatrix();
    }	

	// Tail's Fan
    {
        glPushMatrix();
        glRotatef(cAngle*2.0f, 0.0f, 0.0f, 1.0f);
        glTranslatef(-1.0f, 0.0f, -7.5f);
        glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        glScalef(1.0f, 0.05f, 0.2f);
        glColor4fv(MediumGrey);
        DrawWings();
        glPopMatrix();

        glPushMatrix();
        glRotatef(cAngle*2.0f, 0.0f, 0.0f, 1.0f);	
        glTranslatef(1.0f, 0.0f, -7.5f);
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        glScalef(1.0f, 0.05f, 0.2f);
        glColor4fv(MediumGrey);
        DrawWings();
        glPopMatrix();
    }	

	// Smoke 
    {
        if(bJetSmokeEnabled)
        {
            glPushMatrix();        
            glScalef(1.5f, 0.05f, 15.0f);
            glTranslatef(0.0f, -3.0f, -1.0f);
            glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
            glColor4fv(smokeColor);
            DrawCube();
            glPopMatrix();
        }
    }
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);		
}

void FlyingJet()
{
	glPushMatrix();
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	glTranslatef(0.0f, fJetTranslateYFactor, fJetTranslateXFactor - 8.0f); // z, y, x // -15f is offset to bring nose on the curve
	glRotatef(40.0f, 0.0f, 0.0f, 1.0f); // fCurveAngle
    glRotatef(fCurveAngle, 1.0f, 0.0f, 0.0f);
	DrawFighterJet(Saffron);
	glPopMatrix();	

	glPushMatrix();
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	glTranslatef(0.0f, 0.0f, fJetTranslateXFactor - 10.0f); // -15f is offset to bring nose on the curve
	glRotatef(30.0f, 0.0f, 0.0f, 1.0f);
	DrawFighterJet(White);
	glPopMatrix();	

	glPushMatrix();
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	glTranslatef(0.0f, -fJetTranslateYFactor, fJetTranslateXFactor - 8.0f); // -15f is offset to bring nose on the curve
	glRotatef(40.0f, 0.0f, 0.0f, 1.0f);
    glRotatef(-fCurveAngle, 1.0f, 0.0f, 0.0f);
	DrawFighterJet(Green);
	glPopMatrix();	
}

void DrawGraph()
{
	for (int i = -100; i <= 100; i++)
	{
		//glLoadIdentity(); // Set Matrix to identity

		glBegin(GL_LINES);

		glVertex3f(-100.0f, -5.0f, (GLfloat)i);

		glVertex3f(100.0f, -5.0f, (GLfloat)i);

		glEnd();

		glBegin(GL_LINES);

		glVertex3f((GLfloat)i, -5.0f, -100.0f);

		glVertex3f((GLfloat)i, -5.0f, 100.0f);

		glEnd();
	}

}

void DrawWings()
{
	float fTextureTileFraction0 = 0.0f;
	float fTextureTileFraction1 = 1.0;

	float fScaleFactorX = 1.0f;
	float fScaleFactorY = 1.0f;
	float fScaleFactorZ = 1.5f;

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
