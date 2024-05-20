#include "Scene1_Vasana.h"

int Scene1Initialize()
{

    BOOL bResult = FALSE;
    // Loading Images - Create Texture

	/*bResult = loadGlTexture(&texture_hut_front_chappar, MAKEINTRESOURCE(CUBE_FRONT_CHAPPAR_HUT_BITMAP));
	if (bResult == FALSE)
	{
		fprintf(gpFile, "loadGlTexture(CUBE_FRONT_CHAPPAR_HUT_BITMAP) failed .../n");
		return -12;
	}*/

	//Initialize Human quadric
	{
        h=gluNewQuadric();
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_TEXTURE_2D);
		gluQuadricTexture(h, GL_TRUE);
		gluQuadricDrawStyle(h, GLU_FILL);
		
		n=gluNewQuadric();
		gluQuadricTexture(n, GL_TRUE);
		gluQuadricDrawStyle(n, GLU_FILL);

		t=gluNewQuadric();
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_TEXTURE_2D);
		gluQuadricTexture(t, GL_TRUE);
        gluQuadricDrawStyle(t, GLU_FILL);

		jp=gluNewQuadric();
		gluQuadricTexture(jp, GL_TRUE);
		gluQuadricDrawStyle(jp, GLU_FILL);

        lua=gluNewQuadric();
		gluQuadricTexture(lua, GL_TRUE);
    	gluQuadricDrawStyle(lua, GLU_FILL);

        lla=gluNewQuadric();
		gluQuadricTexture(lla, GL_TRUE);
        gluQuadricDrawStyle(lla, GLU_FILL);

        lhnd=gluNewQuadric();
		gluQuadricTexture(lhnd, GL_TRUE);
		gluQuadricDrawStyle(lhnd, GLU_FILL);

		rhnd=gluNewQuadric();
		gluQuadricTexture(rhnd, GL_TRUE);
		gluQuadricDrawStyle(rhnd, GLU_FILL);

		rft=gluNewQuadric();
		gluQuadricTexture(rft, GL_TRUE);
		gluQuadricDrawStyle(rft, GLU_FILL);

		lft=gluNewQuadric();
		gluQuadricTexture(lft, GL_TRUE);
		gluQuadricDrawStyle(lft, GLU_FILL);

		rua=gluNewQuadric();
		gluQuadricTexture(rua, GL_TRUE);
        gluQuadricDrawStyle(rua, GLU_FILL);

        rla=gluNewQuadric();
		gluQuadricTexture(rla, GL_TRUE);
        gluQuadricDrawStyle(rla, GLU_FILL);

        lul=gluNewQuadric();
		gluQuadricTexture(lul, GL_TRUE);
        gluQuadricDrawStyle(lul, GLU_FILL);

        lll=gluNewQuadric();
		gluQuadricTexture(lll, GL_TRUE);
        gluQuadricDrawStyle(lll, GLU_FILL);

        rul=gluNewQuadric();
		gluQuadricTexture(rul, GL_TRUE);
        gluQuadricDrawStyle(rul, GLU_FILL);

        rll=gluNewQuadric();
		gluQuadricTexture(rll, GL_TRUE);
        gluQuadricDrawStyle(rll, GLU_FILL);	
	}

	// Loading Images - Create Texture
	bResult = loadGlTexture(&texture_floor, MAKEINTRESOURCE(GROUND_CITY_TEXTURE));
	if (bResult == FALSE)
	{
		fprintf(gpFile, "loadGlTexture(GROUND_TEXTURE) failed ...\n");
		return -11;
	}

	// Loading Images - Create Texture
	bResult = loadGlTexture(&texture_village_floor, MAKEINTRESOURCE(GROUND_VILLAGE_TEXTURE));
	if (bResult == FALSE)
	{
		fprintf(gpFile, "loadGlTexture(WATER_TEXTURE) failed ...\n");
		return -12;
	}

	// Loading Images - Create Texture
	bResult = loadGlTexture(&texture_water, MAKEINTRESOURCE(WATER_TEXTURE));
	if (bResult == FALSE)
	{
		fprintf(gpFile, "loadGlTexture(WATER_TEXTURE) failed ...\n");
		return -13;
	}

	// Loading Images - Create Texture
	bResult = loadGlTexture(&texture_background, MAKEINTRESOURCE(BACKGROUND_TEXTURE));
	if (bResult == FALSE)
	{
		fprintf(gpFile, "loadGlTexture(BACKGROUND_TEXTURE) failed ...\n");
		return -14;
	}

	// Loading Images - Create Texture
	bResult = loadGlTexture(&texture_city_road, MAKEINTRESOURCE(CITYROAD_TEXTURE));
	if (bResult == FALSE)
	{
		fprintf(gpFile, "loadGlTexture(CITYROAD_TEXTURE) failed ...\n");
		return -14;
	}

	// Loading Images - Create Texture
	bResult = loadGlTexture(&texture_village, MAKEINTRESOURCE(VILLAGE_TEXTURE));
	if (bResult == FALSE)
	{
		fprintf(gpFile, "loadGlTexture(VILLAGE_TEXTURE) failed ...\n");
		return -24;
	}

 	// Loading Images - Create Texture - Human1
	bResult = loadGlTexture(&texture2, MAKEINTRESOURCE(BODY));
	if (bResult == FALSE)
	{
		fprintf(gpFile, "loadGlTexture(BODY) failed .../n");
		return -15;
	}

	bResult = loadGlTexture(&texture1, MAKEINTRESOURCE(PHOTOM));
	if (bResult == FALSE)
	{
		fprintf(gpFile, "loadGlTexture(PHOTOM) failed .../n");
		return -16;
	}

	bResult = loadGlTexture(&texture3, MAKEINTRESOURCE(SHORTS));
	if (bResult == FALSE)
	{
		fprintf(gpFile, "loadGlTexture(SHORTS) failed .../n");
		return -17;
	}

	bResult = loadGlTexture(&texture4, MAKEINTRESOURCE(SKIN));
	if (bResult == FALSE)
	{
		fprintf(gpFile, "loadGlTexture(SKIN) failed .../n");
		return -18;
	}

	bResult = loadGlTexture(&texture5, MAKEINTRESOURCE(ARM));
	if (bResult == FALSE)
	{
		fprintf(gpFile, "loadGlTexture(SKIN) failed .../n");
		return -18;
	}

 	// Loading Images - Create Texture - Human2
	bResult = loadGlTexture(&human2_texture2, MAKEINTRESOURCE(HUMAN2_BODY));
	if (bResult == FALSE)
	{
		fprintf(gpFile, "loadGlTexture(BODY) failed .../n");
		return -19;
	}

	bResult = loadGlTexture(&human2_texture1, MAKEINTRESOURCE(HUMAN2_PHOTOM));
	if (bResult == FALSE)
	{
		fprintf(gpFile, "loadGlTexture(PHOTOM) failed .../n");
		return -20;
	}

	bResult = loadGlTexture(&human2_texture3, MAKEINTRESOURCE(HUMAN2_SHORTS));
	if (bResult == FALSE)
	{
		fprintf(gpFile, "loadGlTexture(SHORTS) failed .../n");
		return -21;
	}

	bResult = loadGlTexture(&human2_texture4, MAKEINTRESOURCE(HUMAN2_SKIN));
	if (bResult == FALSE)
	{
		fprintf(gpFile, "loadGlTexture(HUMAN2_SKIN) failed .../n");
		return -22;
	}

	bResult = loadGlTexture(&human2_texture5, MAKEINTRESOURCE(HUMAN2_ARM));
	if (bResult == FALSE)
	{
		fprintf(gpFile, "loadGlTexture(SKIN) failed .../n");
		return -23;
	}

	InitializeParticles();

    return 0;
}

