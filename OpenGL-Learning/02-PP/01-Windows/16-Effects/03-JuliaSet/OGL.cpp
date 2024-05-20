//Windows Header Files
#include <windows.h>
#include <stdio.h> //For File IO
#include <stdlib.h> //For exit()
#include <wingdi.h>
#include <chrono>
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
	AGS_ATTRIBUTE_TEXCORD
};
GLuint vao = 0;
GLuint vbo_position = 0;
GLuint vbo_texture = 0;

GLuint mvpMatrixUniform = 0;
GLuint textureSamplerUniform = 0;
GLuint juliaCUniform = 0;
GLuint maxIterationUniform = 0;
GLuint offsetUniform = 0;
GLuint juliaZoomUniform = 0;

mat4 perspectiveProjectionMatrix; //4 by 4 matrix,mat4 is in vmath.h

float juliC[] = {0.0f, 0.0f};
float juliOffset[] = {0.0f, 0.0f};
float juliaZoom = 1.0f;
float timeOffset = 0.0f;
BOOL paused = FALSE;
//Texture ObjectuTexGradient
GLuint texture_julia = 0;
double gTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();


static const unsigned char palette[] =
{
    0xFF, 0x00, 0x00, 0xFF, 0x0E, 0x03, 0xFF, 0x1C,
    0x07, 0xFF, 0x2A, 0x0A, 0xFF, 0x38, 0x0E, 0xFF,
    0x46, 0x12, 0xFF, 0x54, 0x15, 0xFF, 0x62, 0x19,
    0xFF, 0x71, 0x1D, 0xFF, 0x7F, 0x20, 0xFF, 0x88,
    0x22, 0xFF, 0x92, 0x25, 0xFF, 0x9C, 0x27, 0xFF,
    0xA6, 0x2A, 0xFF, 0xB0, 0x2C, 0xFF, 0xBA, 0x2F,
    0xFF, 0xC4, 0x31, 0xFF, 0xCE, 0x34, 0xFF, 0xD8,
    0x36, 0xFF, 0xE2, 0x39, 0xFF, 0xEC, 0x3B, 0xFF,
    0xF6, 0x3E, 0xFF, 0xFF, 0x40, 0xF8, 0xFE, 0x40,
    0xF1, 0xFE, 0x40, 0xEA, 0xFE, 0x41, 0xE3, 0xFD,
    0x41, 0xDC, 0xFD, 0x41, 0xD6, 0xFD, 0x42, 0xCF,
    0xFC, 0x42, 0xC8, 0xFC, 0x42, 0xC1, 0xFC, 0x43,
    0xBA, 0xFB, 0x43, 0xB4, 0xFB, 0x43, 0xAD, 0xFB,
    0x44, 0xA6, 0xFA, 0x44, 0x9F, 0xFA, 0x45, 0x98,
    0xFA, 0x45, 0x92, 0xF9, 0x45, 0x8B, 0xF9, 0x46,
    0x84, 0xF9, 0x46, 0x7D, 0xF8, 0x46, 0x76, 0xF8,
    0x46, 0x6F, 0xF8, 0x47, 0x68, 0xF8, 0x47, 0x61,
    0xF7, 0x47, 0x5A, 0xF7, 0x48, 0x53, 0xF7, 0x48,
    0x4C, 0xF6, 0x48, 0x45, 0xF6, 0x49, 0x3E, 0xF6,
    0x49, 0x37, 0xF6, 0x4A, 0x30, 0xF5, 0x4A, 0x29,
    0xF5, 0x4A, 0x22, 0xF5, 0x4B, 0x1B, 0xF5, 0x4B,
    0x14, 0xF4, 0x4B, 0x0D, 0xF4, 0x4C, 0x06, 0xF4,
    0x4D, 0x04, 0xF1, 0x51, 0x0D, 0xE9, 0x55, 0x16,
    0xE1, 0x59, 0x1F, 0xD9, 0x5D, 0x28, 0xD1, 0x61,
    0x31, 0xC9, 0x65, 0x3A, 0xC1, 0x69, 0x42, 0xB9,
    0x6D, 0x4B, 0xB1, 0x71, 0x54, 0xA9, 0x75, 0x5D,
    0xA1, 0x79, 0x66, 0x99, 0x7D, 0x6F, 0x91, 0x81,
    0x78, 0x89, 0x86, 0x80, 0x81, 0x8A, 0x88, 0x7A,
    0x8E, 0x90, 0x72, 0x92, 0x98, 0x6A, 0x96, 0xA1,
    0x62, 0x9A, 0xA9, 0x5A, 0x9E, 0xB1, 0x52, 0xA2,
    0xBA, 0x4A, 0xA6, 0xC2, 0x42, 0xAA, 0xCA, 0x3A,
    0xAE, 0xD3, 0x32, 0xB2, 0xDB, 0x2A, 0xB6, 0xE3,
    0x22, 0xBA, 0xEB, 0x1A, 0xBE, 0xF4, 0x12, 0xC2,
    0xFC, 0x0A, 0xC6, 0xF5, 0x02, 0xCA, 0xE6, 0x09,
    0xCE, 0xD8, 0x18, 0xD1, 0xCA, 0x27, 0xD5, 0xBB,
    0x36, 0xD8, 0xAD, 0x45, 0xDC, 0x9E, 0x54, 0xE0,
    0x90, 0x62, 0xE3, 0x82, 0x6F, 0xE6, 0x71, 0x7C,
    0xEA, 0x61, 0x89, 0xEE, 0x51, 0x96, 0xF2, 0x40,
    0xA3, 0xF5, 0x30, 0xB0, 0xF9, 0x20, 0xBD, 0xFD,
    0x0F, 0xE3, 0xFF, 0x01, 0xE9, 0xFF, 0x01, 0xEE,
    0xFF, 0x01, 0xF4, 0xFF, 0x00, 0xFA, 0xFF, 0x00,
    0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x0A, 0xFF, 0xFF,
    0x15, 0xFF, 0xFF, 0x20, 0xFF, 0xFF, 0x2B, 0xFF,
    0xFF, 0x36, 0xFF, 0xFF, 0x41, 0xFF, 0xFF, 0x4C,
    0xFF, 0xFF, 0x57, 0xFF, 0xFF, 0x62, 0xFF, 0xFF,
    0x6D, 0xFF, 0xFF, 0x78, 0xFF, 0xFF, 0x81, 0xFF,
    0xFF, 0x8A, 0xFF, 0xFF, 0x92, 0xFF, 0xFF, 0x9A,
    0xFF, 0xFF, 0xA3, 0xFF, 0xFF, 0xAB, 0xFF, 0xFF,
    0xB4, 0xFF, 0xFF, 0xBC, 0xFF, 0xFF, 0xC4, 0xFF,
    0xFF, 0xCD, 0xFF, 0xFF, 0xD5, 0xFF, 0xFF, 0xDD,
    0xFF, 0xFF, 0xE6, 0xFF, 0xFF, 0xEE, 0xFF, 0xFF,
    0xF7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF9,
    0xFF, 0xFF, 0xF3, 0xFF, 0xFF, 0xED, 0xFF, 0xFF,
    0xE7, 0xFF, 0xFF, 0xE1, 0xFF, 0xFF, 0xDB, 0xFF,
    0xFF, 0xD5, 0xFF, 0xFF, 0xCF, 0xFF, 0xFF, 0xCA,
    0xFF, 0xFF, 0xC4, 0xFF, 0xFF, 0xBE, 0xFF, 0xFF,
    0xB8, 0xFF, 0xFF, 0xB2, 0xFF, 0xFF, 0xAC, 0xFF,
    0xFF, 0xA6, 0xFF, 0xFF, 0xA0, 0xFF, 0xFF, 0x9B,
    0xFF, 0xFF, 0x96, 0xFF, 0xFF, 0x90, 0xFF, 0xFF,
    0x8B, 0xFF, 0xFF, 0x86, 0xFF, 0xFF, 0x81, 0xFF,
    0xFF, 0x7B, 0xFF, 0xFF, 0x76, 0xFF, 0xFF, 0x71,
    0xFF, 0xFF, 0x6B, 0xFF, 0xFF, 0x66, 0xFF, 0xFF,
    0x61, 0xFF, 0xFF, 0x5C, 0xFF, 0xFF, 0x56, 0xFF,
    0xFF, 0x51, 0xFF, 0xFF, 0x4C, 0xFF, 0xFF, 0x47,
    0xFF, 0xFF, 0x41, 0xF9, 0xFF, 0x40, 0xF0, 0xFF,
    0x40, 0xE8, 0xFF, 0x40, 0xDF, 0xFF, 0x40, 0xD7,
    0xFF, 0x40, 0xCF, 0xFF, 0x40, 0xC6, 0xFF, 0x40,
    0xBE, 0xFF, 0x40, 0xB5, 0xFF, 0x40, 0xAD, 0xFF,
    0x40, 0xA4, 0xFF, 0x40, 0x9C, 0xFF, 0x40, 0x95,
    0xFF, 0x40, 0x8D, 0xFF, 0x40, 0x86, 0xFF, 0x40,
    0x7E, 0xFF, 0x40, 0x77, 0xFF, 0x40, 0x6F, 0xFF,
    0x40, 0x68, 0xFF, 0x40, 0x60, 0xFF, 0x40, 0x59,
    0xFF, 0x40, 0x51, 0xFF, 0x40, 0x4A, 0xFA, 0x43,
    0x42, 0xF3, 0x48, 0x3E, 0xED, 0x4E, 0x3D, 0xE6,
    0x53, 0x3B, 0xDF, 0x58, 0x39, 0xD8, 0x5E, 0x37,
    0xD2, 0x63, 0x35, 0xCB, 0x68, 0x34, 0xC4, 0x6D,
    0x32, 0xBD, 0x73, 0x30, 0xB7, 0x78, 0x2E, 0xB0,
    0x7D, 0x2D, 0xA9, 0x83, 0x2B, 0xA2, 0x88, 0x29,
    0x9C, 0x8D, 0x27, 0x95, 0x92, 0x25, 0x8E, 0x98,
    0x24, 0x87, 0x9D, 0x22, 0x81, 0xA2, 0x20, 0x7A,
    0xA6, 0x1E, 0x74, 0xAB, 0x1D, 0x6E, 0xB0, 0x1B,
    0x68, 0xB5, 0x1A, 0x61, 0xB9, 0x18, 0x5B, 0xBE,
    0x17, 0x55, 0xC3, 0x15, 0x4F, 0xC8, 0x13, 0x48,
    0xCD, 0x12, 0x42, 0xD1, 0x10, 0x3C, 0xD6, 0x0F,
    0x36, 0xDB, 0x0D, 0x2F, 0xE0, 0x0C, 0x29, 0xE4,
    0x0A, 0x23, 0xE9, 0x08, 0x1D, 0xEE, 0x07, 0x16,
    0xF3, 0x05, 0x10, 0xF7, 0x04, 0x0A, 0xFC, 0x02,
    0x04, 0xFB, 0x01, 0x04, 0xEF, 0x00, 0x12, 0xE4,
    0x00, 0x1F, 0xD9, 0x00, 0x2D, 0xCE, 0x00, 0x3B,
    0xC2, 0x00, 0x48, 0xB7, 0x00, 0x56, 0xAC, 0x00,
    0x64, 0xA0, 0x00, 0x72, 0x95, 0x00, 0x7F, 0x8A,
    0x00, 0x88, 0x7F, 0x00, 0x92, 0x75, 0x00, 0x9C,
    0x6B, 0x00, 0xA6, 0x61, 0x00, 0xB0, 0x57, 0x00,
    0xBA, 0x4E, 0x00, 0xC4, 0x44, 0x00, 0xCE, 0x3A,
    0x00, 0xD8, 0x30, 0x00, 0xE2, 0x27, 0x00, 0xEC,
    0x1D, 0x00, 0xF6, 0x13, 0x00, 0xFF, 0x09, 0x00,
};

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
		case 'P':
		case 'p':
			paused = !paused;
			break;
		case '8': timeOffset += 0.00001f;
			break;
		case '7': timeOffset -= 0.00001f;
			break;
		case '6': timeOffset += 0.0001f;
			break;
		case '5': timeOffset -= 0.0001f;
			break;
		case '4': timeOffset += 0.01f;
			break;
		case '3': timeOffset -= 0.01f;
			break;
		case '2': timeOffset += 1.0f;
			break;
		case '1': timeOffset -= 1.0f;
			break;
		case 'Z': juliaZoom *= 1.02f;
			break;
		case 'z': juliaZoom /= 1.02f;
			break;
		case 'W':
		case 'w': juliOffset[1] -= juliaZoom * 0.2f;
			break;
		case 'A':
		case 'a': juliOffset[0] -= juliaZoom * 0.2f;
			break;
		case 'S':	
		case 's': juliOffset[1] += juliaZoom * 0.2f;
			break;
		case 'D':	
		case 'd': juliOffset[0] += juliaZoom * 0.2f;
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
	BOOL loadGlTexture(GLuint* , TCHAR[]);
	void loadGl1DTexture(GLuint texture);

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

	//Initialize GLEW 
	if(glewInit() != GLEW_OK)
	{
		fprintf(gpFile, "glewInit() failed ...\n");
		return -6;		
	}

	//printGLInfo();
	//PP related code

	//Vertex Shader
	const GLchar* vertexShaderSourceCode = 
				"#version 460 core" \
				"\n" \
				"in vec4 aPosition;\n" \
				"out vec2 oInitialZ;\n" \
				"uniform mat4 umvpMatrix;\n" \
				"uniform vec2 uOffset;\n" \
				"uniform float ujuliaZoom;\n" \
				"void main(void)\n" \
				"{\n" \
				"//oInitialZ = (aPosition.xy * ujuliaZoom) + uOffset;\n" \
				"//gl_Position = aPosition;\n" \
				"    const vec4 vertices[4] = vec4[4](vec4(-1.0, -1.0, 0.5, 1.0),                       \n" \
				"                                     vec4( 1.0, -1.0, 0.5, 1.0),                       \n" \
				"                                     vec4( 1.0,  1.0, 0.5, 1.0),                       \n" \
				"                                     vec4(-1.0,  1.0, 0.5, 1.0));                      \n" \
				"    oInitialZ = (vertices[gl_VertexID].xy * ujuliaZoom) + uOffset;                     \n" \
				"    gl_Position = vertices[gl_VertexID];                                               \n" \
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
					"in vec2 oInitialZ;\n" \
					"uniform vec2 uJuliaC;\n" \
					"uniform sampler1D uTexGradient;" \
					"uniform int uMaxIterations;" \
					"out vec4 FragColor;\n" \
					"void main(void)\n" \
					"{\n" \
					"int iterations = 0;" \
					"vec2 z = oInitialZ;" \
					"const float threshold_squared = 64.0f;" \
					"while(iterations < uMaxIterations &&  (dot(z, z) < threshold_squared))" \
					"{" \
					"vec2 z_squared;" \
					"z_squared.x = (z.x * z.x) - (z.y * z.y);" \
					"z_squared.y = 2.0f * (z.x * z.y);" \
					"z = z_squared + uJuliaC;" \
					"iterations++;" \
					"}" \
					"if(iterations == uMaxIterations)" \
					"{\n" \
					"FragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f); \n" \
					"}" \
					"else" \
					"{" \
					"FragColor = texture(uTexGradient, float(iterations)/float(uMaxIterations)); \n" \
					"}" \
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
	//glBindAttribLocation(shaderProgramObject, AGS_ATTRIBUTE_TEXCORD, "aTexCoord");
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
	//textureSamplerUniform = glGetUniformLocation(shaderProgramObject, "uTexGradient");
	juliaCUniform = glGetUniformLocation(shaderProgramObject, "uJuliaC");
	maxIterationUniform = glGetUniformLocation(shaderProgramObject, "uMaxIterations");
	offsetUniform = glGetUniformLocation(shaderProgramObject, "uOffset");
	juliaZoomUniform = glGetUniformLocation(shaderProgramObject, "ujuliaZoom");

	const GLfloat rectangle_position[] = {
						1.0f, 1.0f, 0.0f,
						-1.0f, 1.0f, 0.0f,
						-1.0f, -1.0f, 0.0f,
						1.0f, -1.0f, 0.0f
						};

	const GLfloat rectangle_texcoord[] =
	{
		1.0f,1.0f,
		0.0f,1.0f,
		0.0f,0.0f,
		1.0f,0.0f
	};

	//VAO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//VBO for postions		
	glGenBuffers(1, &vbo_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle_position), rectangle_position, GL_STATIC_DRAW);
	glVertexAttribPointer(AGS_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AGS_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* //VBO for texture		
	glGenBuffers(1, &vbo_texture);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_texture);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle_texcoord), rectangle_texcoord, GL_STATIC_DRAW);
	glVertexAttribPointer(AGS_ATTRIBUTE_TEXCORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AGS_ATTRIBUTE_TEXCORD);
	glBindBuffer(GL_ARRAY_BUFFER, 0); */
	
	glGenTextures(1, &texture_julia);
	glBindTexture(GL_TEXTURE_1D, texture_julia);
	glTexStorage1D(GL_TEXTURE_1D, 8, GL_RGB8, 256);
	glTexSubImage1D(GL_TEXTURE_1D, 0, 0, 256, GL_RGB, GL_UNSIGNED_BYTE, palette);
	glGenerateMipmap(GL_TEXTURE_1D);

	//Unbind with VAO
	glBindVertexArray(0);

	//Enabling Depth
	glClearDepth(1.0f); // Compulsory - Default depth
	glEnable(GL_DEPTH_TEST); // Compulsory - Enable Depth Test
	glDepthFunc(GL_LEQUAL); // Compulsary - Depth is less than or equal to 

	glEnable(GL_TEXTURE_1D);

	//Set Clear Color Of Window to Blue -- Its just informing but not yet colored
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// initialize orthographicProjectionMatrix
	perspectiveProjectionMatrix = vmath::mat4::identity();
	resize(WIN_WIDTH, WIN_HEIGHT);  

	//Here OPEN GL Starts
	return 0;
}

