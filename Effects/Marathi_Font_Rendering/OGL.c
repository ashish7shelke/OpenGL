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
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) }; //All elements are set to value in  '{}' //memset() & ZeroMemory()
BOOL gbFullScreen = FALSE;

// OPENGL Related Global Variables
HDC ghdc = NULL;
HGLRC ghrc = NULL; // Handle Graphic Rendering Context

GLfloat pAngle = 0.0f;
GLfloat cAngle = 0.0f;

GLfloat fBaseDepth = -6.0f;

//Texture Object
GLuint texture_front = 0;
GLuint texture_back = 0;
GLuint texture_left = 0;
GLuint texture_right = 0;
GLuint texture_top = 0;
GLuint texture_bottom = 0;

GLfloat fCamx = 0.0f;
GLfloat fCamy = 0.0f;
GLfloat fCamz = -6.0f;
GLfloat fCamSpeed = 0.5f;

GLuint nFontList = 0;

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
			fprintf(gpFile, "fCamx = %f\n", fCamx);
			break;
		case VK_RIGHT: //27
			fCamx = fCamx + fCamSpeed;
			fprintf(gpFile, "fCamx = %f\n", fCamx);
			break;
		case VK_UP: //27
			fCamy = fCamy + fCamSpeed;
			fprintf(gpFile, "fCamy = %f\n", fCamy);
			break;
		case VK_DOWN: //27
			fCamy = fCamy - fCamSpeed;
			fprintf(gpFile, "fCamy = %f\n", fCamy);
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
			fprintf(gpFile, "fCamz = %f\n", fCamz);
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
	int setup3DTextRC(HDC);
	int setup2DTextRC(HDC);

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

	//Set Clear Color Of Window to Blue -- Its just informing but not yet colored
	glClearColor(0.5f, 0.5f, 1.0f, 1.0f);

	// Loading Images - Create Texture
	bResult = loadGlTexture(&texture_front, MAKEINTRESOURCE(CUBE_FRONT_BITMAP));
	if (bResult == FALSE)
	{
		fprintf(gpFile, "loadGlTexture(MY_KUNDALI_BITMAP) failed .../n");
		return -7;
	}
	bResult = loadGlTexture(&texture_back, MAKEINTRESOURCE(CUBE_BACK_BITMAP));
	if (bResult == FALSE)
	{
		fprintf(gpFile, "loadGlTexture(MY_KUNDALI_BITMAP) failed .../n");
		return -8;
	}
	bResult = loadGlTexture(&texture_left, MAKEINTRESOURCE(CUBE_LEFT_BITMAP));
	if (bResult == FALSE)
	{
		fprintf(gpFile, "loadGlTexture(MY_KUNDALI_BITMAP) failed .../n");
		return -9;
	}
	bResult = loadGlTexture(&texture_right, MAKEINTRESOURCE(CUBE_RIGHT_BITMAP));
	if (bResult == FALSE)
	{
		fprintf(gpFile, "loadGlTexture(MY_KUNDALI_BITMAP) failed .../n");
		return -10;
	}
	bResult = loadGlTexture(&texture_top, MAKEINTRESOURCE(CUBE_TOP_BITMAP));
	if (bResult == FALSE)
	{
		fprintf(gpFile, "loadGlTexture(MY_KUNDALI_BITMAP) failed ...\n");
		return -11;
	}
	bResult = loadGlTexture(&texture_bottom, MAKEINTRESOURCE(CUBE_BOTTOM_BITMAP));
	if (bResult == FALSE)
	{
		fprintf(gpFile, "loadGlTexture(MY_KUNDALI_BITMAP) failed ...\n");
		return -12;
	}	

	//Setting up 3D Text
	if (setup3DTextRC(ghdc) != 0)
	{
		fprintf(gpFile, "setup3DTextRC failed ...\n");
		return -13;
	}

	glEnable(GL_TEXTURE_2D);

	resize(WIN_WIDTH, WIN_HEIGHT);

	//Here OPEN GL Starts
	return 0;
}

