//Windows Header Files
#include <windows.h>
#include <stdio.h> //For File IO
#include <stdlib.h> //For exit()
#include <wingdi.h>

// OPENGL Header Files
#include<gl/GL.h>
#include<gl/GLU.h>

#include "OGL.h"

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

//Link with OpenGL library
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

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

//Lights Variables
BOOL gbLight = FALSE;

GLfloat lightAmbientZero[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat lightDiffusedZero[] = { 0.0f,1.0f,0.0f,1.0f };
GLfloat lightSpecularZero[] = { 0.0f,1.0f,0.0f,1.0f };
GLfloat lightPositionZero[4];

GLfloat lightAmbientOne[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat lightDiffusedOne[] = { 1.0f,0.0f,0.0f,1.0f };
GLfloat lightSpecularOne[] = { 1.0f,0.0f,0.0f,1.0f };
GLfloat lightPositionOne[4];

GLfloat lightAmbientTwo[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat lightDiffusedTwo[] = { 0.0f,0.0f,1.0f,1.0f };
GLfloat lightSpecularTwo[] = { 0.0f,0.0f,1.0f,1.0f };
GLfloat lightPositionTwo[4];

//Material Properties
GLfloat fMaterialAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat fMaterialDiffused[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat fMaterialSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat fMaterialShyniness[] = {128.0f};

GLfloat fLightAngleZero = 0.0f;
GLfloat fLightAngleOne = 0.0f;
GLfloat fLightAngleTwo = 0.0f;

GLUquadric* quadric = NULL;

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
	gpFile = fopen("Log.txt", "w");
	if (gpFile == NULL)
	{
		MessageBox(NULL, TEXT("Log File Cannot be opened"), TEXT("Error"), MB_OK | MB_ICONERROR);
		exit(0);
	}
	fprintf(gpFile, "Program started succesfully.\n");

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
		fprintf(gpFile, "Program Window Active = TRUE.\n");
		gbActive = TRUE;
		break;
	
	case WM_KILLFOCUS:
		fprintf(gpFile, "Program Window Active = FALSE.\n");
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
			if (gbLight == FALSE)
			{
				glEnable(GL_LIGHTING);
				gbLight = TRUE;
			}
			else
			{	
				glDisable(GL_LIGHTING);
				gbLight = FALSE;
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
		fprintf(gpFile, "GetDc() failed .../n");
		return -1;
	}

	//Get Closet Pixel Format Index from OS
	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormatIndex == 0)
	{
		fprintf(gpFile, "ChoosePixelFormat() failed .../n");
		return -2;
	}

	//Set Pixel Format
	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
	{
		fprintf(gpFile, "SetPixelFormat() failed .../n");
		return -3;
	}

	//Bridging API - Windowing to OPENGL
	//WGL - Windows Graphics Library
	//Tell WGL Bridging library to give OPENGL compatible DC from current window DC (Device Context)

	//Create Open GL Rendrting context from DC
	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
	{
		fprintf(gpFile, "wglCreateContext() failed .../n");
		return -4;
	}

	//Now ghdc will end its role & will give control to ghrc
	//Make Rendering Context Current
	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
	{
		fprintf(gpFile, "wglMakeCurrent() failed .../n");
		return -5;
	}

	//Enabling Depth
	glShadeModel(GL_SMOOTH); // Beautification - Whenever lighting and fading 
	glClearDepth(1.0f); // Compulsory - Default depth
	glEnable(GL_DEPTH_TEST); // Compulsory - Enable Depth Test
	glDepthFunc(GL_LEQUAL); // Compulsary - Depth is less than or equal to 
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); //Beutification - 

	// Light Initialization
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbientZero);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffusedZero);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecularZero);
	//glLightfv(GL_LIGHT0, GL_POSITION, lightPositionZero);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbientOne);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffusedOne);
	glLightfv(GL_LIGHT1, GL_SPECULAR, lightSpecularOne);
	//glLightfv(GL_LIGHT1, GL_POSITION, lightPositionOne);
	glEnable(GL_LIGHT1);

	glLightfv(GL_LIGHT2, GL_AMBIENT, lightAmbientTwo);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, lightDiffusedTwo);
	glLightfv(GL_LIGHT2, GL_SPECULAR, lightSpecularTwo);
	//glLightfv(GL_LIGHT2, GL_POSITION, lightPositionTwo);
	glEnable(GL_LIGHT2);

	//Material Initialization
	glMaterialfv(GL_FRONT, GL_AMBIENT, fMaterialAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, fMaterialDiffused); 
	glMaterialfv(GL_FRONT, GL_SPECULAR, fMaterialSpecular);
	glMaterialfv(GL_FRONT, GL_SHININESS, fMaterialShyniness);

	//Initialize quadric
	quadric = gluNewQuadric();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//Set Clear Color Of Window to Blue -- Its just informing but not yet colored
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	resize(WIN_WIDTH, WIN_HEIGHT);

	//Here OPEN GL Starts
	return 0;
}

