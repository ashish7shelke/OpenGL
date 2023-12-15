#pragma once

#define TORSO_HEIGHT 5.0
#define TORSO_RADIUS 1.3
#define UPPER_ARM_HEIGHT 2.5
#define LOWER_ARM_HEIGHT 2.3
#define UPPER_ARM_RADIUS  0.5
#define LOWER_ARM_RADIUS  0.5
#define UPPER_LEG_RADIUS  0.5
#define LOWER_LEG_RADIUS  0.5
#define LOWER_LEG_HEIGHT 3.1
#define UPPER_LEG_HEIGHT 3.3
#define HEAD_HEIGHT 1.2
#define HEAD_RADIUS 1.4
#define HAND_RADIUS 0.6
#define HAND_HEIGHT 0.9
#define FOOT_RADIUS 0.6
#define FOOT_HEIGHT 1.25
#define NECK_RADIUS 0.5
#define NECK_HEIGHT 1.0
#define JOINT_POINT_RADIUS 0.5
#define JOINT_POINT_HEIGHT 0.5

BOOL flag1=TRUE, flag2=FALSE,  flag3=TRUE, flag4=FALSE,  flag5=TRUE, flag6=FALSE,  hflag=TRUE;

/*
Initialization of body parts' angles
*/
static GLfloat theta[17] = {0.0,   0.0,   0.0, 90.0,
							-20.0, 90.0, -20.0, 180.0,
							0.0, 180.0, 0.0, 0.0,
						    0.0, 0.0,  0.0, 0.0, 
							0.0
							};
static GLint choise = 2;
static GLint m_choise = 0;


//Body parts
GLUquadricObj *t,   *h,   *n,    *lh,  *rh,   *lf,  *rf,   *jp,
			 *lhnd, *lft, *lua,  *lla, *rua,  *rla, *lll,  *rll, 
			 *rul,  *lul, *rhnd, *rft;

typedef struct _area {
    int id;
    int x, y;
    float min, max;
    float value;
    float step;
} area;

area translation[5] = {
    { 0, 120, 40, -30.0, 30.0, 0.0, 0.05,},
    { 1, 180, 40, -30.0, 30.0, 0.0, 0.05,},
    { 2, 180, 40, -200.0, 200.0, 0.0, 0.1,},
	{ 3, 180, 120, -100, 100, 0.0, 0.5},
	{ 4, 240, 120, -100, 100, 0.0, 0.5}
};

//Animation variables and angles
GLuint texture1=0; 
GLuint texture2=0;
GLuint texture3=0;
GLuint texture4=0;

GLuint old_thetaW3=0;
GLuint old_thetaX4=0;
GLuint old_thetaE5=0;
GLuint old_thetaC6=0;
GLuint old_thetaS11=0;
GLuint old_thetaD12=0;

GLint selection = 0;

/*
gluLookAt initial values, view point rotation
*/
GLfloat eye[3] = { 0.0, 0.0, 60.0 };
GLfloat at[3]  = { 0.0, 0.0, 0.0 };
GLfloat up[3]  = { 0.0, 1.0, 0.0 };

void head(){
   glPushMatrix();
   //glColor3f (1.0, 0.0, 5.0);
   glRotatef(-90.0, 1.0, 0.0, 0.0);
   glRotatef(15.0, 0.0, 0.0, 1.0);
   glScalef(HEAD_HEIGHT, HEAD_HEIGHT, HEAD_RADIUS);
   gluSphere(h,1.0,10,10);
   glPopMatrix();
}

void neck(){
   glPushMatrix();
   glRotatef(-90.0, 1.0, 0.0, 0.0);
   gluCylinder(n,NECK_RADIUS, NECK_RADIUS, NECK_HEIGHT,10,10);
   glPopMatrix();
}

void torso(){
   glPushMatrix();
   glRotatef(-90.0, 1.0, 0.0, 0.0);
   gluCylinder(t,TORSO_RADIUS, TORSO_RADIUS, TORSO_HEIGHT,10,10);
   glPopMatrix();
}

void joint_point(){
   glPushMatrix();
   glScalef(JOINT_POINT_RADIUS, JOINT_POINT_HEIGHT, 
								JOINT_POINT_RADIUS);
   gluSphere(jp,1.0,10,10);
   glPopMatrix();
}

void left_upper_arm(){
   glPushMatrix();
   gluCylinder(lua,UPPER_ARM_RADIUS, UPPER_ARM_RADIUS, 
									UPPER_ARM_HEIGHT,10,10);
   glPopMatrix();
}

void left_lower_arm(){
   glPushMatrix();
   glRotatef(0.0, 1.0, 0.0, 0.0);
   gluCylinder(lla,LOWER_ARM_RADIUS, LOWER_ARM_RADIUS, 
									LOWER_ARM_HEIGHT,10,10);
   glPopMatrix();
}

