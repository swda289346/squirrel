#pragma once

#include <msctf.h>

class Squirrel;

class SquirrelLangBarItemButton : IUnknown, ITfSource, ITfLangBarItemButton
{
	public:
		int count;
		Squirrel *parent;
		TF_LANGBARITEMINFO langBarItemInfo;
		ITfLangBarItemSink *langBarItemSink;
		
		SquirrelLangBarItemButton(Squirrel *parent, GUID type);
		~SquirrelLangBarItemButton();
		bool isDisabled() const;
		void switchEnabled();
		bool isEnabled() const;
		void update();
		
		HRESULT __stdcall QueryInterface(REFIID iid, void **ret);
		ULONG __stdcall AddRef();
		ULONG __stdcall Release();
		
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
};
