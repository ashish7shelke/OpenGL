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
char choosenShader = 'F';

GLfloat fBaseDepth = -20.0f;

// OPENGL Related Global Variables
HDC ghdc = NULL;
HGLRC ghrc = NULL; // Handle Graphic Rendering Context
GLuint shaderProgramObject_PF = 0;
GLuint shaderProgramObject_PV = 0;

enum {
	AGS_ATTRIBUTE_POSITION = 0,
	AGS_ATTRIBUTE_COLOR,
	AGS_ATTRIBUTE_TEXCORD,
	AGS_ATTRIBUTE_NORMAL
};

GLuint gVao_sphere = 0;
GLuint gVbo_sphere_position = 0;
GLuint gVbo_sphere_normal = 0;
GLuint gVbo_sphere_element = 0;
GLuint gVbo_sphere_texture = 0;

unsigned int gNumVertices = 0;
unsigned int gNumElements = 0;

float sphere_positions[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];

GLuint modelMatrixUniform = 0;
GLuint viewMatrixUniform = 0;
GLuint projectionMatrixUniform = 0;

GLuint lightAmbientUniform = 0;
GLuint lightDiffusedUniform = 0;
GLuint lightSpecularUniform = 0;
GLuint lightPositionUniform = 0;

GLuint materialAmbientUniform = 0;
GLuint materialDiffusedUniform = 0;
GLuint materialSpecularUniform = 0;
GLuint materialShynessUniform = 0;

GLuint keyPressedUniform = 0;

BOOL bLightingEnabled = FALSE;

GLuint KeyPressed;
GLfloat fAngleForXRotation = 0.0f;
GLfloat fAngleForYRotation = 0.0f;
GLfloat fAngleForZRotation = 0.0f;

