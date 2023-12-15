//Windows Header Files
#include <windows.h>
#include <stdio.h> //For File IO
#include <stdlib.h> //For exit()
#include <wingdi.h>

// OPENGL Header Files
#include<gl/GL.h>
#include<gl/GLU.h>

#include "OGL.h"
#include "Human.h"

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
BOOL gbWalkEnable = TRUE;

// OPENGL Related Global Variables
HDC ghdc = NULL;
HGLRC ghrc = NULL; // Handle Graphic Rendering Context

GLfloat fCamx = 0.0f;
GLfloat fCamy = 0.0f;
GLfloat fCamz = -12.5f;
GLfloat fObjectY = 90.0f;
GLfloat fCamSpeed = 0.1f;

GLuint texture_front = 0;
GLuint texture_back = 0;
GLuint texture_left = 0;
GLuint texture_right = 0;
GLuint texture_top = 0;
GLuint texture_bottom = 0;

//Lights
BOOL gbLight = FALSE;

GLfloat fLightAmbient[] = {0.1f, 0.1f, 0.1f, 1.0f}; // Grey
GLfloat fLightDiffused[] = {1.0f, 1.0f, 1.0f, 1.0f}; // White
GLfloat fLightSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat fLightPosition[] = {0.0f, 4.0f, 0.0f, 1.0f};
//GLfloat fSpotLightPos[] = { 0.0, 0.4, 0.0, 1.0 };
GLfloat fSpotLightDir[] = { 0.0, 1.0, 0.0, 1.0 };