int setup3DTextRC(HDC hDC)
{
	// Setup the font characteristics
	HFONT hFont;
	GLYPHMETRICSFLOAT agmf[128];
	LOGFONT logfont;

	logfont.lfHeight = -5.0f;
	logfont.lfWidth = 0.0f;
	logfont.lfEscapement = 0.0f;
	logfont.lfOrientation = 0.0f;
	logfont.lfWeight = FW_BOLD;
	logfont.lfItalic = FALSE;
	logfont.lfUnderline = FALSE;
	logfont.lfStrikeOut = FALSE;
	logfont.lfCharSet = ANSI_CHARSET;
	logfont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	logfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logfont.lfQuality = DEFAULT_QUALITY;
	logfont.lfPitchAndFamily = DEFAULT_PITCH;

	//strcpy(logfont.lfFaceName, "Kruti Dev 010");
	strcpy(logfont.lfFaceName, "Shivaji01");


	// Create the font and display list
	hFont = CreateFontIndirect(&logfont);
	SelectObject(hDC, hFont);

	// Create display lists for glyphs 0 through 128 with 0.1 extrusion and default deviation
	// Display list numbering starts at 1000 (It could be any number)
	nFontList = glGenLists(128);

	if((GL_INVALID_OPERATION == nFontList) || (GL_INVALID_VALUE == nFontList))
		return -1;

	wglUseFontOutlines(hDC, 0, 128, nFontList, 0.0f, 0.05f, WGL_FONT_POLYGONS, agmf);

	DeleteObject(hFont);

	return 0;
}

int setup2DTextRC(HDC hDC)
{
	// Setup the font characteristics
	HFONT hFont;
	//GLYPHMETRICSFLOAT agmf[128];
	LOGFONT logfont;

	logfont.lfHeight = -10.0f;
	logfont.lfWidth = 0.0f;
	logfont.lfEscapement = 0.0f;
	logfont.lfOrientation = 0.0f;
	logfont.lfWeight = FW_BOLD;
	logfont.lfItalic = FALSE;
	logfont.lfUnderline = FALSE;
	logfont.lfStrikeOut = FALSE;
	logfont.lfCharSet = ANSI_CHARSET;
	logfont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	logfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logfont.lfQuality = DEFAULT_QUALITY;
	logfont.lfPitchAndFamily = DEFAULT_PITCH;
	strcpy(logfont.lfFaceName, "Shivaji01");

	// Create the font and display list
	hFont = CreateFontIndirect(&logfont);
	SelectObject(hDC, hFont);

	// Create display lists for glyphs 0 through 128 with 0.1 extrusion and default deviation
	// Display list numbering starts at 1000 (It could be any number)
	
	nFontList = glGenLists(128);

	if((GL_INVALID_OPERATION == nFontList) || (GL_INVALID_VALUE == nFontList))
		return -1;

	wglUseFontBitmaps(hDC, 0, 128, nFontList);

	DeleteObject(hFont);

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
	void DrawCube();
	void Pasaydan();

	//Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Now here screen gets colored as color mentioned in initialize()

	glMatrixMode(GL_MODELVIEW); //

	glLoadIdentity(); // Set Matrix to identity

	//gluLookAt(fCamx, fCamy, fCamz, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	glPushMatrix();
	// Matrix STACK concept
	glTranslatef(0.0f, 0.0f, fBaseDepth);
	//glScalef(0.75f, 0.75f, 0.75f);
	//glRotatef(cAngle, 1.0f, 0.0f, 0.0f);
	//glRotatef(cAngle, 0.0f, 1.0f, 0.0f);
	//glRotatef(cAngle, 0.0f, 0.0f, 1.0f);

	//DrawCube();

	Pasaydan();

	glPopMatrix();

	SwapBuffers(ghdc);
}

