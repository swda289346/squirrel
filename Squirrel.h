#pragma once

#include <map>
#include <vector>
#include <string>
#include <windows.h>
#include <msctf.h>
#include "CandidateWindow.h"
#include "KeyState.h"
#include "SquirrelLangBarItemButton.h"
using namespace std;

#ifdef _DEBUG
// {AA95DE4F-99FA-41CF-844C-57D01901E740}
static const GUID guid = 
{ 0xaa95de4f, 0x99fa, 0x41cf, { 0x84, 0x4c, 0x57, 0xd0, 0x19, 0x1, 0xe7, 0x40 } };
#else
// {7841FDFF-FBE7-4D1F-9E57-F56CAF7F05A5}
static const GUID guid = 
{ 0x7841fdff, 0xfbe7, 0x4d1f, { 0x9e, 0x57, 0xf5, 0x6c, 0xaf, 0x7f, 0x5, 0xa5 } };
#endif

class Squirrel : IUnknown, ITfTextInputProcessor, ITfKeyEventSink, ITfEditSession, ITfCompositionSink, ITfThreadFocusSink, ITfThreadMgrEventSink
{
	public:
		int count;
		bool enabled;
		bool disabled;
		ITfThreadMgr *ptim;
		TfClientId tid;
		ITfLangBarItemSink *langBarItemSink;
		ITfContext *pic;
		wchar_t textToSet;
		ITfComposition *composition;
		CandidateWindow *candidateWindow;
		map<wstring, vector<wstring>> codeTable;
		KeyState keyState;
		SquirrelLangBarItemButton *lbi, *lbiTray;
		
		Squirrel();
		void putChar(ITfContext *pic, wchar_t c);
		void putCharToComposition(wchar_t c);
		void disable();
		void completeComposition(TfEditCookie ec);
		
		HRESULT __stdcall QueryInterface(REFIID iid, void **ret);
		ULONG __stdcall AddRef();
		ULONG __stdcall Release();
		
		STDMETHODIMP Activate(ITfThreadMgr *ptim, TfClientId tid);
		STDMETHODIMP Deactivate();
		
		STDMETHODIMP OnKeyDown(ITfContext *pic, WPARAM wParam, LPARAM lParam, BOOL *pfEaten);
		STDMETHODIMP OnKeyUp(ITfContext *pic, WPARAM wParam, LPARAM lParam, BOOL *pfEaten);
		STDMETHODIMP OnPreservedKey(ITfContext *pic, REFGUID rguid, BOOL *pfEaten);
		STDMETHODIMP OnSetFocus(BOOL fForeground);
		STDMETHODIMP OnTestKeyDown(ITfContext *pic, WPARAM wParam, LPARAM lParam, BOOL *pfEaten);
		STDMETHODIMP OnTestKeyUp(ITfContext *pic, WPARAM wParam, LPARAM lParam, BOOL *pfEaten);
		
		HRESULT __stdcall DoEditSession(TfEditCookie ec);
		
		HRESULT __stdcall OnCompositionTerminated(TfEditCookie ecWrite, ITfComposition *pComposition);
		
		HRESULT __stdcall OnKillThreadFocus();
		HRESULT __stdcall OnSetThreadFocus();
		
		HRESULT __stdcall OnInitDocumentMgr(ITfDocumentMgr *pdim);
		HRESULT __stdcall OnPopContext(ITfContext *pic);
		HRESULT __stdcall OnPushContext(ITfContext *pic);
		HRESULT __stdcall OnSetFocus(ITfDocumentMgr *pdimFocus, ITfDocumentMgr *pdimPrevFocus);
		HRESULT __stdcall OnUninitDocumentMgr(ITfDocumentMgr *pdim);
};
