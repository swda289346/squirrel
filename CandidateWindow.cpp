#include <map>
#include <windows.h>
#include "CandidateWindow.h"
#include "util.h"
using namespace std;

static map<HWND, CandidateWindow *> m;

static LRESULT CALLBACK myWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_PAINT:
			m[hwnd]->update();
			break;
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}

CandidateWindow::CandidateWindow(HINSTANCE hInstance, HWND parent, vector<wstring> candidates, int x, int y) : hInstance(hInstance), page(0), candidates(candidates)
{
	HRESULT hr;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = myWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = L"Candidate";
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	hr = RegisterClassEx(&wc);
	lprintf("RegisterClassEx %08x\n", hr);
	hwnd = CreateWindowEx(WS_EX_TOOLWINDOW|WS_EX_TOPMOST, L"Candidate", L"Candidate", WS_POPUP|WS_CLIPCHILDREN, x, y, 70, 360, parent, NULL, hInstance, NULL);
	lprintf("CreateWindow %08x\n", hwnd);
	m[hwnd] = this;
	font = (HFONT) GetStockObject(DEFAULT_GUI_FONT);
	LOGFONT logFont;
	GetObject(font, sizeof(LOGFONT), &logFont);
	logFont.lfHeight = 36;
	logFont.lfWeight = FW_BOLD;
	font = CreateFontIndirect(&logFont);
	ShowWindow(hwnd, SW_SHOWNOACTIVATE);
	UpdateWindow(hwnd);
}

CandidateWindow::~CandidateWindow()
{
	DestroyWindow(hwnd);
	UnregisterClass(L"Candidate", hInstance);
}

void CandidateWindow::nextPage()
{
	lout << "CandidateWindow::nextPage" << endl;
	page++;
	if (candidates.size()<=page*9)
		page = 0;
	RECT rec;
	SetRect(&rec, 0, 0, 70, 360);
	InvalidateRect(hwnd, &rec, TRUE);
}

static void setText(HWND hwnd, HDC hdc, int x, int y, wstring text)
{
	RECT rec;
	SetRect(&rec, 0, 0, 70, 360);
	TextOut(hdc, x, y, text.c_str(), text.size());
}

void CandidateWindow::update()
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps);
	SelectObject(hdc, font);
	setText(hwnd, hdc, 0, 0, L"1");
	setText(hwnd, hdc, 0, 40, L"2");
	setText(hwnd, hdc, 0, 80, L"3");
	setText(hwnd, hdc, 0, 120, L"4");
	setText(hwnd, hdc, 0, 160, L"5");
	setText(hwnd, hdc, 0, 200, L"6");
	setText(hwnd, hdc, 0, 240, L"7");
	setText(hwnd, hdc, 0, 280, L"8");
	setText(hwnd, hdc, 0, 320, L"9");
	for (int i=0;i<9;i++)
	{
		int pos = page*9+i;
		if (pos<candidates.size())
		{
			lout << "draw " << pos << endl;
			const wstring text = candidates[pos];
			setText(hwnd, hdc, 30, 40*i, text);
		}
	}
	EndPaint(hwnd, &ps);
	ReleaseDC(hwnd, hdc);
}
