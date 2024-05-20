// Standard header file
#include <iostream>
#include <stdio.h>  // For printf
#include <stdlib.h> // For Exit
#include <memory.h> // for memset 

// X11 Headerfile
#include <X11/Xlib.h> //For all X11 API
#include <X11/Xutil.h> // For XVisualInfo and related API
#include <X11/XKBlib.h>
#include <X11/keysym.h>
//OpenGL Heasder files
#include <GL/glew.h> //must be before gl.h
#include <GL/gl.h>
#include <GL/glx.h>

#include "vmath.h"

//Macros
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

//Global
FILE* gpFile = NULL;
Display *display = NULL;
Colormap colormap; 
Window window;
XVisualInfo* defaultVisualInfo = NULL;
Bool bFullscreen = False;
Bool bActiveWindow = False;

//OpenGL global Variable
//Visual info must be obtained from frame buffer config GLXFBConfig
//GLXcontext must not be created from glxCreateContext
//Must be creatd from a) core profile attributes b) Visual from gram buffer config
//glxContext should be supporting 'direct rendering' also called as hardware rendering
typedef GLXContext (*glXCreateContextAttribsArbProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);
glXCreateContextAttribsArbProc glXCreateContextAttribsArb = NULL;
GLXFBConfig glxFBConfig;
GLXContext glxContext = NULL;

//PP related variables
GLuint shaderProgramObject = 0;
enum {
	AGS_ATTRIBUTE_POSITION = 0
};
GLuint vao = 0;
GLuint vbo_position = 0;

