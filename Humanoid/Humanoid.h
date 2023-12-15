#pragma once

// Arms movement
int shoulderForwardUpRightMovementM = 0;
int shoulderSideUpDownMovementM = 0;
int shoulderForwardLeftRightMovementM = 0;

int elbowMovementM = 0;

int wristUpDownMovementM = 0;
int wristLeftRightMovementM = 0;

// Legs movement
int hipForwardUpRightMovementM = 0;
int hipForwardLeftRightMovementM = 0;

int kneeMovementM = 0;

int towUpDownMovementM = 0;
int towLeftRightMovementM = 0;

GLUquadric* quadric = NULL;

void arm(int shoulderForwardUpRightMovement, int  shoulderSideUpDownMovement, int shoulderForwardLeftRightMovement, int elbowMovement, int wristUpDownMovement, int wristLeftRightMovement)
{
    glPushMatrix();

	//Do transformation for ARM
	glRotatef((GLfloat)shoulderForwardUpRightMovement, 0.0f, 1.0f, 0.0f);
    glRotatef((GLfloat)shoulderSideUpDownMovement, 0.0f, 0.0f, 1.0f);
    glRotatef((GLfloat)shoulderForwardLeftRightMovement, 1.0f, 0.0f, 0.0f);
	
    glTranslatef(1.0f, 0.0f, 0.0f);
	
	glPushMatrix();

	glScalef(2.0f, 0.5f, 1.0f);

	// Draw ARM
	glColor3f(0.8f, 0.6f, 0.4f);
	gluSphere(quadric, 0.5f, 10, 10);

	//Pop the Matrix --> to come back to point where ARM ended
	glPopMatrix();

	//Do transformation for Fore ARM in CTM(MT RT)
	glTranslatef(1.0f, 0.0f, 0.0f);

	glRotatef((GLfloat)elbowMovement, 0.0f, 0.0f, 1.0f);

	glTranslatef(1.0f, 0.0f, 0.0f);

	glPushMatrix();

	glScalef(2.0f, 0.5f, 1.0f);

	//Draw Fore Fore ARM
	glColor3f(0.8f, 0.6f, 0.4f);
	gluSphere(quadric, 0.5f, 10, 10);

	//Pop the Matrix --> to come back to point where Fore ARM ended
	glPopMatrix();

	//Do transformation for Wrist in CTM(MT RT)
	glTranslatef(1.0f, 0.0f, 0.0f);

	glRotatef((GLfloat)wristUpDownMovement, 0.0f, 1.0f, 0.0f);
	glRotatef((GLfloat)wristLeftRightMovement, 0.0f, 0.0f, 1.0f);

	glTranslatef(0.25f, 0.0f, 0.0f);

	glPushMatrix();

	glScalef(1.0f, 0.5f, 1.0f);

	//Draw Wrist
	glColor3f(0.8f, 0.6f, 0.4f);
	gluSphere(quadric, 0.25f, 10, 10);

	glPopMatrix();
	glPopMatrix();
}

void leg(int hipForwardUpRightMovementM, int hipForwardLeftRightMovementM, int kneeMovementM, int towUpDownMovementM, int towLeftRightMovementM)
{
    glPushMatrix();

	//Do transformation for ARM
	glRotatef((GLfloat)hipForwardUpRightMovementM, 0.0f, 1.0f, 0.0f);
    glRotatef((GLfloat)hipForwardLeftRightMovementM, 0.0f, 0.0f, 1.0f);
	
    glTranslatef(1.0f, 0.0f, 0.0f);
	
	glPushMatrix();

	glScalef(2.0f, 0.5f, 1.0f);

	// Draw ARM
	glColor3f(0.8f, 0.6f, 0.4f);
	gluSphere(quadric, 0.5f, 10, 10);

	//Pop the Matrix --> to come back to point where ARM ended
	glPopMatrix();

	//Do transformation for Fore ARM in CTM(MT RT)
	glTranslatef(1.0f, 0.0f, 0.0f);

	glRotatef((GLfloat)kneeMovementM, 0.0f, 0.0f, 1.0f);

	glTranslatef(1.0f, 0.0f, 0.0f);

	glPushMatrix();

	glScalef(2.0f, 0.5f, 1.0f);

	//Draw Fore Fore ARM
	glColor3f(0.8f, 0.6f, 0.4f);
	gluSphere(quadric, 0.5f, 10, 10);

	//Pop the Matrix --> to come back to point where Fore ARM ended
	glPopMatrix();

	//Do transformation for Wrist in CTM(MT RT)
	glTranslatef(1.0f, 0.0f, 0.0f);

	glRotatef((GLfloat)towUpDownMovementM, 0.0f, 1.0f, 0.0f);
	glRotatef((GLfloat)towLeftRightMovementM, 0.0f, 0.0f, 1.0f);

	glTranslatef(0.25f, 0.0f, 0.0f);

	glPushMatrix();

	glScalef(1.0f, 0.5f, 1.0f);

	//Draw Wrist
	glColor3f(0.8f, 0.6f, 0.4f);
	gluSphere(quadric, 0.25f, 10, 10);

	glPopMatrix();
	glPopMatrix();
}

