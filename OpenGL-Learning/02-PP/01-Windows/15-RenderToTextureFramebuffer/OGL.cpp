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
#include "Sphere.h"

#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#define FBO_WIDTH 512
#define FBO_HEIGHT 512

//Link with OpenGL library
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "Sphere.lib")

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
GLuint shaderProgramObject_cube = 0;

enum {
	AGS_ATTRIBUTE_POSITION = 0,
	AGS_ATTRIBUTE_COLOR,
	AGS_ATTRIBUTE_TEXCORD,
	AGS_ATTRIBUTE_NORMAL
};

GLuint vao_cube = 0;
GLuint vbo_position_cube = 0;
GLuint vbo_texture_cube = 0;
GLfloat cAngle_cube = 0.0f;

GLint winWidth = 0;
GLint winHeight = 0;

GLuint mvpMatrixUniform_cube = 0;
mat4 perspectiveProjectionMatrix_cube; //4 by 4 matrix,mat4 is in vmath.h
GLuint textureSamplerUniform_cube = 0;

//Sphere related global variables
GLuint shaderProgramObject_PF_sphere = 0;
GLuint shaderProgramObject_PV_sphere = 0;

GLuint gVao_sphere = 0;
GLuint gVbo_sphere_position = 0;
GLuint gVbo_sphere_normal = 0;
GLuint gVbo_sphere_element = 0;
GLuint gVbo_sphere_texture = 0;

unsigned int gNumVertices_sphere = 0;
unsigned int gNumElements_sphere = 0;

float sphere_positions[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];

GLfloat angle_sphere = 0.0f;

GLuint modelMatrixUniform_sphere = 0;
GLuint viewMatrixUniform_sphere = 0;
GLuint projectionMatrixUniform_sphere = 0;

GLuint light_AmbientUniform_sphere[3];
GLuint light_DiffusedUniform_sphere[3];
GLuint light_SpecularUniform_sphere[3];
GLuint light_PositionUniform_sphere[3];

GLuint materialAmbientUniform_sphere = 0;
GLuint materialDiffusedUniform_sphere = 0;
GLuint materialSpecularUniform_sphere = 0;
GLuint materialShynessUniform_sphere = 0;

GLuint keyPressedUniform_sphere = 0;

BOOL blight_sphereingEnabled = FALSE;
BOOL bToggleShader_sphere = FALSE;
char choosenShader = 'V';

GLfloat flight_sphereAngleZero = 0.0f;
GLfloat flight_sphereAngleOne = 0.0f;
GLfloat flight_sphereAngleTwo = 0.0f;

// light_sphere 0
GLfloat light_sphereAmbientZero[] = {0.0f, 0.0f, 0.0f, 1.0f };
GLfloat light_sphereDiffusedZero[] = {0.0f, 1.0f, 0.0f, 1.0f};
GLfloat light_sphereSpecularZero[] = { 0.0f, 1.0f, 0.0f, 1.0f };
GLfloat light_spherePostionZero[] = {0.0f, 0.0f, 2.0f, 1.0f};

// light_sphere 1
GLfloat light_sphereAmbientOne[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat light_sphereDiffusedOne[] = { 1.0f, 0.0f, 0.0f, 1.0f };
GLfloat light_sphereSpecularOne[] = { 1.0f, 0.0f, 0.0f, 1.0f };
GLfloat light_spherePostionOne[] = { 2.0f, 0.0f, 0.0f, 1.0f };

// light_sphere 1
GLfloat light_sphereAmbientTwo[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat light_sphereDiffusedTwo[] = { 0.0f, 0.0f, 1.0f, 1.0f };
GLfloat light_sphereSpecularTwo[] = { 0.0f, 0.0f, 1.0f, 1.0f };
GLfloat light_spherePostionTwo[] = { 0.0f, 2.0f, 0.0f, 1.0f };

//Material Properties
GLfloat materialAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat materialDiffused[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat materialSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat materialShyniness = 128.0f;

mat4 perspectiveProjectionMatrix_sphere; //4 by 4 matrix,mat4 is in vmath.h

struct Light
{
	vec3 ambient;
	vec3 diffused;
	vec3 specular;
	vec4 position;
};

struct Light light_sphere[3];

//FBO related global variables
GLuint fbo = 0; // Frame bugger object
GLuint rbo = 0; // Render buffer objec
GLuint texture_fbo = 0;
BOOL fboResult = FALSE;

//Entry Point Function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//Function Declarations
	int initialize_cube(void);
	void uninitialize_cube(void);
	void display_cube(void);
	void update_cube(void);

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
	iResult = initialize_cube();
	if (iResult != 0)
	{
		MessageBox(hwnd, TEXT("initialize_cube() Failed"), TEXT("Error"), MB_OK | MB_ICONERROR);
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
				display_cube();
				
				//update_cube
				update_cube();
			}
			else
			{

			}
		}
	}
	//Uninitalization
	uninitialize_cube();

	return ((int)msg.wParam);
}