GLfloat fLightAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat fLightDiffused[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // White
GLfloat fLightSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat fLightPosition[] = { 0.0f, 0.0f, 0.0f, 1.0f };

//View Port
const int numberOfRows = 6;
const int numberOfColumns = 4;

int viewportWidth = 1;
int viewportHeight = 1;
int viewportInitialX = 0;
int viewportInitialY = 0;

//Material Properties
GLfloat fMaterialAmbient[numberOfColumns][numberOfRows][4] = {
    {   // Column 1
        {0.0215f, 0.1745f, 0.0215f, 1.0f },
        {0.135f, 0.2225f, 0.1575f, 1.0f },
        {0.05375f, 0.05f, 0.06625f, 1.0f },
        {0.25f, 0.20725f, 0.20725f, 1.0f },
        {0.1745f, 0.01175f, 0.01175f, 1.0f },
        {0.1f, 0.18725f, 0.1745f, 1.0f }
    },
    {   // Column 2
        {0.329412f, 0.223529f, 0.027451f, 1.0f },
        {0.2125f, 0.1275f, 0.054f, 1.0f },
        {0.25f, 0.25f, 0.25f, 1.0f },
        {0.19125f, 0.0735f, 0.0225f, 1.0f },
        {0.24725f, 0.1995f, 0.0745f, 1.0f },
        {0.19225f, 0.19225f, 0.19225f, 1.0f }
    },
    {   // Column 3
        {0.0f, 0.0f, 0.0f, 1.0f },
        {0.0f, 0.1f, 0.06f, 1.0f },
        {0.0f, 0.0f, 0.0f, 1.0f },
        {0.0f, 0.0f, 0.0f, 1.0f },
        {0.0f, 0.0f, 0.0f, 1.0f },
        {0.0f, 0.0f, 0.0f, 1.0f }
    },
    {   // Column 4
        {0.02f, 0.02f, 0.02f, 1.0f },
        {0.0f, 0.05f, 0.05f, 1.0f },
        {0.0f, 0.05f, 0.0f, 1.0f },
        {0.05f, 0.0f, 0.0f, 1.0f },
        {0.05f, 0.05f, 0.05f, 1.0f },
        {0.05f, 0.05f, 0.0f, 1.0f }
    }
};

GLfloat fMaterialDiffused[numberOfColumns][numberOfRows][4] = {
    {   // Column 1
        {0.07568f, 0.61424f, 0.07568f, 1.0f},
        {0.54f, 0.89f, 0.63f, 1.0f},
        {0.18275f, 0.17f, 0.22525f, 1.0f},
        {1.0f, 0.829f, 0.829f, 1.0f},
        {0.61424f, 0.04136f, 0.04136f, 1.0f},
        {0.396f, 0.74151f, 0.69102f, 1.0f},
    },
    {   // Column 2
        {0.780392f, 0.568627f, 0.113725f, 1.0f},
        {0.714f, 0.4284f, 0.18144f, 1.0f},
        {0.4f, 0.4f, 0.4f, 1.0f},
        {0.7038f, 0.27048f, 0.0828f, 1.0f},
        {0.75164f, 0.60648f, 0.22648f, 1.0f},
        {0.50754f, 0.50754f, 0.50754f, 1.0f},
    },
    {   // Column 3
        {0.01f, 0.01f, 0.01f, 1.0f},
        {0.0f, 0.50980392f, 0.50980392f, 1.0f},
        {0.1f, 0.35f, 0.1f, 1.0f},
        {0.5f, 0.0f, 0.0f, 1.0f},
        {0.55f, 0.55f, 0.55f, 1.0f},
        {0.5f, 0.5f, 0.0f, 1.0f},
    },
    {   // Column 4
        {0.01f, 0.01f, 0.01f, 1.0f},
        {0.4f, 0.5f, 0.5f, 1.0f},
        {0.4f, 0.5f, 0.4f, 1.0f},
        {0.5f, 0.4f, 0.4f, 1.0f},
        {0.5f, 0.5f, 0.5f, 1.0f},
        {0.5f, 0.5f, 0.4f, 1.0f},
    },
};

GLfloat fMaterialSpecular[numberOfColumns][numberOfRows][4] = {
    {   // Column 1
        {0.633f, 0.727811f, 0.633f, 1.0f},
        {0.316228f, 0.316228f, 0.316228f, 1.0f},
        {0.332741f, 0.328634f, 0.346435f, 1.0f},
        {0.296648f, 0.296648f, 0.296648f, 1.0f},
        {0.727811f, 0.626959f, 0.626959f, 1.0f},
        {0.297254f, 0.30829f, 0.306678f, 1.0f},
    },
    {   // Column 2
        {0.992157f, 0.941176f, 0.807843f, 1.0f},
        {0.393548f, 0.271906f, 0.166721f, 1.0f},
        {0.774597f, 0.774597f, 0.774597f, 1.0f},
        {0.256777f, 0.137622f, 0.086014f, 1.0f},
        {0.628281f, 0.555802f, 0.366065f, 1.0f},
        {0.508273f, 0.508273f, 0.508273f, 1.0f},
    },
    {   // Column 3
        {0.50f, 0.50f, 0.50f, 1.0f},
        {0.50196078f, 0.50196078f, 0.50196078f, 1.0f},
        {0.45f, 0.55f, 0.45f, 1.0f},
        {0.7f, 0.6f, 0.6f, 1.0f},
        {0.70f, 0.70f, 0.70f, 1.0f},
        {0.60f, 0.60f, 0.50f, 1.0f},
    },
    {   // Column 4
        {0.4f, 0.4f, 0.4f, 1.0f},
        {0.04f, 0.7f, 0.7f, 1.0f},
        {0.04f, 0.7f, 0.04f, 1.0f},
        {0.7f, 0.04f, 0.04f, 1.0f},
        {0.7f, 0.7f, 0.7f, 1.0f},
        {0.7f, 0.7f, 0.04f, 1.0f},
    }
};

GLfloat fMaterialShininess[numberOfColumns][numberOfRows] = {
    {   // Column 1
        0.6f * 128.0f,
        0.1f * 128.0f,
        0.3f * 128.0f,
        0.088f * 128.0f,
        0.6f * 128.0f,
        0.1f * 128.0f
    },
    {   // Column 2
        0.21794872f * 128.0f,
        0.2f * 128.0f,
        0.6f * 128.0f,
        0.1f * 128.0f,
        0.4f * 128.0f,
        0.4f * 128.0f
    },
    {   // Column 3
        0.25f * 128.0f,
        0.25f * 128.0f,
        0.25f * 128.0f,
        0.25f * 128.0f,
        0.25f * 128.0f,
        0.25f * 128.0f
    },
    {   // Column 4
        0.078125f * 128.0f,
        0.078125f * 128.0f,
        0.078125f * 128.0f,
        0.078125f * 128.0f,
        0.078125f * 128.0f,
        0.078125f * 128.0f
    }
};

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
			if (bLightingEnabled == FALSE)
			{
				bLightingEnabled = TRUE;
				choosenShader = 'V';
			}
			else
			{	
				bLightingEnabled = FALSE;
			}
			break;

		case 'F':
		case 'f':
			choosenShader = 'F';
			break;

		case 'V':
		case 'v':
			choosenShader = 'V';
			break;

		case 'Q':
		case 'q':
			fprintf(gpFile, "Escape Pressed ... Exiting !!!\n");
			DestroyWindow(hwnd);
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

	//Phong lighting model

	//Per Vertex & PerFragment lights we dont need normal matrix
	const GLchar* vertexShaderSourceCode_PF = 
					"#version 460 core" \
					"\n" \
					"in vec4 aPosition;\n" \
					"in vec3 aNormal;\n" \
					"uniform mat4 uModelMatrix;\n" \
					"uniform mat4 uViewMatrix;\n" \
					"uniform mat4 uProjectionMatrix;\n" \
					"uniform vec4 uLightPosition;\n" \
					"uniform int uKeyPressed;\n" \
					"out vec3 oTransformedNormals;\n" \
					"out vec3 oLightDirection;\n" \
					"out vec3 oViewerVector;\n" \
					"void main(void)\n" \
					"{\n" \
					"if(uKeyPressed == 1) \n" \
					"{\n" \
					"vec4 eyeCoordinates = uViewMatrix * uModelMatrix * aPosition; \n" \
					"oTransformedNormals = mat3(uViewMatrix * uModelMatrix) * aNormal; \n" \
					"oLightDirection = vec3(uLightPosition - eyeCoordinates); \n" \
					"oViewerVector = -eyeCoordinates.xyz;\n" \
					"}\n" \
					"else \n" \
					"{\n" \
					"oTransformedNormals = vec3(0.0f, 0.0f, 0.0f); \n" \
					"oLightDirection = vec3(0.0f, 0.0f, 0.0f); \n" \
					"oViewerVector = vec3(0.0f, 0.0f, 0.0f); \n" \
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
		uninitialize();
	}

	//Fragment Shader
	const GLchar* fragmentShaderSourceCode_PF = 
					"#version 460 core" \
					"\n" \
					"in vec3 oTransformedNormals;\n" \
					"in vec3 oLightDirection;\n" \
					"in vec3 oViewerVector;\n" \
					"uniform vec3 uLightAmbient;\n" \
					"uniform vec3 uLightDiffused;\n" \
					"uniform vec3 uLightSpecular;\n" \
					"uniform vec4 uLightPosition;\n" \
					"uniform vec3 uMaterialAmbient;\n" \
					"uniform vec3 uMaterialDiffused;\n" \
					"uniform vec3 uMaterialSpecular;\n" \
					"uniform float uMaterialShyniness;\n" \
					"uniform int uKeyPressed;\n" \
					"out vec4 FragColor;\n" \
					"void main(void)\n" \
					"{\n" \
					"vec3 phongADSLight; \n" \
					"if(uKeyPressed == 1) \n" \
					"{\n" \
					"vec3 normalizedTransformedNormals = normalize(oTransformedNormals); \n" \
					"vec3 normalizedLightDirection = normalize(oLightDirection); \n" \
					"vec3 normalizedViewerVector = normalize(oViewerVector); \n" \
					"vec3 reflectionVector = reflect(-normalizedLightDirection, normalizedTransformedNormals);\n" \
					"vec3 diffusedLight = uLightDiffused * uMaterialDiffused * max(dot(normalizedLightDirection, normalizedTransformedNormals), 0.0f); \n" \
					"vec3 ambientLight = uLightAmbient * uMaterialAmbient;\n" \
					"vec3 specularLight = uLightSpecular * uMaterialSpecular * pow(max(dot(reflectionVector, normalizedViewerVector), 0.0f), uMaterialShyniness);\n" \
					"phongADSLight = (ambientLight + diffusedLight + specularLight); \n" \
					"}\n" \
					"else \n" \
					"{\n" \
					"phongADSLight = vec3(1.0f, 1.0f, 1.0f); \n" \
					"}\n" \
					"FragColor = vec4(phongADSLight, 1.0f); \n" \
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
		uninitialize();
	}


	//Shader Program
	shaderProgramObject_PF = glCreateProgram();
	glAttachShader(shaderProgramObject_PF, vertexShaderObject_PF);
	glAttachShader(shaderProgramObject_PF, fragmentShaderObject_PF);
	glBindAttribLocation(shaderProgramObject_PF, AGS_ATTRIBUTE_POSITION, "aPosition");
	glBindAttribLocation(shaderProgramObject_PF, AGS_ATTRIBUTE_NORMAL, "aNormal");
	
	//Link the PF shader program
	glLinkProgram(shaderProgramObject_PF);

	status = 0;
	infoLogLength = 0;
	szInfoLog = NULL;
	
	glGetProgramiv(shaderProgramObject_PF, GL_LINK_STATUS, &status);
	
	if(status == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject_PF, GL_INFO_LOG_LENGTH, &infoLogLength);
		if(infoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(infoLogLength);

			if(szInfoLog != NULL)
			{
				glGetProgramInfoLog(shaderProgramObject_PF, infoLogLength, NULL, szInfoLog);
				fprintf(gpFile, "Shader Program linking Log : %s \n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
				
			}
		}
		uninitialize();
	}	

	// get shader uniform location
	modelMatrixUniform = glGetUniformLocation(shaderProgramObject_PF, "uModelMatrix");
	viewMatrixUniform = glGetUniformLocation(shaderProgramObject_PF, "uViewMatrix");
	projectionMatrixUniform = glGetUniformLocation(shaderProgramObject_PF, "uProjectionMatrix");

	lightAmbientUniform = glGetUniformLocation(shaderProgramObject_PF, "uLightAmbient");
	lightDiffusedUniform = glGetUniformLocation(shaderProgramObject_PF, "uLightDiffused");
	lightSpecularUniform = glGetUniformLocation(shaderProgramObject_PF, "uLightSpecular");
	lightPositionUniform = glGetUniformLocation(shaderProgramObject_PF, "uLightPosition");

	materialAmbientUniform = glGetUniformLocation(shaderProgramObject_PF, "uMaterialAmbient");
	materialDiffusedUniform = glGetUniformLocation(shaderProgramObject_PF, "uMaterialDiffused");
	materialSpecularUniform = glGetUniformLocation(shaderProgramObject_PF, "uMaterialSpecular");
	materialShynessUniform = glGetUniformLocation(shaderProgramObject_PF, "uMaterialShyniness");
	keyPressedUniform = glGetUniformLocation(shaderProgramObject_PF, "uKeyPressed");

    getSphereVertexData(sphere_positions, sphere_normals, sphere_textures, sphere_elements); //Elements - Face indices
    gNumVertices = getNumberOfSphereVertices();
    gNumElements = getNumberOfSphereElements();

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
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

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

    viewportWidth = width / 6;
    viewportHeight = height / 6;
    viewportInitialX = (width - (viewportWidth * 4)) / 2;
    viewportInitialY = (height - (viewportHeight * 6)) / 2;

	//glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void display(void)
{
	void drawSphere(float columnNumber, float rowNumber);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glUseProgram(shaderProgramObject_PF);

    for(int columnCounter = 0; columnCounter < numberOfColumns; ++columnCounter)
    {
        for(int rowCounter = 0; rowCounter < numberOfRows; ++rowCounter)
        {
            glViewport(viewportInitialX + (columnCounter * viewportWidth), viewportInitialY + (rowCounter * viewportHeight), viewportWidth, viewportHeight);

			if(bLightingEnabled == TRUE)
			{
				if(KeyPressed == 1)
				{
					fLightPosition[0] = 0.0f;
					fLightPosition[1] = sinf(fAngleForXRotation) * 100.0f - 3.0f;
					fLightPosition[2] = cosf(fAngleForXRotation) * 100.0f - 3.0f;
				}
				else if(KeyPressed == 2)
				{
					fLightPosition[0] = sinf(fAngleForYRotation) * 100.0f - 3.0f;
					fLightPosition[1] = 0.0f;
					fLightPosition[2] = cosf(fAngleForYRotation) * 100.0f - 3.0f;
				}
				else if(KeyPressed == 3)
				{
					fLightPosition[0] = sinf(fAngleForZRotation) * 100.0f - 3.0f;
					fLightPosition[1] = cosf(fAngleForZRotation) * 100.0f - 3.0f;
					fLightPosition[2] = 0.0f;
				}

				glUniform1i(keyPressedUniform, 1);
				
				glUniform3fv(lightAmbientUniform, 1, fLightAmbient); // light diffused is 4 elements array but passing only 3 elements as shader has vec3
				glUniform3fv(lightDiffusedUniform, 1, fLightDiffused);
				glUniform3fv(lightSpecularUniform, 1, fLightSpecular);
				glUniform4fv(lightPositionUniform, 1, fLightPosition);

				glUniform3fv(materialAmbientUniform, 1, fMaterialAmbient[columnCounter][rowCounter]); 
				glUniform3fv(materialDiffusedUniform, 1, fMaterialDiffused[columnCounter][rowCounter]); 
				glUniform3fv(materialSpecularUniform, 1, fMaterialSpecular[columnCounter][rowCounter]); 
				glUniform1f(materialShynessUniform, fMaterialShininess[columnCounter][rowCounter]); 
			}
			else
			{
				glUniform1i(keyPressedUniform, 0);
			}
	
            drawSphere((float)columnCounter, (float)rowCounter);
        }
    }

    glUseProgram(0);

    SwapBuffers(ghdc);
}

