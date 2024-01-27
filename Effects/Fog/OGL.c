//Windows Header Files
#include <windows.h>
#include <stdio.h> //For File IO
#include <stdlib.h> //For exit()
#include <wingdi.h>
#include <math.h>

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
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) }; //All elements are set to value in  '{}' //memset() & ZeroMemory()
BOOL gbFullScreen = FALSE;

// OPENGL Related Global Variables
HDC ghdc = NULL;
HGLRC ghrc = NULL; // Handle Graphic Rendering Context

GLfloat pAngle = 0.0f;
GLfloat cAngle = 0.0f;

//Texture Object
GLuint texture_floor = 0;

//Fog
GLfloat fFogColor[4] = {0.5, 0.5, 0.5, 1};
BOOL bFogEnable = FALSE;
GLfloat fDensity = 0.0f;

GLfloat fCamx = 0.0f;
GLfloat fCamy = 0.0f;
GLfloat fCamz = -15.0f;
GLfloat fCamSpeed = 0.5f;

//Lights Variables
BOOL gbLight = FALSE;
GLfloat fLightAmbientZero[] = {1.0f, 1.0f, 1.0f, 1.0f}; // Grey
GLfloat fLightDiffusedZero[] = {1.0f, 1.0f, 1.0f, 1.0f}; // White 
GLfloat fLightSpecularZero[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat fLightPositionZero[] = {0.0f, 4.0f, 0.0f, 1.0f};
GLfloat fLightSpotDirectionZero[] = {0.0f, -1.0f, 0.0f};
GLfloat fLightSpotCutoffZero = 30.0f;
GLfloat fLightSpotExponentZero = 40.0f;

GLfloat fLightAmbientOne[] = {0.0f, 0.0f, 1.0f, 1.0f}; // Grey
GLfloat fLightDiffusedOne[] = {0.0f, 0.0f, 1.0f, 1.0f}; // White 
GLfloat fLightSpecularOne[] = {0.0f, 0.0f, 1.0f, 1.0f};
GLfloat fLightPositionOne[] = {-6.0f, 4.0f, 4.0f, 1.0f};
GLfloat fLightSpotDirectionOne[] = {0.0f, -1.0f, 0.0f};
GLfloat fLightSpotCutoffOne = 30.0f;
GLfloat fLightSpotExponentOne = 40.0f;

GLfloat fLightAmbientTwo[] = {0.0f, 1.0f, 0.0f, 1.0f}; // Grey
GLfloat fLightDiffusedTwo[] = {0.0f, 1.0f, 0.0f, 1.0f}; // White 
GLfloat fLightSpecularTwo[] = {0.0f, 1.0f, 0.0f, 1.0f};
GLfloat fLightPositionTwo[] = {-6.0f, 4.0f, -4.0f, 1.0f};
GLfloat fLightSpotDirectionTwo[] = {0.0f, -1.0f, 0.0f};
GLfloat fLightSpotCutoffTwo = 30.0f;
GLfloat fLightSpotExponentTwo = 40.0f;

GLfloat fLightAmbientThree[] = {1.0f, 0.0f, 0.0f, 1.0f}; // Grey
GLfloat fLightDiffusedThree[] = {1.0f, 0.0f, 0.0f, 1.0f}; // White 
GLfloat fLightSpecularThree[] = {1.0f, 0.0f, 0.0f, 1.0f};
GLfloat fLightPositionThree[] = {6.0f, 4.0f, -4.0f, 1.0f};
GLfloat fLightSpotDirectionThree[] = {0.0f, -1.0f, 0.0f};
GLfloat fLightSpotCutoffThree = 30.0f;
GLfloat fLightSpotExponentThree = 40.0f;

GLfloat fLightAmbientFour[] = {1.0f, 1.0f, 0.0f, 1.0f}; // Grey
GLfloat fLightDiffusedFour[] = {1.0f, 1.0f, 0.0f, 1.0f}; // White 
GLfloat fLightSpecularFour[] = {1.0f, 1.0f, 0.0f, 1.0f};
GLfloat fLightPositionFour[] = {6.0f, 4.0f, 4.0f, 1.0f};
GLfloat fLightSpotDirectionFour[] = {0.0f, -1.0f, 0.0f};
GLfloat fLightSpotCutoffFour = 30.0f;
GLfloat fLightSpotExponentFour = 40.0f;

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

	int iWindow_Xpos = (iScreen_Width / 2) - (WIN_WIDTH / 2);
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

	case WM_KEYDOWN:
		switch (LOWORD(wParam))
		{
		case VK_ESCAPE: //27
			fprintf(gpFile, "Escape Pressed ... Exiting !!!\n");
			DestroyWindow(hwnd);
			break;
		case VK_LEFT: //27
			fCamx = fCamx - fCamSpeed;
			break;
		case VK_RIGHT: //27
			fCamx = fCamx + fCamSpeed;
			break;
		case VK_UP: //27
			fCamy = fCamy + fCamSpeed;
			break;
		case VK_DOWN: //27
			fCamy = fCamy - fCamSpeed;
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
		case 'D':
			fDensity = fDensity + 0.01f;
			break;
		case 'd':
			fDensity = fDensity - 0.01f;
			break;

		case '+':
			fCamz = fCamz + fCamSpeed;
			break;
		case '-':
			fCamz = fCamz - fCamSpeed;
			break;

		case 'R':
		case 'r':
			fCamx = 0.0f;
			fCamy = 0.0f;
			fCamz = 0.0f;
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
	MONITORINFO  mi = { sizeof(MONITORINFO) };

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
	BOOL loadGlTexture(GLuint*, TCHAR[]);

	//Code
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex = 0;
	BOOL bResult;

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
	glLightfv(GL_LIGHT0, GL_AMBIENT, fLightAmbientZero);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, fLightDiffusedZero);
	glLightfv(GL_LIGHT0, GL_SPECULAR, fLightSpecularZero);
	glLightfv(GL_LIGHT0, GL_POSITION, fLightPositionZero);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, fLightSpotCutoffZero);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, fLightSpotDirectionZero);
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, fLightSpotExponentZero);
	glEnable(GL_LIGHT0);

	// Light Initialization
	glLightfv(GL_LIGHT1, GL_AMBIENT, fLightAmbientOne);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, fLightDiffusedOne);
	glLightfv(GL_LIGHT1, GL_SPECULAR, fLightSpecularOne);
	glLightfv(GL_LIGHT1, GL_POSITION, fLightPositionOne);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, fLightSpotCutoffOne);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, fLightSpotDirectionOne);
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, fLightSpotExponentOne);
	glEnable(GL_LIGHT1);

	// Light Initialization
	glLightfv(GL_LIGHT2, GL_AMBIENT, fLightAmbientTwo);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, fLightDiffusedTwo);
	glLightfv(GL_LIGHT2, GL_SPECULAR, fLightSpecularTwo);
	glLightfv(GL_LIGHT2, GL_POSITION, fLightPositionTwo);
	glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, fLightSpotCutoffTwo);
	glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, fLightSpotDirectionTwo);
	glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, fLightSpotExponentTwo);
	glEnable(GL_LIGHT2);

	// Light Initialization
	glLightfv(GL_LIGHT3, GL_AMBIENT, fLightAmbientThree);
	glLightfv(GL_LIGHT3, GL_DIFFUSE, fLightDiffusedThree);
	glLightfv(GL_LIGHT3, GL_SPECULAR, fLightSpecularThree);
	glLightfv(GL_LIGHT3, GL_POSITION, fLightPositionThree);
	glLightf(GL_LIGHT3, GL_SPOT_CUTOFF, fLightSpotCutoffThree);
	glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, fLightSpotDirectionThree);
	glLightf(GL_LIGHT3, GL_SPOT_EXPONENT, fLightSpotExponentThree);
	glEnable(GL_LIGHT3);

	// Light Initialization
	glLightfv(GL_LIGHT4, GL_AMBIENT, fLightAmbientFour);
	glLightfv(GL_LIGHT4, GL_DIFFUSE, fLightDiffusedFour);
	glLightfv(GL_LIGHT4, GL_SPECULAR, fLightSpecularFour);
	glLightfv(GL_LIGHT4, GL_POSITION, fLightPositionFour);
	glLightf(GL_LIGHT4, GL_SPOT_CUTOFF, fLightSpotCutoffFour);
	glLightfv(GL_LIGHT4, GL_SPOT_DIRECTION, fLightSpotDirectionFour);
	glLightf(GL_LIGHT4, GL_SPOT_EXPONENT, fLightSpotExponentFour);
	glEnable(GL_LIGHT4);			

	glEnable(GL_FOG);
	glFogi(GL_FOG_MODE, GL_EXP);
	glFogfv(GL_FOG_COLOR, fFogColor);
	glFogf(GL_FOG_DENSITY, fDensity);
	glHint(GL_FOG_HINT, GL_DONT_CARE);
	glFogf(GL_FOG_START, 1.0f);
	glFogf(GL_FOG_END, 20.0);

	//Initialize quadric
	quadric = gluNewQuadric();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//Set Clear Color Of Window to Blue -- Its just informing but not yet colored
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	// Loading Images - Create Texture
	bResult = loadGlTexture(&texture_floor, MAKEINTRESOURCE(GROUND_TEXTURE));
	if (bResult == FALSE)
	{
		fprintf(gpFile, "loadGlTexture(MY_STONE_BITMAP) failed .../n");
		return -6;
	}

	glEnable(GL_TEXTURE_2D);

	resize(WIN_WIDTH, WIN_HEIGHT);

	//Here OPEN GL Starts
	return 0;
}