void Scene1Uninitialize()
{
	if (t)
		gluDeleteQuadric(t);
	if (h)
		gluDeleteQuadric(h);
	if (n)
		gluDeleteQuadric(n);
	if (lh)
		gluDeleteQuadric(lh);
	if (rh)
		gluDeleteQuadric(rh);
	if (lf)
		gluDeleteQuadric(lf);
	if (rf)
		gluDeleteQuadric(rf);
	if (jp)
		gluDeleteQuadric(jp);
	if (lhnd)
		gluDeleteQuadric(lhnd);
	if (lft)
		gluDeleteQuadric(lft);
	if (lua)
		gluDeleteQuadric(lua);
	if (lla)
		gluDeleteQuadric(lla);
	if (rua)
		gluDeleteQuadric(rua);
	if (rla)
		gluDeleteQuadric(rla);
	if (lll)
		gluDeleteQuadric(lll);
	if (rll)
		gluDeleteQuadric(rll);
	if (rul)
		gluDeleteQuadric(rul);
	if (lul)
		gluDeleteQuadric(lul);
	if (rhnd)
		gluDeleteQuadric(rhnd);
	if (rft)
		gluDeleteQuadric(rft);

	texture1=0; 
	texture2=0;
	texture3=0;
	texture4=0;
}