void drawSphere(float columnNumber, float rowNumber)
{
    vmath::mat4 modelMatrix = vmath::mat4::identity();
    vmath::mat4 viewMatrix = vmath::mat4::identity();

    // Translate the modal matrix.
    //modelMatrix = vmath::translate(-9.0f + (columnNumber * 3.0f), 6.0f - (rowNumber * 2.5f), -3.0f);
	modelMatrix = vmath::translate(0.0f, 0.0f, -3.0f);

	// push above mvp into vertex shader's mvpuniform
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

    // Now bind the VAO to which we want to use
    glBindVertexArray(gVao_sphere);

    // Draw the sphere
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // unbind the vao
    glBindVertexArray(0);
}

void update(void)
{
	//Code
	//Code
	if (KeyPressed == 1)
	{
		fAngleForXRotation = fAngleForXRotation + 0.1f;
		if (fAngleForXRotation >= 360.0f)
			fAngleForXRotation = fAngleForXRotation - 360.0f;
	}
	if (KeyPressed == 2)
	{
		fAngleForYRotation = fAngleForYRotation + 0.1f;
		if (fAngleForYRotation >= 360.0f)
			fAngleForYRotation = fAngleForYRotation - 360.0f;
	}
	if (KeyPressed == 3)
	{
		fAngleForZRotation = fAngleForZRotation + 0.1f;
		if (fAngleForZRotation >= 360.0f)
			fAngleForZRotation = fAngleForZRotation - 360.0f;
	}
}

void uninitialize(void)
{
	//Function Declarations
	void ToggleFullSCreen(void);

	//Code
	if(shaderProgramObject_PF)
	{
		glUseProgram(shaderProgramObject_PF);
		
		GLint numShaders = 0;
		glGetProgramiv(shaderProgramObject_PF, GL_ATTACHED_SHADERS, &numShaders);

		if(numShaders > 0)
		{
			GLuint* pShaders = (GLuint*)malloc(numShaders * sizeof(GLuint));

			if(pShaders != NULL)
			{
				glGetAttachedShaders(shaderProgramObject_PF, numShaders, NULL, pShaders);

				for(GLint i = 0; i < numShaders; i++)
				{
					glDetachShader(shaderProgramObject_PF, pShaders[i]);
					glDeleteShader(pShaders[i]);
					pShaders[i] = 0;
				}
				free(pShaders);
				pShaders = NULL;
			}
		}
		glUseProgram(0);

		glDeleteProgram(shaderProgramObject_PF);
		shaderProgramObject_PF = 0;
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
