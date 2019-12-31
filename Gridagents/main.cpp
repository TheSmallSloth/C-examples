#include<Windows.h>
#include<gdiplus.h>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")
#include<vector>
#include<string>
#include"Agent.h"
#include"Grid.h"

Grid* pGrid;

int state = 0;
std::wstring stateMsgs[3];
std::wstring valMsgs[3];

LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_PAINT:
		{
			if (state == 3)
			{
				pGrid->draw(hwnd);
			}
			else
			{
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(hwnd, &ps);
				Graphics graphics(hdc);

				FontFamily  fontFamily(L"Times New Roman");
				Font        font(&fontFamily, 24, FontStyleRegular, UnitPixel);
				PointF      pointF(100.0f, 300.0f);
				PointF      point2F(100.0f, 400.0f);
				SolidBrush  solidBrush(Color(255, 255, 255, 255));
				SolidBrush backGroundBrush(Color(255, 0, 0, 0));

				graphics.FillRectangle(&backGroundBrush, 0, 0, 800, 800);
				graphics.DrawString(stateMsgs[state].c_str(), -1, &font, pointF, &solidBrush);
				graphics.DrawString(valMsgs[state].c_str(), -1, &font, point2F, &solidBrush);
			}
			return 0;
		}
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}
		case WM_KEYDOWN:
		{
			if (state == 3)
			{
				if (pGrid->isFinished())
				{
					PostQuitMessage(0);
				}
				else if (wParam == VK_SPACE)
				{
					pGrid->update();
					RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
				}
			}
			else
			{
				if (wParam>=0x30 && wParam <= 0x39)
				{
					valMsgs[state].push_back((wchar_t)wParam);
				}
				else if (wParam >= 0x60 && wParam <= 0x69)
				{
					valMsgs[state].push_back((wchar_t)(wParam-0x30));
				}
				else if (wParam == VK_BACK)
				{
					if (valMsgs[state].size() > 0)
					{
						valMsgs[state].pop_back();
					}
				}
				else if (wParam == VK_ESCAPE)
				{
					pGrid = new Grid(100, 512, 20);
					state = 3;
				}
				else if (wParam == VK_RETURN)
				{
					if (valMsgs[state].size() == 0)
					{
						return 0;
					}
					if (state == 2)
					{
						unsigned int gridsize = std::stoi(valMsgs[0]);
						unsigned int numObstacles = std::stoi(valMsgs[1]);
						unsigned int numAgents = std::stoi(valMsgs[2]);
						if ((numObstacles + 2 * numAgents) > (gridsize * gridsize))
						{
							MessageBox(hwnd, "Too many obstacles/agents for grid size, default values used", "Error", MB_OK);
							pGrid = new Grid(100, 512, 20);
						}
						else
						{
							pGrid = new Grid(gridsize, numObstacles, numAgents);
						}
					}
					state++;
				}
				RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
			}
			return 0;
		}
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
	//GDI+ Startup
	ULONG_PTR gdiplusToken;
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	//Creating window
	char wndClassName[] = "Application Window";
	char wndTitle[] = "Grid";

	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WindowProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIconA(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursorA(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = TEXT(wndClassName);
	wcex.hIconSm = LoadIconA(hInstance, IDI_APPLICATION);

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,
			"Call to RegisterClassEx failed!",
			"Error",
			NULL);
		return GetLastError();
	}

	HWND hwnd = CreateWindow(TEXT(wndClassName),
		TEXT(wndTitle),
		WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		800,
		800,
		NULL,
		NULL,
		hInstance,
		NULL);
	if (!hwnd)
	{
		MessageBox(NULL,
			"Call to CreateWindow failed!",
			"Error",
			NULL);

		return 1;
	}

	stateMsgs[0] = std::wstring(L"Press escape to use default values or\ntype the grid size then press enter.");
	stateMsgs[1] = std::wstring(L"Press escape to use default values or\ntype the number of obstacles then press enter.");
	stateMsgs[2] = std::wstring(L"Press escape to use default values or\ntype the number of agents then press enter.");
	valMsgs[0] = std::wstring();
	valMsgs[1] = std::wstring();
	valMsgs[2] = std::wstring();

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	GdiplusShutdown(gdiplusToken);
	delete pGrid;
	return 0;
}