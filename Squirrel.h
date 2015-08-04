#pragma once

#include <windows.h>
#include <msctf.h>

// {7841FDFF-FBE7-4D1F-9E57-F56CAF7F05A5}
static const GUID guid = 
{ 0x7841fdff, 0xfbe7, 0x4d1f, { 0x9e, 0x57, 0xf5, 0x6c, 0xaf, 0x7f, 0x5, 0xa5 } };

class Squirrel : IUnknown, ITfTextInputProcessor, ITfKeyEventSink
{
	public:
		int count;
		ITfThreadMgr *ptim;
		TfClientId tid;
		
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
		
};
