//Windows Header Files
#include <windows.h>

//Global Function Declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//Entry Point Function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//Local Variable declarations
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("AGS Window");

	//Code

	MessageBox(NULL, NULL, TEXT("Message"), MB_OK);

	//WNDCLASSEX Initialization
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW; //WM_PAINT HANDLING FOR resizing
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

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
	hwnd = CreateWindow(szAppName,
						TEXT("Ashish Shelke - Messages"),
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

	//Show The Window
	ShowWindow(hwnd, iCmdShow);

	//Paint\Redraw The Window
	UpdateWindow(hwnd);

	//Message Loop
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	return ((int)msg.wParam);
}

//CALLBACK Function
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//Code
	switch (iMsg)
	{
	case WM_CREATE:
		MessageBox(hwnd, TEXT("WM_CREATE Arrived"), TEXT("Message"), MB_OK);
		break;

	case WM_SIZE:
		MessageBox(hwnd, TEXT("WM_SIZE Arrived"), TEXT("Message"), MB_OK);
		break;

	case WM_KEYDOWN:
		switch (LOWORD(wParam))
		{
		case VK_ESCAPE:
			MessageBox(hwnd, TEXT("WM_KEYDOWN - VK_ESCAPE Arrived"), TEXT("Message"), MB_OK);
			break;
		}
		break;

	case WM_CHAR:
		switch (LOWORD(wParam))
		{
		case 'F':
		case 'f':
			MessageBox(hwnd, TEXT("WM_CHAR Arrived"), TEXT("Message"), MB_OK);
			break;
		}
		break;
	case WM_LBUTTONDOWN:
		MessageBox(hwnd, TEXT("WM_LBUTTONDOWN Arrived"), TEXT("Message"), MB_OK);
		break;
		
	case WM_DESTROY:
			PostQuitMessage(0);
			//PostMessage(hwnd, WM_QUIT, 0, 0L);
			break;
	default:
		break;
	}
	return (DefWindowProc(hwnd, iMsg, wParam, lParam));
}
// cl.exe /c /EHsc  Window.c
// link.exe Window.obj User32.lib GDI32.lib /SUBSYSTEM:WINDOWS
