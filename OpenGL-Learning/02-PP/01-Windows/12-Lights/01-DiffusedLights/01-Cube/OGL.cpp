//Windows Header Files
#include <windows.h>
#include <stdio.h> //For File IO
#include <stdlib.h> //For exit()
#include <wingdi.h>

// OPENGL Header Files
#include<gl/GLEW.h> //This must be before GL.h
#include "vmath.h"
using namespace vmath;

#include "OGL.h"

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

//Link with OpenGL library
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib")

//Global Function Declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//Global Variables
FILE* gpFile = NULL;

HWND ghwnd = NULL;
BOOL gbActive = FALSE;

DWORD dwstyle = 0;
WINDOWPLACEMENT wpPrev = {sizeof(WINDOWPLACEMENT)}; //All elements are set to value in  '{}' //memset() & ZeroMemory()
BOOL gbFullScreen = FALSE;

// OPENGL Related Global Variables
HDC ghdc = NULL;
HGLRC ghrc = NULL; // Handle Graphic Rendering Context
GLuint shaderProgramObject = 0;
enum {
	AGS_ATTRIBUTE_POSITION = 0,
	AGS_ATTRIBUTE_COLOR,
	AGS_ATTRIBUTE_TEXCORD,
	AGS_ATTRIBUTE_NORMAL
};

GLuint vao_cube = 0;
GLuint vbo_position_cube = 0;
GLuint vbo_normal_cube = 0;
GLfloat cAngle = 0.0f;

GLuint modelViewMatrixUniform = 0;
GLuint projectionMatrixUniform = 0;
GLuint ldUniform = 0;
GLuint kdUniform = 0;
GLuint lightPositionUniform = 0;
GLuint keyPressedUniform = 0;

BOOL bLightingEnabled = FALSE;
BOOL bAnimationEnabled = FALSE;

//GLfloat fLightAmbient[] = {0.5f, 0.5f, 0.5f, 1.0f}; // Grey
GLfloat fLightDiffused[] = {1.0f, 1.0f, 1.0f, 1.0f}; // White 
GLfloat fLightPosition[] = {0.0f, 0.0f, 2.0f, 1.0f};

GLfloat fMaterialDiffused[] = {0.5f, 0.5f, 0.5f, 1.0f}; // White 

mat4 perspectiveProjectionMatrix; //4 by 4 matrix,mat4 is in vmath.h

//Entry Point Function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//Function Declarations
	int initialize(void);
	void uninitialize(void);
	void display(void);
	void update(void);

	//Local Variable declarations
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("AGSWindow");
	int iResult = 0;
	BOOL bDone = FALSE;

	//Code
	if (fopen_s(&gpFile, "Log.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Log File Cannot be opened"), TEXT("Error"), MB_OK | MB_ICONERROR);
		exit(0);
	}
	fprintf(gpFile, "Program started succesfully.\n");
	fprintf(gpFile, "-------------------------------------------------------\n");
	//WNDCLASSEX Initialization
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; // CS_OWNDC: OS purge window memory when it owns DC, so taking ownership
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));

	//Rgister WNDCLASSEX
	RegisterClassEx(&wndclass);

	//Centering Position of Window wrt to screen
	int iScreen_Width = GetSystemMetrics(SM_CXSCREEN);
	int iScreen_Height = GetSystemMetrics(SM_CYSCREEN);	//GetDeviceCaps

	int iWindow_Xpos = (iScreen_Width / 2) - (WIN_WIDTH /2);
	int iWindow_Ypos = (iScreen_Height / 2) - (WIN_HEIGHT / 2);

	//Create Window
	hwnd = CreateWindowEx(WS_EX_APPWINDOW, //It will help to bring your window on top of the taskbar
						szAppName,
						TEXT("Ashish Shelke"),
						WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
						// WS_CLIPCHILDREN	: If Window has child windows, and child window clip/cover own window then exclude clipped by child window 
						// WS_CLIPSIBLINGS	: If Window has siblings then exclude clipped area by siblings  
						// WS_VISIBLE		: After these exludes own window should be visible
						iWindow_Xpos,	//X
						iWindow_Ypos,	//Y
						WIN_WIDTH,	//Width
						WIN_HEIGHT,	//Height
						NULL,
						NULL,
						hInstance,
						NULL						
						);
	ghwnd = hwnd;

	//Initialization
	iResult = initialize();
	if (iResult != 0)
	{
		MessageBox(hwnd, TEXT("initialize() Failed"), TEXT("Error"), MB_OK | MB_ICONERROR);
		DestroyWindow(hwnd);
	}


	//Show The Window
	ShowWindow(hwnd, iCmdShow);

	SetForegroundWindow(hwnd); // Bring window to foreground
	SetFocus(hwnd);


	// Game Loop
	while (bDone == FALSE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				bDone = TRUE;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			//Render
			if (gbActive == TRUE)
			{
				//Render
				display();
				
				//Update
				if(bAnimationEnabled == TRUE)
					update();
			}
			else
			{

			}
		}
	}
	//Uninitalization
	uninitialize();

	return ((int)msg.wParam);
}

