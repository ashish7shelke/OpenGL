// Standard header file
#include<stdio.h>  // For printf
#include<stdlib.h> // For Exit
#include<memory.h> // for memset 

// X11 Headerfile
#include<X11/Xlib.h> //For all X11 API
#include<X11/Xutil.h> // For XVisualInfo and related API
#include<X11/XKBlib.h>

//OpenGL Heasder files
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>
#include <SOIL/SOIL.h>

//Macros
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

//Global 
Display *display = NULL;
Colormap colormap; 
Window window;
XVisualInfo* defaultVisualInfo = NULL;
Bool bFullscreen = False;
Bool bActiveWindow = False;

//OpenGL global Variable
GLXContext glxContext = NULL;

GLfloat pAngle = 0.0f;
GLfloat cAngle = 0.0f;

// Lights Variables
Bool gbLight = False;
//Texture Object
GLuint texture_floor = 0;

//Lights Variables
GLfloat fLightAmbientZero[] = {1.0f, 1.0f, 1.0f, 1.0f}; // Grey
GLfloat fLightDiffusedZero[] = {1.0f, 1.0f, 1.0f, 1.0f}; // White 
GLfloat fLightSpecularZero[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat fLightPositionZero[] = {0.0f, 4.0f, 0.0f, 1.0f};
GLfloat fLightSpotDirectionZero[] = {0.0f, -1.0f, 0.0f};
GLfloat fLightSpotCutoffZero = 30.0f;
GLfloat fLightSpotExponentZero = 40.0f;

GLfloat fLightAmbientOne[] = {0.0f, 0.0f, 1.0f, 1.0f}; // Grey
GLfloat fLightDiffusedOne[] = {0.0f, 0.0f, 1.0f, 1.0f}; // White 
GLfloat fLightSpecularOne[] = {0.0f, 0.0f, 1.0f, 1.0f};
GLfloat fLightPositionOne[] = {-6.0f, 4.0f, 4.0f, 1.0f};
GLfloat fLightSpotDirectionOne[] = {0.0f, -1.0f, 0.0f};
GLfloat fLightSpotCutoffOne = 30.0f;
GLfloat fLightSpotExponentOne = 40.0f;

GLfloat fLightAmbientTwo[] = {0.0f, 1.0f, 0.0f, 1.0f}; // Grey
GLfloat fLightDiffusedTwo[] = {0.0f, 1.0f, 0.0f, 1.0f}; // White 
GLfloat fLightSpecularTwo[] = {0.0f, 1.0f, 0.0f, 1.0f};
GLfloat fLightPositionTwo[] = {-6.0f, 4.0f, -4.0f, 1.0f};
GLfloat fLightSpotDirectionTwo[] = {0.0f, -1.0f, 0.0f};
GLfloat fLightSpotCutoffTwo = 30.0f;
GLfloat fLightSpotExponentTwo = 40.0f;

GLfloat fLightAmbientThree[] = {1.0f, 0.0f, 0.0f, 1.0f}; // Grey
GLfloat fLightDiffusedThree[] = {1.0f, 0.0f, 0.0f, 1.0f}; // White 
GLfloat fLightSpecularThree[] = {1.0f, 0.0f, 0.0f, 1.0f};
GLfloat fLightPositionThree[] = {6.0f, 4.0f, -4.0f, 1.0f};
GLfloat fLightSpotDirectionThree[] = {0.0f, -1.0f, 0.0f};
GLfloat fLightSpotCutoffThree = 30.0f;
GLfloat fLightSpotExponentThree = 40.0f;

GLfloat fLightAmbientFour[] = {1.0f, 1.0f, 0.0f, 1.0f}; // Grey
GLfloat fLightDiffusedFour[] = {1.0f, 1.0f, 0.0f, 1.0f}; // White 
GLfloat fLightSpecularFour[] = {1.0f, 1.0f, 0.0f, 1.0f};
GLfloat fLightPositionFour[] = {6.0f, 4.0f, 4.0f, 1.0f};
GLfloat fLightSpotDirectionFour[] = {0.0f, -1.0f, 0.0f};
GLfloat fLightSpotCutoffFour = 30.0f;
GLfloat fLightSpotExponentFour = 40.0f;

GLUquadric* quadric = NULL;


int main(void)
{
    //Local Function declarations
    void toggleFullScreen(void);
    int initialize(void);
    void resize(int, int);
    void draw(void);
    void update(void);
    void uninitialize(void);


    //Local Variables
    int defaultScreen;
    XSetWindowAttributes windowAtrributes;
    int styleMask;
    Atom windowMangerDelete;
    XEvent event;
    KeySym keySymbol;
    int screenWidth;
    int screenHeight;
    char keys[26];// = {'F', 'f', 'L', 'l'};
    Bool bDone = False;

    //OpenGL Variables
    int frameBufferAttributes[] = {
                                    GLX_DOUBLEBUFFER, True,
                                    GLX_RGBA,
                                    GLX_RED_SIZE, 8,
                                    GLX_GREEN_SIZE, 8,
                                    GLX_BLUE_SIZE, 8,
                                    GLX_ALPHA_SIZE, 8,
                                    GLX_DEPTH_SIZE, 24,
                                    None
                                };

    //Code

    //Step 1: Open the connection with Xserver and get the display interface
    display = XOpenDisplay(NULL); //Open connection and get display - Parameter NULL(Default) or display ID
    if(display == NULL)
    {
        printf("XOpenDisplay() Failed \n");
        uninitialize();
        exit(1);
    }

    //Step 2: Get default screen from above display
    defaultScreen = XDefaultScreen(display);

    //Step 4: Get Visual info from above 3
    //This visualInfo is xwindow specific, openGl specific and bridging API
    defaultVisualInfo = glXChooseVisual(display, defaultScreen, frameBufferAttributes);
    if(defaultVisualInfo == NULL)
    {
        printf("glXChooseVisual() Failed \n");
        uninitialize();
        exit(1);
    }

    //Step5: Set window atributes / properties
    memset((void *)&windowAtrributes, 0, sizeof(XSetWindowAttributes));
    windowAtrributes.border_pixel = 0; // 0 for default
    windowAtrributes.background_pixel = XBlackPixel(display, defaultVisualInfo->screen); // for default BlackPixel(display,visualinfo.screen);
    windowAtrributes.background_pixmap = 0;
    windowAtrributes.colormap = XCreateColormap(display, 
                                                XRootWindow(display, defaultVisualInfo->screen),
                                                defaultVisualInfo->visual,
                                                AllocNone);

    //Assign thie colormap to global colormap
    colormap = windowAtrributes.colormap;
    
    //Set the style of window - WS_OVERLAPPED_WINDOW 
    styleMask = CWBorderPixel | CWBackPixel | CWColormap | CWEventMask;

    //Step 6: Create Window
    window = XCreateWindow(display, 
                            XRootWindow(display, defaultVisualInfo->screen),
                            0,
                            0,
                            WIN_WIDTH,
                            WIN_HEIGHT,
                            0,
                            defaultVisualInfo->depth,
                            InputOutput,
                            defaultVisualInfo->visual,
                            styleMask,
                            &windowAtrributes);

    if(!window)
    {
        printf("XCreateWindow failed \n");
        uninitialize();
        exit(1);
    }

    //Step 7: Specify to which events this window should response
    //ExposureMask - WM_PAINT
    //VisibilityChangeMask - Minimize/Maximize
    //StructureNotifyMask - WM_STYLE
    //PointerMotion - WM_MOUSEMOVE
    XSelectInput(display, window, ExposureMask | VisibilityChangeMask | StructureNotifyMask | KeyPressMask | ButtonPressMask | PointerMotionMask | FocusChangeMask);
    
    //Step 8: Specify Window Manager Delete Atom
    windowMangerDelete = XInternAtom(display, "WM_DELETE_WINDOW", True);

    //Chrestofer Tronche & Kenton Li - XLib documentation
    //HPUX - HP Unix 
    //MAN Pages

    //Step 9: Add/Set above Atom as protocol for WindowManager
    XSetWMProtocols(display, window, &windowMangerDelete, 1);

    //Step 10: Give CAption to Window
    XStoreName(display, window, "AGS-XWindow");

    //Step 11:Map/Show the Window
    XMapWindow(display, window);

    //Centering of Window
    screenWidth = XWidthOfScreen(XScreenOfDisplay(display, defaultVisualInfo->screen));
    screenHeight = XHeightOfScreen(XScreenOfDisplay(display, defaultVisualInfo->screen));
    XMoveWindow(display, window, (screenWidth - WIN_WIDTH)/2, (screenHeight - WIN_HEIGHT)/2);

    //Step 12: OpenGL initialization
    if(initialize())
    {
        uninitialize();
        exit(1);
    }

    //Step 13: Game Loop
    while(bDone == False)
    {
        while(XPending(display))
        {
            
            XNextEvent(display, &event);

            switch(event.type)
            {
                case MapNotify: //Treated as WM_CREATE, 
                    
                    break;

                case FocusIn: //SetFocus
                    bActiveWindow = True;
                    break; 

                case FocusOut: //KillFocus
                    bActiveWindow = False;
                    break; 

                case ConfigureNotify: // WM_SIZE
                    resize(event.xconfigure.width, event.xconfigure.height);
                    break;

                case ButtonPress:
                    switch(event.xbutton.button)
                    {
                        case 1:
                            
                            break; 
                        case 2:
                            
                            break; 
                        case 3:
                            
                            break; 
                    }
                    break;
            
                case KeyPress:
                    {
                        keySymbol = XkbKeycodeToKeysym(display, event.xkey.keycode, 0, 0);

                        switch (keySymbol)
                        {
                            case XK_Escape:
                                bDone = True;
                                break;  
                            
                            default:
                                break;
                        }
                    }
                    XLookupString(&event.xkey, keys, sizeof(keys), NULL, NULL);
                    switch(keys[0])
                    {
                        case 'F':
                        case 'f':
                            if(bFullscreen == False)
                            {
                                toggleFullScreen();
                                bFullscreen = True;
                            }
                            else
                            {
                                toggleFullScreen();
                                bFullscreen = False;
                            }
                            break;

                        case 'L':
                        case 'l':

                            if (gbLight == False)
                            {
                                glEnable(GL_LIGHTING);
                                gbLight = True;
                            }
                            else
                            {
                                glDisable(GL_LIGHTING);
                                gbLight = False;
                            }
                            break;

                        default:
                            break;   
                    }
                    break;

                case 33:
                    bDone = True;
                    break;

                default:
                    break;
            }
        }

        //Rendering
        if(bActiveWindow == True)
        {
            draw();
            update();
        }   
    }

    uninitialize();
    return 0;
}

void toggleFullScreen(void)
{
    //Local Variable declarations
    Atom windowManagerStateNormal;
    Atom windowManagerStateFullscreen;
    XEvent event; 

    //Code

    //Step 1: Get Atom from Window Manager 
    windowManagerStateNormal = XInternAtom(display, "_NET_WM_STATE", False);

    //Step 2: Window Manager State Full Screen
    windowManagerStateFullscreen = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", False);

    //Step 3: Memset and Fill Event Structure with above 2 items
    memset((void *)&event, 0, sizeof(XEvent));
    event.type = ClientMessage;
    event.xclient.window = window;
    event.xclient.message_type = windowManagerStateNormal;
    event.xclient.format = 32;
    event.xclient.data.l[0] = bFullscreen ? 0 : 1;
    event.xclient.data.l[1] = windowManagerStateFullscreen;

    //Step 4: Send the event
    XSendEvent(display, 
                XRootWindow(display, defaultVisualInfo->screen),
                False,//Dont pass event to child window OR event propagation to child
                SubstructureNotifyMask, //It informs that your size is changing, Substructure Notify Mask
                &event);    

}

int initialize(void)
{   
    //Local function declarations
    void resize(int , int);    
    void uninitialize(void);

    //Crete openGL context
    glxContext = glXCreateContext(display, defaultVisualInfo, NULL, True);
    if(glxContext == NULL)
    {
        printf("glXCreateContext() failed \n");
        return -1;
    }

    //Make this context as current context
    if(glXMakeCurrent(display, window, glxContext) == False)
    {
        printf("glXMakeCurrent() failed \n");
        return -2;
    }

	//Enabling Depth
	glShadeModel(GL_SMOOTH); // Beautification - Whenever lighting and fading 
	glClearDepth(1.0f); // Compulsory - Default depth
	glEnable(GL_DEPTH_TEST); // Compulsory - Enable Depth Test
	glDepthFunc(GL_LEQUAL); // Compulsary - Depth is less than or equal to 
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); //Beutification - 
    
	// Light Initialization
	glLightfv(GL_LIGHT0, GL_AMBIENT, fLightAmbientZero);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, fLightDiffusedZero);
	glLightfv(GL_LIGHT0, GL_SPECULAR, fLightSpecularZero);
	glLightfv(GL_LIGHT0, GL_POSITION, fLightPositionZero);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, fLightSpotCutoffZero);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, fLightSpotDirectionZero);
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, fLightSpotExponentZero);
	glEnable(GL_LIGHT0);

	// Light Initialization
	glLightfv(GL_LIGHT1, GL_AMBIENT, fLightAmbientOne);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, fLightDiffusedOne);
	glLightfv(GL_LIGHT1, GL_SPECULAR, fLightSpecularOne);
	glLightfv(GL_LIGHT1, GL_POSITION, fLightPositionOne);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, fLightSpotCutoffOne);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, fLightSpotDirectionOne);
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, fLightSpotExponentOne);
	glEnable(GL_LIGHT1);

	// Light Initialization
	glLightfv(GL_LIGHT2, GL_AMBIENT, fLightAmbientTwo);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, fLightDiffusedTwo);
	glLightfv(GL_LIGHT2, GL_SPECULAR, fLightSpecularTwo);
	glLightfv(GL_LIGHT2, GL_POSITION, fLightPositionTwo);
	glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, fLightSpotCutoffTwo);
	glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, fLightSpotDirectionTwo);
	glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, fLightSpotExponentTwo);
	glEnable(GL_LIGHT2);

	// Light Initialization
	glLightfv(GL_LIGHT3, GL_AMBIENT, fLightAmbientThree);
	glLightfv(GL_LIGHT3, GL_DIFFUSE, fLightDiffusedThree);
	glLightfv(GL_LIGHT3, GL_SPECULAR, fLightSpecularThree);
	glLightfv(GL_LIGHT3, GL_POSITION, fLightPositionThree);
	glLightf(GL_LIGHT3, GL_SPOT_CUTOFF, fLightSpotCutoffThree);
	glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, fLightSpotDirectionThree);
	glLightf(GL_LIGHT3, GL_SPOT_EXPONENT, fLightSpotExponentThree);
	glEnable(GL_LIGHT3);

	// Light Initialization
	glLightfv(GL_LIGHT4, GL_AMBIENT, fLightAmbientFour);
	glLightfv(GL_LIGHT4, GL_DIFFUSE, fLightDiffusedFour);
	glLightfv(GL_LIGHT4, GL_SPECULAR, fLightSpecularFour);
	glLightfv(GL_LIGHT4, GL_POSITION, fLightPositionFour);
	glLightf(GL_LIGHT4, GL_SPOT_CUTOFF, fLightSpotCutoffFour);
	glLightfv(GL_LIGHT4, GL_SPOT_DIRECTION, fLightSpotDirectionFour);
	glLightf(GL_LIGHT4, GL_SPOT_EXPONENT, fLightSpotExponentFour);
	glEnable(GL_LIGHT4);	

    //Initialize Quadric
	quadric = gluNewQuadric();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    //Useal OpenGL Code
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    //Warm Resize
    resize(WIN_WIDTH, WIN_HEIGHT);

    return 0;

}

