//Windows Header Files
#include <windows.h>
#include <stdio.h> //For File IO
#include <stdlib.h> //For exit()
#include <wingdi.h>
#include <math.h>
#include <mmsystem.h>

// OPENGL Header Files
#include<gl/GL.h>
#include<gl/GLU.h>

#include "OGL.h"
#include "BharatLetters.h"
#include "FighterJet.h"

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#define PI 3.14f
//Link with OpenGL library
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "winmm.lib")

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

GLUquadric* quadric = NULL; // Used for Sphere

GLfloat cAngle = 0.0f;

GLfloat fCamx = 0.0f;
GLfloat fCamy = 0.0f;
GLfloat fCamz = 0.0f;
GLfloat fCamSpeed = 1.0f;

GLfloat fBaseDepth = -23.8f;
GLfloat fAlphaFactor = 1.0f;
void DrawPartialCircle(float fXCenterPos, float fYCenterPos, float fRadius, int firstPos, int secondPos, float* fColorRgbV, float* fColorRgbV2, int);

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
	case WM_CREATE:
		//PlaySound(MAKEINTRESOURCE(MY_MUSIC), NULL, SND_RESOURCE | SND_ASYNC | SND_LOOP);
		break;

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
		case VK_LEFT: //27
			//fCamx = fCamx - fCamSpeed;
			fJetTranslateXFactor = fJetTranslateXFactor - 0.1f;
			break;
		case VK_RIGHT: //27
			//fCamx = fCamx + fCamSpeed;
			fJetTranslateXFactor = fJetTranslateXFactor + 0.1f;
			break;
		case VK_UP: //27
			//fCamy = fCamy + fCamSpeed;
			fJetTranslateYFactor = fJetTranslateYFactor + 0.1f;
			break;
		case VK_DOWN: //27
			//fCamy = fCamy - fCamSpeed;
			fJetTranslateYFactor = fJetTranslateYFactor - 0.1f;
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
		case '+':
			fCamz = fCamz + fCamSpeed;
			fprintf(gpFile, "fCamz = %f\n", fCamz);
			break;
		case '-':
			fCamz = fCamz - fCamSpeed;
			break;
		case 'R':
		case 'r':
			fCamx = 0.0f;
			fCamy = 0.0f;
			fCamz = 0.0f;
			fCamSpeed = 0.5f;
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

	quadric = gluNewQuadric();
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

	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 1000.0f);

	/*
	* height = (tan(fovy/2) / (180.0f * PI) ) * near
	* width = height * aspectRatio
	* glFrustrum(-w, w, -H, H, near, far);
	*/
}

float fBTranslateFactor = -30.0f; // As like infinete
float fHTranslateFactor = -35.0f; // As like infinete
float fA1TranslateFactor = -40.0f; // As like infinete
float fRTranslateFactor = 40.0f; // As like infinete
float fA2TranslateFactor = 35.0f; // As like infinete
float fTTranslateFactor = 30.0f; // As like infinete

BOOL bJetEnabled = FALSE;
BOOL bJetSmokeEnabled = TRUE;

void display(void)
{
	void DynamicBharat();
	void Background();
	void BackGroundSphere();

	//Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Now here screen gets colored as color mentioned in initialize()

	glMatrixMode(GL_MODELVIEW); //

	glLoadIdentity(); // Set Matrix to identity

	//gluLookAt(fCamx, fCamy, fCamz - fBaseDepth, -fCamx, -fCamy, -fCamz + fBaseDepth, 0.0f, 1.0f, 0.0f);
	gluLookAt(fCamx, fCamy, fCamz - fBaseDepth, 0.0, 0.0,  fBaseDepth, 0.0f, 1.0f, 0.0f);
	
	glTranslatef(0.0f, 0.0f, fBaseDepth);

	glPushMatrix();
	DynamicBharat();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glPopMatrix();

	if(bJetEnabled)
	{
		glPushMatrix();
		glScalef(0.35f, 0.2f, 0.15f);
		glTranslatef(0.0f, 0.0f, 10.0f);
		FlyingJet();
		glPopMatrix();
	}

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	SwapBuffers(ghdc);
}

void DynamicBharat()
{
	glTranslatef(-fLetterHalfWidth * 2.5f * 3.0f, 0.0f, 0.0f);

	if (bBletterDisplayed)
	{
		glTranslatef(fBTranslateFactor, 0.0f, 0.0f);
		DrawB();
	}
	if (bHletterDisplayed)
	{
		glTranslatef(fHTranslateFactor + (fLetterHalfWidth * 3.0f), -fHTranslateFactor, 0.0f);
		DrawH();	
	}

	if (bA1letterDisplayed)
	{
		glTranslatef(fA1TranslateFactor + (fLetterHalfWidth * 3.0f), +fA1TranslateFactor, 0.0f);
		DrawA(bA1letterTirangaDisplayed);
		//bA1letterDisplayed = TRUE;
	}
	if (bRletterDisplayed)
	{
		glTranslatef(fRTranslateFactor + (fLetterHalfWidth * 3.0f), + fRTranslateFactor, 0.0f);
		DrawR();
		//bRletterDisplayed = TRUE;
	}
	if (bA2letterDisplayed)
	{
		glTranslatef(fA2TranslateFactor + (fLetterHalfWidth * 3.0f), -fA2TranslateFactor, 0.0f);
		DrawA(bA2letterTirangaDisplayed);
		//bA2letterDisplayed = TRUE;
	}
	if (bTletterDisplayed)
	{
		glTranslatef(fTTranslateFactor + (fLetterHalfWidth * 3.0f), 0.0f, 0.0f);
		DrawT();
		//bTletterDisplayed = TRUE;
	}
}