void loadGl1DTexture(GLuint texture)
{
	    glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_1D, texture);
        glTexStorage1D(GL_TEXTURE_1D, 8, GL_RGB8, 256);
        glTexSubImage1D(GL_TEXTURE_1D, 0, 0, 256, GL_RGB, GL_UNSIGNED_BYTE, palette);
        glGenerateMipmap(GL_TEXTURE_1D);
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
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //4 - R G B A good for allignment // 1 is good for performance

	//Step 10 - Set Texture Parameter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	//Step 11 - Crate Multiple Bitmap Images
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bmp.bmWidth, bmp.bmHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, (void*)bmp.bmBits);
	glGenerateMipmap(GL_TEXTURE_2D);
	
	//Step 12 - Unbind from binding point
	glBindTexture(GL_TEXTURE_2D, 0);

	//Step 13 - Delete Image Resource
	DeleteObject(hBitmap);
	hBitmap = NULL;
 
	return TRUE;
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
	static float timer = 0.0f;
	double currentMilliSeconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	float t = (float)((currentMilliSeconds - gTime)/1000.0);
	//fprintf(gpFile, "t = %f, currentMilliSeconds = %f \n", t, currentMilliSeconds);
    //t = t * 0.1f;
	//t = t - floor(t);
	//Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Now here screen gets colored as color mentioned in initialize()

	glUseProgram(shaderProgramObject);

	// Transformation
	mat4 modelViewMatrix = vmath::translate(0.0f, 0.0f, -3.0f);
	mat4 modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix; // order of multiplication is very imp

	float r = 0.0f;
	r = t + timeOffset;
	juliC[0] =  (sinf(r * 0.1f) + cosf(r * 0.23f)) * 0.5f;
	juliC[1] =  (cosf(r * 0.13f) + sinf(r * 0.21f)) * 0.5f;
	fprintf(gpFile, "juliC[0] = %f, juliC[1] = %f \n", juliC[0], juliC[1]);

	// push above mvp into vertex shader's mvpuniform
	glUniformMatrix4fv(mvpMatrixUniform, 1,GL_FALSE, modelViewProjectionMatrix);
	glUniformMatrix2fv(juliaCUniform, 1,GL_FALSE, juliC);
	glUniform2f(offsetUniform, juliOffset[0], juliOffset[1]);
	glUniform1f(juliaZoomUniform, juliaZoom);
	glUniform1i(maxIterationUniform, 256);

	//Call Texture
	glActiveTexture(GL_TEXTURE0); // Multiple textures can be given 0 is unit of texture
	glBindTexture(GL_TEXTURE_1D, texture_julia);
	//glUniform1i(textureSamplerUniform, 0);

	glBindVertexArray(vao);

	//glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glBindTexture(GL_TEXTURE_1D, 0);
	glBindVertexArray(0);

	glUseProgram(0);	

	SwapBuffers(ghdc);
}

void update(void)
{
	//Code
	//When Annimation

 	/*if(juliC[0] < 0.5f)
	{
	juliC[0] = juliC[1] + 0.0005f;
	juliC[1] = juliC[0] - 0.0005f ;
	}
	else
	{
		juliC[0] = 0.0005f;
		juliC[1] = 0.0005f ;
	}*/
	

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

	if(vbo_texture)
	{
		glDeleteBuffers(1, &vbo_texture);
		vbo_texture = 0;
	}

	if(vbo_position)
	{
		glDeleteBuffers(1, &vbo_position);
		vbo_position = 0;
	}

	if(vao)
	{
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}

	if (texture_julia)
	{
		glDeleteTextures(1, &texture_julia);
		texture_julia = 0;
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
