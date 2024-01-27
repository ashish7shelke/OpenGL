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
#include "Particle.h"

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

//Particle variables
BOOL bRainbow = TRUE;
GLfloat fPArticleSpeed = 2.0f;

GLfloat slowdown = 0.2f;
GLuint  loop;               // Misc Loop Variable
GLuint  col;                // Current Color Selection
GLuint  delay;              // Rainbow Effect Delay
GLuint  textureParticle[1];         // Storage For Our Particle Texture

// OPENGL Related Global Variables
HDC ghdc = NULL;
HGLRC ghrc = NULL; // Handle Graphic Rendering Context

GLfloat pAngle = 0.0f;
GLfloat cAngle = 0.0f;

//Texture Object
GLuint texture_floor = 0;
GLuint texture_particle = 0;

//Fog
GLfloat fFogColor[4] = {0.5, 0.5, 0.5, 1};
BOOL bFogEnable = FALSE;
GLfloat fDensity = 0.0f;

float	xspeed;						// Base X Speed (To Allow Keyboard Direction Of Tail)
float	yspeed;						// Base Y Speed (To Allow Keyboard Direction Of Tail)
float	zoom= 0.0f;				// Used To Zoom Out

float posX = 0.0f; 
float posY = -17.5f; 
float posZ = 0.0f;

float gravX = 0.0f;
float gravY = 0.0f;
float gravZ = 0.0f;

GLfloat fCamx = 0.0f;
GLfloat fCamy = 0.0f;
GLfloat fCamz = 45.0f;
GLfloat fCamSpeed = 0.5f;

//Lights Variables
BOOL gbLight = FALSE;
GLfloat fLightAmbientZero[] = {0.2f, 0.2f, 0.2f, 1.0f}; // Grey 240,127,19)
GLfloat fLightDiffusedZero[] = {0.7f, 0.7f, 0.7f, 1.0f}; // White 
GLfloat fLightSpecularZero[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat fLightPositionZero[] = {0.0f, 0.0f, 1.0f, 1.0f};

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
			fCamz = -5.0f;
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
	glEnable(GL_LIGHT0);

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
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Loading Images - Create Texture
	bResult = loadGlTexture(&texture_floor, MAKEINTRESOURCE(GROUND_TEXTURE));
	if (bResult == FALSE)
	{
		fprintf(gpFile, "loadGlTexture(MY_STONE_BITMAP) failed .../n");
		return -6;
	} 

	bResult = loadGlTexture(&texture_particle, MAKEINTRESOURCE(PARTICLE_TEXTURE));
	if (bResult == FALSE)
	{
		fprintf(gpFile, "loadGlTexture(MY_STONE_BITMAP) failed .../n");
		return -6;
	}

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);									// Enable Blending
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);					// Type Of Blending To Perform
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);	// Really Nice Perspective Calculations
	glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);	
	glEnable(GL_TEXTURE_2D);

	for (loop=0;loop<MAX_PARTICLES;loop++)                   // Initialize All The Textures
	{
		particle[loop].active=TRUE;								// Make All The Particles Active
		particle[loop].life=1.0f;								// Give All The Particles Full Life
		particle[loop].fade=(float)(rand()%100)/1000.0f+0.003f;	// Random Fade Speed
		particle[loop].r=white[0];	
		particle[loop].g=white[1];	
		particle[loop].b=white[2];	
		particle[loop].xi=(float)((rand()%50)-25.0f)*10.0f;		// Random Speed On X Axis
		particle[loop].yi=(float)((rand()%50)-25.0f)*10.0f;		// Random Speed On Y Axis
		particle[loop].zi=(float)((rand()%50)-25.0f)*10.0f;		// Random Speed On Z Axis
		particle[loop].xg=0.0f;									// Set Horizontal Pull 
		particle[loop].yg=0.8f;									// Set Vertical Pull 
		particle[loop].zg=0.0f;									// Set Pull On Z Axis To Zero

	}
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
	void DrawFire();
	//Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Now here screen gets colored as color mentioned in initialize()

	glMatrixMode(GL_MODELVIEW); //

	glLoadIdentity(); // Set Matrix to identity

	gluLookAt(fCamx, fCamy, fCamz, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	glPushMatrix();
	DrawFire();
	glPopMatrix();

	/*glPushMatrix();
	glTranslatef(-20.0f, 0.0f, 0.0f);
	DrawFire();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(20.0f, 0.0f, 0.0f);
	DrawFire();
	glPopMatrix();*/

	glColor3f(1.0f, 1.0f, 1.0f);
	//DrawGrid();

	glFogf(GL_FOG_DENSITY, fDensity);
	SwapBuffers(ghdc);
}