//CALLBACK Function
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//Function Declarations
	void ToggleFullSCreen(void);
	void resize(int, int);

	//Local Variables
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rc;
	TCHAR str[] = TEXT("Game Loop");

	//Code
	switch (iMsg)
	{
	case WM_SETFOCUS:
		//fprintf(gpFile, "Program Window Active = TRUE.\n");
		gbActive = TRUE;
		break;
	
	case WM_KILLFOCUS:
		//fprintf(gpFile, "Program Window Active = FALSE.\n");
		gbActive = FALSE;
		break;

	case WM_SIZE:
		resize(LOWORD(lParam), HIWORD(lParam)); //Each message has its own lParam, wParam, width & height is given by lParam of WM_SIZE
		break;

	/*case WM_ERASEBKGND:
		return 0;*/	

	case WM_KEYDOWN:
		switch (LOWORD(wParam))
		{
		case VK_ESCAPE: //27
			fprintf(gpFile, "Escape Pressed ... Exiting !!!\n");
			DestroyWindow(hwnd);
			break;
		}
		break;

	case WM_CHAR:
		switch (LOWORD(wParam))
		{
		case 'F':
		case 'f':
			if (gbFullScreen == FALSE)
			{
				fprintf(gpFile, "Fullscreen = TRUE\n");
				ToggleFullSCreen();
				gbFullScreen = TRUE;
			}
			else
			{
				fprintf(gpFile, "Fullscreen = FALSE\n");
				ToggleFullSCreen();
				gbFullScreen = FALSE;
			}
			break;

		case 'L':
		case 'l':
			if (bLightingEnabled == FALSE)
			{
				bLightingEnabled = TRUE;
			}
			else
			{	
				bLightingEnabled = FALSE;
			}
			break;

		case 'A':
		case 'a':
			if (bAnimationEnabled == FALSE)
			{
				bAnimationEnabled = TRUE;
			}
			else
			{	
				bAnimationEnabled = FALSE;
			}
			break;
		}
		break;

	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		break;
	}
	return (DefWindowProc(hwnd, iMsg, wParam, lParam));
}
// cl.exe /c /EHsc  Window.c
// link.exe Window.obj User32.lib GDI32.lib /SUBSYSTEM:WINDOWS