void neck()
{
    glPushMatrix();
    glTranslatef(-1.0f, 1.0f, 0.0f);
	glScalef(1.0f, 1.0f, 1.0f);
    glRotatef(90, 1.0f, 0.0f, 0.0f);
	// Draw ARM
	glColor3f(0.8f, 0.6f, 0.4f);
	//gluSphere(quadric, 0.5f, 10, 10);
    gluCylinder(quadric, 0.2f, 0.2f, 0.55f, 10, 10);
	//Pop the Matrix --> to come back to point where ARM ended
	glPopMatrix();
}

void chest_abdomen()
{
    glPushMatrix();

    glTranslatef(-1.0f, 0.25f, 0.0f);
	glScalef(2.0f, 1.0f, 1.0f);
    glRotatef(90, 1.0f, 0.0f, 0.0f);
	// Draw ARM
	glColor3f(0.8f, 0.6f, 0.4f);
	//gluSphere(quadric, 0.5f, 10, 10);
    gluCylinder(quadric, 0.5f, 0.5f, 4.5f, 10, 10);
	//Pop the Matrix --> to come back to point where ARM ended
	glPopMatrix();
}

void face()
{
    glPushMatrix();

    glTranslatef(-1.0f, 1.8f, 0.0f);
	glScalef(0.9f, 1.2f, 1.0f);

	// Draw ARM
	glColor3f(0.8f, 0.6f, 0.4f);
	gluSphere(quadric, 0.6f, 10, 10);

	//Pop the Matrix --> to come back to point where ARM ended
	glPopMatrix();
}

void humanoid()
{
	// Matrix STACK concept
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glTranslatef(0.0f, 2.5f, -12.0f);
    glScalef(0.8f, 0.8f, 1.0f);
    
    glPushMatrix();
    face();
    glPopMatrix(); 
 

    glPushMatrix();
    neck();
    glPopMatrix(); 

    glPushMatrix();
    glTranslatef(0.0f, 0.2f, 0.0f);
    glRotatef(-60.0f, 0.0f, 0.0f, 1.0f);
    arm(shoulderForwardUpRightMovementM, shoulderSideUpDownMovementM, shoulderForwardLeftRightMovementM, elbowMovementM, wristUpDownMovementM, wristLeftRightMovementM);
    glPopMatrix();

    glPushMatrix();
    chest_abdomen();
    glPopMatrix();  

    glPushMatrix();
    glTranslatef(-2.0f, 0.0f, 0.0f);
    glTranslatef(0.0f, 0.2f, 0.0f);
    glScalef(-1.0f, -1.0f, 1.0f);
    glRotatef(60.0f, 0.0f, 0.0f, 1.0f);
    arm(shoulderForwardUpRightMovementM, -shoulderSideUpDownMovementM, shoulderForwardLeftRightMovementM, elbowMovementM, wristUpDownMovementM, wristLeftRightMovementM);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.5f, -4.0f, 0.0f);
    glScalef(1.2f, 1.0f, 1.0f); 
    glRotatef(-90.0f, 0.0f, 0.0f, 1.0f); 
    leg(0, 0, 0, 0, 0);
    glPopMatrix(); 

    glPushMatrix();
    glTranslatef(-1.5f, -4.0f, 0.0f);
    //glScalef(1.2f, 1.0f, 1.0f); //Thickness
    glScalef(-1.2f, -1.0f, 1.0f);
    glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
    leg(0, 0, 0, 0, 0);
    glPopMatrix();
    
}