//Material Properties
GLfloat fMaterialAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat fMaterialDiffused[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat fMaterialSpecular[] = {0.1f, 1.0f, 1.0f, 1.0f};
GLfloat fMaterialShyniness[] = {128.0f};

GLfloat cAngle = 0.0f;
GLfloat fBaseDepth = -6.0f;
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

		case 'W':
		case 'w':
			if (gbWalkEnable == FALSE)
			{
				gbWalkEnable = TRUE;
			}
			else
			{
				gbWalkEnable = FALSE;
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

		case '+':
			fCamz = fCamz + fCamSpeed;
			fprintf(gpFile, "fCamz = %f .\n", fCamz);
			break;
		case '-':
			fCamz = fCamz - fCamSpeed;
			fprintf(gpFile, "fCamz = %f .\n", fCamz);
			break;

		case 'O':
			fObjectY = fObjectY + fCamSpeed + 10.0f;
			break;
		case 'o':
			fObjectY = fObjectY - fCamSpeed - 10.0f;
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
	void resize(int, int);
	BOOL loadGlTexture(GLuint* texture, TCHAR imageResourceID[]);
	BOOL bResult;

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

	//Set Clear Color Of Window to Blue -- Its just informing but not yet colored
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	// Loading Images - Create Texture
	bResult = loadGlTexture(&texture2, MAKEINTRESOURCE(BODY));
	if (bResult == FALSE)
	{
		fprintf(gpFile, "loadGlTexture(BODY) failed .../n");
		return -6;
	}

	bResult = loadGlTexture(&texture1, MAKEINTRESOURCE(PHOTOM));
	if (bResult == FALSE)
	{
		fprintf(gpFile, "loadGlTexture(PHOTOM) failed .../n");
		return -7;
	}

	bResult = loadGlTexture(&texture3, MAKEINTRESOURCE(SHORTS));
	if (bResult == FALSE)
	{
		fprintf(gpFile, "loadGlTexture(SHORTS) failed .../n");
		return -8;
	}

	bResult = loadGlTexture(&texture4, MAKEINTRESOURCE(SKIN));
	if (bResult == FALSE)
	{
		fprintf(gpFile, "loadGlTexture(SKIN) failed .../n");
		return -9;
	}

	bResult = loadGlTexture(&texture_front, MAKEINTRESOURCE(CUBE_FRONT_BITMAP));
	if (bResult == FALSE)
	{
		fprintf(gpFile, "loadGlTexture(CUBE_FRONT_BITMAP) failed .../n");
		return -10;
	}
	bResult = loadGlTexture(&texture_back, MAKEINTRESOURCE(CUBE_BACK_BITMAP));
	if (bResult == FALSE)
	{
		fprintf(gpFile, "loadGlTexture(CUBE_BACK_BITMAP) failed .../n");
		return -11;
	}
	bResult = loadGlTexture(&texture_left, MAKEINTRESOURCE(CUBE_LEFT_BITMAP));
	if (bResult == FALSE)
	{
		fprintf(gpFile, "loadGlTexture(CUBE_LEFT_BITMAP) failed .../n");
		return -12;
	}
	bResult = loadGlTexture(&texture_right, MAKEINTRESOURCE(CUBE_RIGHT_BITMAP));
	if (bResult == FALSE)
	{
		fprintf(gpFile, "loadGlTexture(CUBE_RIGHT_BITMAP) failed .../n");
		return -13;
	}
	bResult = loadGlTexture(&texture_top, MAKEINTRESOURCE(CUBE_TOP_BITMAP));
	if (bResult == FALSE)
	{
		fprintf(gpFile, "loadGlTexture(CUBE_TOP_BITMAP) failed .../n");
		return -14;
	}
	bResult = loadGlTexture(&texture_bottom, MAKEINTRESOURCE(CUBE_BOTTOM_BITMAP));
	if (bResult == FALSE)
	{
		fprintf(gpFile, "loadGlTexture(CUBE_BOTTOM_BITMAP) failed .../n");
		return -15;
	}

	glEnable(GL_TEXTURE_2D);

	// Light Initialization
	glLightfv(GL_LIGHT0, GL_AMBIENT, fLightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, fLightDiffused);
	glLightfv(GL_LIGHT0, GL_SPECULAR, fLightSpecular);
	glLightfv(GL_LIGHT0, GL_POSITION, fLightPosition);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, fSpotLightDir);
	glEnable(GL_LIGHT0);

	//Material Initialization
	glMaterialfv(GL_FRONT, GL_AMBIENT, fMaterialAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, fMaterialDiffused); 
	glMaterialfv(GL_FRONT, GL_SPECULAR, fMaterialSpecular);
	glMaterialfv(GL_FRONT, GL_SHININESS, fMaterialShyniness);
	//glLightfv(GL_LIGHT1, GL_POSITION, spotLight1Pos);
	//glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spotLight1Dir);
	//glEnable(GL_LIGHT1);

	//Initialize quadric
	//quadric = gluNewQuadric();
	/* allocate quadrics with filled drawing style */
        h=gluNewQuadric();
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_TEXTURE_2D);
		gluQuadricTexture(h, GL_TRUE);
		gluQuadricDrawStyle(h, GLU_FILL);
		
		n=gluNewQuadric();
		gluQuadricTexture(n, GL_TRUE);
		gluQuadricDrawStyle(n, GLU_FILL);

		t=gluNewQuadric();
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_TEXTURE_2D);
		gluQuadricTexture(t, GL_TRUE);
        gluQuadricDrawStyle(t, GLU_FILL);

		jp=gluNewQuadric();
		gluQuadricTexture(jp, GL_TRUE);
		gluQuadricDrawStyle(jp, GLU_FILL);

        lua=gluNewQuadric();
		gluQuadricTexture(lua, GL_TRUE);
    	gluQuadricDrawStyle(lua, GLU_FILL);

        lla=gluNewQuadric();
		gluQuadricTexture(lla, GL_TRUE);
        gluQuadricDrawStyle(lla, GLU_FILL);

        lhnd=gluNewQuadric();
		gluQuadricTexture(lhnd, GL_TRUE);
		gluQuadricDrawStyle(lhnd, GLU_FILL);

		rhnd=gluNewQuadric();
		gluQuadricTexture(rhnd, GL_TRUE);
		gluQuadricDrawStyle(rhnd, GLU_FILL);

		rft=gluNewQuadric();
		gluQuadricTexture(rft, GL_TRUE);
		gluQuadricDrawStyle(rft, GLU_FILL);

		lft=gluNewQuadric();
		gluQuadricTexture(lft, GL_TRUE);
		gluQuadricDrawStyle(lft, GLU_FILL);

		rua=gluNewQuadric();
		gluQuadricTexture(rua, GL_TRUE);
        gluQuadricDrawStyle(rua, GLU_FILL);

        rla=gluNewQuadric();
		gluQuadricTexture(rla, GL_TRUE);
        gluQuadricDrawStyle(rla, GLU_FILL);

        lul=gluNewQuadric();
		gluQuadricTexture(lul, GL_TRUE);
        gluQuadricDrawStyle(lul, GLU_FILL);

        lll=gluNewQuadric();
		gluQuadricTexture(lll, GL_TRUE);
        gluQuadricDrawStyle(lll, GLU_FILL);

        rul=gluNewQuadric();
		gluQuadricTexture(rul, GL_TRUE);
        gluQuadricDrawStyle(rul, GLU_FILL);

        rll=gluNewQuadric();
		gluQuadricTexture(rll, GL_TRUE);
        gluQuadricDrawStyle(rll, GLU_FILL);

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