//CALLBACK Function
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//Function Declarations
	void ToggleFullSCreen(void);
	void resize_cube(int, int);

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
		resize_cube(LOWORD(lParam), HIWORD(lParam)); //Each message has its own lParam, wParam, width & height is given by lParam of WM_SIZE
		break;

	/*case WM_ERASEBKGND:
		return 0;*/	

	case WM_KEYDOWN:
		switch (LOWORD(wParam))
		{
		case VK_ESCAPE: //27
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
		
		default:
			break;
		}
		break;

	case WM_CHAR:
		switch (LOWORD(wParam))
		{
		case 'L':
		case 'l':
			if (blight_sphereingEnabled == FALSE)
			{
				blight_sphereingEnabled = TRUE;
				if(choosenShader != 'V')
				{
					bToggleShader_sphere = TRUE;
					choosenShader = 'V';
				}
			}
			else
			{	
				blight_sphereingEnabled = FALSE;
			}
			break;

		case 'F':
		case 'f':
				if(choosenShader != 'F')
				{
					bToggleShader_sphere = TRUE;
					choosenShader = 'F';
				}
			break;

		case 'V':
		case 'v':
				if(choosenShader != 'V')
				{
					bToggleShader_sphere = TRUE;
					choosenShader = 'V';
				}
			break;

		case 'Q':
		case 'q':
			fprintf(gpFile, "Escape Pressed ... Exiting !!!\n");
			DestroyWindow(hwnd);
			break;

		default:
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

int initialize_cube(void)
{
	//Function Declarations
	void printGLInfo(void);
	void uninitialize_cube(void);
	void resize_cube(int, int);

	BOOL initialize_sphere(GLint, GLint);

	//Code
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex = 0;
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
	BOOL bResult;

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

	//initialize_cube GLEW 
	if(glewInit() != GLEW_OK)
	{
		fprintf(gpFile, "glewInit() failed ...\n");
		return -6;		
	}

	printGLInfo();
	//PP related code

	//Vertex Shader
	const GLchar* vertexShaderSourceCode = 
				"#version 460 core" \
				"\n" \
				"in vec4 aPosition;\n" \
				"in vec2 aTexCoord;\n" \
				"out vec2 oTexCoord;\n" \
				"uniform mat4 umvpMatrix;\n" \
				"void main(void)\n" \
				"{\n" \
				"gl_Position = umvpMatrix*aPosition;\n" \
				"oTexCoord= aTexCoord;\n" \
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
		uninitialize_cube();
	}

	//Fragment Shader
	const GLchar* fragmentShaderSourceCode = 
		"#version 460 core" \
		"\n" \
		"in vec2 oTexCoord;\n" \
		"uniform sampler2D uTextureSampler;\n" \
		"out vec4 fragColor;\n" \
		"void main(void)\n" \
		"{\n" \
		"fragColor = texture(uTextureSampler, oTexCoord);\n" \
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
		uninitialize_cube();
	}

	//Shader Program
	shaderProgramObject_cube = glCreateProgram();
	glAttachShader(shaderProgramObject_cube, vertexShaderObject);
	glAttachShader(shaderProgramObject_cube, fragmentShaderObject);
	glBindAttribLocation(shaderProgramObject_cube, AGS_ATTRIBUTE_POSITION, "aPosition");
	glBindAttribLocation(shaderProgramObject_cube, AGS_ATTRIBUTE_TEXCORD, "aTexCoord");
	
	//Link the shader program
	glLinkProgram(shaderProgramObject_cube);

	status = 0;
	infoLogLength = 0;
	szInfoLog = NULL;
	
	glGetProgramiv(shaderProgramObject_cube, GL_LINK_STATUS, &status);
	
	if(status == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject_cube, GL_INFO_LOG_LENGTH, &infoLogLength);
		if(infoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(infoLogLength);

			if(szInfoLog != NULL)
			{
				glGetProgramInfoLog(shaderProgramObject_cube, infoLogLength, NULL, szInfoLog);
				fprintf(gpFile, "Shader Program linking Log : %s \n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
				
			}
		}
		uninitialize_cube();
	}	

	// get shader uniform location
	mvpMatrixUniform_cube = glGetUniformLocation(shaderProgramObject_cube, "umvpMatrix");
	textureSamplerUniform_cube = glGetUniformLocation(shaderProgramObject_cube, "uTextureSampler");

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

	const GLfloat cube_texcoord[] =
	{
		// front
		1.0f, 1.0f, // top-right of front
		0.0f, 1.0f, // top-left of front
		0.0f, 0.0f, // bottom-left of front
		1.0f, 0.0f, // bottom-right of front

		// right
		1.0f, 1.0f, // top-right of right
		0.0f, 1.0f, // top-left of right
		0.0f, 0.0f, // bottom-left of right
		1.0f, 0.0f, // bottom-right of right

		// back
		1.0f, 1.0f, // top-right of back
		0.0f, 1.0f, // top-left of back
		0.0f, 0.0f, // bottom-left of back
		1.0f, 0.0f, // bottom-right of back

		// left
		1.0f, 1.0f, // top-right of left
		0.0f, 1.0f, // top-left of left
		0.0f, 0.0f, // bottom-left of left
		1.0f, 0.0f, // bottom-right of left

		// top
		1.0f, 1.0f, // top-right of top
		0.0f, 1.0f, // top-left of top
		0.0f, 0.0f, // bottom-left of top
		1.0f, 0.0f, // bottom-right of top

		// bottom
		1.0f, 1.0f, // top-right of bottom
		0.0f, 1.0f, // top-left of bottom
		0.0f, 0.0f, // bottom-left of bottom
		1.0f, 0.0f, // bottom-right of bottom
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

	//VBO for texture		
	glGenBuffers(1, &vbo_texture_cube);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_texture_cube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_texcoord), cube_texcoord, GL_STATIC_DRAW);
	glVertexAttribPointer(AGS_ATTRIBUTE_TEXCORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AGS_ATTRIBUTE_TEXCORD);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Unbind vao Square
	glBindVertexArray(0);

	//Enabling Depth
	glClearDepth(1.0f); // Compulsory - Default depth
	glEnable(GL_DEPTH_TEST); // Compulsory - Enable Depth Test
	glDepthFunc(GL_LEQUAL); // Compulsary - Depth is less than or equal to 

	glEnable(GL_TEXTURE_2D);

	//Set Clear Color Of Window to Blue -- Its just informing but not yet colored
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	// initialize_cube orthographicProjectionMatrix
	perspectiveProjectionMatrix_cube = vmath::mat4::identity();
	resize_cube(WIN_WIDTH, WIN_HEIGHT);  

	//FBO related code
	BOOL createFBO(GLint, GLint);
	if(createFBO(FBO_WIDTH, FBO_HEIGHT) == TRUE)
	{
		fboResult = initialize_sphere(FBO_WIDTH, FBO_HEIGHT);
	}

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

