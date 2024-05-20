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

GLfloat pAngle = 0.0f;

GLuint modelMatrixUniform = 0;
GLuint viewMatrixUniform = 0;
GLuint projectionMatrixUniform = 0;

GLuint lightAmbientUniform[3];
GLuint lightDiffusedUniform[3];
GLuint lightSpecularUniform[3];
GLuint lightPositionUniform[3];

GLuint materialAmbientUniform = 0;
GLuint materialDiffusedUniform = 0;
GLuint materialSpecularUniform = 0;
GLuint materialShynessUniform = 0;

GLuint keyPressedUniform = 0;

BOOL bLightingEnabled = FALSE;
BOOL bAnimationEnabled = FALSE;

GLfloat fLightAngleZero = 0.0f;
GLfloat fLightAngleOne = 0.0f;
GLfloat fLightAngleTwo = 0.0f;

// Light 0
GLfloat lightAmbientZero[] = {0.0f, 0.0f, 0.0f, 1.0f };
GLfloat lightDiffusedZero[] = {0.0f, 1.0f, 0.0f, 1.0f};
GLfloat lightSpecularZero[] = { 0.0f, 1.0f, 0.0f, 1.0f };
GLfloat lightPostionZero[] = {0.0f, 0.0f, 2.0f, 1.0f};