void ToggleFullSCreen(void)
{
	// ********* Algorithm ************
	// We will do toggeling of fullscreen on Kay press for 'F'/''
	// 
	//Step 1:
	// if (NotFullScreen)

		//Step A:
		//  Get the Current Window Style
		//		If That Window style "contains" WS_OVERLAPPEDWINDOW then 
		//			a) GetCurrentWindowPlacement (Atchi window chi length, width, postion ghya)
		//			b) Get Monitor Width and Height, 
		//				for that first get Monitor Handle
		//			c) If step a && step b are succesful means true, then 
		//				i) "Remove" WS_OVERLAPPEDWINDOW from current window style
		//					WS_OVERLAPPEDWINDOW = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMZBOX | WS_MAXIMZBOX   Only WS_OVERLAPPED is needed
		//				ii) Set the new fulscreen window such that it will be overlapped, It will begin from left-top of the monitor. Its client area begins from left top of the monitor
		//					Its width will be width of monitor, its height will be height of monitor, It will be top of the Z-order( Depth of the window screens assuming depth exists though its false)
		//
		// Step B:
		//  Conventionally in fullscreen mode system cursor is hidden so hide the cursor
		//
	// Now the window is Full screen
  								
	//Step 2:
	// if (Window is already in full screen mode)
	//	
		//Step A:
		//	Set the window placement, width height to the previous of the fullscreen
		//Step B
		//  "Add" windows style to WS_OVERLAPPEDWINDOW 
		//Step C:
		//  Set window position such that it will be unaffected by parents Z-order, parents movement , parents repaint   
		//Step D:
		//  Start showing system cursor
	// Now window is normal just before the full screen

	// Local Variables Declaraions
	MONITORINFO  mi = {sizeof(MONITORINFO)};

	//Code
	if (gbFullScreen == FALSE)
	{
		dwstyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwstyle & WS_OVERLAPPEDWINDOW)
		{
			if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, (dwstyle & ~WS_OVERLAPPEDWINDOW));

				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, (mi.rcMonitor.right - mi.rcMonitor.left), (mi.rcMonitor.bottom - mi.rcMonitor.top), SWP_NOZORDER | SWP_FRAMECHANGED);
				//SWP_FRAMECHANGED SEmds message WM_NCCALSIZE (Re-Calculate new client area size)
			}

		}
		ShowCursor(FALSE);
	}
	else
	{
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowLong(ghwnd, GWL_STYLE, dwstyle | WS_OVERLAPPEDWINDOW);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}

}