BOOL initialize_sphere(GLint textureWidth, GLint textureHeight)
{
	//Function Declarations
	void uninitialize_sphere(void);
	void resize_sphere(int, int);
	BOOL createFBO(GLint, GLint);

	//Vertex Shader
	const GLchar* vertexShaderSourceCode_PF = 
					"#version 460 core" \
					"\n" \
					"in vec4 aPosition;\n" \
					"in vec3 aNormal;\n" \
					"uniform mat4 uModelMatrix;\n" \
					"uniform mat4 uViewMatrix;\n" \
					"uniform mat4 uProjectionMatrix;\n" \
					"uniform vec4 ulight_spherePosition[3];\n" \
					"uniform int uKeyPressed;\n" \
					"out vec3 oTransformedNormals;\n" \
					"out vec3 olight_sphereDirection[3];\n" \
					"out vec3 oViewerVector;\n" \
					"void main(void)\n" \
					"{\n" \
					"if(uKeyPressed == 1) \n" \
					"{\n" \
					"vec4 eyeCoordinates = uViewMatrix * uModelMatrix * aPosition; \n" \
					"oTransformedNormals = mat3(uViewMatrix * uModelMatrix) * aNormal; \n" \
					"for(int i = 0; i < 3; i++)\n" \
					"{\n" \
					"olight_sphereDirection[i] = vec3(ulight_spherePosition[i] - eyeCoordinates); \n" \
					"}\n" \
					"oViewerVector = -eyeCoordinates.xyz;\n" \
					"}\n" \
					"else \n" \
					"{\n" \
					"oTransformedNormals = vec3(0.0f, 0.0f, 0.0f); \n" \
					"olight_sphereDirection[0] = vec3(0.0f, 0.0f, 0.0f); \n" \
					"olight_sphereDirection[1] = vec3(0.0f, 0.0f, 0.0f); \n" \
					"oViewerVector = vec3(0.0f, 0.0f, 0.0f); \n" \
					"}\n" \
					"gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * aPosition;\n" \
					"}\n";

	const GLchar* vertexShaderSourceCode_PV = 
					"#version 460 core" \
					"\n" \
					"in vec4 aPosition;\n" \
					"in vec3 aNormal;\n" \
					"uniform mat4 uModelMatrix;\n" \
					"uniform mat4 uViewMatrix;\n" \
					"uniform mat4 uProjectionMatrix;\n" \
					"uniform vec3 ulight_sphereAmbient[3];\n" \
					"uniform vec3 ulight_sphereDiffused[3];\n" \
					"uniform vec3 ulight_sphereSpecular[3];\n" \
					"uniform vec4 ulight_spherePosition[3];\n" \
					"uniform vec3 uMaterialAmbient;\n" \
					"uniform vec3 uMaterialDiffused;\n" \
					"uniform vec3 uMaterialSpecular;\n" \
					"uniform float uMaterialShyniness;\n" \
					"uniform int uKeyPressed;\n" \
					"out vec3 oPhongADSlight_sphere;\n" \
					"void main(void)\n" \
					"{\n" \
					"oPhongADSlight_sphere = vec3(0.0f, 0.0f, 0.0f); \n" \
					"if(uKeyPressed == 1) \n" \
					"{\n" \
					"vec3 diffusedlight_sphere[3]; \n" \
					"vec3 ambientlight_sphere[3];\n" \
					"vec3 specularlight_sphere[3];\n" \
					"vec3 light_sphereDirection[3];\n" \
					"vec3 reflectionVector[3];\n" \
					"vec4 eyeCoordinates = uViewMatrix * uModelMatrix * aPosition; \n" \
					"vec3 transformedNormals = normalize(mat3(uViewMatrix * uModelMatrix) * aNormal); \n" \
					"vec3 viewerVector = normalize(-eyeCoordinates.xyz);\n" \
					"for(int i = 0; i < 3; i++)\n" \
					"{\n" \
					"light_sphereDirection[i] = normalize(vec3(ulight_spherePosition[i] - eyeCoordinates)); \n" \
					"reflectionVector[i] = reflect(-light_sphereDirection[i], transformedNormals);\n" \
					"diffusedlight_sphere[i] = ulight_sphereDiffused[i] * uMaterialDiffused * max(dot(light_sphereDirection[i],transformedNormals), 0.0f); \n" \
					"ambientlight_sphere[i] = ulight_sphereAmbient[i] * uMaterialAmbient;\n" \
					"specularlight_sphere[i] = ulight_sphereSpecular[i] * uMaterialSpecular * pow(max(dot(reflectionVector[i], viewerVector), 0.0f), uMaterialShyniness);\n" \
					"oPhongADSlight_sphere =  oPhongADSlight_sphere + ambientlight_sphere[i] + diffusedlight_sphere[i] + specularlight_sphere[i]; \n" \
					"}\n" \
					"}\n" \
					"else \n" \
					"{\n" \
					"oPhongADSlight_sphere = vec3(0.0f, 0.0f, 0.0f); \n" \
					"}\n" \
					"gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * aPosition;\n" \
					"}\n";

	//Create PF Vertex Shader Object
	GLuint vertexShaderObject_PF = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderObject_PF, 1, (const GLchar **)&vertexShaderSourceCode_PF, NULL);
	glCompileShader(vertexShaderObject_PF);
	
	GLint status = 0;
	GLint infoLogLength = 0;
	GLchar* szInfoLog = NULL;
	
	glGetShaderiv(vertexShaderObject_PF, GL_COMPILE_STATUS, &status);
	
	if(status == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObject_PF, GL_INFO_LOG_LENGTH, &infoLogLength);
		if(infoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(infoLogLength);

			if(szInfoLog != NULL)
			{
				glGetShaderInfoLog(vertexShaderObject_PF, infoLogLength, NULL, szInfoLog);
				fprintf(gpFile, "Vertex Shader Compilation Log : %s \n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
			}
		}
		uninitialize_sphere();
	}

	//Create PV Vertex Shader Object
	GLuint vertexShaderObject_PV = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderObject_PV, 1, (const GLchar **)&vertexShaderSourceCode_PV, NULL);
	glCompileShader(vertexShaderObject_PV);

	status = 0;
	infoLogLength = 0;
	szInfoLog = NULL;
	
	glGetShaderiv(vertexShaderObject_PV, GL_COMPILE_STATUS, &status);
	
	if(status == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObject_PV, GL_INFO_LOG_LENGTH, &infoLogLength);
		if(infoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(infoLogLength);

			if(szInfoLog != NULL)
			{
				glGetShaderInfoLog(vertexShaderObject_PV, infoLogLength, NULL, szInfoLog);
				fprintf(gpFile, "Vertex Shader Compilation Log : %s \n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
			}
		}
		uninitialize_sphere();
	}

	//Fragment Shader
	const GLchar* fragmentShaderSourceCode_PF = 
					"#version 460 core" \
					"\n" \
					"in vec3 oTransformedNormals;\n" \
					"in vec3 olight_sphereDirection[3];\n" \
					"in vec3 oViewerVector;\n" \
					"uniform vec3 ulight_sphereAmbient[3];\n" \
					"uniform vec3 ulight_sphereDiffused[3];\n" \
					"uniform vec3 ulight_sphereSpecular[3];\n" \
					"uniform vec4 ulight_spherePosition[3];\n" \
					"uniform vec3 uMaterialAmbient;\n" \
					"uniform vec3 uMaterialDiffused;\n" \
					"uniform vec3 uMaterialSpecular;\n" \
					"uniform float uMaterialShyniness;\n" \
					"uniform int uKeyPressed;\n" \
					"out vec4 FragColor;\n" \
					"void main(void)\n" \
					"{\n" \
					"vec3 phongADSlight_sphere = vec3(0.0f, 0.0f, 0.0f); \n" \
					"if(uKeyPressed == 1) \n" \
					"{\n" \
					"vec3 diffusedlight_sphere[3]; \n" \
					"vec3 ambientlight_sphere[3];\n" \
					"vec3 specularlight_sphere[3];\n" \
					"vec3 normalizedlight_sphereDirection[3];\n" \
					"vec3 reflectionVector[3];\n" \
					"vec3 normalizedTransformedNormals = normalize(oTransformedNormals); \n" \
					"vec3 normalViewerVector = normalize(oViewerVector);\n" \
					"for(int i = 0; i < 3; i++)\n" \
					"{\n" \
					"normalizedlight_sphereDirection[i] = normalize(olight_sphereDirection[i]); \n" \
					"reflectionVector[i] = reflect(-normalizedlight_sphereDirection[i], normalizedTransformedNormals);\n" \
					"diffusedlight_sphere[i] = ulight_sphereDiffused[i] * uMaterialDiffused * max(dot(normalizedlight_sphereDirection[i], normalizedTransformedNormals), 0.0f); \n" \
					"ambientlight_sphere[i] = ulight_sphereAmbient[i] * uMaterialAmbient;\n" \
					"specularlight_sphere[i] = ulight_sphereSpecular[i] * uMaterialSpecular * pow(max(dot(reflectionVector[i], normalViewerVector), 0.0f), uMaterialShyniness);\n" \
					"phongADSlight_sphere = phongADSlight_sphere + ambientlight_sphere[i] + diffusedlight_sphere[i] + specularlight_sphere[i]; \n" \
					"}\n" \
					"}\n" \
					"else \n" \
					"{\n" \
					"phongADSlight_sphere = vec3(1.0f, 1.0f, 1.0f); \n" \
					"}\n" \
					"FragColor = vec4(phongADSlight_sphere, 1.0f); \n" \
					"}\n";

	const GLchar* fragmentShaderSourceCode_PV = 
					"#version 460 core" \
					"\n" \
					"in vec3 oPhongADSlight_sphere;\n" \
					"uniform int uKeyPressed;\n" \
					"out vec4 FragColor;\n" \
					"void main(void)\n" \
					"{\n" \
					"if(uKeyPressed == 1) \n" \
					"{\n" \
					"FragColor = vec4(oPhongADSlight_sphere, 1.0f);\n" \
					"}\n" \
					"else \n" \
					"{\n" \
					"FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n" \
					"}\n" \
					"}\n";
	

	//Create PF Fragment Shader Object
	GLuint fragmentShaderObject_PF = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderObject_PF, 1, (const GLchar **)&fragmentShaderSourceCode_PF, NULL);
	glCompileShader(fragmentShaderObject_PF);
	
	status = 0;
	infoLogLength = 0;
	szInfoLog = NULL;
	
	glGetShaderiv(fragmentShaderObject_PF, GL_COMPILE_STATUS, &status);
	
	if(status == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObject_PF, GL_INFO_LOG_LENGTH, &infoLogLength);
		if(infoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(infoLogLength);

			if(szInfoLog != NULL)
			{
				glGetShaderInfoLog(fragmentShaderObject_PF, infoLogLength, NULL, szInfoLog);
				fprintf(gpFile, "Fragment Shader Compilation Log : %s \n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
				
			}
		}
		uninitialize_sphere();
	}

	//Create PV Fragment Shader Object
	GLuint fragmentShaderObject_PV = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderObject_PV, 1, (const GLchar **)&fragmentShaderSourceCode_PV, NULL);
	glCompileShader(fragmentShaderObject_PV);

	status = 0;
	infoLogLength = 0;
	szInfoLog = NULL;
	
	glGetShaderiv(fragmentShaderObject_PV, GL_COMPILE_STATUS, &status);
	
	if(status == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObject_PV, GL_INFO_LOG_LENGTH, &infoLogLength);
		if(infoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(infoLogLength);

			if(szInfoLog != NULL)
			{
				glGetShaderInfoLog(fragmentShaderObject_PV, infoLogLength, NULL, szInfoLog);
				fprintf(gpFile, "Fragment Shader Compilation Log : %s \n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
				
			}
		}
		uninitialize_sphere();
	}

	//Shader Program
	shaderProgramObject_PF_sphere = glCreateProgram();
	glAttachShader(shaderProgramObject_PF_sphere, vertexShaderObject_PF);
	glAttachShader(shaderProgramObject_PF_sphere, fragmentShaderObject_PF);
	glBindAttribLocation(shaderProgramObject_PF_sphere, AGS_ATTRIBUTE_POSITION, "aPosition");
	glBindAttribLocation(shaderProgramObject_PF_sphere, AGS_ATTRIBUTE_NORMAL, "aNormal");
	
	shaderProgramObject_PV_sphere = glCreateProgram();
	glAttachShader(shaderProgramObject_PV_sphere, vertexShaderObject_PV);
	glAttachShader(shaderProgramObject_PV_sphere, fragmentShaderObject_PV);
	glBindAttribLocation(shaderProgramObject_PV_sphere, AGS_ATTRIBUTE_POSITION, "aPosition");
	glBindAttribLocation(shaderProgramObject_PV_sphere, AGS_ATTRIBUTE_NORMAL, "aNormal");	

	//Link the PF shader program
	glLinkProgram(shaderProgramObject_PF_sphere);

	status = 0;
	infoLogLength = 0;
	szInfoLog = NULL;
	
	glGetProgramiv(shaderProgramObject_PF_sphere, GL_LINK_STATUS, &status);

	if(status == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject_PF_sphere, GL_INFO_LOG_LENGTH, &infoLogLength);
		if(infoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(infoLogLength);

			if(szInfoLog != NULL)
			{
				glGetProgramInfoLog(shaderProgramObject_PF_sphere, infoLogLength, NULL, szInfoLog);
				fprintf(gpFile, "Shader Program linking Log : %s \n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
				
			}
		}
		uninitialize_sphere();
	}	

	//Link the PV shader program
	glLinkProgram(shaderProgramObject_PV_sphere);

	status = 0;
	infoLogLength = 0;
	szInfoLog = NULL;
	
	glGetProgramiv(shaderProgramObject_PV_sphere, GL_LINK_STATUS, &status);
	
	if(status == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject_PV_sphere, GL_INFO_LOG_LENGTH, &infoLogLength);
		if(infoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(infoLogLength);

			if(szInfoLog != NULL)
			{
				glGetProgramInfoLog(shaderProgramObject_PV_sphere, infoLogLength, NULL, szInfoLog);
				fprintf(gpFile, "Shader Program linking Log : %s \n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
				
			}
		}
		uninitialize_sphere();
	}

	// get shader uniform location
	modelMatrixUniform_sphere = glGetUniformLocation(shaderProgramObject_PV_sphere, "uModelMatrix");
	viewMatrixUniform_sphere = glGetUniformLocation(shaderProgramObject_PV_sphere, "uViewMatrix");
	projectionMatrixUniform_sphere = glGetUniformLocation(shaderProgramObject_PV_sphere, "uProjectionMatrix");

	light_AmbientUniform_sphere[0] = glGetUniformLocation(shaderProgramObject_PV_sphere, "ulight_sphereAmbient[0]");
	light_DiffusedUniform_sphere[0] = glGetUniformLocation(shaderProgramObject_PV_sphere, "ulight_sphereDiffused[0]");
	light_SpecularUniform_sphere[0] = glGetUniformLocation(shaderProgramObject_PV_sphere, "ulight_sphereSpecular[0]");
	light_PositionUniform_sphere[0] = glGetUniformLocation(shaderProgramObject_PV_sphere, "ulight_spherePosition[0]");

	light_AmbientUniform_sphere[1] = glGetUniformLocation(shaderProgramObject_PV_sphere, "ulight_sphereAmbient[1]");
	light_DiffusedUniform_sphere[1] = glGetUniformLocation(shaderProgramObject_PV_sphere, "ulight_sphereDiffused[1]");
	light_SpecularUniform_sphere[1] = glGetUniformLocation(shaderProgramObject_PV_sphere, "ulight_sphereSpecular[1]");
	light_PositionUniform_sphere[1] = glGetUniformLocation(shaderProgramObject_PV_sphere, "ulight_spherePosition[1]");

	light_AmbientUniform_sphere[2] = glGetUniformLocation(shaderProgramObject_PV_sphere, "ulight_sphereAmbient[2]");
	light_DiffusedUniform_sphere[2] = glGetUniformLocation(shaderProgramObject_PV_sphere, "ulight_sphereDiffused[2]");
	light_SpecularUniform_sphere[2] = glGetUniformLocation(shaderProgramObject_PV_sphere, "ulight_sphereSpecular[2]");
	light_PositionUniform_sphere[2] = glGetUniformLocation(shaderProgramObject_PV_sphere, "ulight_spherePosition[2]");

	materialAmbientUniform_sphere = glGetUniformLocation(shaderProgramObject_PV_sphere, "uMaterialAmbient");
	materialDiffusedUniform_sphere = glGetUniformLocation(shaderProgramObject_PV_sphere, "uMaterialDiffused");
	materialSpecularUniform_sphere = glGetUniformLocation(shaderProgramObject_PV_sphere, "uMaterialSpecular");
	materialShynessUniform_sphere = glGetUniformLocation(shaderProgramObject_PV_sphere, "uMaterialShyniness");

	keyPressedUniform_sphere = glGetUniformLocation(shaderProgramObject_PV_sphere, "uKeyPressed");

    getSphereVertexData(sphere_positions, sphere_normals, sphere_textures, sphere_elements); //Elements - Face indices
    gNumVertices_sphere = getNumberOfSphereVertices();
    gNumElements_sphere = getNumberOfSphereElements();

	// vao
    glGenVertexArrays(1, &gVao_sphere);
    glBindVertexArray(gVao_sphere);

    // position vbo
    glGenBuffers(1, &gVbo_sphere_position);
    glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_position);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_positions), sphere_positions, GL_STATIC_DRAW);
    glVertexAttribPointer(AGS_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(AGS_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // normal vbo
    glGenBuffers(1, &gVbo_sphere_normal);
    glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_normal);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);
    glVertexAttribPointer(AGS_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(AGS_ATTRIBUTE_NORMAL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

	//textures vbo
	glGenBuffers(1, &gVbo_sphere_texture);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_texture);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_textures), sphere_textures, GL_STATIC_DRAW);
	glVertexAttribPointer(AGS_ATTRIBUTE_TEXCORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AGS_ATTRIBUTE_TEXCORD);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

    // element vbo
	// Core Geometry drawing in PP 
	// glDrawArrays() -> it takes position arrays
	// glDrawElements -> As possible as dont use positions because lot of positions in model in position arrays are redundent
	// Model gives positions and indices -> here list of indices are such that redundant positions are eliminated
	// Shader dont take elements as input so no vertex attributes
    glGenBuffers(1, &gVbo_sphere_element);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//Unbind vao Square
	glBindVertexArray(0);

	//Enabling Depth
	glClearDepth(1.0f); // Compulsory - Default depth
	glEnable(GL_DEPTH_TEST); // Compulsory - Enable Depth Test
	glDepthFunc(GL_LEQUAL); // Compulsary - Depth is less than or equal to 

	//Set Clear Color Of Window to Blue -- Its just informing but not yet colored
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  	light_sphere[0].ambient = vec3(light_sphereAmbientZero[0], light_sphereAmbientZero[1], light_sphereAmbientZero[2]);
	light_sphere[0].diffused = vec3(light_sphereDiffusedZero[0], light_sphereDiffusedZero[1], light_sphereDiffusedZero[2]);
	light_sphere[0].specular = vec3(light_sphereSpecularZero[0], light_sphereSpecularZero[1], light_sphereSpecularZero[2]);
	//light_sphere[0].position = vec4(light_spherePostionZero[0], light_spherePostionZero[1], light_spherePostionZero[2], light_spherePostionZero[3]);

	light_sphere[1].ambient = vec3(light_sphereAmbientOne[0], light_sphereAmbientOne[1], light_sphereAmbientOne[2]);
	light_sphere[1].diffused = vec3(light_sphereDiffusedOne[0], light_sphereDiffusedOne[1], light_sphereDiffusedOne[2]);
	light_sphere[1].specular = vec3(light_sphereSpecularOne[0], light_sphereSpecularOne[1], light_sphereSpecularOne[2]);
	//light_sphere[1].position = vec4(light_spherePostionOne[0], light_spherePostionOne[1], light_spherePostionOne[2], light_spherePostionOne[3]);

	light_sphere[2].ambient = vec3(light_sphereAmbientTwo[0], light_sphereAmbientTwo[1], light_sphereAmbientTwo[2]);
	light_sphere[2].diffused = vec3(light_sphereDiffusedTwo[0], light_sphereDiffusedTwo[1], light_sphereDiffusedTwo[2]);
	light_sphere[2].specular = vec3(light_sphereSpecularTwo[0], light_sphereSpecularTwo[1], light_sphereSpecularTwo[2]);
	//light_sphere[1].position = vec4(light_spherePostionOne[0], light_spherePostionOne[1], light_spherePostionOne[2], light_spherePostionOne[3]);

	// initialize_sphere orthographicProjectionMatrix
	perspectiveProjectionMatrix_sphere = vmath::mat4::identity();
	resize_sphere(FBO_WIDTH, FBO_HEIGHT);  

	return TRUE;
}