BOOL loadGlTexture(GLuint* texture, TCHAR imageResourceID[])
{
	//Step 4 - Local Variable Declarations
	HBITMAP hBitmap = NULL;
	BITMAP bmp;

	//Step 5 - Load the Image
	hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), imageResourceID, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	if (hBitmap == NULL)
	{
		fprintf(gpFile, "LoadImage() failed .../n");
		return FALSE;
	}

	//Step 6 - Get Image Data
	GetObject(hBitmap, sizeof(BITMAP), &bmp);

	//Step 7 - Create OpenGL Texture
	glGenTextures(1, texture);

	//Step 8 - Bind To the generated texture
	glBindTexture(GL_TEXTURE_2D, *texture);

	//Step 9 - Unpack Image Data 
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4); //4 - R G B A

	//Step 10 - Set Texture Parameter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	//Step 11 - Crate Multiple Bitmap Images
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, bmp.bmWidth, bmp.bmHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, (void*)bmp.bmBits);

	//Step 12 - Unbind from binding point
	glBindTexture(GL_TEXTURE_2D, 0);

	//Step 13 - Delete Image Resource
	DeleteObject(hBitmap);
	hBitmap = NULL;
 
	return TRUE;
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
	void DrawGrid(void);
	//Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Now here screen gets colored as color mentioned in initialize()

	glMatrixMode(GL_MODELVIEW); //

	glLoadIdentity(); // Set Matrix to identity

	gluLookAt(fCamx, fCamy, fCamz, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	glPushMatrix();
	glLightfv(GL_LIGHT0, GL_POSITION, fLightPositionZero);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, fLightSpotCutoffZero);
	glPushAttrib(GL_LIGHTING_BIT);
	glDisable(GL_LIGHTING);
	glTranslatef(fLightPositionZero[0], fLightPositionZero[1], fLightPositionZero[2]);
	glColor3f(1.0f, 1.0f, 1.0f);
	gluSphere(quadric, 0.5f, 50, 50);
	glPopAttrib();
	glPopMatrix();

	glPushMatrix();
	glLightfv(GL_LIGHT1, GL_POSITION, fLightPositionOne);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, fLightSpotCutoffOne);
	glPushAttrib(GL_LIGHTING_BIT);
	glDisable(GL_LIGHTING);
	glTranslatef(fLightPositionOne[0], fLightPositionOne[1], fLightPositionOne[2]);
	glColor3f(0.0f, 0.0f, 1.0f);
	gluSphere(quadric, 0.5f, 50, 50);
	glPopAttrib();
	glPopMatrix();

	glPushMatrix();
	glLightfv(GL_LIGHT2, GL_POSITION, fLightPositionTwo);
	glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, fLightSpotCutoffTwo);
	glPushAttrib(GL_LIGHTING_BIT);
	glDisable(GL_LIGHTING);
	glTranslatef(fLightPositionTwo[0], fLightPositionTwo[1], fLightPositionTwo[2]);
	glColor3f(0.0f, 1.0f, 0.0f);
	gluSphere(quadric, 0.5f, 50, 50);
	glPopAttrib();
	glPopMatrix();

	glPushMatrix();
	glLightfv(GL_LIGHT3, GL_POSITION, fLightPositionThree);
	glLightf(GL_LIGHT3, GL_SPOT_CUTOFF, fLightSpotCutoffThree);
	glPushAttrib(GL_LIGHTING_BIT);
	glDisable(GL_LIGHTING);
	glTranslatef(fLightPositionThree[0], fLightPositionThree[1], fLightPositionThree[2]);
	glColor3f(1.0f, 0.0f, 0.0f);
	gluSphere(quadric, 0.5f, 50, 50);
	glPopAttrib();
	glPopMatrix();

	glPushMatrix();
	glLightfv(GL_LIGHT4, GL_POSITION, fLightPositionFour);
	glLightf(GL_LIGHT4, GL_SPOT_CUTOFF, fLightSpotCutoffFour);
	glPushAttrib(GL_LIGHTING_BIT);
	glDisable(GL_LIGHTING);
	glTranslatef(fLightPositionFour[0], fLightPositionFour[1], fLightPositionFour[2]);
	glColor3f(1.0f, 1.0f, 0.0f);
	gluSphere(quadric, 0.5f, 50, 50);
	glPopAttrib();
	glPopMatrix();				

	glColor3f(1.0f, 1.0f, 1.0f);
	DrawGrid();

	glFogf(GL_FOG_DENSITY, fDensity);


	SwapBuffers(ghdc);
}