void DrawPartialCircle(float fXCenterPos, float fYCenterPos, float fRadius, int iStartingAngle, int iEndAngle, float* fColorRgbV, float* fColorRgbV2, int ColrFragmentFactor)
{
	float fFirstAng = 0.0f;
	float fSecondAng = 0.0f;

	glPointSize(2.0f);
	
	if (iStartingAngle > iEndAngle)
	{
		iStartingAngle = -1 * (360 - iStartingAngle);

 	}
	float fColorGradient = 0.0f;
	float j = 1;
	for (int i = iStartingAngle; i < iEndAngle; i++) //(2.0f * PI) / firstPos
	{
		float angle = i * 3.142 / 180;
		float fXCord = fXCenterPos + (2.0 * fRadius * cos(angle));
		float fYCord = fYCenterPos + (fRadius * sin(angle));
		glBegin(GL_QUADS);

		fColorGradient = fColorGradient + 0.0055f;
		float fColor[4];
		fColor[0] = (fColorRgbV[0]* (1.0f - fColorGradient) )+ (fColorRgbV2[0] * fColorGradient);
		fColor[1] = (fColorRgbV[1] * (1.0f - fColorGradient)) + (fColorRgbV2[1] * fColorGradient);
		fColor[2] = (fColorRgbV[2] * (1.0f - fColorGradient)) + (fColorRgbV2[2] * fColorGradient);

		//glColor4f(fColor[0], fColor[1], fColor[2], 1.0f);
		glColor4fv(LetterColor);
		//glVertex3f(fXCord, fYCord, 0.0f);
		glVertex3f(fXCord, fYCord- fLetterStickHalfWidth, 0.0f);
		glVertex3f(fXCord - fLetterStickHalfWidth, fYCord- fLetterStickHalfWidth, 0.0f);
		glVertex3f(fXCord - fLetterStickHalfWidth, fYCord + fLetterStickHalfWidth, 0.0f);
		glVertex3f(fXCord, fYCord + fLetterStickHalfWidth, 0.0f);
		glEnd();
	}

	
}

void DrawPartialTirangaCircle(float fXCenterPos, float fYCenterPos, float fRadius, int iStartingAngle, int iEndAngle, float* fColorRgbV, float* fColorRgbV2, int ColrFragmentFactor)
{
	float fFirstAng = 0.0f;
	float fSecondAng = 0.0f;

	glPointSize(2.0f);
	
	if (iStartingAngle > iEndAngle)
	{
		iStartingAngle = -1 * (360 - iStartingAngle);

 	}
	float fColorGradient = 0.0f;
	float j = 1;
	for (int i = iStartingAngle; i <= iEndAngle; i++) //(2.0f * PI) / firstPos
	{
		float angle = i * 3.142 / 180;
		float fXCord = fXCenterPos + (2.0 * fRadius * cos(angle));
		float fYCord = fYCenterPos + (fRadius * sin(angle));
		glBegin(GL_QUADS);

		fColorGradient = fColorGradient + 0.055f; //0.0055f;
		float fColor[4];
		fColor[0] = (fColorRgbV[0]* (1.0f - fColorGradient) )+ (fColorRgbV2[0] * fColorGradient);
		fColor[1] = (fColorRgbV[1] * (1.0f - fColorGradient)) + (fColorRgbV2[1] * fColorGradient);
		fColor[2] = (fColorRgbV[2] * (1.0f - fColorGradient)) + (fColorRgbV2[2] * fColorGradient);

		//glColor4f(fColor[0], fColor[1], fColor[2], 1.0f);
		//glColor4fv(LetterColor);
		//glVertex3f(fXCord, fYCord, 0.0f);
		glColor4fv(fColorRgbV);
		glVertex3f(fXCord, fYCord- fLetterStickHalfWidth, 0.0f);
		glVertex3f(fXCord - fLetterStickHalfWidth, fYCord- fLetterStickHalfWidth, 0.0f);
		glColor4fv(fColorRgbV2);
		glVertex3f(fXCord - fLetterStickHalfWidth, fYCord + fLetterStickHalfWidth, 0.0f);
		glVertex3f(fXCord, fYCord + fLetterStickHalfWidth, 0.0f);
		glEnd();
	}

	
}

