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
	AGS_ATTRIBUTE_COLOR
};
GLuint vao_Triangle = 0;
GLuint vbo_Position_Triangle = 0;
GLuint vbo_Color_Triangle = 0;
GLfloat tAngle = 0.0f;

GLuint vao_Square = 0;
GLuint vbo_Position_Square = 0;
GLuint vbo_Color_Square = 0;
GLfloat sAngle = 0.0f;

GLuint mvpMatrixUniform = 0;
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

	printGLInfo();
	//PP related code

	//Vertex Shader
	const GLchar* vertexShaderSourceCode = 
				"#version 460 core" \
				"\n" \
				"in vec4 aPosition;\n" \
				"in vec4 aColor;\n" \
				"out vec4 oColor;\n" \
				"uniform mat4 umvpMatrix;\n" \
				"void main(void)\n" \
				"{\n" \
				"gl_Position = umvpMatrix*aPosition;\n" \
				"oColor=aColor;\n" \
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
		"in vec4 oColor;\n" \
		"out vec4 fragColor;\n" \
		"void main(void)\n" \
		"{\n" \
		"fragColor = oColor;\n" \
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
	glBindAttribLocation(shaderProgramObject, AGS_ATTRIBUTE_COLOR, "aColor");
	
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
	mvpMatrixUniform = glGetUniformLocation(shaderProgramObject, "umvpMatrix");

	const GLfloat traingle_position[] = {
						0.0f,1.0f,0.0f,
						-1.0f,-1.0f,0.0f,
						1.0f,-1.0f,0.0f
						};

	const GLfloat triangle_color[] =
	{
		1.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,
		0.0f,0.0f,1.0f
	};

	const GLfloat square_position[] = {
						1.0f, 1.0f, 0.0f,
						-1.0f, 1.0f, 0.0f,
						-1.0f, -1.0f, 0.0f,
						1.0f, -1.0f, 0.0f
						};

	const GLfloat square_color[] =
	{
		0.0f,0.0f,1.0f,
		0.0f,0.0f,1.0f,
		0.0f,0.0f,1.0f,
		0.0f,0.0f,1.0f
	};	

	//For Traingle
	//VAO
	glGenVertexArrays(1, &vao_Triangle);
	glBindVertexArray(vao_Triangle);

	//VBO for postions		
	glGenBuffers(1, &vbo_Position_Triangle);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_Position_Triangle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(traingle_position), traingle_position, GL_STATIC_DRAW);
	glVertexAttribPointer(AGS_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AGS_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//VBO for color		
	glGenBuffers(1, &vbo_Color_Triangle);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_Color_Triangle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_color), triangle_color, GL_STATIC_DRAW);
	glVertexAttribPointer(AGS_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AGS_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//Unbind with VAO
	glBindVertexArray(0);

	//For Square
	//VAO
	glGenVertexArrays(1, &vao_Square);
	glBindVertexArray(vao_Square);

	//VBO for postions		
	glGenBuffers(1, &vbo_Position_Square);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_Position_Square);
	glBufferData(GL_ARRAY_BUFFER, sizeof(square_position), square_position, GL_STATIC_DRAW);
	glVertexAttribPointer(AGS_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AGS_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//color		
	//glVertexAtrrib3f(AGS_ATTRIBUTE_COLOR, 0.0f, 0.0f, 1.0f);
	glGenBuffers(1, &vbo_Color_Square);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_Color_Square);
	glBufferData(GL_ARRAY_BUFFER, sizeof(square_color), square_color, GL_STATIC_DRAW);
	glVertexAttribPointer(AGS_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AGS_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Unbind with VAO
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

	// Triangle
	// Transformation
	mat4 modelViewMatrix = mat4::identity();
	modelViewMatrix = vmath::translate(-1.5f, 0.0f, -6.0f);
	modelViewMatrix = modelViewMatrix * vmath::rotate(tAngle, 0.0f, 1.0f, 0.0f);
	mat4 modelViewProjectionMatrix =  mat4::identity();
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix; // order of multiplication is very imp

	// push above mvp into vertex shader's mvpuniform
	glUniformMatrix4fv(mvpMatrixUniform, 1,GL_FALSE, modelViewProjectionMatrix);

	glBindVertexArray(vao_Triangle);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glBindVertexArray(0);

	// Square
	// Transformation
	modelViewMatrix =  vmath::mat4::identity();
	modelViewMatrix = vmath::translate(1.5f, 0.0f, -6.0f);
	modelViewMatrix = modelViewMatrix * vmath::rotate(tAngle, 1.0f, 0.0f, 0.0f);
	modelViewProjectionMatrix =  mat4::identity();
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix; // order of multiplication is very imp

	// push above mvp into vertex shader's mvpuniform
	glUniformMatrix4fv(mvpMatrixUniform, 1,GL_FALSE, modelViewProjectionMatrix);

	glBindVertexArray(vao_Square);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glBindVertexArray(0);	
	glUseProgram(0);	

	SwapBuffers(ghdc);
}

void update(void)
{
	//Code
	//When Annimation
	tAngle = tAngle + 0.1f;
	if (tAngle >= 360.0f)
	{
		tAngle = -360.0f;
	}

	//updtae rectangle
	sAngle = sAngle - 0.1f;
	if (sAngle <= 0.0f)
	{
		sAngle = sAngle + 360.0f;
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

	if(vbo_Color_Triangle)
	{
		glDeleteBuffers(1, &vbo_Color_Triangle);
		vbo_Color_Triangle = 0;
	}

	if(vbo_Position_Triangle)
	{
		glDeleteBuffers(1, &vbo_Position_Triangle);
		vbo_Position_Triangle = 0;
	}

	if(vao_Triangle)
	{
		glDeleteVertexArrays(1, &vao_Triangle);
		vao_Triangle = 0;
	}

	if(vbo_Color_Triangle)
	{
		glDeleteBuffers(1, &vbo_Color_Triangle);
		vbo_Color_Triangle = 0;
	}

	if(vbo_Position_Triangle)
	{
		glDeleteBuffers(1, &vbo_Position_Triangle);
		vbo_Position_Triangle = 0;
	}

	if(vao_Square)
	{
		glDeleteVertexArrays(1, &vao_Triangle);
		vao_Triangle = 0;
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
