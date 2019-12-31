#pragma once
bool looping = true;
double mousePosX=0;
double mousePosY=0;
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_DESTROY)
	{
		looping = false;
	}
	if (uMsg == WM_MOUSEMOVE)
	{
		mousePosX = LOWORD(lParam);
		mousePosY = HIWORD(lParam);
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
class Window
{
private:

	HWND hwnd;
	HDC deviceContext;
	WNDCLASSEX windowStruct;
	HGLRC context;

public:

	Window(TCHAR* name, int sizeX, int sizeY, int posX, int posY, HINSTANCE hInstance, int nCmdShow)
	{
		ZeroMemory(&windowStruct, sizeof(WNDCLASSEX));
		windowStruct.lpfnWndProc = WindowProc;
		windowStruct.style = CS_OWNDC;
		windowStruct.lpszClassName = _T("Window Class");
		windowStruct.hInstance = hInstance;
		windowStruct.cbSize = sizeof(WNDCLASSEX);
		RegisterClassEx(&windowStruct);
		hwnd = CreateWindowEx(NULL, _T("Window Class"), name, WS_OVERLAPPEDWINDOW, posX, posY, sizeX, sizeY, NULL, NULL, hInstance, NULL);
		ShowWindow(hwnd, nCmdShow);
		deviceContext = GetDC(hwnd);
		PIXELFORMATDESCRIPTOR pfd;
		ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
		pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_DRAW_TO_WINDOW;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 24;
		pfd.cAlphaBits = 8;
		pfd.cDepthBits = 32;
		SetPixelFormat(deviceContext, ChoosePixelFormat(deviceContext, &pfd), &pfd);
		context = wglCreateContext(deviceContext);
		wglMakeCurrent(deviceContext, context);
	}

	HWND getWindowHandle()
	{
		return hwnd;
	}

	HDC getDeviceContext()
	{
		return deviceContext;
	}

	void update()
	{
		SwapBuffers(deviceContext);
		if (GetKeyState(VK_ESCAPE) <= -127)
		{
			SendMessage(this->hwnd, WM_DESTROY, 0, 0);
		}
	}
};