void resize(int width, int height)
{
    if(height == 0)
    {
        height = 1;
    }

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();   

    glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void draw(void)
{
    void DrawGrid(void);
    
 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Now here screen gets colored as color mentioned in initialize()

	glMatrixMode(GL_MODELVIEW); //

	glLoadIdentity(); // Set Matrix to identity

	gluLookAt(0.0f, 0.0f, 15.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	glPushMatrix();
	glLightfv(GL_LIGHT0, GL_POSITION, fLightPositionZero);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, fLightSpotCutoffZero);
	glPushAttrib(GL_LIGHTING_BIT);
	glDisable(GL_LIGHTING);
	glTranslatef(fLightPositionZero[0], fLightPositionZero[1], fLightPositionZero[2]);
	glColor3f(1.0f, 1.0f, 1.0f);
	gluSphere(quadric, 0.5f, 50, 50);
	glPopAttrib();
	glPopMatrix();

	glPushMatrix();
	glLightfv(GL_LIGHT1, GL_POSITION, fLightPositionOne);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, fLightSpotCutoffOne);
	glPushAttrib(GL_LIGHTING_BIT);
	glDisable(GL_LIGHTING);
	glTranslatef(fLightPositionOne[0], fLightPositionOne[1], fLightPositionOne[2]);
	glColor3f(0.0f, 0.0f, 1.0f);
	gluSphere(quadric, 0.5f, 50, 50);
	glPopAttrib();
	glPopMatrix();

	glPushMatrix();
	glLightfv(GL_LIGHT2, GL_POSITION, fLightPositionTwo);
	glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, fLightSpotCutoffTwo);
	glPushAttrib(GL_LIGHTING_BIT);
	glDisable(GL_LIGHTING);
	glTranslatef(fLightPositionTwo[0], fLightPositionTwo[1], fLightPositionTwo[2]);
	glColor3f(0.0f, 1.0f, 0.0f);
	gluSphere(quadric, 0.5f, 50, 50);
	glPopAttrib();
	glPopMatrix();

	glPushMatrix();
	glLightfv(GL_LIGHT3, GL_POSITION, fLightPositionThree);
	glLightf(GL_LIGHT3, GL_SPOT_CUTOFF, fLightSpotCutoffThree);
	glPushAttrib(GL_LIGHTING_BIT);
	glDisable(GL_LIGHTING);
	glTranslatef(fLightPositionThree[0], fLightPositionThree[1], fLightPositionThree[2]);
	glColor3f(1.0f, 0.0f, 0.0f);
	gluSphere(quadric, 0.5f, 50, 50);
	glPopAttrib();
	glPopMatrix();

	glPushMatrix();
	glLightfv(GL_LIGHT4, GL_POSITION, fLightPositionFour);
	glLightf(GL_LIGHT4, GL_SPOT_CUTOFF, fLightSpotCutoffFour);
	glPushAttrib(GL_LIGHTING_BIT);
	glDisable(GL_LIGHTING);
	glTranslatef(fLightPositionFour[0], fLightPositionFour[1], fLightPositionFour[2]);
	glColor3f(1.0f, 1.0f, 0.0f);
	gluSphere(quadric, 0.5f, 50, 50);
	glPopAttrib();
	glPopMatrix();				

	glColor3f(1.0f, 1.0f, 1.0f);
	DrawGrid();


	glXSwapBuffers(display, window);
}

