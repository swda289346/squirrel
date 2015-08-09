#pragma once

#include <vector>
#include <string>
#include <windows.h>
using namespace std;

class CandidateWindow
{
	public:
		HINSTANCE hInstance;
		WNDCLASSEX wc;
		HWND hwnd;
		int page;
		vector<wstring> candidates;
		HFONT font;
		
		CandidateWindow(HINSTANCE hInstance, HWND parent, vector<wstring> candidates, int x, int y);
		~CandidateWindow();
		void nextPage();
		void update();
};
