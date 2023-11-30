//Windows Header Files
#include <windows.h>
#include <stdio.h> //For File IO
#include <stdlib.h> //For exit()
#include <wingdi.h>

//Global Function Declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//Global Variables
FILE* gpFile = NULL;

HWND ghwnd = NULL;
BOOL gbActive = FALSE;
BOOL bDone = FALSE;

DWORD dwstyle = 0;
WINDOWPLACEMENT wpPrev = {sizeof(WINDOWPLACEMENT)}; //All elements are set to value in  '{}' 

int iRed = 0;
int iGreen = 255;
int iBlue = 255;

//Entry Point Function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//Local Variable declarations
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("AGSWindow");

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
	wndclass.style = CS_HREDRAW | CS_VREDRAW; //WM_PAINT HANDLING FOR resizing
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(hInstance, IDI_APPLICATION);

	//Rgister WNDCLASSEX
	RegisterClassEx(&wndclass);

	//Centering Position of Window wrt to screen
	int iScreen_Height = GetSystemMetrics(SM_CYSCREEN);	//GetDeviceCaps
	int iScreen_Width = GetSystemMetrics(SM_CXSCREEN);

	int iWindow_Height = 600;
	int iWindow_Width = 800;
	int iWindow_Xpos = (iScreen_Width / 2) - (iWindow_Width/2);
	int iWindow_Ypos = (iScreen_Height / 2) - (iWindow_Height / 2);

	//Create Window
	hwnd = CreateWindowEx(WS_EX_APPWINDOW, //It will help to bring your window on top of the taskbar
						szAppName,
						TEXT("Ashish Shelke"),
						WS_OVERLAPPEDWINDOW,
						iWindow_Xpos,	//X
						iWindow_Ypos,	//Y
						iWindow_Width,	//Width
						iWindow_Height,	//Height
						NULL,
						NULL,
						hInstance,
						NULL						
						);
	ghwnd = hwnd;

	//Show The Window
	ShowWindow(hwnd, iCmdShow);

	//Paint\Redraw The Window
	UpdateWindow(hwnd);

	//Message Loop
	/*while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}*/
	int iFrameCounter = 0;
	PAINTSTRUCT ps;
	HDC hdc;
	GRADIENT_TRIANGLE gTriangle;
	TRIVERTEX vertex[3];
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
		}
	}

	return ((int)msg.wParam);
}

//CALLBACK Function
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//Function Declarations
	void RefreshTextColor(HWND);
	void ReDrawWindow(HWND);
	//Local Variables
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rc;
	TCHAR str[] = TEXT("Hello World !!!");

	//Code
	switch (iMsg)
	{
	case WM_PAINT:
		GetClientRect(hwnd, &rc);
		hdc = BeginPaint(hwnd, &ps);
		SetBkColor(hdc, RGB(0, 0, 0));
		SetTextColor(hdc, RGB(iRed, iGreen, iBlue));
		DrawText(hdc, str, -1, &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		EndPaint(hwnd, &ps);
		break;

	case WM_DESTROY:
		if (gpFile) {
			fprintf(gpFile, "Program finished successfully.\n");
			fclose(gpFile);
			gpFile = NULL;
		}
		PostQuitMessage(0);
		break;

	default:
		break;
	}
	return (DefWindowProc(hwnd, iMsg, wParam, lParam));
}
// cl.exe /c /EHsc  Window.c
// link.exe Window.obj User32.lib GDI32.lib /SUBSYSTEM:WINDOWS

void RefreshTextColor(HWND hwnd)
{
	//Local Variables
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rc;
	TCHAR str[] = TEXT("Dynamic Color");

	GetClientRect(hwnd, &rc);
	hdc = BeginPaint(hwnd, &ps);
	SetBkColor(hdc, RGB(0, 0, 0));
	SetTextColor(hdc, RGB(iRed, iGreen, iBlue));
	DrawText(hdc, str, -1, &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	EndPaint(hwnd, &ps);
}

void ReDrawWindow(HWND hwnd)
{
	//Local Variables
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rc;
	TCHAR str[] = TEXT("Dynamic Color");

	GetClientRect(hwnd, &rc);
	RedrawWindow(hwnd, &rc, NULL, RDW_FRAME);
	EndPaint(hwnd, &ps);
}