// Draw the ground as a series of triangle strips
void DrawGrid(void)
{
	GLfloat fExtent = 10.0f;
	GLfloat fStep = 0.3f;
	GLfloat y = -2.5f;
	static float fT = -1.0f;
	GLfloat fStrip, fRun;
	GLfloat s = 0.0f;
	GLfloat t = 0.0f;
	GLfloat texStep = 1.0f / (fExtent * .075f);
	// Ground is a tiling texture
	glBindTexture(GL_TEXTURE_2D, texture_floor);
	// Lay out strips and repeat textures coordinates
	for(fStrip = -fExtent; fStrip <= fExtent; fStrip += fStep)
	{
		t = 0.0f;
		glBegin(GL_TRIANGLE_STRIP);
		for(fRun = fExtent; fRun >= -fExtent; fRun -= fStep)
		{
			glTexCoord2f(s, t);
			glNormal3f(0.0f, 1.0f, 0.0f); // All Point up
			glVertex3f(fStrip, y, fRun);
			glTexCoord2f(s + texStep, t);
			glNormal3f(0.0f, 1.0f, 0.0f); // All Point up
			glVertex3f(fStrip + fStep, y, fRun);
			t += texStep;
		}
		glEnd();
		s += texStep;
	}
}

void update(void)
{
	//Code
	pAngle = pAngle + 0.1f;

	if (pAngle >= 360.0f)
		pAngle = pAngle - 360.0f;

	cAngle = cAngle - 0.1f;

	if (cAngle <= 0.0f)
		cAngle = cAngle + 360.0f;
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

	if (texture_floor)
	{
		glDeleteTextures(1, &texture_floor);
		texture_floor = 0;
	}

	//Close the log file
	if (gpFile) {
		fprintf(gpFile, "Program finished successfully.\n");
		fclose(gpFile);
		gpFile = NULL;
	}
}