void resize(int width, int height)
{
	//Code
	if (height <= 0)
		height = 1;

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	glViewport(0, 0, (GLsizei)width, (GLsizei)height); // Focussing on perticular area of window, here its entire window

	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

	/*
	* height = (tan(fovy/2) / (180.0f * PI) ) * near
	* width = height * aspectRatio
	* glFrustrum(-w, w, -H, H, near, far);
	*/
}

void display(void)
{
	//Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Now here screen gets colored as color mentioned in initialize()

	glMatrixMode(GL_MODELVIEW); //

	glLoadIdentity(); // Set Matrix to identity

	//gluLookAt(0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	// Matrix STACK concept
	glPushMatrix();
	gluLookAt(0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	//Rendering Light 0
	glPushMatrix();
	glRotatef(fLightAngleZero, 1.0f	, 0.0f, 0.0f);
	lightPositionZero[0] = 0.0f;
	lightPositionZero[1] = 0.0f;
	lightPositionZero[2] = fLightAngleZero;
	lightPositionZero[3] = 1.0f;
	glLightfv(GL_LIGHT0, GL_POSITION, lightPositionZero);
	glPopMatrix();

	//Rendering Light 1
	glPushMatrix();
	glRotatef(fLightAngleOne, 0.0f, 1.0f, 0.0f);
	lightPositionOne[0] = fLightAngleOne;
	lightPositionOne[1] = 0.0f;
	lightPositionOne[2] = 0.0f;
	lightPositionOne[3] = 1.0f;
	glLightfv(GL_LIGHT1, GL_POSITION, lightPositionOne);
	glPopMatrix();

	//Rendering Light 2
	glPushMatrix();
	glRotatef(fLightAngleTwo, 0.0f, 0.0f, 1.0f);
	lightPositionTwo[0] = 0.0f;
	lightPositionTwo[1] = fLightAngleTwo;
	lightPositionTwo[2] = 0.0f;
	lightPositionTwo[3] = 1.0f;
	glLightfv(GL_LIGHT2, GL_POSITION, lightPositionTwo);
	glPopMatrix();

	gluSphere(quadric, 0.5f, 50, 50); //gluSphere internally creates all needed normal for you

	glPopMatrix();

	SwapBuffers(ghdc);
}

void update(void)
{
	//Code

	//Making Light 0
	fLightAngleZero = fLightAngleZero + 0.05f;
	if (fLightAngleZero >= 360.0f)
	{
		fLightAngleZero = 0.0f;
	}

	//Making Light 1
	fLightAngleOne = fLightAngleOne + 0.05f;
	if (fLightAngleOne >= 360.0f)
	{
		fLightAngleOne = 0.0f;
	}

	//Making Light 2
	fLightAngleTwo = fLightAngleTwo + 0.05f;
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

	if (quadric)
	{
		gluDeleteQuadric(quadric);
	}
	//Close the log file
	if (gpFile) {
		fprintf(gpFile, "Program finished successfully.\n");
		fclose(gpFile);
		gpFile = NULL;
	}
}