int initialize(void)
{
	//Function Declarations
	void printGLInfo(void);
	void uninitialize(void);
	void resize(int, int);

	//Code
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex = 0;
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
	
	//Initialization of PIXELFORMATDESCRIPTOR
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cDepthBits = 32; //Change 1

	//Get HDC
	ghdc = GetDC(ghwnd);
	if (ghdc == NULL)
	{
		fprintf(gpFile, "GetDc() failed ...\n");
		return -1;
	}

	//Get Closet Pixel Format Index from OS
	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormatIndex == 0)
	{
		fprintf(gpFile, "ChoosePixelFormat() failed ...\n");
		return -2;
	}

	//Set Pixel Format
	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
	{
		fprintf(gpFile, "SetPixelFormat() failed ...\n");
		return -3;
	}

	//Bridging API - Windowing to OPENGL
	//WGL - Windows Graphics Library
	//Tell WGL Bridging library to give OPENGL compatible DC from current window DC (Device Context)

	//Create Open GL Rendrting context from DC
	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
	{
		fprintf(gpFile, "wglCreateContext() failed ...\n");
		return -4;
	}

	//Now ghdc will end its role & will give control to ghrc
	//Make Rendering Context Current
	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
	{
		fprintf(gpFile, "wglMakeCurrent() failed ...\n");
		return -5;
	}

	//Initialize GLEW 
	if(glewInit() != GLEW_OK)
	{
		fprintf(gpFile, "glewInit() failed ...\n");
		return -6;		
	}

	//printGLInfo();
	//PP related code

	// "\n" \
	//Vertex Shader
	const GLchar* vertexShaderSourceCode = 
				"#version 460 core" \
				"\n" \
				"in vec4 aPosition;\n" \
				"in vec3 aNormal;\n" \
				"uniform mat4 uModelViewMatrix;\n" \
				"uniform mat4 uProjectionMatrix;\n" \
				"uniform vec3 uLd;\n" \
				"uniform vec3 uKd;\n" \
				"uniform vec4 uLightPosition;\n" \
				"uniform int uKeyPressed;\n" \
				"out vec3 oDiffusedLight;\n" \
				"void main(void)\n" \
				"{\n" \
				"if(uKeyPressed == 1) \n" \
				"{\n" \
				"vec4 eyePosition = uModelViewMatrix * aPosition; \n" \
				"mat3 normalMatrix = mat3(transpose(inverse(uModelViewMatrix))); \n" \
				"vec3 n = normalize(normalMatrix * aNormal); \n" \
				"vec3 s = normalize(vec3(uLightPosition - eyePosition)); \n" \
				"oDiffusedLight = uLd * uKd * max(dot(s,n), 0.0f); \n" \
				"}\n" \
				"else \n" \
				"{\n" \
				"oDiffusedLight = vec3(0.0f, 0.0f, 0.0f); \n" \
				"}\n" \
				"gl_Position = uProjectionMatrix * uModelViewMatrix * aPosition;\n" \
				"}\n";

	//Create Vertex Shader Object
	GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderObject, 1, (const GLchar **)&vertexShaderSourceCode, NULL);
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
				fprintf(gpFile, "Vertex Shader Compilation Log : %s \n", szInfoLog);
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
		"in vec3 oDiffusedLight;\n" \
		"uniform int uKeyPressed;\n" \
		"out vec4 fragColor;\n" \
		"void main(void)\n" \
		"{\n" \
		"if(uKeyPressed == 1) \n" \
		"{\n" \
		"fragColor = vec4(oDiffusedLight, 1.0f);\n" \
		"}\n" \
		"else \n" \
		"{\n" \
		"fragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n" \
		"}\n" \
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
				fprintf(gpFile, "Fragment Shader Compilation Log : %s \n", szInfoLog);
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
	glBindAttribLocation(shaderProgramObject, AGS_ATTRIBUTE_NORMAL, "aNormal");
	
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
				fprintf(gpFile, "Shader Program linking Log : %s \n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
				
			}
		}
		uninitialize();
	}	

	// get shader uniform location
	modelViewMatrixUniform = glGetUniformLocation(shaderProgramObject, "uModelViewMatrix");
	projectionMatrixUniform = glGetUniformLocation(shaderProgramObject, "uProjectionMatrix");
	ldUniform = glGetUniformLocation(shaderProgramObject, "uLd");
	kdUniform = glGetUniformLocation(shaderProgramObject, "uKd");
	lightPositionUniform = glGetUniformLocation(shaderProgramObject, "uLightPosition");
	keyPressedUniform = glGetUniformLocation(shaderProgramObject, "uKeyPressed");

	const GLfloat cube_position[] =
	{
		// front
		1.0f,  1.0f,  1.0f, // top-right of front
		-1.0f,  1.0f,  1.0f, // top-left of front
		-1.0f, -1.0f,  1.0f, // bottom-left of front
		1.0f, -1.0f,  1.0f, // bottom-right of front

		// right
		1.0f,  1.0f, -1.0f, // top-right of right
		1.0f,  1.0f,  1.0f, // top-left of right
		1.0f, -1.0f,  1.0f, // bottom-left of right
		1.0f, -1.0f, -1.0f, // bottom-right of right

		// back
		1.0f,  1.0f, -1.0f, // top-right of back
		-1.0f,  1.0f, -1.0f, // top-left of back
		-1.0f, -1.0f, -1.0f, // bottom-left of back
		1.0f, -1.0f, -1.0f, // bottom-right of back

		// left
		-1.0f,  1.0f,  1.0f, // top-right of left
		-1.0f,  1.0f, -1.0f, // top-left of left
		-1.0f, -1.0f, -1.0f, // bottom-left of left
		-1.0f, -1.0f,  1.0f, // bottom-right of left

		// top
		1.0f,  1.0f, -1.0f, // top-right of top
		-1.0f,  1.0f, -1.0f, // top-left of top
		-1.0f,  1.0f,  1.0f, // bottom-left of top
		1.0f,  1.0f,  1.0f, // bottom-right of top

		// bottom
		1.0f, -1.0f,  1.0f, // top-right of bottom
		-1.0f, -1.0f,  1.0f, // top-left of bottom
		-1.0f, -1.0f, -1.0f, // bottom-left of bottom
		1.0f, -1.0f, -1.0f, // bottom-right of bottom

	};

	const GLfloat cube_normal[] =
	{
		// front surface
		0.0f,  0.0f,  1.0f, // top-right of front
		0.0f,  0.0f,  1.0f, // top-left of front
		0.0f,  0.0f,  1.0f, // bottom-left of front
		0.0f,  0.0f,  1.0f, // bottom-right of front

		// right surface
		1.0f,  0.0f,  0.0f, // top-right of right
		1.0f,  0.0f,  0.0f, // top-left of right
		1.0f,  0.0f,  0.0f, // bottom-left of right
		1.0f,  0.0f,  0.0f, // bottom-right of right

		// back surface
		0.0f,  0.0f, -1.0f, // top-right of back
		0.0f,  0.0f, -1.0f, // top-left of back
		0.0f,  0.0f, -1.0f, // bottom-left of back
		0.0f,  0.0f, -1.0f, // bottom-right of back

		// left surface
		-1.0f,  0.0f,  0.0f, // top-right of left
		-1.0f,  0.0f,  0.0f, // top-left of left
		-1.0f,  0.0f,  0.0f, // bottom-left of left
		-1.0f,  0.0f,  0.0f, // bottom-right of left

		// top surface
		0.0f,  1.0f,  0.0f, // top-right of top
		0.0f,  1.0f,  0.0f, // top-left of top
		0.0f,  1.0f,  0.0f, // bottom-left of top
		0.0f,  1.0f,  0.0f, // bottom-right of top

		// bottom surface
		0.0f, -1.0f,  0.0f, // top-right of bottom
		0.0f, -1.0f,  0.0f, // top-left of bottom
		0.0f, -1.0f,  0.0f, // bottom-left of bottom
		0.0f, -1.0f,  0.0f, // bottom-right of bottom
	};


	//For Square
	glGenVertexArrays(1, &vao_cube);
	glBindVertexArray(vao_cube);

	//VBO for position
	glGenBuffers(1, &vbo_position_cube);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_cube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_position), cube_position, GL_STATIC_DRAW);
	glVertexAttribPointer(AGS_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AGS_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//VBO for normal			
	glGenBuffers(1, &vbo_normal_cube);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_normal_cube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_normal), cube_normal, GL_STATIC_DRAW);
	glVertexAttribPointer(AGS_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AGS_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Unbind vao Square
	glBindVertexArray(0);

	//Enabling Depth
	glClearDepth(1.0f); // Compulsory - Default depth
	glEnable(GL_DEPTH_TEST); // Compulsory - Enable Depth Test
	glDepthFunc(GL_LEQUAL); // Compulsary - Depth is less than or equal to 

	//Set Clear Color Of Window to Blue -- Its just informing but not yet colored
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// initialize orthographicProjectionMatrix
	perspectiveProjectionMatrix = vmath::mat4::identity();
	resize(WIN_WIDTH, WIN_HEIGHT);  

	//Here OPEN GL Starts
	return 0;
}