void Scene1Update()
{
	if(bScene0Complete)
	{
		if(gbWalkEnable)
		{
			fCamx = fCamx + (0.01f * camSpeed) ;
			timer_walk();
		}
		if(fCamx >= 70.0f)
		{
			//fprintf(gpFile, "fCamx = %f.\n", fCamx);
			bScene1Complete = TRUE;
		}
	}
}

void Scene1Vasana()
{
    glPushMatrix();   
	DrawGrid();
	glPopMatrix();

	glPushMatrix(); 
	glTranslatef(0.0f, 0.0f, 10.0f);
	glScalef(1.0f, 1.0f, 1.0f);
	DrawWaterParticle();
	glPopMatrix();

	glPushMatrix();
		glPushMatrix();
		glTranslatef(fCamx - 5.0f, -4.0f, -12.0f);
		glScalef(0.15f, 0.15f, 0.15f);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		human1();
		glPopMatrix();

		if(fCamx > 25.0f)
		{
			glPushMatrix();
			glTranslatef(fCamx - 5.0f, -4.0f, -10.0f);
			glScalef(0.15f, 0.15f, 0.15f);
			glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
			human2();
			glPopMatrix();
		}

	glPopMatrix();
}

// Draw the ground as a series of triangle strips
void DrawGrid(void)
{
	GLfloat fExtent = 80.0f;
	//GLfloat fExtent = 30.0f;
	GLfloat fStep = 0.3f;
	GLfloat y = posY - 1.5f;// + 5.0f; //-2.5f;
	static float fT = -1.0f;
	GLfloat fStrip, fRun;
	GLfloat s = 0.0f;
	GLfloat t = 0.0f;
	GLfloat texStep = (70 * 0.003f);
	//GLfloat texStep = /1000.0f+0.5f;
	// Ground is a tiling texture
	// Lay out strips and repeat textures coordinates
	for(fStrip = -fExtent; fStrip <= fExtent; fStrip += fStep)
	{
		//t = 0.0f;
		//s = 0.0f;

		if(fStrip > 0)
			glBindTexture(GL_TEXTURE_2D, texture_floor);
		else
			glBindTexture(GL_TEXTURE_2D, texture_village_floor);

		glBegin(GL_TRIANGLE_STRIP);
		for(fRun = fExtent/5; fRun >= -fExtent; fRun -= fStep)
		{
			glTexCoord2f(s, t);
			glNormal3f(0.0f, 1.0f, 0.0f); // All Point up
			glVertex3f(fStrip, y, fRun);
			glTexCoord2f(s + texStep, t);
			glNormal3f(0.0f, 1.0f, 0.0f); // All Point up
			glVertex3f(fStrip + fStep, y, fRun);
			t += texStep;
			s += texStep;
			//t = (GLfloat)(rand() / RAND_MAX / 10);
			
		}
		glEnd();
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	//Back Wall
	glBindTexture(GL_TEXTURE_2D, texture_background);
	//glBindTexture(GL_TEXTURE_2D, texture_village);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-fExtent, y, -30);
	glTexCoord2f(1, 0);
	glVertex3f(fExtent, y, -30);
	glTexCoord2f(1, 1);
	glVertex3f(fExtent, y + fExtent/2, -30);
	glTexCoord2f(0, 1);
	glVertex3f(-fExtent, y + fExtent/2, -30);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);

	//Right Wall
	glBindTexture(GL_TEXTURE_2D, texture_background);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(fExtent, y + fExtent/2, -30);
	glTexCoord2f(1, 0);
	glVertex3f(fExtent, y, -30);
	glTexCoord2f(1, 1);
	glVertex3f(fExtent, y , fExtent/5);
	glTexCoord2f(0, 1);
	glVertex3f(fExtent, y + fExtent/2, fExtent/5);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);	

	//Left Wall
	glBindTexture(GL_TEXTURE_2D, texture_background);
	glBegin(GL_QUADS);
	glTexCoord2f(1, 1);
	glVertex3f(-fExtent, y + fExtent/2, -30);
	glTexCoord2f(0, 1);
	glVertex3f(-fExtent, y, -30);
	glTexCoord2f(0, 0);
	glVertex3f(-fExtent, y , fExtent/5);
	glTexCoord2f(1, 0);
	glVertex3f(-fExtent, y + fExtent/2, fExtent/5);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);

	//City Road
	glBindTexture(GL_TEXTURE_2D, texture_city_road);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(0.0, y+ 0.05f, -25);
	glTexCoord2f(20, 0);
	glVertex3f(fExtent, y+ 0.05f, -25);
	glTexCoord2f(20, 20);
	glVertex3f(fExtent, y+ 0.05f , -15);
	glTexCoord2f(0, 20);
	glVertex3f(0.0, y + 0.05f , -15);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);		
	
}

