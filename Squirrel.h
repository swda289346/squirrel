#pragma once

#include <map>
#include <vector>
#include <string>
#include <windows.h>
#include <msctf.h>
#include "CandidateWindow.h"
#include "KeyState.h"
using namespace std;

// {7841FDFF-FBE7-4D1F-9E57-F56CAF7F05A5}
static const GUID guid = 
{ 0x7841fdff, 0xfbe7, 0x4d1f, { 0x9e, 0x57, 0xf5, 0x6c, 0xaf, 0x7f, 0x5, 0xa5 } };

class Squirrel : IUnknown, ITfTextInputProcessor, ITfSource, ITfLangBarItemButton, ITfKeyEventSink, ITfEditSession, ITfCompositionSink
{
	public:
		int count;
		bool enabled;
		ITfThreadMgr *ptim;
		TfClientId tid;
		TF_LANGBARITEMINFO langBarItemInfo;
		ITfLangBarItemSink *langBarItemSink;
		ITfContext *pic;
		wchar_t textToSet;
		ITfComposition *composition;
		CandidateWindow *candidateWindow;
		map<wstring, vector<wstring>> codeTable;
		KeyState keyState;
		
		Squirrel();
		void putChar(ITfContext *pic, wchar_t c);
		void disable();
		
		HRESULT __stdcall QueryInterface(REFIID iid, void **ret);
		ULONG __stdcall AddRef();
		ULONG __stdcall Release();
		
		STDMETHODIMP Activate(ITfThreadMgr *ptim, TfClientId tid);
		STDMETHODIMP Deactivate();
		
		HRESULT __stdcall AdviseSink(REFIID riid, IUnknown *punk, DWORD *pdwCookie);
		HRESULT __stdcall UnadviseSink(DWORD pdwCookie);
		
		HRESULT __stdcall GetInfo(TF_LANGBARITEMINFO *pInfo);
		HRESULT __stdcall GetStatus(DWORD *pdwStatus);
		HRESULT __stdcall GetTooltipString(BSTR *pbstrToolTip);
		HRESULT __stdcall Show(BOOL fShow);
		
		HRESULT __stdcall GetIcon(HICON *phIcon);
		HRESULT __stdcall GetText(BSTR *pbstrText);
		HRESULT __stdcall InitMenu(ITfMenu *pMenu);
		HRESULT __stdcall OnClick(TfLBIClick click, POINT pt, const RECT *prcArea);
		HRESULT __stdcall OnMenuSelect(UINT wID);
		
		STDMETHODIMP OnKeyDown(ITfContext *pic, WPARAM wParam, LPARAM lParam, BOOL *pfEaten);
		STDMETHODIMP OnKeyUp(ITfContext *pic, WPARAM wParam, LPARAM lParam, BOOL *pfEaten);
		STDMETHODIMP OnPreservedKey(ITfContext *pic, REFGUID rguid, BOOL *pfEaten);
		STDMETHODIMP OnSetFocus(BOOL fForeground);
		STDMETHODIMP OnTestKeyDown(ITfContext *pic, WPARAM wParam, LPARAM lParam, BOOL *pfEaten);
		STDMETHODIMP OnTestKeyUp(ITfContext *pic, WPARAM wParam, LPARAM lParam, BOOL *pfEaten);
		
		HRESULT __stdcall DoEditSession(TfEditCookie ec);
		
		HRESULT __stdcall OnCompositionTerminated(TfEditCookie ecWrite, ITfComposition *pComposition);
};
