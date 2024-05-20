// Standard header file
#include<stdio.h>  // For printf
#include<stdlib.h> // For Exit
#include<memory.h> // for memset 

// X11 Headerfile
#include<X11/Xlib.h> //For all X11 API
#include<X11/Xutil.h> // For XVisualInfo and related API
#include<X11/XKBlib.h>

//Macros
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

//Global 
Display *display = NULL;
Colormap colormap; 
Window window;

int main(void)
{
    //Local Function declarations
    void uninitialize(void);

    //Local Variables
    int defaultScreen;
    int defaultDepth;
    XVisualInfo defaultVisualInfo;
    Status status;
    XSetWindowAttributes windowAtrributes;
    int styleMask;
    Atom windowMangerDelete;
    XEvent event;
    KeySym keySymbol;

    //Code

    //Step 1: Open the connection with Xserver and get the display interface
    display = XOpenDisplay(NULL); //Open connection and get display - Parameter NULL(Default) or display ID
    if(display == NULL)
    {
        printf("XOpenDisplay() Failed \n");
        uninitialize();
        exit(1);
    }

    //Step 2: Get default screen from above display
    defaultScreen = XDefaultScreen(display);

    //Step 3: Get default depth from above display & screen
    defaultDepth = XDefaultDepth(display, defaultScreen);

    //Step 4: Get Visual info from above 3
    memset((void *)&defaultVisualInfo, 0, sizeof(XVisualInfo));
    status = XMatchVisualInfo(display, defaultScreen, defaultDepth, TrueColor, &defaultVisualInfo);
    if(status == 0)
    {
        printf("XMatchVisualInfo() Failed \n");
        uninitialize();
        exit(1);
    }

    //Step5: Set window atributes / properties
    memset((void *)&windowAtrributes, 0, sizeof(XSetWindowAttributes));
    windowAtrributes.border_pixel = 0; // 0 for default
    windowAtrributes.background_pixel = XBlackPixel(display, defaultVisualInfo.screen); // for default BlackPixel(display,visualinfo.screen);
    windowAtrributes.background_pixmap = 0;
    windowAtrributes.colormap = XCreateColormap(display, 
                                                XRootWindow(display, defaultVisualInfo.screen),
                                                defaultVisualInfo.visual,
                                                AllocNone);

    //Assign thie colormap to global colormap
    colormap = windowAtrributes.colormap;
    
    //Set the style of window - WS_OVERLAPPED_WINDOW 
    styleMask = CWBorderPixel | CWBackPixel | CWColormap | CWEventMask;


    //Step 6: Create Window
    window = XCreateWindow(display, 
                            XRootWindow(display, defaultVisualInfo.screen),
                            0,
                            0,
                            WIN_WIDTH,
                            WIN_HEIGHT,
                            0,
                            defaultVisualInfo.depth,
                            InputOutput,
                            defaultVisualInfo.visual,
                            styleMask,
                            &windowAtrributes);

    if(!window)
    {
        printf("XCreateWindow failed \n");
        uninitialize();
        exit(1);
    }

    //Step 7: Specify to which events this window should response
    //ExposureMask - WM_PAINT
    //VisibilityChangeMask - Minimize/Maximize
    //StructureNotifyMask - WM_STYLE
    //PointerMotion - WM_MOUSEMOVE
    XSelectInput(display, window, ExposureMask | VisibilityChangeMask | StructureNotifyMask | KeyPressMask | ButtonPressMask | PointerMotionMask);
    
    //Step 8: Specify Window Manager Delete Atom
    windowMangerDelete = XInternAtom(display, "WM_DELETE_WINDOW", True);

    //Chrestofer Tronche & Kenton Li - XLib documentation
    //HPUX - HP Unix 
    //MAN Pages

    //Step 9: Add/Set above Atom as protocol for WindowManager
    XSetWMProtocols(display, window, &windowMangerDelete, 1);

    //Step 10: Give CAption to Window
    XStoreName(display, window, "AGS-XWindow");

    //Step 11:Map/Show the Window
    XMapWindow(display, window);

    //Step 12: Event Loop
    while(1)
    {
        memset((void *)&event, 0, sizeof(XEvent));
        XNextEvent(display, &event);

        switch(event.type)
        {
            case KeyPress:
                {
                    keySymbol = XkbKeycodeToKeysym(display, event.xkey.keycode, 0, 0);

                    switch (keySymbol)
                    {
                        case XK_Escape:
                            uninitialize();
                            exit(0);
                            break;
                        
                        default:
                            break;
                    }
                }
                break;
            case 33:
                uninitialize();
                exit(0); 
                break;

            default:
                break;
        }
    } 

    return 0;
}

void uninitialize(void)
{
    //Code
    if(window)
        XDestroyWindow(display, window);

    if(colormap)
        XFreeColormap(display, colormap);

    if(display)
    {
        XCloseDisplay(display);
        display = NULL;
    }
         
}
