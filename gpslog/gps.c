#include <windows.h>


HINSTANCE	g_hInst = NULL;     // Handle to the application instance
HWND		g_hwndMain = NULL;       // Handle to the application main window
HWND		g_htext = 0;
HWND		g_hbutton = 0;
HANDLE		g_hPort = 0;
// Application main window name
TCHAR		g_szTitle[80] = TEXT ("Main Window");
// Main window class name
TCHAR		g_szClassName[80] = TEXT ("Main window class");

void UpdateSize()
{
    RECT rect;
    GetClientRect( g_hwndMain, &rect);
    MoveWindow(g_htext, 0, 0, rect.right-rect.left, (rect.bottom-rect.top)/2, TRUE);
    MoveWindow(g_hbutton, 0, (rect.bottom-rect.top)/2, rect.right-rect.left, (rect.bottom-rect.top)/2, TRUE);
}

/***********************************************************************

FUNCTION:
  WndProc

PURPOSE:
  The callback function for the main window. It processes messages that
  are sent to the main window.

***********************************************************************/
LRESULT CALLBACK WndProc (HWND hwnd, UINT umsg, WPARAM wParam,
                          LPARAM lParam)
{
    switch (umsg)
    {
        // Add cases such as WM_CREATE, WM_COMMAND, WM_PAINT if you don't
        // want to pass these messages along for default processing.

    case WM_SIZE:
        UpdateSize();
        break;
    case WM_CREATE:
        PlaySound(L"hellowin.wav", NULL, SND_SYNC | SND_FILENAME);
        return 0;
    case WM_COMMAND:
        if (LOWORD(wParam)==100)
            PostQuitMessage (0);
        return 0;
    case WM_CLOSE:
        DestroyWindow (hwnd);
        return 0;

    case WM_DESTROY:
        PostQuitMessage (0);
        return 0;
    }

    return DefWindowProc (hwnd, umsg, wParam, lParam);
}

/***********************************************************************

FUNCTION:
  InitInstance

PURPOSE:
  Create and display the main window.

***********************************************************************/
BOOL InitInstance (HINSTANCE hInstance, int iCmdShow)
{

    g_hInst = hInstance;

    g_hwndMain = CreateWindow (
                     g_szClassName,  // Registered class name
                     g_szTitle,      // Application window name
                     WS_OVERLAPPED | WS_SIZEBOX | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX, // Window style
                     CW_USEDEFAULT,  // Horizontal position of the window
                     CW_USEDEFAULT,  // Vertical position of the window
                     CW_USEDEFAULT,  // Window width
                     CW_USEDEFAULT,  // Window height
                     NULL,           // Handle to the parent window
                     NULL,           // Handle to the menu the identifier
                     hInstance,      // Handle to the application instance
                     NULL);          // Pointer to the window-creation data

    // If it failed to create the window, return FALSE.
    if (!g_hwndMain)
        return FALSE;

    ShowWindow (g_hwndMain, iCmdShow);
    UpdateWindow (g_hwndMain);
    //SHFullScreen (g_hwndMain, SHFS_HIDESTARTICON | SHFS_HIDETASKBAR | SHFS_HIDESIPBUTTON);
    return TRUE;
}

/***********************************************************************

FUNCTION:
  InitApplication

PURPOSE:
  Declare the window class structure, assign values to the window class
  structure members, and register the window class.

***********************************************************************/
BOOL InitApplication (HINSTANCE hInstance)
{
    WNDCLASS wndclass;

    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = (WNDPROC)WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hIcon = NULL;
    wndclass.hInstance = hInstance;
    wndclass.hCursor = NULL;
    wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = g_szClassName;

    return RegisterClass (&wndclass);
}

