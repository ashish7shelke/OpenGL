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

// Lights Variables
Bool gbLight = False;
GLfloat fLightAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat fLightDiffused[] = { 1.0f, 1.0f, 1.0f, 1.0f };// White
//GLfloat fLightSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat fLightPosition[] = { 0.0f, 0.0f, 0.0f, 1.0f };

GLfloat fBaseDepth = -24.0f;

GLfloat fAngleForXRotation = 0.0f;
GLfloat fAngleForYRotation = 0.0f;
GLfloat fAngleForZRotation = 0.0f;

GLuint KeyPressed;

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
                        case 'X':
                        case 'x':
                            KeyPressed = 1;
                            fAngleForXRotation = 0.0f; //Reset
                            break;

                        case 'Y':
                        case 'y':
                            KeyPressed = 2;
                            fAngleForYRotation = 0.0f; // Reset
                            break;

                        case 'Z':
                        case 'z':
                            KeyPressed = 3;
                            fAngleForZRotation = 0.0f; // Reset
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
    
    // Light related initialization
	// Light Initialization
	glLightfv(GL_LIGHT0, GL_AMBIENT, fLightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, fLightDiffused);
	glEnable(GL_LIGHT0);

    //Initialize Quadric
	quadric = gluNewQuadric();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    //Useal OpenGL Code
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

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
    GLfloat MaterialAmbient[4];
	GLfloat MaterialDiffused[4];
	GLfloat MaterialSpecular[4];
	GLfloat MaterialShininess;

 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Now here screen gets colored as color mentioned in initialize()

	glMatrixMode(GL_MODELVIEW); //

	glLoadIdentity(); // Set Matrix to identity

	//Animation
	if (KeyPressed == 1)
	{
		glRotatef(fAngleForXRotation, 1.0f, 0.0f, 0.0f);
		fLightPosition[2] = fAngleForXRotation;
	}
	else if (KeyPressed == 2)
	{
		glRotatef(fAngleForYRotation, 0.0f, 1.0f, 0.0f);
		fLightPosition[0] = fAngleForYRotation;
	}
	else if (KeyPressed == 3)
	{
		glRotatef(fAngleForZRotation, 0.0f, 0.0f, 1.0f);
		fLightPosition[1] = fAngleForZRotation;
	}
	glLightfv(GL_LIGHT0, GL_POSITION, fLightPosition);

	// Material Details
	//First Column Precious Stone: 
	//Emarald material
	MaterialAmbient[0] = 0.0215; // r
	MaterialAmbient[1] = 0.1745; // g
	MaterialAmbient[2] = 0.0215; // b
	MaterialAmbient[3] = 1.0f;   // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffused[0] = 0.07568; // r
	MaterialDiffused[1] = 0.61424; // g
	MaterialDiffused[2] = 0.07568; // b
	MaterialDiffused[3] = 1.0f;   // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffused);

	MaterialSpecular[0] = 0.633; // r
	MaterialSpecular[1] = 0.727811; // g
	MaterialSpecular[2] = 0.633; // b
	MaterialSpecular[3] = 1.0f;   // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShininess = 0.6 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);

	glLoadIdentity();
	glTranslatef(-9.0f, 6.0f, fBaseDepth);
	gluSphere(quadric, 1.0f, 30, 30);

	//Jade
	MaterialAmbient[0] = 0.135; // r
	MaterialAmbient[1] = 0.2225; // g
	MaterialAmbient[2] = 0.1575; // b
	MaterialAmbient[3] = 1.0f;   // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffused[0] = 0.54; // r
	MaterialDiffused[1] = 0.54; // g
	MaterialDiffused[2] = 0.63; // b
	MaterialDiffused[3] = 1.0f;   // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffused);

	MaterialSpecular[0] = 0.316228; // r
	MaterialSpecular[1] = 0.316228; // g
	MaterialSpecular[2] = 0.316228; // b
	MaterialSpecular[3] = 1.0f;   // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShininess = 0.1 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);

	glLoadIdentity();
	glTranslatef(-9.0f, 3.5f, fBaseDepth);
	gluSphere(quadric, 1.0f, 30, 30);

	//Obsidian
	MaterialAmbient[0] = 0.05375; // r
	MaterialAmbient[1] = 0.05; // g
	MaterialAmbient[2] = 0.06625; // b
	MaterialAmbient[3] = 1.0f;   // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffused[0] = 0.18275; // r
	MaterialDiffused[1] = 0.17; // g
	MaterialDiffused[2] = 0.22525; // b
	MaterialDiffused[3] = 1.0f;   // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffused);

	MaterialSpecular[0] = 0.332741; // r
	MaterialSpecular[1] = 0.328634; // g
	MaterialSpecular[2] = 0.346435; // b
	MaterialSpecular[3] = 1.0f;   // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShininess = 0.3 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);

	glLoadIdentity();
	glTranslatef(-9.0f, 1.0f, fBaseDepth);
	gluSphere(quadric, 1.0f, 30, 30);

	//Pearl
	MaterialAmbient[0] = 0.25; // r
	MaterialAmbient[1] = 0.20725; // g
	MaterialAmbient[2] = 0.20725; // b
	MaterialAmbient[3] = 1.0f;   // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffused[0] = 1.0; // r
	MaterialDiffused[1] = 0.829; // g
	MaterialDiffused[2] = 0.829; // b
	MaterialDiffused[3] = 1.0f;   // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffused);

	MaterialSpecular[0] = 0.296648; // r
	MaterialSpecular[1] = 0.296648; // g
	MaterialSpecular[2] = 0.296648; // b
	MaterialSpecular[3] = 1.0f;   // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShininess = 0.088 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);

	glLoadIdentity();
	glTranslatef(-9.0f, -1.5f, fBaseDepth);
	gluSphere(quadric, 1.0f, 30, 30);

	//Ruby
	MaterialAmbient[0] = 0.1745; // r
	MaterialAmbient[1] = 0.01175; // g
	MaterialAmbient[2] = 0.01175; // b
	MaterialAmbient[3] = 1.0f;   // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffused[0] = 0.61424; // r
	MaterialDiffused[1] = 0.04136; // g
	MaterialDiffused[2] = 0.04136; // b
	MaterialDiffused[3] = 1.0f;   // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffused);

	MaterialSpecular[0] = 0.727811; // r
	MaterialSpecular[1] = 0.626959; // g
	MaterialSpecular[2] = 0.626959; // b
	MaterialSpecular[3] = 1.0f;   // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShininess = 0.6 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);

	glLoadIdentity();
	glTranslatef(-9.0f, -4.0f, fBaseDepth);
	gluSphere(quadric, 1.0f, 30, 30);

	//Turquoise
	MaterialAmbient[0] = 0.1; // r
	MaterialAmbient[1] = 0.18725; // g
	MaterialAmbient[2] = 0.1745; // b
	MaterialAmbient[3] = 1.0f;   // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffused[0] = 0.396; // r
	MaterialDiffused[1] = 0.74151; // g
	MaterialDiffused[2] = 0.69102; // b
	MaterialDiffused[3] = 1.0f;   // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffused);

	MaterialSpecular[0] = 0.297254; // r
	MaterialSpecular[1] = 0.30829; // g
	MaterialSpecular[2] = 0.306678; // b
	MaterialSpecular[3] = 1.0f;   // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShininess = 0.1 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);

	glLoadIdentity();
	glTranslatef(-9.0f, -6.5f, fBaseDepth);
	gluSphere(quadric, 1.0f, 30, 30);

	//Second Column
	//Brass
	MaterialAmbient[0] = 0.329412; // r
	MaterialAmbient[1] = 0.223529; // g
	MaterialAmbient[2] = 0.027451; // b
	MaterialAmbient[3] = 1.0f;   // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffused[0] = 0.780392; // r
	MaterialDiffused[1] = 0.568627; // g
	MaterialDiffused[2] = 0.113725; // b
	MaterialDiffused[3] = 1.0f;   // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffused);

	MaterialSpecular[0] = 0.992157; // r
	MaterialSpecular[1] = 0.941176; // g
	MaterialSpecular[2] = 0.807843; // b
	MaterialSpecular[3] = 1.0f;   // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShininess = 0.21794872 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);

	glLoadIdentity();
	glTranslatef(-3.0f, 6.0f, fBaseDepth);
	gluSphere(quadric, 1.0f, 30, 30);

	//Bronze
	MaterialAmbient[0] = 0.2125; // r
	MaterialAmbient[1] = 0.1275; // g
	MaterialAmbient[2] = 0.054;  // b
	MaterialAmbient[3] = 1.0f;   // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffused[0] = 0.714;   // r
	MaterialDiffused[1] = 0.4284;  // g
	MaterialDiffused[2] = 0.18144; // b
	MaterialDiffused[3] = 1.0f;    // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffused);

	MaterialSpecular[0] = 0.393548; // r
	MaterialSpecular[1] = 0.271906; // g
	MaterialSpecular[2] = 0.166721; // b
	MaterialSpecular[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShininess = 0.2 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);

	glLoadIdentity();
	glTranslatef(-3.0f, 3.5f, fBaseDepth);
	gluSphere(quadric, 1.0f, 30, 30);

	//Chrome
	MaterialAmbient[0] = 0.25; // r
	MaterialAmbient[1] = 0.25; // g
	MaterialAmbient[2] = 0.25; // b
	MaterialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffused[0] = 0.4;  // r
	MaterialDiffused[1] = 0.4;  // g
	MaterialDiffused[2] = 0.4;  // b
	MaterialDiffused[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffused);

	MaterialSpecular[0] = 0.774597; // r
	MaterialSpecular[1] = 0.774597; // g
	MaterialSpecular[2] = 0.774597; // b
	MaterialSpecular[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShininess = 0.6 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);

	glLoadIdentity();
	glTranslatef(-3.0f, 1.0f, fBaseDepth);
	gluSphere(quadric, 1.0f, 30, 30);

	//Copper
	MaterialAmbient[0] = 0.19125; // r
	MaterialAmbient[1] = 0.0735;  // g
	MaterialAmbient[2] = 0.0225;  // b
	MaterialAmbient[3] = 1.0f;    // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffused[0] = 0.7038;  // r
	MaterialDiffused[1] = 0.27048; // g
	MaterialDiffused[2] = 0.0828;  // b
	MaterialDiffused[3] = 1.0f;    // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffused);

	MaterialSpecular[0] = 0.256777; // r
	MaterialSpecular[1] = 0.137622; // g
	MaterialSpecular[2] = 0.086014; // b
	MaterialSpecular[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShininess = 0.1 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);

	glLoadIdentity();
	glTranslatef(-3.0f, -1.5f, fBaseDepth);
	gluSphere(quadric, 1.0f, 30, 30);

	//gold
	MaterialAmbient[0] = 0.24725; // r
	MaterialAmbient[1] = 0.1995;  // g
	MaterialAmbient[2] = 0.0745;  // b
	MaterialAmbient[3] = 1.0f;    // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffused[0] = 0.75164; // r
	MaterialDiffused[1] = 0.60648; // g
	MaterialDiffused[2] = 0.22648; // b
	MaterialDiffused[3] = 1.0f;    // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffused);

	MaterialSpecular[0] = 0.628281; // r
	MaterialSpecular[1] = 0.555802; // g
	MaterialSpecular[2] = 0.366065; // b
	MaterialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShininess = 0.4 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);

	glLoadIdentity();
	glTranslatef(-3.0f, -4.0f, fBaseDepth);
	gluSphere(quadric, 1.0f, 30, 30);

	//silver
	MaterialAmbient[0] = 0.19225; // r
	MaterialAmbient[1] = 0.19225; // g
	MaterialAmbient[2] = 0.19225; // b
	MaterialAmbient[3] = 1.0f;    // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffused[0] = 0.50754; // r
	MaterialDiffused[1] = 0.50754; // g
	MaterialDiffused[2] = 0.50754; // b
	MaterialDiffused[3] = 1.0f;    // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffused);

	MaterialSpecular[0] = 0.508273; // r
	MaterialSpecular[1] = 0.508273; // g
	MaterialSpecular[2] = 0.508273; // b
	MaterialSpecular[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShininess = 0.4 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);

	glLoadIdentity();
	glTranslatef(-3.0f, -6.5f, fBaseDepth);
	gluSphere(quadric, 1.0f, 30, 30);

	//Third Column
	//Black
	MaterialAmbient[0] = 0.0;  // r
	MaterialAmbient[1] = 0.0;  // g
	MaterialAmbient[2] = 0.0;  // b
	MaterialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffused[0] = 0.01; // r
	MaterialDiffused[1] = 0.01; // g
	MaterialDiffused[2] = 0.01; // b
	MaterialDiffused[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffused);

	MaterialSpecular[0] = 0.50; // r
	MaterialSpecular[1] = 0.50; // g
	MaterialSpecular[2] = 0.50; // b
	MaterialSpecular[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShininess = 0.25 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);

	glLoadIdentity();
	glTranslatef(3.0f, 6.0f, fBaseDepth);
	gluSphere(quadric, 1.0f, 30, 30);

	//cyan
	MaterialAmbient[0] = 0.0;  // r
	MaterialAmbient[1] = 0.1;  // g
	MaterialAmbient[2] = 0.06; // b
	MaterialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffused[0] = 0.0;        // r
	MaterialDiffused[1] = 0.50980392; // g
	MaterialDiffused[2] = 0.50980392; // b
	MaterialDiffused[3] = 1.0f;       // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffused);

	MaterialSpecular[0] = 0.50196078; // r
	MaterialSpecular[1] = 0.50196078; // g
	MaterialSpecular[2] = 0.50196078; // b
	MaterialSpecular[3] = 1.0f;       // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShininess = 0.25 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);

	glLoadIdentity();
	glTranslatef(3.0f, 3.5f, fBaseDepth);
	gluSphere(quadric, 1.0f, 30, 30);

	//green
	MaterialAmbient[0] = 0.0;  // r
	MaterialAmbient[1] = 0.0;  // g
	MaterialAmbient[2] = 0.0;  // b
	MaterialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffused[0] = 0.1;  // r
	MaterialDiffused[1] = 0.35; // g
	MaterialDiffused[2] = 0.1;  // b
	MaterialDiffused[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffused);

	MaterialSpecular[0] = 0.45; // r
	MaterialSpecular[1] = 0.55; // g
	MaterialSpecular[2] = 0.45; // b
	MaterialSpecular[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShininess = 0.25 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);

	glLoadIdentity();
	glTranslatef(3.0f, 1.0f, fBaseDepth);
	gluSphere(quadric, 1.0f, 30, 30);

	//red
	MaterialAmbient[0] = 0.0;  // r
	MaterialAmbient[1] = 0.0;  // g
	MaterialAmbient[2] = 0.0;  // b
	MaterialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffused[0] = 0.5;  // r
	MaterialDiffused[1] = 0.0;  // g
	MaterialDiffused[2] = 0.0;  // b
	MaterialDiffused[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffused);

	MaterialSpecular[0] = 0.7;  // r
	MaterialSpecular[1] = 0.6;  // g
	MaterialSpecular[2] = 0.6;  // b
	MaterialSpecular[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShininess = 0.25 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);

	glLoadIdentity();
	glTranslatef(3.0f, -1.5f, fBaseDepth);
	gluSphere(quadric, 1.0f, 30, 30);

	//white
	MaterialAmbient[0] = 0.0;  // r
	MaterialAmbient[1] = 0.0;  // g
	MaterialAmbient[2] = 0.0;  // b
	MaterialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffused[0] = 0.55; // r
	MaterialDiffused[1] = 0.55; // g
	MaterialDiffused[2] = 0.55; // b
	MaterialDiffused[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffused);

	MaterialSpecular[0] = 0.70; // r
	MaterialSpecular[1] = 0.70; // g
	MaterialSpecular[2] = 0.70; // b
	MaterialSpecular[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShininess = 0.25 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);

	glLoadIdentity();
	glTranslatef(3.0f, -4.0f, fBaseDepth);
	gluSphere(quadric, 1.0f, 30, 30);

	//Yellow
	MaterialAmbient[0] = 0.0;  // r
	MaterialAmbient[1] = 0.0;  // g
	MaterialAmbient[2] = 0.0;  // b
	MaterialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffused[0] = 0.5; // r
	MaterialDiffused[1] = 0.5; // g
	MaterialDiffused[2] = 0.0; // b
	MaterialDiffused[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffused);

	MaterialSpecular[0] = 0.60; // r
	MaterialSpecular[1] = 0.60; // g
	MaterialSpecular[2] = 0.50; // b
	MaterialSpecular[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShininess = 0.25 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);

	glLoadIdentity();
	glTranslatef(3.0f, -6.5f, fBaseDepth);
	gluSphere(quadric, 1.0f, 30, 30);

	//Fourth Column - Rubber
	//Black
	MaterialAmbient[0] = 0.02; // r
	MaterialAmbient[1] = 0.02; // g
	MaterialAmbient[2] = 0.02; // b
	MaterialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffused[0] = 0.01; // r
	MaterialDiffused[1] = 0.01; // g
	MaterialDiffused[2] = 0.01; // b
	MaterialDiffused[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffused);

	MaterialSpecular[0] = 0.4;  // r
	MaterialSpecular[1] = 0.4;  // g
	MaterialSpecular[2] = 0.4;  // b
	MaterialSpecular[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShininess = 0.25 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);

	glLoadIdentity();
	glTranslatef(9.0f, 6.0f, fBaseDepth);
	gluSphere(quadric, 1.0f, 30, 30);

	//Cyan
	MaterialAmbient[0] = 0.0;  // r
	MaterialAmbient[1] = 0.05; // g
	MaterialAmbient[2] = 0.05; // b
	MaterialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffused[0] = 0.4;  // r
	MaterialDiffused[1] = 0.5;  // g
	MaterialDiffused[2] = 0.5;  // b
	MaterialDiffused[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffused);

	MaterialSpecular[0] = 0.04; // r
	MaterialSpecular[1] = 0.7;  // g
	MaterialSpecular[2] = 0.7;  // b
	MaterialSpecular[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShininess = 0.078125 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);

	glLoadIdentity();
	glTranslatef(9.0f, 3.5f, fBaseDepth);
	gluSphere(quadric, 1.0f, 30, 30);

	//Green
	MaterialAmbient[0] = 0.0;  // r
	MaterialAmbient[1] = 0.05; // g
	MaterialAmbient[2] = 0.0;  // b
	MaterialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffused[0] = 0.4;  // r
	MaterialDiffused[1] = 0.5;  // g
	MaterialDiffused[2] = 0.4;  // b
	MaterialDiffused[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffused);

	MaterialSpecular[0] = 0.04; // r
	MaterialSpecular[1] = 0.7;  // g
	MaterialSpecular[2] = 0.04; // b
	MaterialSpecular[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShininess = 0.078125 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);

	glLoadIdentity();
	glTranslatef(9.0f, 1.0f, fBaseDepth);
	gluSphere(quadric, 1.0f, 30, 30);

	//Red
	MaterialAmbient[0] = 0.05; // r
	MaterialAmbient[1] = 0.0;  // g
	MaterialAmbient[2] = 0.0;  // b
	MaterialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffused[0] = 0.5;  // r
	MaterialDiffused[1] = 0.4;  // g
	MaterialDiffused[2] = 0.4;  // b
	MaterialDiffused[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffused);

	MaterialSpecular[0] = 0.7;  // r
	MaterialSpecular[1] = 0.04; // g
	MaterialSpecular[2] = 0.04; // b
	MaterialSpecular[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShininess = 0.078125 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);

	glLoadIdentity();
	glTranslatef(9.0f, -1.5f, fBaseDepth);
	gluSphere(quadric, 1.0f, 30, 30);

	//White
	MaterialAmbient[0] = 0.05; // r
	MaterialAmbient[1] = 0.05; // g
	MaterialAmbient[2] = 0.05; // b
	MaterialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffused[0] = 0.5;  // r
	MaterialDiffused[1] = 0.5;  // g
	MaterialDiffused[2] = 0.5;  // b
	MaterialDiffused[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffused);

	MaterialSpecular[0] = 0.7;  // r
	MaterialSpecular[1] = 0.7;  // g
	MaterialSpecular[2] = 0.7;  // b
	MaterialSpecular[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShininess = 0.078125 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);

	glLoadIdentity();
	glTranslatef(9.0f, -4.0f, fBaseDepth);
	gluSphere(quadric, 1.0f, 30, 30);

	//Yellow
	MaterialAmbient[0] = 0.05; // r
	MaterialAmbient[1] = 0.05; // g
	MaterialAmbient[2] = 0.0;  // b
	MaterialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffused[0] = 0.5;  // r
	MaterialDiffused[1] = 0.5;  // g
	MaterialDiffused[2] = 0.4;  // b
	MaterialDiffused[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffused);

	MaterialSpecular[0] = 0.7;  // r
	MaterialSpecular[1] = 0.7;  // g
	MaterialSpecular[2] = 0.04; // b
	MaterialSpecular[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShininess = 0.078125 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);

	glLoadIdentity();
	glTranslatef(9.0f, -6.5f, fBaseDepth);
	gluSphere(quadric, 1.0f, 30, 30);

	glXSwapBuffers(display, window);
}

void update(void)
{
    // Code
	if (KeyPressed == 1)
	{
		fAngleForXRotation = fAngleForXRotation + 1.0f;
		if (fAngleForXRotation >= 360.0f)
			fAngleForXRotation = fAngleForXRotation - 360.0f;
	}
	if (KeyPressed == 2)
	{
		fAngleForYRotation = fAngleForYRotation + 1.0f;
		if (fAngleForYRotation >= 360.0f)
			fAngleForYRotation = fAngleForYRotation - 360.0f;
	}
	if (KeyPressed == 3)
	{
		fAngleForZRotation = fAngleForZRotation + 1.0f;
		if (fAngleForZRotation >= 360.0f)
			fAngleForZRotation = fAngleForZRotation - 360.0f;
	}
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