void left_hand(){
   glPushMatrix();
   glRotatef(90.0, 1.0, 0.0, 0.0);
   glScalef(HAND_RADIUS, HAND_HEIGHT, HAND_RADIUS);
   gluSphere(lhnd,1.0,10,10);
   glPopMatrix();
}

void right_upper_arm(){
   glPushMatrix();
   gluCylinder(rua,UPPER_ARM_RADIUS, UPPER_ARM_RADIUS, 
									UPPER_ARM_HEIGHT,10,10);
   glPopMatrix();
}

void right_lower_arm(){
   glPushMatrix();
   gluCylinder(rla,LOWER_ARM_RADIUS, LOWER_ARM_RADIUS, 
									LOWER_ARM_HEIGHT,10,10);
   glPopMatrix();
}

void right_hand(){
   glPushMatrix();
   glRotatef(90.0, 1.0, 0.0, 0.0);
   glScalef(HAND_RADIUS, HAND_HEIGHT, HAND_RADIUS);
   gluSphere(rhnd,1.0,10,10);
   glPopMatrix();
}

void left_upper_leg(){
   glPushMatrix();
   glRotatef(-90.0, 1.0, 0.0, 0.0);
   gluCylinder(lul,UPPER_LEG_RADIUS, UPPER_LEG_RADIUS, 
									UPPER_LEG_HEIGHT,10,10);
   glPopMatrix();
}

void left_lower_leg(){
   glPushMatrix();
   glRotatef(-90.0, 1.0, 0.0, 0.0);
   gluCylinder(lll,LOWER_LEG_RADIUS, LOWER_LEG_RADIUS, 
									LOWER_LEG_HEIGHT,10,10);
   glPopMatrix();
}

void left_foot(){
   glPushMatrix();
   glRotatef(90.0, 1.0, 0.0, 0.0);
   glScalef(FOOT_RADIUS, FOOT_HEIGHT, FOOT_RADIUS);
   gluSphere(lft,0.8,10,10);
   glPopMatrix();
}

void right_upper_leg(){
   glPushMatrix();
   glRotatef(-90.0, 1.0, 0.0, 0.0);
   gluCylinder(rul,UPPER_LEG_RADIUS, UPPER_LEG_RADIUS, 
									UPPER_LEG_HEIGHT,10,10);
   glPopMatrix();
}

void right_lower_leg(){
   glPushMatrix();
   glRotatef(-90.0, 1.0, 0.0, 0.0);
   gluCylinder(rll,LOWER_LEG_RADIUS, LOWER_LEG_RADIUS, 
									LOWER_LEG_HEIGHT,10,10);
   glPopMatrix();
}

void right_foot(){
   glPushMatrix();
   glRotatef(90.0, 1.0, 0.0, 0.0);
   glScalef(FOOT_RADIUS, FOOT_HEIGHT, FOOT_RADIUS);
   gluSphere(rft,0.8,10,10);
   glPopMatrix();
}

