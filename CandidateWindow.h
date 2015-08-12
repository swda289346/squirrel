#pragma once

#include <vector>
#include <string>
#include <functional>
#include <windows.h>
using namespace std;

class CandidateWindow
{
	public:
		HINSTANCE hInstance;
		WNDCLASSEX wc;
		HWND hwnd;
		int pos;
		int page;
		vector<wstring> candidates;
		HFONT font;
		function<void()> onSelect;
		
		CandidateWindow(HINSTANCE hInstance, HWND parent, vector<wstring> candidates, int x, int y);
		~CandidateWindow();
		void show();
		void hide();
		void nextItem();
		void lastItem();
		void nextPage();
		void lastPage();
		void onClick(short x, short y);
		wstring getCandidate() const;
		wstring getCandidate(int pos) const;
		void update();
};