void display(void)
{
	void humanoid();
	//Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Now here screen gets colored as color mentioned in initialize()

	glMatrixMode(GL_MODELVIEW); //

	glLoadIdentity(); // Set Matrix to identity

	gluLookAt(fCamx, fCamy, fCamz, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	
	glTranslatef(0.0f ,0.0f, -6.0f);

	glPushMatrix();
	glTranslatef(-0.2f, -0.055f, fBaseDepth);
	glScalef(0.004f, 0.004f, 0.004f);
	glRotatef(fObjectY, 0.0f, 1.0f, 0.0f);
	human();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, fBaseDepth);
	//glScalef(0.75f, 0.75f, 0.75f);
	//glRotatef(cAngle, 1.0f, 0.0f, 0.0f);
	glRotatef(cAngle, 0.0f, 1.0f, 0.0f);
	//glRotatef(cAngle, 0.0f, 0.0f, 1.0f);

	DrawCube();
	glPopMatrix();

	glColor3f(1.0f, 1.0f, 1.0f);

	SwapBuffers(ghdc);
}

extern BOOL flag1;
void update(void)
{
	//Code
	//When Annimation

	if(gbWalkEnable)
	{
		timer_walk();
		//if(flag1)
		cAngle = cAngle - 0.0025f;
	}

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

	if (t)
		gluDeleteQuadric(t);
	if (h)
		gluDeleteQuadric(h);
	if (n)
		gluDeleteQuadric(n);
	if (lh)
		gluDeleteQuadric(lh);
	if (rh)
		gluDeleteQuadric(rh);
	if (lf)
		gluDeleteQuadric(lf);
	if (rf)
		gluDeleteQuadric(rf);
	if (jp)
		gluDeleteQuadric(jp);
	if (lhnd)
		gluDeleteQuadric(lhnd);
	if (lft)
		gluDeleteQuadric(lft);
	if (lua)
		gluDeleteQuadric(lua);
	if (lla)
		gluDeleteQuadric(lla);
	if (rua)
		gluDeleteQuadric(rua);
	if (rla)
		gluDeleteQuadric(rla);
	if (lll)
		gluDeleteQuadric(lll);
	if (rll)
		gluDeleteQuadric(rll);
	if (rul)
		gluDeleteQuadric(rul);
	if (lul)
		gluDeleteQuadric(lul);
	if (rhnd)
		gluDeleteQuadric(rhnd);
	if (rft)
		gluDeleteQuadric(rft);

	texture1=0; 
	texture2=0;
	texture3=0;
	texture4=0;
	
	texture_front = 0;
	texture_back = 0;
	texture_left = 0;
	texture_right = 0;
	texture_top = 0;
	texture_bottom = 0;

	//Close the log file
	if (gpFile) {
		fprintf(gpFile, "Program finished successfully.\n");
		fclose(gpFile);
		gpFile = NULL;
	}
}