void DrawCube()
{
	float fTextureTileFraction0 = 0.0f;
	float fTextureTileFraction1 = 1.0;

	float fScaleFactorX = 1.0f;
	float fScaleFactorY = 1.0f;
	float fScaleFactorZ = 1.0f;

	float fV1[3] = {1.0f, 1.0f, 1.0f};
	float fV2[3] = {-1.0f, 1.0f, fScaleFactorZ * 1.0f};
	float fV3[3] = {-1.0f, -1.0f, fScaleFactorZ * 1.0f};
	float fV4[3] = {1.0f, -1.0f, 1.0f};

	float bV1[3] = {1.0f, 1.0f, -1.0f};
	float bV2[3] = {-1.0f, 1.0f, fScaleFactorZ * -1.0f};
	float bV3[3] = {-1.0f, -1.0f, fScaleFactorZ * -1.0f};
	float bV4[3] = {1.0f, -1.0f, -1.0f};

	glPushMatrix();

	//Front Face
	glBindTexture(GL_TEXTURE_2D, texture_front);
	glBegin(GL_QUADS);
	glTexCoord2f(fTextureTileFraction1, fTextureTileFraction1);
	glVertex3fv(fV1);
	glTexCoord2f(fTextureTileFraction0, fTextureTileFraction1);
	glVertex3fv(fV2);
	glTexCoord2f(fTextureTileFraction0, fTextureTileFraction0);
	glVertex3fv(fV3);
	glTexCoord2f(fTextureTileFraction1, fTextureTileFraction0);
	glVertex3fv(fV4);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);

	//Right Side Face
	glBindTexture(GL_TEXTURE_2D, texture_right);
	glBegin(GL_QUADS);
	glTexCoord2f(fTextureTileFraction1, fTextureTileFraction1);
	glVertex3fv(bV1);
	glTexCoord2f(fTextureTileFraction0, fTextureTileFraction1);
	glVertex3fv(fV1);
	glTexCoord2f(fTextureTileFraction0, fTextureTileFraction0);
	glVertex3fv(fV4);
	glTexCoord2f(fTextureTileFraction1, fTextureTileFraction0);
	glVertex3fv(bV4);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);

	//Back Face
	glBindTexture(GL_TEXTURE_2D, texture_back);
	glBegin(GL_QUADS);
	glTexCoord2f(fTextureTileFraction1, fTextureTileFraction1);
	glVertex3fv(bV1);
	glTexCoord2f(fTextureTileFraction0, fTextureTileFraction1);
	glVertex3fv(bV2);
	glTexCoord2f(fTextureTileFraction0, fTextureTileFraction0);
	glVertex3fv(bV3);
	glTexCoord2f(fTextureTileFraction1, fTextureTileFraction0);
	glVertex3fv(bV4);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);

	//Left Side Face
	glBindTexture(GL_TEXTURE_2D, texture_left);
	glBegin(GL_QUADS);
	glTexCoord2f(fTextureTileFraction1, fTextureTileFraction1);
	glVertex3fv(fV2);
	glTexCoord2f(fTextureTileFraction0, fTextureTileFraction1);
	glVertex3fv(bV2);
	glTexCoord2f(fTextureTileFraction0, fTextureTileFraction0);
	glVertex3fv(bV3);
	glTexCoord2f(fTextureTileFraction1, fTextureTileFraction0);
	glVertex3fv(fV3);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);

	//Top Side Face
	glBindTexture(GL_TEXTURE_2D, texture_top);
	glBegin(GL_QUADS);
	glTexCoord2f(fTextureTileFraction1, fTextureTileFraction1);
	glVertex3fv(bV1);
	glTexCoord2f(fTextureTileFraction0, fTextureTileFraction1);
	glVertex3fv(bV2);
	glTexCoord2f(fTextureTileFraction0, fTextureTileFraction0);
	glVertex3fv(fV2);
	glTexCoord2f(fTextureTileFraction1, fTextureTileFraction0);
	glVertex3fv(fV1);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);

	//Bottom Side Face
	glBindTexture(GL_TEXTURE_2D, texture_bottom);
	glBegin(GL_QUADS);
	glTexCoord2f(fTextureTileFraction1, fTextureTileFraction1);
	glVertex3fv(bV4);
	glTexCoord2f(fTextureTileFraction0, fTextureTileFraction1);
	glVertex3fv(bV3);
	glTexCoord2f(fTextureTileFraction0, fTextureTileFraction0);
	glVertex3fv(fV3);
	glTexCoord2f(fTextureTileFraction1, fTextureTileFraction0);
	glVertex3fv(fV4);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);

	glPopMatrix();
}