void InitializeParticles()
{	
	int loop;
	for (loop=0;loop<MAX_PARTICLES;loop++)                   // Initialize All The Textures
	{
		particle[loop].active=TRUE;								// Make All The Particles Active
		particle[loop].life=0.1f;								// Give All The Particles Full Life
		particle[loop].fade=(float)(rand()%100)/1000.0f+0.9f;	// Random Fade Speed
		particle[loop].r=blue_dark[0];	
		particle[loop].g=blue_dark[1];	
		particle[loop].b=blue_dark[2];	
		particle[loop].xi=xspeed+(float)((rand()%60)-30.0f);	// X Axis Speed And Direction
		particle[loop].yi=posY;//yspeed+(float)((rand()%10)-30.0f);	// Y Axis Speed And Direction
		particle[loop].zi=zspeed+(float)((rand()%60)-30.0f);
		particle[loop].x = particle[loop].xi + 0.0f;
		particle[loop].y = posY;
		particle[loop].z = particle[loop].zi + 0.0f;
		particle[loop].xg=0.0f;									// Set Horizontal Pull 
		particle[loop].yg=0.8f;									// Set Vertical Pull 
		particle[loop].zg=0.0f;									// Set Pull On Z Axis To Zero

	}
}

void DrawWaterParticle()
{
	glDisable(GL_DEPTH_TEST);
	glPushMatrix();
	
	//glPushAttrib(GL_LIGHTING_BIT);
	//glDisable(GL_LIGHTING);
	int loop;

	for (loop=0;loop<MAX_PARTICLES;loop++)					// Loop Through All The Particles
	{
		//if (particle[loop].active)							// If The Particle Is Active
		{	
			if (particle[loop].active)
			{
				float x=particle[loop].x;						// Grab Our Particle X Position
				float y=particle[loop].y;						// Grab Our Particle Y Position
				float z=particle[loop].z;					// Particle Z Pos + Zoom

				// Draw The Particle Using Our RGB Values, Fade The Particle Based On It's Life
				glColor4f(particle[loop].r,particle[loop].g,particle[loop].b, particle[loop].life);
				glBindTexture(GL_TEXTURE_2D, texture_water);
				glBegin(GL_TRIANGLE_STRIP);						// Build Quad From A Triangle Strip
					glTexCoord2d(0.55,0.55); glVertex3f(x+PARTICLE_SIDE,y,z+PARTICLE_SIDE); // Top Right
					glTexCoord2d(0.45,0.55); glVertex3f(x-PARTICLE_SIDE,y,z+PARTICLE_SIDE); // Top Left
					glTexCoord2d(0.55,0.35); glVertex3f(x+PARTICLE_SIDE,y,z-PARTICLE_SIDE); // Bottom Right
					glTexCoord2d(0.45,0.45); glVertex3f(x-PARTICLE_SIDE,y,z-PARTICLE_SIDE); // Bottom Left
				glEnd();
				glBindTexture(GL_TEXTURE_2D, 0);
				// Done Building Triangle Strip
			}

			particle[loop].x+=particle[loop].xi/(1000.0f);// Move On The X Axis By X Speed
			particle[loop].y= posY - (0.5f * sin(2 * 3.14f * loop * 1.0f));// particle[loop].yi/(1000);// Move On The Y Axis By Y Speed
			particle[loop].z+= particle[loop].zi/(2000.0f);// Move On The Z Axis By Z Speed
			
			if((particle[loop].z < -20.0f) || (particle[loop].z > 20.0f)){
				//particle[loop].active = FALSE;
				particle[loop].life= -1.0f;		
			}

			if((particle[loop].x > 30.0f) || (particle[loop].x < -30.0f)){
				//particle[loop].active = FALSE;
				particle[loop].life= -1.0f;	
			}			
			particle[loop].xi+=(0.2f * sin(2 * 3.14f * loop * 1.0f)) ;//(particle[loop].xg + gravX);			// Take Pull On X Axis Into Account
			particle[loop].yi+= 0.0f;//(particle[loop].yg + gravY);			// Take Pull On Y Axis Into Account
			particle[loop].zi-=(0.2f * cos(2 * 3.14f * loop * 1.0f)) ;//(particle[loop].zg + gravZ);			// Take Pull On Z Axis Into Account
			//particle[loop].life-=particle[loop].fade;		// Reduce Particles Life By 'Fade'
						
			if (particle[loop].life<0.0f)					// If Particle Is Burned Out
			{
				particle[loop].active = TRUE;
				particle[loop].life=1.0f;					// Give It New Life
				particle[loop].fade=(float)(rand()%100)/1000.0f+0.9f;//0.003f;	// Random Fade Value
				particle[loop].x= particle[loop].xi + 0.5f + posX;						// Center On X Axis
				particle[loop].y=posY;// Center On Y Axis
				particle[loop].z= particle[loop].zi;// + 30.0f + posZ;						// Center On Z Axis
				particle[loop].xi=xspeed+(float)((rand()%60)-30.0f);	// X Axis Speed And Direction
				particle[loop].yi=posY + 0.0f;//yspeed+(float)((rand()%10)-30.0f);	// Y Axis Speed And Direction
				particle[loop].zi=zspeed+(float)((rand()%40 - 20.0f));	// Z Axis Speed And Directio

			}
			
			/*else if (particle[loop].life<0.75f && ( (particle[loop].z < -25.0f) || (particle[loop].z > 25.0f) || (	 (particle[loop].x > 25.0f) || (particle[loop].x < -25.0f))) )
			{
				particle[loop].r=blue[0];			
				particle[loop].g=blue[1];			
				particle[loop].b=blue[2];				
			}*/
			else if (particle[loop].life<0.75f && ( (particle[loop].z < -18.0f) || (particle[loop].z > 18.0f) || (	 (particle[loop].x > 28.0f) || (particle[loop].x < -28.0f))) )
			{
				particle[loop].r=blue[0];			
				particle[loop].g=blue[1];			
				particle[loop].b=blue[2];				
			}	

			if((particle[loop].z < -20.0f) || (particle[loop].z > 20.0f)){
				particle[loop].active = FALSE;
				//particle[loop].life= -1.0f;		
			}
	
			if(particle[loop].x <= 0.0f)
			{
				particle[loop].r=blue_dark[0];			
				particle[loop].g=blue_dark[1];			
				particle[loop].b=blue_dark[2];
			}
			else
			{
				particle[loop].r=blue_polluted[0];			
				particle[loop].g=blue_polluted[1];			
				particle[loop].b=blue_polluted[2];
			}

			if((particle[loop].x > 30.0f) || (particle[loop].x < -30.0f)){
				particle[loop].active = FALSE;
				//particle[loop].life= -1.0f;	
			}	

			
		}
    }
	glBindTexture(GL_TEXTURE_2D, 0);
	//glPopAttrib();
	glPopMatrix();

	glEnable(GL_DEPTH_TEST);

	glColor3f(1.0f, 1.0f, 1.0f);
}


