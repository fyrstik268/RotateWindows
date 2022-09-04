#include <RotWnd.h>

static LRESULT CALLBACK ClientWndProc(HWND Window, UINT Message, WPARAM WP, LPARAM LP);
static LRESULT CALLBACK OutputWndProc(HWND Vindu, UINT Message, WPARAM WP, LPARAM LP);

void CreateWindows(void) {
	WNDCLASSW WindowClass = {
		.style = CS_OWNDC,
		.lpfnWndProc = OutputWndProc,
		.cbClsExtra = 0,
		.cbWndExtra = 0,
		.hInstance = Fejalink.ProcessModule,
		.hIcon = LoadIconW(Fejalink.ProcessModule, MAKEINTRESOURCEW(IDI_APP_ICON1)),
		.hCursor = LoadCursorW(NULL, IDC_ARROW),
		.hbrBackground = NULL,
		.lpszMenuName = NULL,
		.lpszClassName = L"You'll see when the time comes that everything is quite acute"
	};
	Fejalink.OutputWndClass = RegisterClassW(&WindowClass);
	WindowClass.lpszClassName = L"Instead of working on stuff with an actual deadline instead I'm here pondering on what odd stuff to write";
	WindowClass.lpfnWndProc = ClientWndProc;
	Fejalink.ClientWndClass = RegisterClassW(&WindowClass);

	POINT CursorPos;
	GetPhysicalCursorPos(&CursorPos);
	HMONITOR ActiveMonitor = MonitorFromPoint(CursorPos, MONITOR_DEFAULTTONEAREST);
	MONITORINFO MonitorInfo;
	MonitorInfo.cbSize = sizeof(MonitorInfo);
	GetMonitorInfoW(ActiveMonitor, &MonitorInfo);

	const DWORD OutputWindowStyle = WS_POPUP | WS_VISIBLE, OutputExtendedStyle = WS_EX_APPWINDOW | WS_EX_LAYERED | WS_EX_NOPARENTNOTIFY, ClientWindowStyle = WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX), ClientExtendedStyle = WS_EX_NOPARENTNOTIFY | WS_EX_OVERLAPPEDWINDOW;
	RECT ClientRectAdj = {
		.right = 640,
		.bottom = 480
	};
	AdjustWindowRectEx(&ClientRectAdj, ClientWindowStyle, FALSE, ClientExtendedStyle);

	const int ClientWidth = ClientRectAdj.right - ClientRectAdj.left, ClientHeight = ClientRectAdj.bottom - ClientRectAdj.top;
	const int OutputX = (MonitorInfo.rcMonitor.right - MonitorInfo.rcMonitor.left) / 2 - ClientWidth / 2, OutputY = (MonitorInfo.rcMonitor.bottom - MonitorInfo.rcMonitor.top) / 2 - ClientHeight / 2;
	const int OutputSize = (int)ceilf(sqrtf(powf((float)ClientWidth, 2.f) / 2) + sqrtf(pow((float)ClientHeight, 2.f) / 2.f)); /* Credits to Melody for the formula */
	Fejalink.OutputWindow = CreateWindowExW(OutputExtendedStyle, MAKEINTATOM(Fejalink.OutputWndClass), L"Wind it up, watch it spin, all around you'll see it goes", OutputWindowStyle, OutputX, OutputY, OutputSize, OutputSize, NULL, NULL, Fejalink.ProcessModule, NULL);
	Fejalink.ClientWindow = CreateWindowExW(ClientExtendedStyle, MAKEINTATOM(Fejalink.ClientWndClass), L"*Dramatic SFX plays*"/*L"Wind it up, Watch it spin, All the lies come round again"*/, ClientWindowStyle, 0, 0, ClientWidth, ClientHeight, Fejalink.OutputWindow, NULL, Fejalink.ProcessModule, NULL);

	SetLayeredWindowAttributes(Fejalink.OutputWindow, RGB(0xCF, 0x37, 0xFE), 0xFF, LWA_COLORKEY);
	return;
}

static LRESULT ClientWndProc_Paint(HWND const Window, HDC DC);
static LRESULT CALLBACK ClientWndProc(HWND Window, UINT Message, WPARAM WP, LPARAM LP) {
	switch(Message) {
		case WM_PRINT: return ClientWndProc_Paint(NULL, (HDC)WP);
		case WM_PAINT: return ClientWndProc_Paint(Window, NULL);
		default: return DefWindowProcW(Window, Message, WP, LP);
	}
}

static LRESULT ClientWndProc_Paint(HWND const Window, HDC DC) {
	PAINTSTRUCT PaintStruct;
	if(!DC) DC = BeginPaint(Window, &PaintStruct);
	RECT Rct = {
		.left = 100,
		.right = 200,
		.bottom = 200,
		.top = 100
	};
	HBRUSH Red = CreateSolidBrush(RGB(0xFF, 0, 0));
	FillRect(DC, &Rct, Red);
	DeleteObject(Red);
	if(!Window) EndPaint(Window, &PaintStruct);
}

static LRESULT OutputWndProc_Paint(HWND const Window);
static LRESULT CALLBACK OutputWndProc(HWND Vindu, UINT Message, WPARAM WP, LPARAM LP) {
	switch(Message) {
		case WM_PAINT: return OutputWndProc_Paint(Vindu);
		default: return DefWindowProcW(Vindu, Message, WP, LP);
	}
}

static LRESULT OutputWndProc_Paint(HWND const Window) {
	RECT WndRect;
	GetWindowRect(Fejalink.ClientWindow, &WndRect);
	const float Formula = sqrtf(powf((float)(WndRect.right - WndRect.left), 2.f) / 2) + sqrtf(powf((float)(WndRect.bottom - WndRect.top), 2.f) / 2.f);

	IDXGISurface1* restrict Surface;
	ID3D11Texture2D_QueryInterface(Fejalink.DX.PostRotateRender, &IID_IDXGISurface1, &Surface);
	HDC SDC;
	IDXGISurface1_GetDC(Surface, FALSE, &SDC);

	PAINTSTRUCT PaintStruct;
	HDC XDC = BeginPaint(Window, &PaintStruct);

	BitBlt(XDC, 0, 0, Formula, Formula, SDC, 0, 0, SRCCOPY);

	EndPaint(Window, &PaintStruct);
	IDXGISurface1_ReleaseDC(Surface, NULL);
	IDXGISurface1_Release(Surface);
	return 0;
}