void Pasaydan()
{
	glPushMatrix();
	glTranslatef(-1.0f, 0.0f, 0.0f);

	glPushMatrix();	// Tital PASAYDAN
	glTranslatef(0.0f, 0.15f, 0.0f);
	glTranslatef(-0.0f, 1.75f, 0.0f);
	glScalef(0.5f, 0.5f, 0.5f);
	glListBase(nFontList);
	char Titel[] = "Pasaayadana";
	glCallLists(sizeof(Titel) - 1, GL_UNSIGNED_BYTE, Titel);
	glPopMatrix();

	glPushMatrix();	// Tital PASAYDAN
	glTranslatef(-1.5f, 1.5f, 0.0f);
	glScalef(0.25f, 0.25f, 0.5f);
	glListBase(nFontList);
	char cOvi1[] = "AataM ivaXvaa%makoM dovaoM । yaoNaoM vaagya&oM tÜYaavaoM । tÜYaÜina maja VavaoM । psaayadana hoM ॥ 1 ॥ ";
	glCallLists(sizeof(cOvi1) - 1, GL_UNSIGNED_BYTE, cOvi1);
	glPopMatrix();

	glPushMatrix();	// Tital PASAYDAN
	glTranslatef(-1.5f, 1.25f, 0.0f);
	glScalef(0.25f, 0.25f, 0.5f);
	glListBase(nFontList);
	char cOvi2[] = "jao KLaMcaI vyaMkTI saaMDÜ । tyaaM sa%kmaI-M rtI vaaZÜ ।  BaUtaM prsproM jaDÜ। maO~ jaIvaaMcaoM ॥ 2 ॥";
	glCallLists(sizeof(cOvi2) - 1, GL_UNSIGNED_BYTE, cOvi2);
	glPopMatrix();

	glPushMatrix();	// Tital PASAYDAN
	glTranslatef(0.0f, -0.2f, 0.0f);
	glTranslatef(-1.5f, 1.0f, 0.0f);
	glScalef(0.25f, 0.25f, 0.5f);
	glListBase(nFontList);
	char cOvi3[] = "duirtacaoM itimar jaavaÜ । ivaXva svaQama- saUyao-M pahÜ । jaÜ jaoM vaaMcCIla tÜ toM laahÜ । p`aiNajaat ॥ 3 ॥";
	glCallLists(sizeof(cOvi3) - 1, GL_UNSIGNED_BYTE, cOvi3);
	glPopMatrix();

	glPushMatrix();	// Tital PASAYDAN
	glTranslatef(0.0f, -0.2f, 0.0f);
	glTranslatef(-1.5f, 0.75f, 0.0f);
	glScalef(0.25f, 0.25f, 0.5f);
	glListBase(nFontList);
	char cOvi4[] = "vaYa-t sakLmaMgaLIM । šXvar inaYzaMcaI maaMidyaaLI । Anavart BaUmaMDLIM । BaoTtu yaa BaUtaM ॥ 4 ॥";
	glCallLists(sizeof(cOvi4) - 1, GL_UNSIGNED_BYTE, cOvi4);
	glPopMatrix();	

	glPushMatrix();	// Tital PASAYDAN
	glTranslatef(0.0f, -0.4f, 0.0f);
	glTranslatef(-1.5f, 0.5f, 0.0f);
	glScalef(0.25f, 0.25f, 0.5f);
	glListBase(nFontList);
	char cOvi5[] = "calaaM klpt$Mcao Aarva । caotnaa icaMtamaNaIcaoM gaaMva । baÜlato jao ANa-va । pIyaUYaacao ॥ 5 ॥";
	glCallLists(sizeof(cOvi5) - 1, GL_UNSIGNED_BYTE, cOvi5);
	glPopMatrix();		

	glPushMatrix();	// Tital PASAYDAN
	glTranslatef(0.0f, -0.4f, 0.0f);
	glTranslatef(-1.5f, 0.25f, 0.0f);
	glScalef(0.25f, 0.25f, 0.5f);
	glListBase(nFontList);
	char cOvi6[] = "caMd`mao jao AlaaMCna । maat-MD jao taphIna । to savaa-MhI sada sajjana । saÜyaro hÜtu ॥ 6 ॥";
	glCallLists(sizeof(cOvi6) - 1, GL_UNSIGNED_BYTE, cOvi6);
	glPopMatrix();	

	glPushMatrix();	// Tital PASAYDAN
	glTranslatef(0.0f, -0.6f, 0.0f);
	glTranslatef(-1.5f, 0.0f, 0.0f);
	glScalef(0.25f, 0.25f, 0.5f);
	glListBase(nFontList);
	char cOvi7[] = "ikMbahunaa sava-sauKIM । pUNa- hÜ}ina ithIM laÜkIM । BaijajaÜ AaidpuÉKIM । AKMiDt ॥ 7 ॥";
	glCallLists(sizeof(cOvi7) - 1, GL_UNSIGNED_BYTE, cOvi7);
	glPopMatrix();

	glPushMatrix();	// Tital PASAYDAN
	glTranslatef(0.0f, -0.6f, 0.0f);
	glTranslatef(-1.5f, -0.25f, 0.0f);
	glScalef(0.25f, 0.25f, 0.5f);
	glListBase(nFontList);
	char cOvi8[] = "AaiNa ga`MqaÜpjaIivayao । ivaXaoYaIM laÜkIM [yaoM । dRYTadRYT ivajayaoM । hÜAavaoM jaI ॥ 8 ॥";
	glCallLists(sizeof(cOvi8) - 1, GL_UNSIGNED_BYTE, cOvi8);
	glPopMatrix();

	glPushMatrix();	// Tital PASAYDAN
	glTranslatef(0.0f, -0.8f, 0.0f);
	glTranslatef(-1.5f, -0.5f, 0.0f);
	glScalef(0.25f, 0.25f, 0.5f);
	glListBase(nFontList);
	char cOvi9[] = "yaoqa mhNao EaIivaXvaoXvaravaÜ । ha hÜšla danapsaavaÜ । yaoNaoM varoM &anadovaÜ । sauiKyaa Jaalaa ॥ 9 ॥";
	glCallLists(sizeof(cOvi9) - 1, GL_UNSIGNED_BYTE, cOvi9);
	glPopMatrix();

	glPopMatrix();
}

void update(void)
{
	//Code
	pAngle = pAngle + 0.02f;

	if (pAngle >= 360.0f)
		pAngle = pAngle - 360.0f;

	cAngle = cAngle - 0.02f;

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

	if (texture_front)
	{
		glDeleteTextures(1, &texture_front);
		texture_front = 0;
	}
	if (texture_back)
	{
		glDeleteTextures(1, &texture_back);
		texture_back = 0;
	}
	if (texture_left)
	{
		glDeleteTextures(1, &texture_left);
		texture_left = 0;
	}
	if (texture_right)
	{
		glDeleteTextures(1, &texture_right);
		texture_right = 0;
	}
	if (texture_top)
	{
		glDeleteTextures(1, &texture_top);
		texture_top = 0;
	}
	if (texture_bottom)
	{
		glDeleteTextures(1, &texture_bottom);
		texture_bottom = 0;
	}					

	//Close the log file
	if (gpFile) {
		fprintf(gpFile, "Program finished successfully.\n");
		fclose(gpFile);
		gpFile = NULL;
	}
}