// Draw the ground as a series of triangle strips
void DrawGrid(void)
{
	GLfloat fExtent = 10.0f;
	GLfloat fStep = 0.3f;
	GLfloat y = -2.5f;
	static float fT = -1.0f;
	GLfloat fStrip, fRun;
	GLfloat s = 0.0f;
	GLfloat t = 0.0f;
	GLfloat texStep = 1.0f / (fExtent * .075f);
	// Ground is a tiling texture
	glBindTexture(GL_TEXTURE_2D, texture_floor);
	// Lay out strips and repeat textures coordinates
	for(fStrip = -fExtent; fStrip <= fExtent; fStrip += fStep)
	{
		t = 0.0f;
		glBegin(GL_TRIANGLE_STRIP);
		for(fRun = fExtent; fRun >= -fExtent; fRun -= fStep)
		{
			glTexCoord2f(s, t);
			glNormal3f(0.0f, 1.0f, 0.0f); // All Point up
			glVertex3f(fStrip, y, fRun);
			glTexCoord2f(s + texStep, t);
			glNormal3f(0.0f, 1.0f, 0.0f); // All Point up
			glVertex3f(fStrip + fStep, y, fRun);
			t += texStep;
		}
		glEnd();
		s += texStep;
	}
}

void update(void)
{
    // Code
	pAngle = pAngle + 0.1f;

	if (pAngle >= 360.0f)
		pAngle = pAngle - 360.0f;

	cAngle = cAngle - 0.1f;

	if (cAngle <= 0.0f)
		cAngle = cAngle + 360.0f;
}

void uninitialize(void)
{
    //local variables
    GLXContext currentGlxContext;

    //Code
    //Uncurrent the context
    if(quadric)
    {
        gluDeleteQuadric(quadric);
        quadric = NULL;
    }
    currentGlxContext = glXGetCurrentContext();
    if((currentGlxContext != NULL) && (currentGlxContext == glxContext))
    {
        glXMakeCurrent(display, None, NULL);
    }

    if(glxContext)
    {
        glXDestroyContext(display, glxContext);
    }

    if(defaultVisualInfo)
    {
        free(defaultVisualInfo);
        defaultVisualInfo = NULL;
    } 

    if(window)
        XDestroyWindow(display, window);

    if(colormap)
        XFreeColormap(display, colormap);

    if(display)
    {
        XCloseDisplay(display);
        display = NULL;
    }    
}