void printGLInfo(void)
{
	fprintf(gpFile, "--------- OpenGL Details: ---------\n");
	//Variable Declarations
	GLint numExtension = 0;
	GLint i = 0;
	//Code
	fprintf(gpFile, "OpenGL Vendor : %s \n", glGetString(GL_VENDOR));
	fprintf(gpFile, "OpenGL Renderor : %s \n", glGetString(GL_RENDERER));
	fprintf(gpFile, "OpenGL Version : %s \n", glGetString(GL_VERSION));
	fprintf(gpFile, "OpenGL GLSL Version : %s \n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	//Liating of supported extensions
	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtension);
	fprintf(gpFile, "OpenGL Supported Extensons : \n");
	for(i = 0; i < numExtension; i++)
	{
		fprintf(gpFile, "	%d) %s \n", i+1, glGetStringi(GL_EXTENSIONS, i));
	}
	fprintf(gpFile, "-------------------------------------------------------\n");
}

void resize(int width, int height)
{
	//Code
	if (height <= 0)
		height = 1;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height); // Focussing on perticular area of window, here its entire window

	perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void display(void)
{
	//Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Now here screen gets colored as color mentioned in initialize()

	glUseProgram(shaderProgramObject);

	//Cube
	// Transformation
	mat4 modelViewMatrix = vmath::mat4::identity();
	mat4 translationMatrix = vmath::mat4::identity();

	mat4 scaleMatrix = vmath::mat4::identity();
	scaleMatrix = vmath::scale(0.75f, 0.75f, 0.75f);

	mat4 rotationMatrix_X = vmath::mat4::identity();
	mat4 rotationMatrix_Y = vmath::mat4::identity();
	mat4 rotationMatrix_Z = vmath::mat4::identity();

	translationMatrix = vmath::translate(0.0f, 0.0f, -6.0f);
	rotationMatrix_X = vmath::rotate(cAngle, 1.0f, 0.0f, 0.0f);
	rotationMatrix_Y = vmath::rotate(cAngle, 0.0f, 1.0f, 0.0f);
	rotationMatrix_Z = vmath::rotate(cAngle, 0.0f, 0.0f, 1.0f);
	modelViewMatrix = translationMatrix * scaleMatrix * rotationMatrix_X *  rotationMatrix_Y * rotationMatrix_Z;

	// push above mvp into vertex shader's mvpuniform
	glUniformMatrix4fv(modelViewMatrixUniform, 1, GL_FALSE, modelViewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	if(bLightingEnabled == TRUE)
	{
		glUniform1i(keyPressedUniform, 1);
		glUniform3fv(ldUniform, 1, fLightDiffused); // light diffused is 4 elements array but passing only 3 elements as shader has vec3
		glUniform3fv(kdUniform, 1, fMaterialDiffused); 
		glUniform4fv(lightPositionUniform, 1, fLightPosition);
	}
	else
	{
		glUniform1i(keyPressedUniform, 0);
	}

	glBindVertexArray(vao_cube);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

	glBindVertexArray(0);	
	glUseProgram(0);	

	SwapBuffers(ghdc);
}

void update(void)
{
	//Code
	//When Annimation
	//updtae rectangle
	cAngle = cAngle - 0.1f;
	if (cAngle <= 0.0f)
	{
		cAngle = cAngle + 360.0f;
	}
}

void uninitialize(void)
{
	//Function Declarations
	void ToggleFullSCreen(void);

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


	//Delete vbo of positions square


	if (vbo_normal_cube)
	{
		glDeleteBuffers(1, &vbo_normal_cube);
		vbo_normal_cube = 0;
	}

	if (vbo_position_cube)
	{
		glDeleteBuffers(1, &vbo_position_cube);
		vbo_position_cube = 0;
	}

	//Delete vao square
	if (vao_cube)
	{
		glDeleteVertexArrays(1, &vao_cube);
		vao_cube = 0;
	}


	//If Application existing in FullScreen
	if (gbFullScreen == TRUE)
	{
		ToggleFullSCreen();
		gbFullScreen = FALSE;
	}

	//Make the hdc as current dc
	if (wglGetCurrentContext() == ghrc)
	{
		wglMakeCurrent(NULL, NULL);
	}

	//Destroy rendering context
	if (ghrc)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
	}

	//Release hdc
	if (ghdc)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	//Destroy Window
	if (ghwnd)
	{
		DestroyWindow(ghwnd);
		ghwnd = NULL;
	}
	fprintf(gpFile, "-------------------------------------------------------\n");
	//Close the log file
	if (gpFile) {
		fprintf(gpFile, "Program finished successfully.\n");
		fclose(gpFile);
		gpFile = NULL;
	}
}
