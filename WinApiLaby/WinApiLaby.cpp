// WinApiLaby.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "WinApiLaby.h"
#include <string>
#include <tchar.h>
#include <time.h>
#include <chrono>
#include <fstream>
#include <iostream>
#include <winbase.h>

#define MAX_LOADSTRING 100
#define MAX_WORDS 16825
//  4 1
//	3 2
#define DrawRect1(hdc, l,t,r,b, curv){ \
int ov = (r-l)/2; \
int oh = (b-t)/2; \
RoundRect(hdc, l + ov , t, r, b - oh, curv, curv); \
Rectangle(hdc, l + ov , t, r-curv,b - oh); \
Rectangle(hdc, l + ov , t+curv, r,b - oh);}

#define DrawRect2(hdc, l,t,r,b, curv){ \
int ov = (r-l)/2; \
int oh = (b-t)/2; \
RoundRect(hdc, l+ov, t+oh, r, b, curv, curv); \
Rectangle(hdc, l+ov, t+oh, r,b-curv); \
Rectangle(hdc, l+ov, t+oh, r-curv,b);}

#define DrawRect3(hdc, l,t,r,b, curv){ \
int ov = (r-l)/2; \
int oh = (b-t)/2; \
RoundRect(hdc, l, t+oh, r-ov, b, curv, curv); \
Rectangle(hdc, l, t+oh, r-ov,b-curv); \
Rectangle(hdc, l+ curv, t+curv+oh, r-ov,b);}

#define DrawRect4(hdc, l,t,r,b, curv){ \
int ov = (r-l)/2; \
int oh = (b-t)/2; \
RoundRect(hdc, l, t, r-ov, b-oh, curv, curv); \
Rectangle(hdc, l+curv, t, r-ov,b-oh); \
Rectangle(hdc, l, t+curv, r-ov,b-oh);}

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND mainWindow[4];
HWND keyboard;
int letters = 0;
int boards = 1;
int attempts;
TCHAR words[MAX_WORDS][6] = {};
int wordsNo = 0;
int passwords[4];
TCHAR buffor[5];
bool solved[4];

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK KeyBoardProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK KeyProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
int Find(TCHAR* tab);
void ResetKeyboard(HDC hdc, int width, int height);

void InitEasy();
void InitMed();
void InitHard();
void closeBoards();
ATOM RegisterOverlay(HINSTANCE hInstance);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = KeyBoardProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(CreateSolidBrush(RGB(255, 255, 255)));
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDR_MENU1);
	wcex.lpszClassName = L"keyboard";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));
	RegisterClassExW(&wcex);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = KeyProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(CreateSolidBrush(RGB(255, 255, 255)));
	wcex.lpszMenuName = 0;
	wcex.lpszClassName = L"key";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));
	RegisterClassExW(&wcex);


	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_WINAPILABY, szWindowClass, MAX_LOADSTRING);
	std::wstring title(L"WORDLE - PUZZLE \0");
	title.copy(szTitle, title.size());

	MyRegisterClass(hInstance);
	RegisterOverlay(hInstance);
	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINAPILABY));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(CreateSolidBrush(RGB(255, 255, 255)));
	wcex.lpszMenuName = 0;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));

	return RegisterClassExW(&wcex);
}

