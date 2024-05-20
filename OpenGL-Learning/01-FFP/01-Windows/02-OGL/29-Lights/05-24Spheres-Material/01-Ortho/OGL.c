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

GLfloat fLightAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat fLightDiffused[] = {1.0f, 1.0f, 1.0f, 1.0f};// White
//GLfloat fLightSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat fLightPosition[] = {0.0f, 0.0f, 0.0f, 1.0f};

/*
//Material Properties
GLfloat fMaterialAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat fMaterialDiffused[] = {1.0f, 0.2f, 0.7f, 1.0f};
GLfloat fMaterialSpecular[] = {0.1f, 1.0f, 1.0f, 1.0f};
GLfloat fMaterialShyniness[] = {128.0f};*/

GLUquadric* quadric = NULL;

GLfloat fAngleForXRotation = 0.0f;
GLfloat fAngleForYRotation = 0.0f;
GLfloat fAngleForZRotation = 0.0f;

GLuint KeyPressed;

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
	glLightfv(GL_LIGHT0, GL_AMBIENT, fLightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, fLightDiffused);
	glEnable(GL_LIGHT0);

	//Initialize quadric
	quadric = gluNewQuadric();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//Set Clear Color Of Window to Blue -- Its just informing but not yet colored
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f); //Dark Grey

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

	if (width <= height)
	{
		glOrtho(0.0f,
			15.5f,
			-0.0f * ((GLfloat)height / (GLfloat)width),
			15.5f * ((GLfloat)height / (GLfloat)width),
			-10.0f,
			10.0f);
	}
	else
	{
		glOrtho(0.0f * ((GLfloat)width / (GLfloat)height),
			15.5f * ((GLfloat)width / (GLfloat)height),
			0.0f,
			15.0f,
			-10.0f,
			10.0f);
	}

}

void display(void)
{
	//Variable Declarations
	GLfloat MaterialAmbient[4];
	GLfloat MaterialDiffused[4];
	GLfloat MaterialSpecular[4];
	GLfloat MaterialShininess;

	//Code
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
	glTranslatef(1.5f, 14.0f, 0.0f);
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
	glTranslatef(1.5f, 11.5f, 0.0f);
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
	glTranslatef(1.5f, 9.0f, 0.0f);
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
	glTranslatef(1.5f, 6.5f, 0.0f);
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
	glTranslatef(1.5f, 4.0f, 0.0f);
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
	glTranslatef(1.5f, 1.5f, 0.0f);
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
	glTranslatef(7.5f, 14.0f, 0.0f);
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
	glTranslatef(7.5f, 11.5f, 0.0f);
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
	glTranslatef(7.5f, 9.0f, 0.0f);
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
	glTranslatef(7.5f, 6.5f, 0.0f);
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
	glTranslatef(7.5f, 4.0f, 0.0f);
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
	glTranslatef(7.5f, 1.5f, 0.0f);
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
	glTranslatef(13.5f, 14.0f, 0.0f);
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
	glTranslatef(13.5f, 11.5f, 0.0f);
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
	glTranslatef(13.5f, 9.0f, 0.0f);
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
	glTranslatef(13.5f, 6.5f, 0.0f);
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
	glTranslatef(13.5f, 4.0f, 0.0f);
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
	glTranslatef(13.5f, 1.5f, 0.0f);
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
	glTranslatef(19.5f, 14.0f, 0.0f);
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
	glTranslatef(19.5f, 11.5f, 0.0f);
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
	glTranslatef(19.5f, 9.0f, 0.0f);
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
	glTranslatef(19.5f, 6.5f, 0.0f);
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
	glTranslatef(19.5f, 4.0f, 0.0f);
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
	glTranslatef(19.5f, 1.5f, 0.0f);
	gluSphere(quadric, 1.0f, 30, 30);

	SwapBuffers(ghdc);
}

void update(void)
{
	//Code
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