void DrawFire()
{
		//Particle Code
	glPushMatrix();
	
	//glPushAttrib(GL_LIGHTING_BIT);
	//glDisable(GL_LIGHTING);
	for (loop=0;loop<MAX_PARTICLES;loop++)					// Loop Through All The Particles
	{
		if (particle[loop].active)							// If The Particle Is Active
		{	
			
			float x=particle[loop].x;						// Grab Our Particle X Position
			float y=particle[loop].y;						// Grab Our Particle Y Position
			float z=particle[loop].z+zoom;					// Particle Z Pos + Zoom

			// Draw The Particle Using Our RGB Values, Fade The Particle Based On It's Life
			glColor4f(particle[loop].r,particle[loop].g,particle[loop].b,particle[loop].life);
			glBindTexture(GL_TEXTURE_2D, texture_particle);
			glBegin(GL_TRIANGLE_STRIP);						// Build Quad From A Triangle Strip
			    glTexCoord2d(0.65,0.65); glVertex3f(x+0.5f,y+0.5f,z); // Top Right
				glTexCoord2d(0.35,0.65); glVertex3f(x-0.5f,y+0.5f,z); // Top Left
				glTexCoord2d(0.65,0.35); glVertex3f(x+0.5f,y-0.5f,z); // Bottom Right
				glTexCoord2d(0.35,0.35); glVertex3f(x-0.5f,y-0.5f,z); // Bottom Left
			glEnd();
			glBindTexture(GL_TEXTURE_2D, 0);
			// Done Building Triangle Strip

			particle[loop].x+=particle[loop].xi/(750);// Move On The X Axis By X Speed
			particle[loop].y+=particle[loop].yi/(1000);// Move On The Y Axis By Y Speed
			particle[loop].z+=particle[loop].zi/(750);// Move On The Z Axis By Z Speed
			
			if((particle[loop].x>posX)&&(particle[loop].y>(0.1+posY))){
				particle[loop].xg=-0.5f;
			} else if((particle[loop].x<posX)&&(particle[loop].y>(0.1+posY))){
				particle[loop].xg=0.5f;
			} else {
				particle[loop].xg=0.0f;
			}
			
			particle[loop].xi+=(particle[loop].xg + gravX);			// Take Pull On X Axis Into Account
			particle[loop].yi+=(particle[loop].yg + gravY);			// Take Pull On Y Axis Into Account
			particle[loop].zi+=(particle[loop].zg + gravZ);			// Take Pull On Z Axis Into Account
			particle[loop].life-=particle[loop].fade;		// Reduce Particles Life By 'Fade'
						
			if (particle[loop].life<0.0f)					// If Particle Is Burned Out
			{
				particle[loop].life=1.0f;					// Give It New Life
				particle[loop].fade=(float)(rand()%100)/1000.0f+0.003f;	// Random Fade Value
				particle[loop].x=posX;						// Center On X Axis
				particle[loop].y=posY;						// Center On Y Axis
				particle[loop].z=posZ;						// Center On Z Axis
				particle[loop].xi=xspeed+(float)((rand()%60)-30.0f);	// X Axis Speed And Direction
				particle[loop].yi=yspeed+(float)((rand()%60)-30.0f);	// Y Axis Speed And Direction
				particle[loop].zi=(float)((rand()%60)-30.0f);	// Z Axis Speed And Direction
				particle[loop].r=white[0];			
				particle[loop].g=white[1];			
				particle[loop].b=white[2];			
			}
			else if (particle[loop].life<0.4f)					
			{
				particle[loop].r=red[0];			
				particle[loop].g=red[1];			
				particle[loop].b=red[2];
			}
			else if (particle[loop].life<0.6f)					
			{
				particle[loop].r=orange[0];			
				particle[loop].g=orange[1];			
				particle[loop].b=orange[2];
			}
			else if (particle[loop].life<0.75f)				
			{
				particle[loop].r=yellow[0];			
				particle[loop].g=yellow[1];			
				particle[loop].b=yellow[2];
			}
			else if (particle[loop].life<0.85f)					
			{
				particle[loop].r=blue[0];			
				particle[loop].g=blue[1];			
				particle[loop].b=blue[2];
			}
		}
    }
	//glBindTexture(GL_TEXTURE_2D, 0);
	//glPopAttrib();
	glPopMatrix();
}

// Draw the ground as a series of triangle strips
void DrawGrid(void)
{
	GLfloat fExtent = 50.0f;
	GLfloat fStep = 0.3f;
	GLfloat y = posY - 0.5f;
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

	if (texture_particle)
	{
		glDeleteTextures(1, &texture_particle);
		texture_particle = 0;
	}

	//Close the log file
	if (gpFile) {
		fprintf(gpFile, "Program finished successfully.\n");
		fclose(gpFile);
		gpFile = NULL;
	}
}