void human()
{
	glPushMatrix();
	
	glBindTexture(GL_TEXTURE_2D, texture2);
    glRotatef(theta[0], 0.0, 1.0, 0.0);
    torso();
    glBindTexture(GL_TEXTURE_2D, 0);

	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, texture4);
	glTranslatef(0,TORSO_HEIGHT, 0.0);
	neck();
    glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, texture1);
	glTranslatef(0.0, NECK_HEIGHT+0.5*HEAD_HEIGHT, 0.0);
	glRotatef(theta[1], 1.0, 0.0, 0.0);
    glRotatef(theta[2], 0.0, 1.0, 0.0);
    head();
    glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();//add JOINT_POINT_
    
	glPushMatrix();//add JOINT_POINT_
	glBindTexture(GL_TEXTURE_2D, texture4);
	glTranslatef(-0.85*(TORSO_RADIUS+JOINT_POINT_RADIUS), 
									0.9*TORSO_HEIGHT, 0.0);
	joint_point();
    glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, texture4);
	glTranslatef(0.0, 0.0, 0.0);
	glRotatef(theta[3], 1.0, 0.0, 0.0);
	glRotatef(theta[11], 0.0, 0.0, 1.0);
    left_upper_arm();
    glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, texture4);
	glTranslatef(0.0, 0.0,UPPER_ARM_HEIGHT);
	joint_point();
    glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, texture4);
    glTranslatef(0.0, 0.1*JOINT_POINT_HEIGHT, 0.0);
    glRotatef(theta[4], 1.0, 0.0, 0.0);
    left_lower_arm();
    glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, texture4);
	glTranslatef(0.0, 0.0, LOWER_ARM_HEIGHT);
	left_hand();
    glBindTexture(GL_TEXTURE_2D, 0);
    glPopMatrix();
    
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, texture4);
    glTranslatef(0.85*(TORSO_RADIUS+JOINT_POINT_RADIUS), 
									0.9*TORSO_HEIGHT, 0.0);
	joint_point();
    glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, texture4);
    glTranslatef(0.0, 0.0, 0.0);
    glRotatef(theta[5], 1.0, 0.0, 0.0);
	glRotatef(theta[12], 0.0, 0.0, 1.0);
    right_upper_arm();
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, texture4);
    glTranslatef(0.0, 0.0, UPPER_ARM_HEIGHT);
	joint_point();
    glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, texture4);
    glTranslatef(0.0, 0.1*JOINT_POINT_HEIGHT, 0.0);
    glRotatef(theta[6], 1.0, 0.0, 0.0);
    right_lower_arm();
    glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, texture4);
	glTranslatef(0.0, 0.0, LOWER_ARM_HEIGHT);
	right_hand();
    glBindTexture(GL_TEXTURE_2D, 0);
    glPopMatrix();

    glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, texture3);
	glTranslatef(-(TORSO_RADIUS-JOINT_POINT_RADIUS), 
							-0.15*JOINT_POINT_HEIGHT, 0.0);
	joint_point();
    glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, texture3);
	glTranslatef(0, 0.1*JOINT_POINT_HEIGHT, 0.0);
    glRotatef(theta[7], 1.0, 0.0, 0.0);
	glRotatef(theta[13], 0.0, 0.0, 1.0);
    left_upper_leg();
    glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, texture4);
	glTranslatef(0.0, UPPER_LEG_HEIGHT, 0.0);
	joint_point();
    glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, texture3);
    glTranslatef(0.0, 0.1*JOINT_POINT_HEIGHT, 0.0);
    glRotatef(theta[8], 1.0, 0.0, 0.0);
    left_lower_leg();
    glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, texture4);
    glTranslatef(0.0, LOWER_LEG_HEIGHT, -0.5*FOOT_HEIGHT);
	glRotatef(theta[15], 1.0, 0.0, 0.0);
	left_foot();
    glBindTexture(GL_TEXTURE_2D, 0);
    glPopMatrix();

    glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, texture3);
    glTranslatef(TORSO_RADIUS-JOINT_POINT_RADIUS, 
							-0.15*JOINT_POINT_HEIGHT, 0.0);
	joint_point();
    glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, texture3);
	glTranslatef(0, 0.1*JOINT_POINT_HEIGHT, 0.0);
    glRotatef(theta[9], 1.0, 0.0, 0.0);
	glRotatef(theta[14], 0.0, 0.0, 1.0);
    right_upper_leg();
    glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, texture4);
	glTranslatef(0.0, UPPER_LEG_HEIGHT, 0.0);
	joint_point();
    glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, texture3);
    glTranslatef(0.0, 0.1*JOINT_POINT_HEIGHT, 0.0);
    glRotatef(theta[10], 1.0, 0.0, 0.0);
    right_lower_leg();
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, texture4);
	glTranslatef(0.0, LOWER_LEG_HEIGHT, -0.5*FOOT_HEIGHT);
	glRotatef(theta[16], 1.0, 0.0, 0.0);
	right_foot();
    glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();

	glPopMatrix();

	glColor3f(1.0f, 1.0f, 1.0f);

}

void timer_walk() 
{
		if(flag1) {
			theta[3] += 0.05;
		}
		else {
			theta[3] -= 0.05;
		}
		if(theta[3] >= 110.0) flag1 = FALSE;
		if(theta[3] <= 70.0)  flag1 = TRUE;
		
		if(flag2) {
		theta[5] += 0.05;
		}
		else {
			theta[5] -= 0.05;
		}
		if(theta[5] >= 110.0) flag2 = FALSE;
		if(theta[5] <= 70.0)  flag2 = TRUE;
		
		if(flag3) {
			theta[9] += 0.05;
		}
		else {
			theta[9] -= 0.05;
		}
		if(theta[9] >= 200.0) flag3 = FALSE;
		if(theta[9] <= 160.0)  flag3 = TRUE;
		
		if(flag4) {
			theta[7] += 0.05;
		}
		else {
			theta[7] -= 0.05;
		}
		if(theta[7] >= 200.0) flag4 = FALSE;
		if(theta[7] <= 160.0)  flag4 = TRUE;
		
		if(hflag) {
			theta[2] += 0.05;
		}
		else {
			theta[2] -= 0.05;
		}
		if(theta[2] >= 30.0) hflag = FALSE;
		if(theta[2] <= -30.0) hflag = TRUE;

}