wchar_t *ToWcs(char *src)
{
    wchar_t *dst = NULL;

	int dstlen = mbstowcs(NULL, src, 0) + 1;
    if (dstlen == 0) return 0;
	dst = (wchar_t *)malloc( dstlen * sizeof(wchar_t) );

    mbstowcs(dst, src, dstlen);
    return dst;
}
/***********************************************************************

FUNCTION:
  WinMain

PURPOSE:
  The WinMain function of the application. It is called by the system as
  the initial entry point for this Windows CE-based application.

***********************************************************************/
int WINAPI WinMain (
    HINSTANCE hInstance,     // Handle to the current instance
    HINSTANCE hPrevInstance, // Handle to the previous instance
    LPWSTR lpCmdLine,        // Pointer to the command line
    int iCmdShow)            // Shows the state of the window
{
    MSG msg;                             // Message structure
    DCB PortDCB;
    #define BUFFSIZE 300
    CHAR buffer[BUFFSIZE];
    DWORD dwi,dwByteCount,dwOut,dwPos = 0;
	HANDLE	log;
	BOOL	bQuit;

    TCHAR path[MAX_PATH+20];
    // table

    if (FindWindow(g_szClassName, 0))
    {
        SetForegroundWindow(FindWindow(g_szClassName, 0));
        return FALSE;
    }


    if (!hPrevInstance)
    {
        if (!InitApplication (hInstance))
            return FALSE;
    }

    if (!InitInstance (hInstance, iCmdShow))
        return FALSE;


    g_hPort = CreateFile(L"COM1:", GENERIC_READ | GENERIC_WRITE,
                         0, NULL, OPEN_EXISTING,
                         0, NULL);


// Initialize the DCBlength member.
    PortDCB.DCBlength = sizeof (DCB);

// Get the default port setting information.
    GetCommState (g_hPort, &PortDCB);

// Change the DCB structure settings.
    PortDCB.BaudRate = 4800;              // Current baud
    PortDCB.fBinary = TRUE;               // Binary mode; no EOF check
    PortDCB.fParity = TRUE;               // Enable parity checking
    PortDCB.fOutxCtsFlow = FALSE;         // No CTS output flow control
    PortDCB.fOutxDsrFlow = FALSE;         // No DSR output flow control
    PortDCB.fDtrControl = DTR_CONTROL_ENABLE;
    // DTR flow control type
    PortDCB.fDsrSensitivity = FALSE;      // DSR sensitivity
    PortDCB.fTXContinueOnXoff = TRUE;     // XOFF continues Tx
    PortDCB.fOutX = FALSE;                // No XON/XOFF out flow control
    PortDCB.fInX = FALSE;                 // No XON/XOFF in flow control
    PortDCB.fErrorChar = FALSE;           // Disable error replacement
    PortDCB.fNull = FALSE;                // Disable null stripping
    PortDCB.fRtsControl = RTS_CONTROL_ENABLE;
    // RTS flow control
    PortDCB.fAbortOnError = FALSE;        // Do not abort reads/writes on
    // error
    PortDCB.ByteSize = 8;                 // Number of bits/byte, 4-8
    PortDCB.Parity = NOPARITY;            // 0-4=no,odd,even,mark,space
    PortDCB.StopBits = ONESTOPBIT;        // 0,1,2 = 1, 1.5, 2

// Configure the port according to the specifications of the DCB
// structure.
    if (!SetCommState (g_hPort, &PortDCB))
    {
        // Could not configure the serial port.
        MessageBox (g_hwndMain, TEXT("Unable to configure the serial port"), TEXT("Error"), MB_OK);
        return FALSE;
    }


    //SetWindowPos(g_hwndMain,HWND_TOPMOST,0,0,GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_SHOWWINDOW);
    //ShowWindow(g_hwndMain,SW_SHOWMAXIMIZED);


    g_htext = CreateWindowEx (WS_TABSTOP, TEXT ("STATIC"), L"GPS", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, g_hwndMain, (HMENU) 100, hInstance, 0);
    g_hbutton = CreateWindowEx (WS_TABSTOP, TEXT ("BUTTON"), L"Exit", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, g_hwndMain, (HMENU) 100, hInstance, 0);
    UpdateSize();

    // Insert code here to load the accelerator table.
    // hAccel = LoadAccelerators (...);


    GetModuleFileName(NULL, path, MAX_PATH);
    *wcsrchr(path,L'\\')=0;
    wcscat(path,L"\\gpslog.txt");

    log = CreateFile(path, GENERIC_WRITE,
                         FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
                         FILE_ATTRIBUTE_NORMAL, NULL);
    bQuit = FALSE;
    while (!bQuit)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        {
            dwByteCount = 0;
            if (ReadFile(g_hPort, buffer+dwPos, BUFFSIZE-dwPos, &dwByteCount, NULL) != 0)
            {
                for (dwi=0;dwi<dwByteCount;dwi++)
                    if (buffer[dwPos+dwi]=='\n')
                        break;
                if (dwi<dwByteCount)
                {
					PWSTR wszText = NULL;
                    WriteFile(log,buffer,dwPos+dwi,&dwOut, NULL);
                    buffer[dwPos+dwi] = 0;
                    wszText = ToWcs (buffer);
                    SetWindowText(g_htext,wszText);
                    //UpdateWindow(g_htext);
                    free(wszText);
                    memmove(buffer,buffer+dwPos+dwi+1,dwByteCount-dwi-1);
                    dwPos = dwByteCount-dwi-1;
                }
            }
        }
        Sleep(1);
    }

    CloseHandle(log);
    return msg.wParam;
}