GLuint mvpMatrixUniform = 0;
vmath::mat4 perspectiveProjectionMatrix; //4 by 4 matrix,mat4 is in vmath.h

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

    //PP related variables
    GLXFBConfig* glxFBConfigs = NULL;
    GLXFBConfig bestGlxFBConfig;
    XVisualInfo* tempXVisualInfo = NULL;
    int numFBConfigs = 0;

    int bestFramebufferConfig = -1;
    int bestNumberOfSamples = -1;
    int bestWorstFrameBufferConfig = -1;
    int worstNumberofSamples= 999;
    int sampleBuffers, samples;
    int iLoop;

    //OpenGL Variables
    int frameBufferAttributes[] = {
                                    GLX_X_RENDERABLE, True,
                                    GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
                                    GLX_RENDER_TYPE, GLX_RGBA_BIT,
                                    GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
                                    GLX_RED_SIZE, 8,
                                    GLX_GREEN_SIZE, 8,
                                    GLX_BLUE_SIZE, 8,
                                    GLX_ALPHA_SIZE, 8,
                                    GLX_DEPTH_SIZE, 24,
                                    GLX_STENCIL_SIZE, 8,
                                    GLX_DOUBLEBUFFER, True,
                                    None
                                };

    //Code
    // Log file code
    gpFile = fopen("log.txt", "w");
    if(gpFile == NULL)
    {
        fprintf(gpFile, "Error: Unable to create log file.");
        exit(EXIT_FAILURE);
    }
    else
        fprintf(gpFile, "Log file created succesfully \n");
 
    
    //Step 1: Open the connection with Xserver and get the display interface
    display = XOpenDisplay(NULL); //Open connection and get display - Parameter NULL(Default) or display ID
    if(display == NULL)
    {
        fprintf(gpFile, "XOpenDisplay() Failed \n");
        uninitialize();
        exit(-1);
    }

    //Step 2: Get default screen from above display
    defaultScreen = XDefaultScreen(display);

    // Step 3: Get avaialbale FBConfig from system
    glxFBConfigs = glXChooseFBConfig(display, XDefaultScreen(display), frameBufferAttributes, &numFBConfigs);
    if(glxFBConfigs == NULL)
    {
        fprintf(gpFile,"Matching glxFBConfigs can not be found \n");
        uninitialize();
        exit(-1);
    }

    fprintf(gpFile,"%d Matching FBConfigs found \n", numFBConfigs);

    // Step : Find best matching FBConfig from above array
    for(iLoop = 0; iLoop < numFBConfigs; iLoop++)
    {
        tempXVisualInfo = glXGetVisualFromFBConfig(display, glxFBConfigs[iLoop]);
        if(tempXVisualInfo != NULL)
        {
            //Get sample Buffers
            glXGetFBConfigAttrib(display, glxFBConfigs[iLoop], GLX_SAMPLE_BUFFERS, &sampleBuffers);
            //Get samples
            glXGetFBConfigAttrib(display, glxFBConfigs[iLoop], GLX_SAMPLES, &samples);

            if(bestFramebufferConfig < 0 || sampleBuffers && samples > bestNumberOfSamples)
            {
                bestFramebufferConfig = iLoop;
                bestNumberOfSamples = samples;
            }

            if(bestWorstFrameBufferConfig < 0 || !sampleBuffers || samples < worstNumberofSamples)
            {
                bestWorstFrameBufferConfig = iLoop;
                worstNumberofSamples = samples;
            }
        }
        XFree(tempXVisualInfo);

    }

    // Accordingly get best glxFBConfig
    bestGlxFBConfig = glxFBConfigs[bestFramebufferConfig];
    //Asign this best FBConfig to global GLXFBConfig
    glxFBConfig = bestGlxFBConfig;

    //Free memory given to array
    XFree(glxFBConfigs);
    glxFBConfigs = NULL;

    //Get best VisualInfo from bestGLXFBconfig    
    defaultVisualInfo = glXGetVisualFromFBConfig(display, glxFBConfig);

    if(defaultVisualInfo == NULL)
    {
        fprintf(gpFile,"Error: Unable to choose visual.\n");
        uninitialize();
        exit(-1);
    }
    fprintf(gpFile,"Selected Best Visaul Info ID %lu \n", defaultVisualInfo->visualid);

    //Step 4: Set window atributes / properties
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
        fprintf(gpFile,"XCreateWindow failed \n");
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
    void printGLInfo(void);

    //Local function declarations
    void resize(int , int);    
    void uninitialize(void);

    //Varaiables
    int attribs_new[] = {
                        GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
                        GLX_CONTEXT_MINOR_VERSION_ARB, 6,
                        GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
                        None
                        };

    int attribs_old[] = {
                        GLX_CONTEXT_MAJOR_VERSION_ARB, 1,
                        GLX_CONTEXT_MINOR_VERSION_ARB, 0,
                        None
                        };
    

    // GEt Address of function in function pointer
    glXCreateContextAttribsArb = (glXCreateContextAttribsArbProc)glXGetProcAddress((GLubyte*)"glXCreateContextAttribsARB");
    if(glXCreateContextAttribsArb == NULL)
    {
        fprintf(gpFile,"CAnnot get required function address \n");
        uninitialize();
        exit(-1);
    }

    // Create PP compatible glxContext
    glxContext = glXCreateContextAttribsArb(display, glxFBConfig, 0, True, attribs_new);

    if(!glxContext)
    {
        fprintf(gpFile,"Core profile based glxContext can not be obtained. \n falling back to old context \n");
        //getting old context
        glxContext = glXCreateContextAttribsArb(display, glxFBConfig, 0, True, attribs_old); 

        if(!glxContext)
        {
            fprintf(gpFile,"Old glxContext can not be obtained. \n");
            uninitialize();
            exit(-1);
        }
        else
        {
            fprintf(gpFile,"Old glxContext found \n");
        }    
    }
    else
    {
        fprintf(gpFile,"Core profile glxContext obtained succesfully \n");
    }

    // Direct renderable
    if(!glXIsDirect(display,glxContext))
    {
        fprintf(gpFile,"Not supporting Hardware rendering \n");
    }
    else
    {
        fprintf(gpFile,"Supportive \n");
    }
    //Make this context as current context
    if(glXMakeCurrent(display, window, glxContext) == False)
    {
        fprintf(gpFile,"glXMakeCurrent() failed \n");
        return -2;
    }

    printGLInfo();

	//Vertex Shader
	const GLchar* vertexShaderSourceCode = 
				"#version 460 core" \
				"\n" \
				"in vec4 aPosition;\n" \
				"uniform mat4 umvpMatrix;\n" \
				"void main(void)\n" \
				"{\n" \
				"gl_Position = umvpMatrix*aPosition;\n" \
				"}\n";

 	//Create Vertex Shader Object
	GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderObject, 1, (const char**)&vertexShaderSourceCode, NULL);
	glCompileShader(vertexShaderObject);
    
	GLint status = 0;
	GLint infoLogLength = 0;
	GLchar* szInfoLog = NULL;
	
	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);

	if(status == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if(infoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(infoLogLength);

			if(szInfoLog != NULL)
			{
				glGetShaderInfoLog(vertexShaderObject, infoLogLength, NULL, szInfoLog);
				fprintf(gpFile,"Vertex Shader Compilation Log : %s \n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;

			}
		}
		uninitialize();
	}
 
	//Fragment Shader
	const GLchar* fragmentShaderSourceCode = 
		"#version 460 core" \
		"\n" \
		"out vec4 fragColor;\n" \
		"void main(void)\n" \
		"{\n" \
		"fragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n" \
		"}\n";

	//Create Fragment Shader Object
	GLuint fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderObject, 1, (const GLchar **)&fragmentShaderSourceCode, NULL);
	glCompileShader(fragmentShaderObject);
	
    status = 0;
	infoLogLength = 0;
	szInfoLog = NULL;
	
	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);
	
	if(status == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if(infoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(infoLogLength);

			if(szInfoLog != NULL)
			{
				glGetShaderInfoLog(fragmentShaderObject, infoLogLength, NULL, szInfoLog);
				fprintf(gpFile,"Fragment Shader Compilation Log : %s \n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
				
			}
		}
		uninitialize();
	}

	//Shader Program
	shaderProgramObject = glCreateProgram();
	glAttachShader(shaderProgramObject, vertexShaderObject);
	glAttachShader(shaderProgramObject, fragmentShaderObject);
	glBindAttribLocation(shaderProgramObject, AGS_ATTRIBUTE_POSITION, "aPosition");
	
	//Link the shader program
	glLinkProgram(shaderProgramObject);

	status = 0;
	infoLogLength = 0;
	szInfoLog = NULL;
	
	glGetProgramiv(shaderProgramObject, GL_LINK_STATUS, &status);
	
	if(status == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if(infoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(infoLogLength);

			if(szInfoLog != NULL)
			{
				glGetProgramInfoLog(shaderProgramObject, infoLogLength, NULL, szInfoLog);
				fprintf(gpFile,"Shader Program linking Log : %s \n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
				
			}
		}
		uninitialize();
	}	

	// get shader uniform location
	mvpMatrixUniform = glGetUniformLocation(shaderProgramObject, "umvpMatrix");

	const GLfloat rectangle_position[] = {
						1.0f, 1.0f, 0.0f,
						-1.0f, 1.0f, 0.0f,
						-1.0f, -1.0f, 0.0f,
						1.0f, -1.0f, 0.0f
						};

	//VAO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//VBO for postions		
	glGenBuffers(1, &vbo_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle_position), rectangle_position, GL_STATIC_DRAW);
	glVertexAttribPointer(AGS_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AGS_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Unbind with VAO
	glBindVertexArray(0);
    //Useal OpenGL Code
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

    //Warm Resize
    resize(WIN_WIDTH, WIN_HEIGHT);

    return 0;

}

void printGLInfo(void)
{
	fprintf(gpFile,"--------- OpenGL Details: ---------\n");
	//Variable Declarations
	GLint numExtension = 0;
	GLint i = 0;
	//Code
	fprintf(gpFile,"OpenGL Vendor : %s \n", glGetString(GL_VENDOR));
	fprintf(gpFile,"OpenGL Renderor : %s \n", glGetString(GL_RENDERER));
	fprintf(gpFile,"OpenGL Version : %s \n", glGetString(GL_VERSION));
	fprintf(gpFile,"OpenGL GLSL Version : %s \n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	//Liating of supported extensions
	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtension);
	fprintf(gpFile,"OpenGL Supported Extensons : \n");

    GLenum err = glewInit();
    if (err != GLEW_OK)
        exit(1); // or handle the error in a nicer way
    if (!GLEW_VERSION_2_1)  // check that the machine supports the 2.1 API.
        exit(1);

	for(i = 0; i < numExtension; i++)
	{
		//fprintf(gpFile, "	%d] %s \n", i+1,glGetStringi(GL_EXTENSIONS, i));  
	}
	fprintf(gpFile,"-------------------------------------------------------\n");
    fflush(gpFile);
}


void resize(int width, int height)
{
	//Code
	if (height <= 0)
		height = 1;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height); // Focussing on perticular area of window, here its entire window

	perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void draw(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Now here screen gets colored as color mentioned in initialize()

	glUseProgram(shaderProgramObject);

	// Transformation
	vmath::mat4 modelViewMatrix = vmath::translate(0.0f, 0.0f, -3.0f);
	vmath::mat4 modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix; // order of multiplication is very imp

	// push above mvp into vertex shader's mvpuniform
	glUniformMatrix4fv(mvpMatrixUniform, 1,GL_FALSE, modelViewProjectionMatrix);

	glBindVertexArray(vao);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glBindVertexArray(0);
	glUseProgram(0);


	glXSwapBuffers(display, window);
}

void update(void)
{
    //Code
}

void uninitialize(void)
{
    //local variables
    GLXContext currentGlxContext;

    //Code

	//Code
	if(shaderProgramObject)
	{
		glUseProgram(shaderProgramObject);
		
		GLint numShaders = 0;
		glGetProgramiv(shaderProgramObject, GL_ATTACHED_SHADERS, &numShaders);

		if(numShaders > 0)
		{
			GLuint* pShaders = (GLuint*)malloc(numShaders * sizeof(GLuint));

			if(pShaders != NULL)
			{
				glGetAttachedShaders(shaderProgramObject, numShaders, NULL, pShaders);

				for(GLint i = 0; i < numShaders; i++)
				{
					glDetachShader(shaderProgramObject, pShaders[i]);
					glDeleteShader(pShaders[i]);
					pShaders[i] = 0;
				}
				free(pShaders);
				pShaders = NULL;
			}
		}
		glUseProgram(0);

		glDeleteProgram(shaderProgramObject);
		shaderProgramObject = 0;
	}

	if(vbo_position)
	{
		glDeleteBuffers(1, &vbo_position);
		vbo_position = 0;
	}

	if(vao)
	{
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}

    //Uncurrent the context
    currentGlxContext = glXGetCurrentContext();
    if((currentGlxContext != NULL) && (currentGlxContext == glxContext))
    {
        glXMakeCurrent(display, 0, 0);
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

    fprintf(gpFile, "---------- OpenGL Debug Logs End ----------\n");
    fflush(gpFile);
    if(gpFile)
        fclose(gpFile);    
}