// Light 1
GLfloat lightAmbientOne[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat lightDiffusedOne[] = { 1.0f, 0.0f, 0.0f, 1.0f };
GLfloat lightSpecularOne[] = { 1.0f, 0.0f, 0.0f, 1.0f };
GLfloat lightPostionOne[] = { 2.0f, 0.0f, 0.0f, 1.0f };

// Light 1
GLfloat lightAmbientTwo[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat lightDiffusedTwo[] = { 0.0f, 0.0f, 1.0f, 1.0f };
GLfloat lightSpecularTwo[] = { 0.0f, 0.0f, 1.0f, 1.0f };
GLfloat lightPostionTwo[] = { 0.0f, 2.0f, 0.0f, 1.0f };

//Material Properties
GLfloat materialAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat materialDiffused[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat materialSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat materialShyniness = 128.0f;

mat4 perspectiveProjectionMatrix; //4 by 4 matrix,mat4 is in vmath.h

struct Light
{
	vec3 ambient;
	vec3 diffused;
	vec3 specular;
	vec4 position;
};

struct Light light[3];

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

	// printGLInfo();
	//PP related code

	//Vertex Shader
	const GLchar* vertexShaderSourceCode = 
					"#version 460 core" \
					"\n" \
					"in vec4 aPosition;\n" \
					"in vec3 aNormal;\n" \
					"uniform mat4 uModelMatrix;\n" \
					"uniform mat4 uViewMatrix;\n" \
					"uniform mat4 uProjectionMatrix;\n" \
					"uniform vec3 uLightAmbient[3];\n" \
					"uniform vec3 uLightDiffused[3];\n" \
					"uniform vec3 uLightSpecular[3];\n" \
					"uniform vec4 uLightPosition[3];\n" \
					"uniform vec3 uMaterialAmbient;\n" \
					"uniform vec3 uMaterialDiffused;\n" \
					"uniform vec3 uMaterialSpecular;\n" \
					"uniform float uMaterialShyniness;\n" \
					"uniform int uKeyPressed;\n" \
					"out vec3 oPhongADSLight;\n" \
					"void main(void)\n" \
					"{\n" \
					"oPhongADSLight = vec3(0.0f, 0.0f, 0.0f); \n" \
					"if(uKeyPressed == 1) \n" \
					"{\n" \
					"vec3 diffusedLight[3]; \n" \
					"vec3 ambientLight[3];\n" \
					"vec3 specularLight[3];\n" \
					"vec3 lightDirection[3];\n" \
					"vec3 reflectionVector[3];\n" \
					"vec4 eyeCoordinates = uViewMatrix * uModelMatrix * aPosition; \n" \
					"vec3 transformedNormals = normalize(mat3(uViewMatrix * uModelMatrix) * aNormal); \n" \
					"vec3 viewerVector = normalize(-eyeCoordinates.xyz);\n" \
					"for(int i = 0; i < 3; i++)\n" \
					"{\n" \
					"lightDirection[i] = normalize(vec3(uLightPosition[i] - eyeCoordinates)); \n" \
					"reflectionVector[i] = reflect(-lightDirection[i], transformedNormals);\n" \
					"diffusedLight[i] = uLightDiffused[i] * uMaterialDiffused * max(dot(lightDirection[i],transformedNormals), 0.0f); \n" \
					"ambientLight[i] = uLightAmbient[i] * uMaterialAmbient;\n" \
					"specularLight[i] = uLightSpecular[i] * uMaterialSpecular * pow(max(dot(reflectionVector[i], viewerVector), 0.0f), uMaterialShyniness);\n" \
					"oPhongADSLight =  oPhongADSLight + ambientLight[i] + diffusedLight[i] + specularLight[i]; \n" \
					"}\n" \
					"}\n" \
					"else \n" \
					"{\n" \
					"oPhongADSLight = vec3(0.0f, 0.0f, 0.0f); \n" \
					"}\n" \
					"gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * aPosition;\n" \
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
					"in vec3 oPhongADSLight;\n" \
					"uniform int uKeyPressed;\n" \
					"out vec4 FragColor;\n" \
					"void main(void)\n" \
					"{\n" \
					"if(uKeyPressed == 1) \n" \
					"{\n" \
					"FragColor = vec4(oPhongADSLight, 1.0f);\n" \
					"}\n" \
					"else \n" \
					"{\n" \
					"FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n" \
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
	modelMatrixUniform = glGetUniformLocation(shaderProgramObject, "uModelMatrix");
	viewMatrixUniform = glGetUniformLocation(shaderProgramObject, "uViewMatrix");
	projectionMatrixUniform = glGetUniformLocation(shaderProgramObject, "uProjectionMatrix");

	lightAmbientUniform[0] = glGetUniformLocation(shaderProgramObject, "uLightAmbient[0]");
	lightDiffusedUniform[0] = glGetUniformLocation(shaderProgramObject, "uLightDiffused[0]");
	lightSpecularUniform[0] = glGetUniformLocation(shaderProgramObject, "uLightSpecular[0]");
	lightPositionUniform[0] = glGetUniformLocation(shaderProgramObject, "uLightPosition[0]");

	lightAmbientUniform[1] = glGetUniformLocation(shaderProgramObject, "uLightAmbient[1]");
	lightDiffusedUniform[1] = glGetUniformLocation(shaderProgramObject, "uLightDiffused[1]");
	lightSpecularUniform[1] = glGetUniformLocation(shaderProgramObject, "uLightSpecular[1]");
	lightPositionUniform[1] = glGetUniformLocation(shaderProgramObject, "uLightPosition[1]");

	lightAmbientUniform[2] = glGetUniformLocation(shaderProgramObject, "uLightAmbient[2]");
	lightDiffusedUniform[2] = glGetUniformLocation(shaderProgramObject, "uLightDiffused[2]");
	lightSpecularUniform[2] = glGetUniformLocation(shaderProgramObject, "uLightSpecular[2]");
	lightPositionUniform[2] = glGetUniformLocation(shaderProgramObject, "uLightPosition[2]");

	materialAmbientUniform = glGetUniformLocation(shaderProgramObject, "uMaterialAmbient");
	materialDiffusedUniform = glGetUniformLocation(shaderProgramObject, "uMaterialDiffused");
	materialSpecularUniform = glGetUniformLocation(shaderProgramObject, "uMaterialSpecular");
	materialShynessUniform = glGetUniformLocation(shaderProgramObject, "uMaterialShyniness");

	keyPressedUniform = glGetUniformLocation(shaderProgramObject, "uKeyPressed");

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
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  	light[0].ambient = vec3(lightAmbientZero[0], lightAmbientZero[1], lightAmbientZero[2]);
	light[0].diffused = vec3(lightDiffusedZero[0], lightDiffusedZero[1], lightDiffusedZero[2]);
	light[0].specular = vec3(lightSpecularZero[0], lightSpecularZero[1], lightSpecularZero[2]);
	//light[0].position = vec4(lightPostionZero[0], lightPostionZero[1], lightPostionZero[2], lightPostionZero[3]);

	light[1].ambient = vec3(lightAmbientOne[0], lightAmbientOne[1], lightAmbientOne[2]);
	light[1].diffused = vec3(lightDiffusedOne[0], lightDiffusedOne[1], lightDiffusedOne[2]);
	light[1].specular = vec3(lightSpecularOne[0], lightSpecularOne[1], lightSpecularOne[2]);
	//light[1].position = vec4(lightPostionOne[0], lightPostionOne[1], lightPostionOne[2], lightPostionOne[3]);

	light[2].ambient = vec3(lightAmbientTwo[0], lightAmbientTwo[1], lightAmbientTwo[2]);
	light[2].diffused = vec3(lightDiffusedTwo[0], lightDiffusedTwo[1], lightDiffusedTwo[2]);
	light[2].specular = vec3(lightSpecularTwo[0], lightSpecularTwo[1], lightSpecularTwo[2]);
	//light[1].position = vec4(lightPostionOne[0], lightPostionOne[1], lightPostionOne[2], lightPostionOne[3]);

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

	//pyramid

	// Transformation
	mat4 modelMatrix = vmath::mat4::identity();
	mat4 ViewMatrix = vmath::mat4::identity();
	mat4 translationMatrix = vmath::mat4::identity();
	mat4 rotationMatrix = vmath::mat4::identity();
	translationMatrix = vmath::translate(0.0f, 0.0f, -3.0f);
	//rotationMatrix = vmath::rotate(pAngle, 0.0f, 0.0f, 0.0f);
	modelMatrix = translationMatrix * rotationMatrix;

	// push above mvp into vertex shader's mvpuniform
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, ViewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	// Update light positions
	mat4 lightRotationMatrix = vmath::mat4::identity();
	lightRotationMatrix = vmath::rotate(fLightAngleZero, 1.0f, 0.0f, 0.0f);
	vec4 lightPostion = vec4(0.0f, 0.0f, fLightAngleZero, 1.0f) * lightRotationMatrix;
	light[0].position = lightPostion;

	lightRotationMatrix = vmath::mat4::identity();
	lightRotationMatrix = vmath::rotate(fLightAngleOne, 0.0f, 1.0f, 0.0f);
	lightPostion = vec4(fLightAngleOne, 0.0f, 0.0f, 1.0f) * lightRotationMatrix;
	light[1].position = lightPostion;

	lightRotationMatrix = vmath::mat4::identity();
	lightRotationMatrix = vmath::rotate(fLightAngleTwo, 0.0f, 0.0f, 1.0f);
	lightPostion = vec4(0.0f, fLightAngleTwo, 0.0f, 1.0f) * lightRotationMatrix;
	light[2].position = lightPostion;

	if(bLightingEnabled == TRUE)
	{
		glUniform1i(keyPressedUniform, 1);
		
		glUniform3fv(lightAmbientUniform[0], 1, light[0].ambient); // light diffused is 4 elements array but passing only 3 elements as shader has vec3
		glUniform3fv(lightDiffusedUniform[0], 1, light[0].diffused);
		glUniform3fv(lightSpecularUniform[0], 1, light[0].specular);
		glUniform4fv(lightPositionUniform[0], 1, light[0].position);

		glUniform3fv(lightAmbientUniform[1], 1, light[1].ambient); // light diffused is 4 elements array but passing only 3 elements as shader has vec3
		glUniform3fv(lightDiffusedUniform[1], 1, light[1].diffused);
		glUniform3fv(lightSpecularUniform[1], 1, light[1].specular);
		glUniform4fv(lightPositionUniform[1], 1, light[1].position);

		glUniform3fv(lightAmbientUniform[2], 1, light[2].ambient); // light diffused is 4 elements array but passing only 3 elements as shader has vec3
		glUniform3fv(lightDiffusedUniform[2], 1, light[2].diffused);
		glUniform3fv(lightSpecularUniform[2], 1, light[2].specular);
		glUniform4fv(lightPositionUniform[2], 1, light[2].position);

		glUniform3fv(materialAmbientUniform, 1, materialAmbient); 
		glUniform3fv(materialDiffusedUniform, 1, materialDiffused); 
		glUniform3fv(materialSpecularUniform, 1, materialSpecular); 
		glUniform1f(materialShynessUniform, materialShyniness); 
	}
	else
	{
		glUniform1i(keyPressedUniform, 0);
	}

	// *** bind vao ***
    glBindVertexArray(gVao_sphere);

    // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

    // *** unbind vao ***
    glBindVertexArray(0);
	
	glUseProgram(0);	

	SwapBuffers(ghdc);
}

void update(void)
{
	//Code
	//When Annimation

	//Making Light 0
	fLightAngleZero = fLightAngleZero + 0.08f;
	if (fLightAngleZero >= 360.0f)
	{
		fLightAngleZero = 0.0f;
	}

	//Making Light 1
	fLightAngleOne = fLightAngleOne + 0.08f;
	if (fLightAngleOne >= 360.0f)
	{
		fLightAngleOne = 0.0f;
	}

	//Making Light 2
	fLightAngleTwo = fLightAngleTwo + 0.08f;
	if (fLightAngleTwo >= 360.0f)
	{
		fLightAngleTwo = 0.0f;
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