BOOL createFBO(GLint textureWidth, GLint textureHeight)
{
	// Check capacity of RenderBuffer
	
	GLint maxRenderBufferSize;

	glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxRenderBufferSize);

	if (maxRenderBufferSize < textureWidth || maxRenderBufferSize < textureHeight)
	{
		fprintf(gpFile, "Texture Size Overflow \n");
	}

	// Create Custom FrameBuffer

	glGenFramebuffers(1, &fbo);
	
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Create texture for FBO in which we are going to render the sphere
	glGenTextures(1, &texture_fbo);

	glBindTexture(GL_TEXTURE_2D, texture_fbo);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, NULL);

	//Attach above texture to framebuffer at default color attachment 0
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_fbo, 0);

	glGenRenderbuffers(1, &rbo);

	glBindRenderbuffer(GL_RENDERBUFFER,rbo);

	// Set the storage of above renderBuffer of texture size for depth
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, textureWidth, textureHeight);

	// Attach above Depth related renderBuffer t FBO at dept attachment.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

	// Check the frambuffer status whether successful or not
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		fprintf(gpFile, "Framebuffer creation status is not complete\n");

		return(FALSE);
	}

	//Unbind frambuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return(TRUE);
}


void resize_cube(int width, int height)
{
	//Code
	if (height <= 0)
		height = 1;

	winWidth = width;
	winHeight = height;
	
	glViewport(0, 0, (GLsizei)width, (GLsizei)height); // Focussing on perticular area of window, here its entire window

	perspectiveProjectionMatrix_cube = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void resize_sphere(int width, int height)
{
	//Code
	if (height <= 0)
		height = 1;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height); // Focussing on perticular area of window, here its entire window

	perspectiveProjectionMatrix_sphere = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void display_cube(void)
{
	void display_sphere(GLint, GLint);
	void update_sphere(void);
	void resize_cube(int, int);

	// Render FBO texture
	if(fboResult == TRUE)
	{	
		display_sphere(FBO_WIDTH, FBO_HEIGHT);
		update_sphere();
	}

	//Call resize cube to compensate change by dsipaly sphere
	resize_cube(winWidth, winHeight);

	//reset background color to white
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	//Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Now here screen gets colored as color mentioned in initialize_cube()

	glUseProgram(shaderProgramObject_cube);

	//pyramid

	// Transformation
	mat4 modelViewMatrix = vmath::mat4::identity();
	mat4 translationMatrix = vmath::mat4::identity();
	mat4 rotationMatrix = vmath::mat4::identity();
	mat4 rotationMatrix_X = vmath::mat4::identity();
	mat4 rotationMatrix_Y = vmath::mat4::identity();
	mat4 rotationMatrix_Z = vmath::mat4::identity();
	mat4 scaleMatrix = vmath::mat4::identity();

	scaleMatrix = vmath::scale(0.75f, 0.75f, 0.75f);
	translationMatrix = vmath::translate(0.0f, 0.0f, -4.0f);
	rotationMatrix_X = vmath::rotate(cAngle_cube, 1.0f, 0.0f, 0.0f);
	rotationMatrix_Y = vmath::rotate(cAngle_cube, 0.0f, 1.0f, 0.0f);
	rotationMatrix_Z = vmath::rotate(cAngle_cube, 0.0f, 0.0f, 1.0f);
	modelViewMatrix = translationMatrix * scaleMatrix * rotationMatrix_X *  rotationMatrix_Y * rotationMatrix_Z;
	mat4 modelViewProjectionMatrix = perspectiveProjectionMatrix_cube * modelViewMatrix; // order of multiplication is important

	// push above mvp into vertex shader's mvpuniform
	glUniformMatrix4fv(mvpMatrixUniform_cube, 1, GL_FALSE, modelViewProjectionMatrix);

	//Call Texture
	glActiveTexture(GL_TEXTURE0); // Multiple textures can be given 0 is unit of texture
	glBindTexture(GL_TEXTURE_2D, texture_fbo);
	glUniform1i(textureSamplerUniform_cube, 0);

	glBindVertexArray(vao_cube);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glUseProgram(0);	

	SwapBuffers(ghdc);
}

void display_sphere(GLint textureWidth, GLint textureHeight)
{
	void resize_sphere(int, int);

	//Bind with FBO
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	//Call resize sphere
	resize_sphere(FBO_WIDTH, FBO_HEIGHT);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Now here screen gets colored as color mentioned in initialize_sphere()

	if(choosenShader == 'F')
		glUseProgram(shaderProgramObject_PF_sphere);
	else if(choosenShader == 'V')
		glUseProgram(shaderProgramObject_PV_sphere);
	else
		glUseProgram(shaderProgramObject_PV_sphere);

	//pyramid

	// Transformation
	mat4 modelMatrix = vmath::mat4::identity();
	mat4 ViewMatrix = vmath::mat4::identity();
	mat4 translationMatrix = vmath::mat4::identity();
	mat4 rotationMatrix = vmath::mat4::identity();
	translationMatrix = vmath::translate(0.0f, 0.0f, -3.0f);
	//rotationMatrix = vmath::rotate(angle_sphere, 0.0f, 0.0f, 0.0f);
	modelMatrix = translationMatrix * rotationMatrix;

	// push above mvp into vertex shader's mvpuniform
	glUniformMatrix4fv(modelMatrixUniform_sphere, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform_sphere, 1, GL_FALSE, ViewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform_sphere, 1, GL_FALSE, perspectiveProjectionMatrix_sphere);

	// update_sphere light_sphere positions
	mat4 light_sphereRotationMatrix = vmath::mat4::identity();
	light_sphereRotationMatrix = vmath::rotate(flight_sphereAngleZero, 1.0f, 0.0f, 0.0f);
	vec4 light_spherePostion = vec4(0.0f, 0.0f, flight_sphereAngleZero, 1.0f) * light_sphereRotationMatrix;
	light_sphere[0].position = light_spherePostion;

	light_sphereRotationMatrix = vmath::mat4::identity();
	light_sphereRotationMatrix = vmath::rotate(flight_sphereAngleOne, 0.0f, 1.0f, 0.0f);
	light_spherePostion = vec4(flight_sphereAngleOne, 0.0f, 0.0f, 1.0f) * light_sphereRotationMatrix;
	light_sphere[1].position = light_spherePostion;

	light_sphereRotationMatrix = vmath::mat4::identity();
	light_sphereRotationMatrix = vmath::rotate(flight_sphereAngleTwo, 0.0f, 0.0f, 1.0f);
	light_spherePostion = vec4(0.0f, flight_sphereAngleTwo, 0.0f, 1.0f) * light_sphereRotationMatrix;
	light_sphere[2].position = light_spherePostion;

	if(blight_sphereingEnabled == TRUE)
	{
		glUniform1i(keyPressedUniform_sphere, 1);
		
		glUniform3fv(light_AmbientUniform_sphere[0], 1, light_sphere[0].ambient); // light_sphere diffused is 4 elements array but passing only 3 elements as shader has vec3
		glUniform3fv(light_DiffusedUniform_sphere[0], 1, light_sphere[0].diffused);
		glUniform3fv(light_SpecularUniform_sphere[0], 1, light_sphere[0].specular);
		glUniform4fv(light_PositionUniform_sphere[0], 1, light_sphere[0].position);

		glUniform3fv(light_AmbientUniform_sphere[1], 1, light_sphere[1].ambient); // light_sphere diffused is 4 elements array but passing only 3 elements as shader has vec3
		glUniform3fv(light_DiffusedUniform_sphere[1], 1, light_sphere[1].diffused);
		glUniform3fv(light_SpecularUniform_sphere[1], 1, light_sphere[1].specular);
		glUniform4fv(light_PositionUniform_sphere[1], 1, light_sphere[1].position);

		glUniform3fv(light_AmbientUniform_sphere[2], 1, light_sphere[2].ambient); // light_sphere diffused is 4 elements array but passing only 3 elements as shader has vec3
		glUniform3fv(light_DiffusedUniform_sphere[2], 1, light_sphere[2].diffused);
		glUniform3fv(light_SpecularUniform_sphere[2], 1, light_sphere[2].specular);
		glUniform4fv(light_PositionUniform_sphere[2], 1, light_sphere[2].position);

		glUniform3fv(materialAmbientUniform_sphere, 1, materialAmbient); 
		glUniform3fv(materialDiffusedUniform_sphere, 1, materialDiffused); 
		glUniform3fv(materialSpecularUniform_sphere, 1, materialSpecular); 
		glUniform1f(materialShynessUniform_sphere, materialShyniness); 
	}
	else
	{
		glUniform1i(keyPressedUniform_sphere, 0);
	}

	// *** bind vao ***
    glBindVertexArray(gVao_sphere);

    // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements_sphere, GL_UNSIGNED_SHORT, 0);

    // *** unbind vao ***
    glBindVertexArray(0);
	
	glUseProgram(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void update_cube(void)
{
	//Code
	//When Annimation

	//updtae rectangle
	cAngle_cube = cAngle_cube - 0.2f;
	if (cAngle_cube <= 0.0f)
	{
		cAngle_cube = cAngle_cube + 360.0f;
	}
}

void uninitialize_cube(void)
{
	void uninitialize_sphere(void);

	//Function Declarations
	void ToggleFullSCreen(void);

	//If Application existing in FullScreen
	if (gbFullScreen == TRUE)
	{
		ToggleFullSCreen();
		gbFullScreen = FALSE;
	}

	uninitialize_sphere();

	if(texture_fbo)
	{
		glDeleteTextures(1, &texture_fbo);
		texture_fbo = 0;
	}

	if(rbo)
	{
		glDeleteRenderbuffers(1, &rbo);
		rbo = 0;
	}

	if(fbo)
	{
		glDeleteFramebuffers(1, &fbo);
		fbo = 0;
	}

	//Code
	if(shaderProgramObject_cube)
	{
		glUseProgram(shaderProgramObject_cube);
		
		GLint numShaders = 0;
		glGetProgramiv(shaderProgramObject_cube, GL_ATTACHED_SHADERS, &numShaders);

		if(numShaders > 0)
		{
			GLuint* pShaders = (GLuint*)malloc(numShaders * sizeof(GLuint));

			if(pShaders != NULL)
			{
				glGetAttachedShaders(shaderProgramObject_cube, numShaders, NULL, pShaders);

				for(GLint i = 0; i < numShaders; i++)
				{
					glDetachShader(shaderProgramObject_cube, pShaders[i]);
					glDeleteShader(pShaders[i]);
					pShaders[i] = 0;
				}
				free(pShaders);
				pShaders = NULL;
			}
		}
		glUseProgram(0);

		glDeleteProgram(shaderProgramObject_cube);
		shaderProgramObject_cube = 0;
	}

	//Delete vbo of positions triangle
	if (vbo_position_cube)
	{
		glDeleteBuffers(1, &vbo_position_cube);
		vbo_position_cube = 0;
	}

	if (vbo_texture_cube)
	{
		glDeleteBuffers(1, &vbo_texture_cube);
		vbo_texture_cube = 0;
	}

	//Delete vao square

	if (vao_cube)
	{
		glDeleteVertexArrays(1, &vao_cube);
		vao_cube = 0;
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

void update_sphere(void)
{
	//Code
	//When Annimation

	//Making light_sphere 0
	if(blight_sphereingEnabled)
	{
		flight_sphereAngleZero = flight_sphereAngleZero + 0.05f;
		if (flight_sphereAngleZero >= 360.0f)
		{
			flight_sphereAngleZero = 0.0f;
		}

		//Making light_sphere 1
		flight_sphereAngleOne = flight_sphereAngleOne + 0.05f;
		if (flight_sphereAngleOne >= 360.0f)
		{
			flight_sphereAngleOne = 0.0f;
		}

		//Making light_sphere 2
		flight_sphereAngleTwo = flight_sphereAngleTwo + 0.05f;
		if (flight_sphereAngleTwo >= 360.0f)
		{
			flight_sphereAngleTwo = 0.0f;
		}
	}

	if(bToggleShader_sphere)
	{
		bToggleShader_sphere = FALSE;
		if(choosenShader == 'V')
		{
			modelMatrixUniform_sphere = glGetUniformLocation(shaderProgramObject_PV_sphere, "uModelMatrix");
			viewMatrixUniform_sphere = glGetUniformLocation(shaderProgramObject_PV_sphere, "uViewMatrix");
			projectionMatrixUniform_sphere = glGetUniformLocation(shaderProgramObject_PV_sphere, "uProjectionMatrix");

			light_AmbientUniform_sphere[0] = glGetUniformLocation(shaderProgramObject_PV_sphere, "ulight_sphereAmbient[0]");
			light_DiffusedUniform_sphere[0] = glGetUniformLocation(shaderProgramObject_PV_sphere, "ulight_sphereDiffused[0]");
			light_SpecularUniform_sphere[0] = glGetUniformLocation(shaderProgramObject_PV_sphere, "ulight_sphereSpecular[0]");
			light_PositionUniform_sphere[0] = glGetUniformLocation(shaderProgramObject_PV_sphere, "ulight_spherePosition[0]");

			light_AmbientUniform_sphere[1] = glGetUniformLocation(shaderProgramObject_PV_sphere, "ulight_sphereAmbient[1]");
			light_DiffusedUniform_sphere[1] = glGetUniformLocation(shaderProgramObject_PV_sphere, "ulight_sphereDiffused[1]");
			light_SpecularUniform_sphere[1] = glGetUniformLocation(shaderProgramObject_PV_sphere, "ulight_sphereSpecular[1]");
			light_PositionUniform_sphere[1] = glGetUniformLocation(shaderProgramObject_PV_sphere, "ulight_spherePosition[1]");

			light_AmbientUniform_sphere[2] = glGetUniformLocation(shaderProgramObject_PV_sphere, "ulight_sphereAmbient[2]");
			light_DiffusedUniform_sphere[2] = glGetUniformLocation(shaderProgramObject_PV_sphere, "ulight_sphereDiffused[2]");
			light_SpecularUniform_sphere[2] = glGetUniformLocation(shaderProgramObject_PV_sphere, "ulight_sphereSpecular[2]");
			light_PositionUniform_sphere[2] = glGetUniformLocation(shaderProgramObject_PV_sphere, "ulight_spherePosition[2]");

			materialAmbientUniform_sphere = glGetUniformLocation(shaderProgramObject_PV_sphere, "uMaterialAmbient");
			materialDiffusedUniform_sphere = glGetUniformLocation(shaderProgramObject_PV_sphere, "uMaterialDiffused");
			materialSpecularUniform_sphere = glGetUniformLocation(shaderProgramObject_PV_sphere, "uMaterialSpecular");
			materialShynessUniform_sphere = glGetUniformLocation(shaderProgramObject_PV_sphere, "uMaterialShyniness");

			keyPressedUniform_sphere = glGetUniformLocation(shaderProgramObject_PV_sphere, "uKeyPressed");
		}
		// get PF shader uniform location
		else if(choosenShader == 'F')
		{
			modelMatrixUniform_sphere = glGetUniformLocation(shaderProgramObject_PF_sphere, "uModelMatrix");
			viewMatrixUniform_sphere = glGetUniformLocation(shaderProgramObject_PF_sphere, "uViewMatrix");
			projectionMatrixUniform_sphere = glGetUniformLocation(shaderProgramObject_PF_sphere, "uProjectionMatrix");

			light_AmbientUniform_sphere[0] = glGetUniformLocation(shaderProgramObject_PF_sphere, "ulight_sphereAmbient[0]");
			light_DiffusedUniform_sphere[0] = glGetUniformLocation(shaderProgramObject_PF_sphere, "ulight_sphereDiffused[0]");
			light_SpecularUniform_sphere[0] = glGetUniformLocation(shaderProgramObject_PF_sphere, "ulight_sphereSpecular[0]");
			light_PositionUniform_sphere[0] = glGetUniformLocation(shaderProgramObject_PF_sphere, "ulight_spherePosition[0]");

			light_AmbientUniform_sphere[1] = glGetUniformLocation(shaderProgramObject_PF_sphere, "ulight_sphereAmbient[1]");
			light_DiffusedUniform_sphere[1] = glGetUniformLocation(shaderProgramObject_PF_sphere, "ulight_sphereDiffused[1]");
			light_SpecularUniform_sphere[1] = glGetUniformLocation(shaderProgramObject_PF_sphere, "ulight_sphereSpecular[1]");
			light_PositionUniform_sphere[1] = glGetUniformLocation(shaderProgramObject_PF_sphere, "ulight_spherePosition[1]");

			light_AmbientUniform_sphere[2] = glGetUniformLocation(shaderProgramObject_PF_sphere, "ulight_sphereAmbient[2]");
			light_DiffusedUniform_sphere[2] = glGetUniformLocation(shaderProgramObject_PF_sphere, "ulight_sphereDiffused[2]");
			light_SpecularUniform_sphere[2] = glGetUniformLocation(shaderProgramObject_PF_sphere, "ulight_sphereSpecular[2]");
			light_PositionUniform_sphere[2] = glGetUniformLocation(shaderProgramObject_PF_sphere, "ulight_spherePosition[2]");

			materialAmbientUniform_sphere = glGetUniformLocation(shaderProgramObject_PF_sphere, "uMaterialAmbient");
			materialDiffusedUniform_sphere = glGetUniformLocation(shaderProgramObject_PF_sphere, "uMaterialDiffused");
			materialSpecularUniform_sphere = glGetUniformLocation(shaderProgramObject_PF_sphere, "uMaterialSpecular");
			materialShynessUniform_sphere = glGetUniformLocation(shaderProgramObject_PF_sphere, "uMaterialShyniness");

			keyPressedUniform_sphere = glGetUniformLocation(shaderProgramObject_PF_sphere, "uKeyPressed");
		}
	}

}

void uninitialize_sphere(void)
{

	//Code
	if(shaderProgramObject_PF_sphere)
	{
		glUseProgram(shaderProgramObject_PF_sphere);
		
		GLint numShaders = 0;
		glGetProgramiv(shaderProgramObject_PF_sphere, GL_ATTACHED_SHADERS, &numShaders);

		if(numShaders > 0)
		{
			GLuint* pShaders = (GLuint*)malloc(numShaders * sizeof(GLuint));

			if(pShaders != NULL)
			{
				glGetAttachedShaders(shaderProgramObject_PF_sphere, numShaders, NULL, pShaders);

				for(GLint i = 0; i < numShaders; i++)
				{
					glDetachShader(shaderProgramObject_PF_sphere, pShaders[i]);
					glDeleteShader(pShaders[i]);
					pShaders[i] = 0;
				}
				free(pShaders);
				pShaders = NULL;
			}
		}
		glUseProgram(0);

		glDeleteProgram(shaderProgramObject_PF_sphere);
		shaderProgramObject_PF_sphere = 0;
	}

	if(shaderProgramObject_PV_sphere)
	{
		glUseProgram(shaderProgramObject_PV_sphere);
		
		GLint numShaders = 0;
		glGetProgramiv(shaderProgramObject_PV_sphere, GL_ATTACHED_SHADERS, &numShaders);

		if(numShaders > 0)
		{
			GLuint* pShaders = (GLuint*)malloc(numShaders * sizeof(GLuint));

			if(pShaders != NULL)
			{
				glGetAttachedShaders(shaderProgramObject_PV_sphere, numShaders, NULL, pShaders);

				for(GLint i = 0; i < numShaders; i++)
				{
					glDetachShader(shaderProgramObject_PV_sphere, pShaders[i]);
					glDeleteShader(pShaders[i]);
					pShaders[i] = 0;
				}
				free(pShaders);
				pShaders = NULL;
			}
		}
		glUseProgram(0);

		glDeleteProgram(shaderProgramObject_PV_sphere);
		shaderProgramObject_PV_sphere = 0;
	}

	//Delete vbo of positions square
	if (gVbo_sphere_texture)
	{
		glDeleteBuffers(1, &gVbo_sphere_texture);
		gVbo_sphere_texture = 0;
	}

	if (gVbo_sphere_element)
	{
		glDeleteBuffers(1, &gVbo_sphere_element);
		gVbo_sphere_element = 0;
	}

	if (gVbo_sphere_normal)
	{
		glDeleteBuffers(1, &gVbo_sphere_normal);
		gVbo_sphere_normal = 0;
	}

	if (gVbo_sphere_position)
	{
		glDeleteBuffers(1, &gVbo_sphere_position);
		gVbo_sphere_position = 0;
	}

	//Delete vao square
	if (gVao_sphere)
	{
		glDeleteVertexArrays(1, &gVao_sphere);
		gVao_sphere = 0;
	}
}