ATOM RegisterOverlay(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = KeyProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(CreateSolidBrush(RGB(0, 255, 0)));
	wcex.lpszMenuName = 0;
	wcex.lpszClassName = L"gOverlay";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));

	return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	/*FILE* in;
	intfopen_s(&in, "Wordle.txt", "r");*/
	std::wifstream in;
	in.open(".\\Wordle.txt");
	TCHAR w[6];
	//std::string w;

	for (int i = 0; i < MAX_WORDS && in >> w; i++)
	{
		if (w[4] == '\0' || w[4] == '\n')
		{
			continue;
		}
		_tcsncpy_s(words[i], w, 6);
		wordsNo = i + 1;
	}

	hInst = hInstance;
	// https://stackoverflow.com/questions/3275989/disable-window-resizing-win32
	keyboard = CreateWindowW(L"keyboard", L"WORDLE - KEYBORD", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInst, nullptr);

	// https://forums.codeguru.com/showthread.php?500841-Set-my-window-position-at-screen-center
	srand(time(NULL));
	int xSize = rand() % (GetSystemMetrics(SM_CXSCREEN) / 4) + (GetSystemMetrics(SM_CXSCREEN) / 4);
	int ySize = rand() % (GetSystemMetrics(SM_CYSCREEN) / 4) + (GetSystemMetrics(SM_CYSCREEN) / 4);

	int xPos = (GetSystemMetrics(SM_CXSCREEN) - xSize) / 2;
	int yPos = (GetSystemMetrics(SM_CYSCREEN) - 2 * ySize) / 4;

	SetWindowLong(keyboard, GWL_EXSTYLE,
		GetWindowLong(keyboard, GWL_EXSTYLE) | WS_EX_LAYERED);
	SetLayeredWindowAttributes(keyboard, 0, (255 * 50) / 100, LWA_ALPHA);

	MoveWindow(keyboard, xPos, 3 * yPos + ySize, xSize, ySize, true);

	// https://stackoverflow.com/questions/14989062/set-a-window-to-be-topmost
	SetWindowPos(keyboard, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	ShowWindow(keyboard, 1);
	UpdateWindow(keyboard);
	TCHAR s[2];
	GetPrivateProfileString(L"WORDLE", L"DIFF", L"1", (TCHAR*)s, 2, L".\\wordle.ini");

	switch (_wtoi(s))
	{
	case 1:
	{
		SendMessage(keyboard, WM_COMMAND, ID_DIFFICULTY_EASY, NULL);
	}
	break;
	case 2:
	{
		SendMessage(keyboard, WM_COMMAND, ID_DIFFICULTY_MEDIUM, NULL);
	}
	break;
	case 4:
	{
		SendMessage(keyboard, WM_COMMAND, ID_DIFFICULTY_HARD, NULL);
	}
	break;
	default:
	{
		SendMessage(keyboard, WM_COMMAND, ID_DIFFICULTY_EASY, NULL);
	}
	break;
	}


	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	const int bufSize = 256;
	TCHAR buf[bufSize];

	switch (message)
	{
	case WM_NCHITTEST:
		return HTCAPTION;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_SIZE:
	{
		//InvalidateRect(hWnd,NULL, TRUE);
		break;
	}
	case WM_PAINT:
	{
		RECT rc;
		GetClientRect(hWnd, &rc);
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		HPEN pen = CreatePen(PS_DOT, 2, RGB(164, 174, 196));
		HPEN oldPen = (HPEN)SelectObject(hdc, pen);
		HBRUSH brush = CreateSolidBrush(RGB(251, 252, 255));
		HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);

		for (int i = 0; i < 5; i++)
			for (int j = 0; j < attempts; j++)
			{
				RoundRect(hdc, 6 + i * 61, 6 + j * 61, 61 + i * 61, 61 + j * 61, 10, 10);
			}

		SelectObject(hdc, oldPen);
		DeleteObject(pen);
		SelectObject(hdc, oldBrush);
		DeleteObject(brush);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		//PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK KeyProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		// TODO: Add any drawing code that uses hdc here...
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		//PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK KeyBoardProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HDC offDC = NULL;
	static HBITMAP offOldBitmap = NULL;
	static HBITMAP offBitmap = NULL;

	static std::chrono::system_clock::time_point start;
	static int row;
	static int height;
	static int width;

	static RECT keyboardrc;

	switch (message)
	{
	case WM_ERASEBKGND:
		return 1;
	case WM_CREATE:
	{
		HDC hdc = GetDC(hWnd);
		offDC = CreateCompatibleDC(hdc);
		ReleaseDC(hWnd, hdc);
		SetTimer(hWnd, 1, 25, NULL);
		start = std::chrono::system_clock::now();
		HMENU hmenu = GetMenu(hWnd);
		//CheckMenuItem(hmenu, ID_DIFFICULTY_EASY, MF_CHECKED);

		break;
	}
	case WM_TIMER:
	{
		std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed_seconds = now - start;
		TCHAR s[256];
		swprintf_s(s, 256,
			L"WORDLE - KEYBORD: %2.4f",
			elapsed_seconds.count());
		SetWindowText(hWnd, s);
		break;
	}
	//case WM_SYSCOMMAND:
	//{
	//	GetClientRect(hWnd, &keyboardrc);
	//	int clientWidth = LOWORD(lParam);
	//	int clientHeight = HIWORD(lParam);
	//	HDC hdc = GetDC(hWnd);
	//	BitBlt(offDC, 0, 0, keyboardrc.right, keyboardrc.bottom, hdc, 0, 0, SRCCOPY);
	//	switch (wParam)
	//	{
	//	case SC_MINIMIZE:
	//		BitBlt(offDC, 0, 0, keyboardrc.right, keyboardrc.bottom, hdc, 0, 0, SRCCOPY);
	//		DefWindowProc(hWnd, message, wParam, lParam);
	//		break;
	//	case SC_RESTORE:
	//		//ResetKeyboard(offDC, clientWidth, clientHeight);
	//		FillRect(offDC, &keyboardrc, CreateSolidBrush(RGB(255, 0, 0)));
	//		BitBlt(hdc, 0, 0, keyboardrc.right, keyboardrc.bottom, offDC, 0, 0, SRCCOPY);
	//		DefWindowProc(hWnd, message, wParam, lParam);
	//		break;

	//	default:
	//		DefWindowProc(hWnd, message, wParam, lParam);
	//	}
	//}
	break;
	case WM_SIZE:
	{
		GetClientRect(hWnd, &keyboardrc);
		int clientWidth = LOWORD(lParam);
		int clientHeight = HIWORD(lParam);
		HDC hdc = GetDC(hWnd);
		//InvalidateRect(hWnd, &keyboardrc, FALSE);
		//BitBlt(offDC, 0, 0, keyboardrc.right, keyboardrc.bottom, hdc, 0, 0, SRCCOPY);
		UpdateWindow(hWnd);
		if (offOldBitmap != NULL) {
			SelectObject(offDC, offOldBitmap);

		}
		if (offBitmap != NULL) {
			DeleteObject(offBitmap);

		}
		offBitmap = CreateCompatibleBitmap(hdc, clientWidth, clientHeight);
		offOldBitmap = (HBITMAP)SelectObject(offDC, offBitmap);
		HBRUSH brush = CreateSolidBrush(RGB(255, 255, 255));
		RECT rc = { 0,0,clientWidth, clientHeight };
		FillRect(offDC, &rc, brush);
		DeleteObject(brush);
		ReleaseDC(hWnd, hdc);
	}
	break;
	case WM_CHAR:
	{
		if (wParam == VK_BACK)
		{
			if (letters <= 0) break;;
			letters--;
			int col = letters % 5;
			RECT rc;

			for (int i = 0; i < boards; i++)
			{
				if (solved[i]) continue;
				HDC hdc = GetDC(mainWindow[i]);
				HPEN pen = CreatePen(PS_DOT, 2, RGB(164, 174, 196));
				HPEN oldPen = (HPEN)SelectObject(hdc, pen);
				HBRUSH brush = CreateSolidBrush(RGB(251, 252, 255));
				HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
				RoundRect(hdc, (col + 1) * 6 + col * 55, (row + 1) * 6 + row * 55, (col + 1) * 61, (row + 1) * 61, 10, 10);
				SelectObject(hdc, oldPen);
				DeleteObject(pen);
				SelectObject(hdc, oldBrush);
				DeleteObject(brush);
				UpdateWindow(hWnd);
			}
			break;
		}
		else if (wParam == VK_RETURN)
		{
			if (letters == 5)
			{
				int p = Find(buffor);
				if (p >= 0)
				{
					for (int k = 0; k < boards; k++)
					{
						if (solved[k]) continue;
						int guess[5] = {};
						for (int i = 0; i < 5; i++)
							for (int j = 0; j < 5; j++)
							{
								if (buffor[i] == words[passwords[k]][j])
								{
									if (i == j)
									{
										guess[i] = 2;
										break;
									}
									guess[i] = 1;
								}
							}
						HDC hdc = GetDC(mainWindow[k]);
						//HDC keyHDC = GetDC(hWnd);
						COLORREF color[3] = { RGB(164, 174, 196), RGB(243,194,55), RGB(121,184,81) };

						HPEN pen[3] = { CreatePen(PS_DOT, 2, color[0]),  CreatePen(PS_DOT, 2, color[1]), CreatePen(PS_DOT, 2, color[2]) };
						HPEN oldPen = (HPEN)SelectObject(hdc, pen);
						HBRUSH brush[3] = { CreateSolidBrush(color[0]), CreateSolidBrush(color[1]), CreateSolidBrush(color[2]) };
						HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);

						RECT rc;


						for (int col = 0; col < 5; col++)
						{
							RECT sq = { (col + 1) * 6 + col * 55, (row + 1) * 6 + row * 55, (col + 1) * 61, (row + 1) * 61 };
							RoundRect(hdc, (col + 1) * 6 + col * 55, (row + 1) * 6 + row * 55, (col + 1) * 61, (row + 1) * 61, 10, 10);

							SelectObject(hdc, pen[guess[col]]);
							SelectObject(hdc, brush[guess[col]]);
							RoundRect(hdc, (col + 1) * 6 + col * 55, (row + 1) * 6 + row * 55, (col + 1) * 61, (row + 1) * 61, 10, 10);

							//SendMessageW(mainWindow[k], WM_COMMAND, (WPARAM)1000, NULL);

							SelectObject(offDC, pen[guess[col]]);
							SelectObject(offDC, brush[guess[col]]);

							std::wstring letters[3] = { L"qwertyuiop", L"asdfghjkl", L"zxcvbnm" };
							int leng[3] = { 10,9,7 };

							wchar_t literka = buffor[col];
							for (int y = 0; y < 3; y++)
								for (int x = 0; x < leng[y]; x++)
								{
									if (letters[y][x] == literka)
									{
										int off;
										switch (y)
										{
										case 0: off = 10; break;
										case 1: off = 10 + width / 2; break;
										case 2: off = 10 + width * 3 / 2; break;
										}
										int curv = 10;
										int l = off + (width + 5) * x;
										int t = 10 + (height + 5) * y;
										int r = off + (width + 5) * x + width;
										int b = 10 + (height + 5) * y + height;

										switch (boards)
										{
										case 1:
										{
											DrawRect1(offDC, l, t, r, b, curv);
											DrawRect2(offDC, l, t, r, b, curv);
											DrawRect3(offDC, l, t, r, b, curv);
											DrawRect4(offDC, l, t, r, b, curv);
											break;
										}
										case 2:
										{
											if (k == 0)
											{
												DrawRect3(offDC, l, t, r, b, curv);
												DrawRect4(offDC, l, t, r, b, curv);
											}
											else
											{
												DrawRect1(offDC, l, t, r, b, curv);
												DrawRect2(offDC, l, t, r, b, curv);
											}
											break;
										}
										case 4:
										{
											switch (k)
											{
											case 0: DrawRect4(offDC, l, t, r, b, curv); break;
											case 1: DrawRect1(offDC, l, t, r, b, curv); break;
											case 2: DrawRect3(offDC, l, t, r, b, curv); break;
											case 3: DrawRect2(offDC, l, t, r, b, curv); break;
											}

											break;
										}
										}
										rc = { l,t,r,b };
										TCHAR c = toupper(buffor[col]);
										SetBkMode(offDC, TRANSPARENT);
										DrawText(offDC, &c, 1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
										UpdateWindow(keyboard);
									}

								}


							rc = { (col + 1) * 6 + col * 55, (row + 1) * 6 + row * 55, (col + 1) * 61, (row + 1) * 61 };


							SetBkMode(hdc, TRANSPARENT);
							TCHAR c = toupper(buffor[col]);
							DrawText(hdc, &c, 1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
						}
						SelectObject(hdc, oldPen);
						DeleteObject(pen[0]);
						DeleteObject(pen[1]);
						DeleteObject(pen[2]);
						SelectObject(hdc, oldBrush);
						DeleteObject(brush[0]);
						DeleteObject(brush[1]);
						DeleteObject(brush[2]);


						/////////////////////////////////
						HDC keyP = GetDC(keyboard);
						BitBlt(keyP, 0, 0, keyboardrc.right, keyboardrc.bottom, offDC, 0, 0, SRCCOPY);
						ReleaseDC(keyboard, keyP);
						UpdateWindow(mainWindow[k]);
					}
					row++; letters = 0;
					for (int i = 0; i < boards; i++)
					{
						if (p == passwords[i])
						{
							solved[i] = true;
							RECT rc;
							GetClientRect(mainWindow[i], &rc);


							HDC hdc = GetDC(mainWindow[i]);;

							HDC memdc = CreateCompatibleDC(hdc);

							HBITMAP hbitmap = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);

							HGDIOBJ holdbmp = SelectObject(memdc, hbitmap);

							HBRUSH brush = CreateSolidBrush(RGB(0, 255, 0));
							FillRect(memdc, &rc, brush);

							AlphaBlend(hdc, 0, 0, rc.right, rc.bottom, memdc, 0, 0, rc.right, rc.bottom, _BLENDFUNCTION{ 0, 0, 128, 0 });

							SelectObject(memdc, holdbmp);

							DeleteObject(hbitmap);
							DeleteObject(brush);
							DeleteDC(memdc);
							SetBkMode(hdc, TRANSPARENT);
							SetTextColor(hdc, RGB(255, 255, 255));
							DrawText(hdc, words[passwords[i]], 5, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
						}
					}
					if (row == attempts)
					{
						for (int i = 0; i < boards; i++)
						{
							if (!solved[i])
							{
								solved[i] = true;
								RECT rc;
								GetClientRect(mainWindow[i], &rc);


								HDC hdc = GetDC(mainWindow[i]);;

								HDC memdc = CreateCompatibleDC(hdc);

								HBITMAP hbitmap = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);

								HGDIOBJ holdbmp = SelectObject(memdc, hbitmap);

								HBRUSH brush = CreateSolidBrush(RGB(255, 0, 0));
								FillRect(memdc, &rc, brush);
								AlphaBlend(hdc, 0, 0, rc.right, rc.bottom, memdc, 0, 0, rc.right, rc.bottom, _BLENDFUNCTION{ 0, 0, 128, 0 });

								SelectObject(memdc, holdbmp);

								DeleteObject(hbitmap);
								DeleteObject(brush);
								DeleteDC(memdc);
								SetBkMode(hdc, TRANSPARENT);
								SetTextColor(hdc, RGB(255, 255, 255));
								DrawText(hdc, words[passwords[i]], 5, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
							}
						}
					}
				}
				else
				{
					while (letters > 0)
					{
						letters--;
						int col = letters % 5;
						RECT rc;

						for (int i = 0; i < boards; i++)
						{
							if (solved[i]) continue;
							HDC hdc = GetDC(mainWindow[i]);
							HPEN pen = CreatePen(PS_DOT, 2, RGB(164, 174, 196));
							HPEN oldPen = (HPEN)SelectObject(hdc, pen);
							HBRUSH brush = CreateSolidBrush(RGB(251, 252, 255));
							HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
							RoundRect(hdc, (col + 1) * 6 + col * 55, (row + 1) * 6 + row * 55, (col + 1) * 61, (row + 1) * 61, 10, 10);
							SelectObject(hdc, oldPen);
							DeleteObject(pen);
							SelectObject(hdc, oldBrush);
							DeleteObject(brush);
						}
						UpdateWindow(hWnd);
					}
				}
			}
			break;
		}

		TCHAR s[256];
		for (int i = 0; i < boards; i++)
		{
			swprintf_s(s, 256,
				L"WORDLE - PUZZLE: %s", words[passwords[i]]);
			SetWindowText(mainWindow[i], s);
		}

		if (letters < 5)
		{
			int col = letters % 5;
			RECT rc;
			TCHAR c = (TCHAR)wParam;
			c = toupper(c);
			buffor[letters % 5] = tolower(c);

			for (int i = 0; i < boards; i++)
			{
				if (solved[i]) continue;
				HDC hdc = GetDC(mainWindow[i]);
				rc = { (col + 1) * 6 + col * 55, (row + 1) * 6 + row * 55, (col + 1) * 61, (row + 1) * 61 };
				DrawText(hdc, &c, 1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				UpdateWindow(hWnd);
			}

			letters++;
		}
	}
	break;
	case WM_PAINT:
	{
		RECT rc;
		GetClientRect(hWnd, &rc);
		height = (rc.bottom - rc.top - 30) / 3;
		width = (rc.right - rc.left - 65) / 10;
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		GetClientRect(hWnd, &rc);
		ResetKeyboard(hdc, width, height);
		//BitBlt(hdc, 0, 0, rc.right, rc.bottom, offDC, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case ID_DIFFICULTY_EASY:
		{
			row = 0;
			boards = 1;
			HMENU hmenu = GetMenu(hWnd);
			CheckMenuItem(hmenu, ID_DIFFICULTY_EASY, MF_CHECKED);
			CheckMenuItem(hmenu, ID_DIFFICULTY_MEDIUM, MF_UNCHECKED);
			CheckMenuItem(hmenu, ID_DIFFICULTY_HARD, MF_UNCHECKED);

			InitEasy();
			ResetKeyboard(offDC, width, height);
			HDC hdc = GetDC(hWnd);
			BitBlt(hdc, 0, 0, keyboardrc.right, keyboardrc.bottom, offDC, 0, 0, SRCCOPY);
			start = std::chrono::system_clock::now();
			ReleaseDC(hWnd, hdc);
			break;
		}
		case ID_DIFFICULTY_MEDIUM:
		{
			row = 0;
			boards = 2;
			HMENU hmenu = GetMenu(hWnd);
			CheckMenuItem(hmenu, ID_DIFFICULTY_EASY, MF_UNCHECKED);
			CheckMenuItem(hmenu, ID_DIFFICULTY_MEDIUM, MF_CHECKED);
			CheckMenuItem(hmenu, ID_DIFFICULTY_HARD, MF_UNCHECKED);

			InitMed();
			ResetKeyboard(offDC, width, height);
			HDC hdc = GetDC(hWnd);
			BitBlt(hdc, 0, 0, keyboardrc.right, keyboardrc.bottom, offDC, 0, 0, SRCCOPY);
			start = std::chrono::system_clock::now();
			ReleaseDC(hWnd, hdc);
			break;
		}
		case ID_DIFFICULTY_HARD:
		{
			row = 0;
			boards = 4;
			HMENU hmenu = GetMenu(hWnd);
			CheckMenuItem(hmenu, ID_DIFFICULTY_EASY, MF_UNCHECKED);
			CheckMenuItem(hmenu, ID_DIFFICULTY_MEDIUM, MF_UNCHECKED);
			CheckMenuItem(hmenu, ID_DIFFICULTY_HARD, MF_CHECKED);

			InitHard();
			ResetKeyboard(offDC, width, height);
			HDC hdc = GetDC(hWnd);
			BitBlt(hdc, 0, 0, keyboardrc.right, keyboardrc.bottom, offDC, 0, 0, SRCCOPY);
			start = std::chrono::system_clock::now();
			ReleaseDC(hWnd, hdc);
			break;
		}
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_DESTROY:
		if (offOldBitmap != NULL) {
			SelectObject(offDC, offOldBitmap);

		}
		if (offDC != NULL) {
			DeleteDC(offDC);

		}
		if (offBitmap != NULL) {
			DeleteObject(offBitmap);

		}
		char s[2];
		sprintf_s(s, "%d", boards);
		WritePrivateProfileStringA("WORDLE", "DIFF", s, ".\\wordle.ini");
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{

	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void closeBoards()
{
	letters = 0;
	SendMessage(keyboard, WM_PAINT, NULL, NULL);
	for (int i = 0; i < 4; i++)
	{
		DestroyWindow(mainWindow[i]);
		solved[i] = false;
	}
	for (int i = 0; i < boards; i++)
		passwords[i] = rand() % wordsNo;
}

void InitEasy()
{
	closeBoards();
	mainWindow[0] = CreateWindowW(szWindowClass, szTitle, (WS_POPUPWINDOW | WS_VISIBLE | WS_CHILD | WS_OVERLAPPED | WS_CAPTION) & ~WS_SYSMENU, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, nullptr, nullptr);

	attempts = 6;
	RECT rc;
	rc.left = 0;
	rc.top = 0;
	rc.bottom = attempts * 55 + (attempts + 1) * 6;
	rc.right = 5 * 55 + 6 * 6;
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME, FALSE);
	int xSize = rc.right - rc.left;
	int ySize = rc.bottom - rc.top;

	// https://forums.codeguru.com/showthread.php?500841-Set-my-window-position-at-screen-center
	int xPos = (GetSystemMetrics(SM_CXSCREEN) - xSize) / 2;
	int yPos = (GetSystemMetrics(SM_CYSCREEN) - ySize) / 2;

	MoveWindow(mainWindow[0], xPos, yPos, xSize, ySize, true);
	ShowWindow(mainWindow[0], 1);
	UpdateWindow(mainWindow[0]);

}

void InitMed()
{
	closeBoards();
	attempts = 8;
	RECT rc;
	rc.left = 0;
	rc.top = 0;
	rc.bottom = attempts * 55 + (attempts + 1) * 6;
	rc.right = 5 * 55 + 6 * 6;
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME, FALSE);
	int xSize = rc.right - rc.left;
	int ySize = rc.bottom - rc.top;

	// https://forums.codeguru.com/showthread.php?500841-Set-my-window-position-at-screen-center
	int xPos = (GetSystemMetrics(SM_CXSCREEN) - 2 * xSize) / 4;
	int yPos = (GetSystemMetrics(SM_CYSCREEN) - ySize) / 2;

	for (int i = 0; i < 2; i++)
		mainWindow[i] = CreateWindowW(szWindowClass, szTitle,
			(WS_POPUPWINDOW | WS_VISIBLE | WS_CHILD | WS_OVERLAPPED | WS_CAPTION) & ~WS_SYSMENU,
			CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, nullptr, nullptr);

	MoveWindow(mainWindow[0], xPos, yPos, xSize, ySize, true);
	MoveWindow(mainWindow[1], 3 * xPos + xSize, yPos, xSize, ySize, true);
	ShowWindow(mainWindow[1], 1);
	ShowWindow(mainWindow[0], 1);

}

void InitHard()
{
	closeBoards();
	attempts = 10;
	RECT rc;
	rc.left = 0;
	rc.top = 0;
	rc.bottom = attempts * 55 + (attempts + 1) * 6;
	rc.right = 5 * 55 + 6 * 6;
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME, FALSE);
	int xSize = rc.right - rc.left;
	int ySize = rc.bottom - rc.top;

	// https://forums.codeguru.com/showthread.php?500841-Set-my-window-position-at-screen-center
	int xPos = (GetSystemMetrics(SM_CXSCREEN) - 2 * xSize) / 4;
	int yPos = (GetSystemMetrics(SM_CYSCREEN) - 2 * ySize) / 4;

	for (int i = 0; i < 4; i++)
		mainWindow[i] = CreateWindowW(szWindowClass, szTitle,
			(WS_POPUPWINDOW | WS_VISIBLE | WS_CHILD | WS_OVERLAPPED | WS_CAPTION) & ~WS_SYSMENU,
			CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, nullptr, nullptr);

	MoveWindow(mainWindow[0], xPos, yPos, xSize, ySize, true);
	MoveWindow(mainWindow[1], 3 * xPos + xSize, yPos, xSize, ySize, true);
	MoveWindow(mainWindow[2], xPos, 3 * yPos + ySize, xSize, ySize, true);
	MoveWindow(mainWindow[3], 3 * xPos + xSize, 3 * yPos + ySize, xSize, ySize, true);

	ShowWindow(mainWindow[0], 1);
	ShowWindow(mainWindow[1], 1);
	ShowWindow(mainWindow[2], 1);
	ShowWindow(mainWindow[3], 1);
}

int Find(TCHAR* tab)
{
	//int isr;
	//int k = str2num(tab);
	//int p = -1, L = 0, ip=0, ik = wordsNo - 1;
	//while (str2num(words[ip]) <= k && (k <= str2num(words[ik])))
	//{
	//	L++;
	//	isr = ip + (k - str2num(words[ip])) * (ik - ip) / (str2num(words[ik]) - str2num(words[ip]));
	//	if (str2num(words[isr]) == k)
	//	{
	//		p = isr; break;
	//	}
	//	else if (k < str2num(words[isr]))
	//		ik = isr - 1;
	//	else 
	//		ip = isr + 1;
	//}
	//return p;

	for (int i = 0; i < wordsNo; i++)
	{
		if (_tcscmp(words[i], tab) == 0) return i;
	}
	return -1;
}

void ResetKeyboard(HDC hdc, int width, int height)
{
	RECT rc;
	HPEN pen = CreatePen(PS_SOLID, 2, RGB(164, 174, 196));
	HPEN oldPen = (HPEN)SelectObject(hdc, pen);
	HBRUSH brush = CreateSolidBrush(RGB(251, 252, 255));
	HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);

	TCHAR letters[3][11] = { _T("QWERTYUIOP"), _T("ASDFGHJKL"), _T("ZXCVBNM") };

	for (int i = 0; i < 10; i++)
	{
		RoundRect(hdc, 10 + i * (width + 5), 10, 10 + i * (width + 5) + width, 10 + height, 10, 10);
		rc = { 10 + i * (width + 5), 10, 10 + i * (width + 5) + width, 10 + height };
		//SetBkColor(hdc, TRANSPARENT);
		DrawText(hdc, &letters[0][i], 1, &rc,
			DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}


	for (int i = 0; i < 9; i++)
	{
		RoundRect(hdc, 10 + i * (width + 5) + width / 2, 15 + height, 10 + i * (width + 5) + width * 3 / 2, 15 + 2 * height, 10, 10);
		rc = { 10 + i * (width + 5) + width / 2, 15 + height, 10 + i * (width + 5) + width * 3 / 2, 15 + 2 * height };
		DrawText(hdc, &letters[1][i], 1, &rc,
			DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}

	for (int i = 0; i < 7; i++)
	{
		RoundRect(hdc, 10 + i * (width + 5) + 3 * width / 2, 20 + 2 * height, 10 + i * (width + 5) + width * 5 / 2, 20 + 3 * height, 10, 10);
		rc = { 10 + i * (width + 5) + 3 * width / 2, 20 + 2 * height, 10 + i * (width + 5) + width * 5 / 2, 20 + 3 * height };
		DrawText(hdc, &letters[2][i], 1, &rc,
			DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}

	SelectObject(hdc, oldPen);
	DeleteObject(pen);
	SelectObject(hdc, oldBrush);
	DeleteObject(brush);

	//HFONT hFont = CreateFont(20, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Arial");
	//SendMessage(keyboard, WM_SETFONT, WPARAM(hFont), TRUE);
}
