// Standard header file
#include<stdio.h>  // For printf
#include<stdlib.h> // For Exit
#include<memory.h> // for memset 
#include <math.h>
// X11 Headerfile
#include<X11/Xlib.h> //For all X11 API
#include<X11/Xutil.h> // For XVisualInfo and related API
#include<X11/XKBlib.h>

//OpenGL Heasder files
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>
#include <SOIL/SOIL.h>

#include "BharatLetters.h"

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


GLfloat fBaseDepth = -15.0f;
GLfloat fAlphaFactor = 1.0f;
void DrawPartialCircle(float fXCenterPos, float fYCenterPos, float fRadius, int firstPos, int secondPos, float* fColorRgbV, float* fColorRgbV2, int);

GLfloat cAngle = 0.0f;

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
    void StaticBharat();
	void Background();

 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Now here screen gets colored as color mentioned in initialize()

	glMatrixMode(GL_MODELVIEW); //

	glLoadIdentity(); // Set Matrix to identity

	glLoadIdentity(); // Set Matrix to identity

	//gluLookAt(fCamx, fCamy, fCamz + 10.0f, 0.0f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f);
	glTranslatef(0.0f, 0.0f, fBaseDepth * 1.5);

	glPushMatrix();

	Background();

	StaticBharat();

	glPopMatrix();

	glXSwapBuffers(display, window);
}

void StaticBharat()
{
	//glRotatef(cAngle, 1.0f, 0.0f, 0.0f);
	//glRotatef(cAngle, 0.0f, 1.0f, 0.0f);
	//glRotatef(cAngle, 0.0f, 0.0f, 1.0f);
    glScalef(0.6f, 0.6f, 1.0f);
	glTranslatef(-fLetterHalfWidth * 2.5f * 3.0f, 0.0f, 0.0f);

	//glRotatef(cAngle, 1.0f, 1.0f, 1.0f);

	DrawB();

	glTranslatef(fLetterHalfWidth * 3.0f, 0.0f, 0.0f);

	DrawH();

	glTranslatef(fLetterHalfWidth * 3.0f, 0.0f, 0.0f);
	
	DrawA();

	glTranslatef(fLetterHalfWidth * 3.0f, 0.0f, 0.0f);
	
	DrawR();

	glTranslatef(fLetterHalfWidth * 3.0f, 0.0f, 0.0f);
	
	DrawA();

	glTranslatef(fLetterHalfWidth * 3.0f, 0.0f, 0.0f);

	DrawT();
}

void Background()
{
	glPushMatrix();

	// Additional Depth increased
	glTranslatef(0.0f, 0.0f, fBaseDepth);

	glBegin(GL_QUADS);
	glColor4f(0.50f, 0.50f, 0.50f, 1.0f);
	glVertex3f(100.0, 100.0, 0.0f);
	glVertex3f(-100.0, 100.0, 0.0f);
	glVertex3f(-100.0, -100.0, 0.0f);
	glVertex3f(100.0, -100.0, 0.0f);
	glEnd();

	glPopMatrix();
}

void DrawPartialCircle(float fXCenterPos, float fYCenterPos, float fRadius, int iStartingAngle, int iEndAngle, float* fColorRgbV, float* fColorRgbV2, int ColrFragmentFactor)
{
	float fFirstAng = 0.0f;
	float fSecondAng = 0.0f;

	glPointSize(2.0f);

	if (iStartingAngle > iEndAngle)
	{
		iStartingAngle = -1 * (360 - iStartingAngle);

	}
	float fColorGradient = 0.0f;
	float j = 1;
	for (int i = iStartingAngle; i <= iEndAngle; i++) //(2.0f * PI) / firstPos
	{
		float angle = i * 3.142 / 180;
		float fXCord = fXCenterPos + (2.0 * fRadius * cos(angle));
		float fYCord = fYCenterPos + (fRadius * sin(angle));
		glBegin(GL_QUADS);

		fColorGradient = fColorGradient + 0.006f;
		float fColor[4];
		fColor[0] = (fColorRgbV[0] * (1.0f - fColorGradient)) + (fColorRgbV2[0] * fColorGradient);
		fColor[1] = (fColorRgbV[1] * (1.0f - fColorGradient)) + (fColorRgbV2[1] * fColorGradient);
		fColor[2] = (fColorRgbV[2] * (1.0f - fColorGradient)) + (fColorRgbV2[2] * fColorGradient);

		glColor4f(fColor[0], fColor[1], fColor[2], 1.0f);
		//glVertex3f(fXCord, fYCord, 0.0f);
		glVertex3f(fXCord, fYCord - fLetterStickHalfWidth, 0.0f);
		glVertex3f(fXCord - fLetterStickHalfWidth, fYCord - fLetterStickHalfWidth, 0.0f);
		glVertex3f(fXCord - fLetterStickHalfWidth, fYCord + fLetterStickHalfWidth, 0.0f);
		glVertex3f(fXCord, fYCord + fLetterStickHalfWidth, 0.0f);
		glEnd();
	}


}

void update(void)
{
	//Code
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