void update(void)
{
	//Code
	cAngle = cAngle - 0.1f;
	if (cAngle <= 0.0f)
		cAngle = cAngle + 360.0f;

	// Letters Animation
	bBletterDisplayed = TRUE;
	if (fBTranslateFactor < 0.0f)
		fBTranslateFactor = fBTranslateFactor + 0.005f;
	else
	{
		bHletterDisplayed = TRUE;
		//bJetEnabled = TRUE;
	}

	if (bHletterDisplayed)
	{
		if (fHTranslateFactor < 0.0f)
			fHTranslateFactor = fHTranslateFactor + 0.005f;
		else
			bA1letterDisplayed = TRUE;
	}

	if (bA1letterDisplayed)
	{
		if (fA1TranslateFactor < 0.0f)
			fA1TranslateFactor = fA1TranslateFactor + 0.005f;
		else
			bRletterDisplayed = TRUE;
	}

	if (bRletterDisplayed)
	{
		if (fRTranslateFactor > 0.0f)
			fRTranslateFactor = fRTranslateFactor - 0.005f;
		else
			bA2letterDisplayed = TRUE;
	}

	if (bA2letterDisplayed)
	{
		if (fA2TranslateFactor > 0.0f)
			fA2TranslateFactor = fA2TranslateFactor - 0.005f;
		else
			bTletterDisplayed = TRUE;
	}

	if (bTletterDisplayed)
	{
		if (fTTranslateFactor > 0.0f)
			fTTranslateFactor = fTTranslateFactor - 0.005f;
		else
			bJetEnabled = TRUE;
	}

	if(bJetEnabled)
	{
		static BOOL bFirstSlopeComplete = FALSE;	
		static float angle = 180.0f; //i * 3.142 / 180;
		if((angle <= 265.0f) /*&& (fJetTranslateYFactor > 0.0f)*/ && (!bFirstSlopeComplete))
		{
			angle = angle + 0.05f;
			fJetTranslateXFactor = fJetTranslateXFactorBegin - ( (fJetTranslateXFactorBegin + 4.5f) * cos(angle * 3.142 / 180));
			fJetTranslateYFactor =  fJetTranslateYFactorBegin + ( fJetTranslateYFactorBegin * sin(angle * 3.142 / 180));	
			fCurveAngle = fCurveAngle - 0.05f;
		}
		else
		{
			bFirstSlopeComplete = TRUE;	 		
		}

		if(bFirstSlopeComplete && (fJetTranslateXFactor < -fJetTranslateXFactorBegin) )
		{
			fJetTranslateXFactor = fJetTranslateXFactor + 0.05f;
		}

		if(fJetTranslateXFactor >=  -10.0f + (fLetterHalfWidth * 3.0f * 2.0f) -10.0f ) // -15f is offset to bring nose on the curve
		{
			bBletterTirangaDisplayed = TRUE;
		}
		if(fJetTranslateXFactor >=  -10.0f + (fLetterHalfWidth * 3.0f * 3.0f) -10.0f) // -15f is offset to bring nose on the curve
		{
			bHletterTirangaDisplayed = TRUE;
		}
		if(fJetTranslateXFactor >=  -10.0f + (fLetterHalfWidth * 3.0f * 4.0f) -10.0f) // -15f is offset to bring nose on the curve
		{
			bA1letterTirangaDisplayed = TRUE;
		}			
		if(fJetTranslateXFactor >=  -10.0f + (fLetterHalfWidth * 3.0f * 6.0f) + 10.0f) // -15f is offset to bring nose on the curve
		{
			bRletterTirangaDisplayed = TRUE;
		}
		if(fJetTranslateXFactor >=  -10.0f + (fLetterHalfWidth * 3.0f * 7.0f) + 10.0f) // -15f is offset to bring nose on the curve
		{
			bA2letterTirangaDisplayed = TRUE;
		}
		if(fJetTranslateXFactor >=  -10.0f + (fLetterHalfWidth * 3.0f * 8.0f) + 11.0f)// -15f is offset to bring nose on the curve 
		{
			bTletterTirangaDisplayed = TRUE;			
			static float angle2 = 275.0f;
			if(angle2 <= 360.0f  && bFirstSlopeComplete)
			{
				angle2 = angle2 + 0.05f;
				fJetTranslateXFactor = -fJetTranslateXFactorBegin - ( (fJetTranslateXFactorBegin - 10.5f ) * cos(angle2 * 3.142 / 180));
				fJetTranslateYFactor =  fJetTranslateYFactorBegin + ( (fJetTranslateYFactorBegin ) * sin(angle2 * 3.142 / 180));
				fCurveAngle = fCurveAngle - 0.05f;
			}
			else
			{
				fJetTranslateXFactor = fJetTranslateXFactor + 0.05f;
				fJetTranslateYFactor = fJetTranslateYFactor + 0.05f;
			}
		}				
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

	//Close the log file
	if (gpFile) {
		fprintf(gpFile, "Program finished successfully.\n");
		fclose(gpFile);
		gpFile = NULL;
	}
}
