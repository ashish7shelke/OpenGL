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

	//Get Window size
	GLint iWidth;
	GLint iHeight;

	//LPRECT rc1;
	//GetClientRect(ghwnd, &rc1);

	if (gbFullScreen)
	{
		iWidth = GetSystemMetrics(SM_CXSCREEN);
		iHeight = GetSystemMetrics(SM_CYSCREEN);	//GetDeviceCaps
	}
	else
	{
		//iWidth = rc1->right - rc1->left;
		//iHeight = rc1->bottom - rc1->top;
		iWidth = WIN_WIDTH;
		iHeight = WIN_HEIGHT;
	}

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
		case 48: //0
		case 0x60: // Numpad 0
			glViewport(0, 0, (GLsizei)iWidth, (GLsizei)iHeight);
			break;

		case 49: //1
		case 0x61 : // Numpad 1
			glViewport(0, 0, (GLsizei)(iWidth /2), (GLsizei)(iHeight /2));
			break;

		case 50: //2
		case 0x62 : // Numpad 2
			glViewport((GLsizei)(iWidth / 2), 0, (GLsizei)(iWidth / 2), (GLsizei)(iHeight / 2));
			break;

		case 51: //3
		case 0x63 : // Numpad 3
			glViewport((GLsizei)(iWidth / 2), (GLsizei)(iHeight / 2), (GLsizei)(iWidth / 2), (GLsizei)(iHeight / 2));
			break;

		case 52: //4
		case 0x64 : // Numpad 4
			glViewport(0, (GLsizei)(iHeight / 2), (GLsizei)(iWidth / 2), (GLsizei)(iHeight / 2));
			break;

		case 53: //5
		case 0x65 : // Numpad 5
			glViewport(0, 0, (GLsizei)iWidth, (GLsizei)(iHeight / 2));
			break;

		case 54: //6
		case 0x66 : // Numpad 6
			glViewport(0, (GLsizei)(iHeight / 2), (GLsizei)iWidth, (GLsizei)(iHeight /2));
			break;

		case 55: //7
		case 0x67 : // Numpad 7
			glViewport(0, 0, (GLsizei)(iWidth /2), (GLsizei)iHeight);
			break;
			
		case 56: //8
		case 0x68 : // Numpad 8
			glViewport((GLsizei)(iWidth / 2), 0, (GLsizei)(iWidth /2), (GLsizei)iHeight);
			break;

		case 57: //9
		case 0x69 : // Numpad 9
			glViewport((GLsizei)(iWidth / 4), (GLsizei)(iHeight / 4), (GLsizei)(iWidth / 2), (GLsizei)(iHeight / 2));
			break;

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
	glClear(GL_COLOR_BUFFER_BIT); //Now here screen gets colored as color mentioned in initialize()

	glMatrixMode(GL_MODELVIEW); //

	glLoadIdentity(); // Set Matrix to identity

	// Matrix STACK concept

	// 1. Following call is done for modelling transformation
	// glTranslatef(0.0f, 0.0f, -3.0f);

	// 2. Following call to be is done when viewing transformation
	// gluLookAt(0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// 3. Following lines are used when both modelling transformationa and viewing transformation to be done first
	gluLookAt(0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	glTranslatef(0.0f, 0.0f, -2.0f);

	glBegin(GL_TRIANGLES);

	glVertex3f(0.0f, 1.0f, 0.0f);

	glVertex3f(-1.0f, -1.0f, 0.0f);

	glVertex3f(1.0f, -1.0f, 0.0f);

	glEnd();	

	SwapBuffers(ghdc);
}

void update(void)
{
	//Code
	//When Annimation
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