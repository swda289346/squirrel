#include <map>
#include "Squirrel.h"
#include "util.h"
using namespace std;

static const GUID GUID_LBI_INPUTMODE = 
{ 0x2C77A81E, 0x41CC, 0x4178, { 0xA3, 0xA7, 0x5F, 0x8A, 0x98, 0x75, 0x68, 0xE6}};

Squirrel::Squirrel() : count(0), enabled(false), langBarItemInfo{guid, GUID_LBI_INPUTMODE, TF_LBI_STYLE_BTN_BUTTON|TF_LBI_STYLE_SHOWNINTRAY, 0, L"Squirrel"}
{
	
}

void Squirrel::putChar(ITfContext *pic, wchar_t c)
{
	lout << "putChar " << (unsigned int) c << endl;
	HRESULT hr, hrSession;
	this->pic = pic;
	pic->AddRef();
	textToSet = c;
	hr = pic->RequestEditSession(tid, this, TF_ES_SYNC|TF_ES_READWRITE, &hrSession);
	this->pic->Release();
	this->pic = NULL;
	lout << "putChar done" << endl;
}

HRESULT __stdcall Squirrel::QueryInterface(REFIID iid, void **ret)
{
	if (iid==IID_IUnknown)
	{
		this->AddRef();
		*ret = (IUnknown *) this;
		return S_OK;
	}
	if (iid==IID_ITfTextInputProcessor)
	{
		this->AddRef();
		*ret = (ITfTextInputProcessor *) this;
		return S_OK;
	}
	if (iid==IID_ITfSource)
	{
		this->AddRef();
		*ret = (ITfSource *) this;
		return S_OK;
	}
	if (iid==IID_ITfLangBarItemButton)
	{
		this->AddRef();
		*ret = (ITfLangBarItemButton *) this;
		return S_OK;
	}
	if (iid==IID_ITfKeyEventSink)
	{
		this->AddRef();
		*ret = (ITfKeyEventSink *) this;
		return S_OK;
	}
	if (iid==IID_ITfEditSession)
	{
		this->AddRef();
		*ret = (ITfEditSession *) this;
		return S_OK;
	}
	return E_NOINTERFACE;
}

ULONG __stdcall Squirrel::AddRef()
{
	count++;
	return count;
}

ULONG __stdcall Squirrel::Release()
{
	count--;
	if (count==0)
		delete this;
	return count;
}

STDMETHODIMP Squirrel::Activate(ITfThreadMgr *ptim, TfClientId tid)
{
	lout << "Activate" << endl;
	HRESULT hr;
	hr = ptim->QueryInterface(IID_ITfThreadMgr, (void **) &(this->ptim));
	this->tid = tid;
	ITfKeystrokeMgr *keystrokeMgr = NULL;
	hr = ptim->QueryInterface(IID_ITfKeystrokeMgr, (void **) &keystrokeMgr);
	if (hr!=S_OK)
		lprintf("Fail %08x\n", hr);
	lout << "AdviseKeyEventSink" << endl;
	hr = keystrokeMgr->AdviseKeyEventSink(tid, (ITfKeyEventSink *) this, TRUE);
	if (hr!=S_OK)
		lprintf("Fail %08x\n", hr);
	keystrokeMgr->Release();
	ITfLangBarItemMgr *langBarItemMgr;
	hr = ptim->QueryInterface(IID_ITfLangBarItemMgr, (void **) &langBarItemMgr);
	if (hr!=S_OK)
		lprintf("Fail %08x\n", hr);
	hr = langBarItemMgr->AddItem((ITfLangBarItem *) this);
	if (hr!=S_OK)
		lprintf("AddItem fail %08x\n", hr);
	langBarItemMgr->Release();
	lout << "Activate done" << endl;
	return S_OK;
}

STDMETHODIMP Squirrel::Deactivate()
{
	lout << "Deactivate" << endl;
	HRESULT hr;
	ITfKeystrokeMgr *keystrokeMgr = NULL;
	hr = ptim->QueryInterface(IID_ITfKeystrokeMgr, (void **) &keystrokeMgr);
	if (hr!=S_OK)
		lprintf("Fail %08x\n", hr);
	lout << "UnadviseKeyEventSink" << endl;
	hr = keystrokeMgr->UnadviseKeyEventSink(tid);
	if (hr!=S_OK)
		lprintf("Fail %08x\n", hr);
	keystrokeMgr->Release();
	ITfLangBarItemMgr *langBarItemMgr;
	hr = ptim->QueryInterface(IID_ITfLangBarItemMgr, (void **) &langBarItemMgr);
	if (hr!=S_OK)
		lprintf("Fail %08x\n", hr);
	hr = langBarItemMgr->RemoveItem(this);
	if (hr!=S_OK)
		lprintf("Fail %08x\n", hr);
	langBarItemMgr->Release();
	ptim->Release();
	ptim = NULL;
	return S_OK;
}

HRESULT __stdcall Squirrel::AdviseSink(REFIID riid, IUnknown *punk, DWORD *pdwCookie)
{
	lout << "AdviseSink" << endl;
	langBarItemSink = (ITfLangBarItemSink *) punk;
	*pdwCookie = 1;
	return S_OK;
}

HRESULT __stdcall Squirrel::UnadviseSink(DWORD pdwCookie)
{
	lout << "UnadviseSink" << endl;
	return S_OK;
}

HRESULT __stdcall Squirrel::GetInfo(TF_LANGBARITEMINFO *pInfo)
{
	lout << "GetInfo" << endl;
	*pInfo = langBarItemInfo;
	return S_OK;
}

HRESULT __stdcall Squirrel::GetStatus(DWORD *pdwStatus)
{
	lout << "GetStatus" << endl;
	*pdwStatus = 0;
	return S_OK;
}

HRESULT __stdcall Squirrel::GetTooltipString(BSTR *pbstrToolTip)
{
	lout << "GetTooltipString" << endl;
	if (enabled)
		*pbstrToolTip = SysAllocString(L"Squirrel");
	else
		*pbstrToolTip = SysAllocString(L"English");
	return S_OK;
}

HRESULT __stdcall Squirrel::Show(BOOL fShow)
{
	lout << "Show" << endl;
	return E_NOTIMPL;
}

HRESULT __stdcall Squirrel::GetIcon(HICON *phIcon)
{
	lout << "GetIcon" << endl;
	if (enabled)
		*phIcon = LoadIcon((HINSTANCE) &__ImageBase, "ICON");
	else
		*phIcon = LoadIcon(NULL, IDI_QUESTION);
	return S_OK;
}

HRESULT __stdcall Squirrel::GetText(BSTR *pbstrText)
{
	lout << "GetText" << endl;
	if (enabled)
		*pbstrText = SysAllocString(L"Squirrel");
	else
		*pbstrText = SysAllocString(L"English");
	return S_OK;
}

HRESULT __stdcall Squirrel::InitMenu(ITfMenu *pMenu)
{
	lout << "InitMenu" << endl;
	return E_FAIL;
}

HRESULT __stdcall Squirrel::OnClick(TfLBIClick click, POINT pt, const RECT *prcArea)
{
	lout << "OnClick" << endl;
	enabled = !enabled;
	langBarItemSink->OnUpdate(TF_LBI_BTNALL);
	return S_OK;
}

HRESULT __stdcall Squirrel::OnMenuSelect(UINT wID)
{
	lout << "OnMenuSelect" << endl;
	return E_FAIL;
}

static const map<char, wchar_t> phoneticTable =
{
	{'1', L'�t'},
	{'Q', L'�u'},
	{'A', L'�v'},
	{'Z', L'�w'},
	{'2', L'�x'},
	{'W', L'�y'},
	{'S', L'�z'},
	{'X', L'�{'},
	{'3', L'��'},
	{'E', L'�|'},
	{'D', L'�}'},
	{'C', L'�~'},
	{'4', L'��'},
	{'R', L'��'},
	{'F', L'��'},
	{'V', L'��'},
	{'5', L'��'},
	{'T', L'��'},
	{'G', L'��'},
	{'B', L'��'},
	{'6', L'��'},
	{'Y', L'��'},
	{'H', L'��'},
	{'N', L'��'},
	{'7', L'��'},
	{'U', L'��'},
	{'J', L'��'},
	{'M', L'��'},
	{'8', L'��'},
	{'I', L'��'},
	{'K', L'��'},
	{188, L'��'},
	{'9', L'��'},
	{'O', L'��'},
	{'L', L'��'},
	{190, L'��'},
	{'0', L'��'},
	{'P', L'��'},
	{186, L'��'},
	{191, L'��'},
	{189, L'��'},
};

STDMETHODIMP Squirrel::OnKeyDown(ITfContext *pic, WPARAM wParam, LPARAM lParam, BOOL *pfEaten)
{
	lout << "OnKeyDown" << endl;
	if (enabled && phoneticTable.count(wParam))
	{
		*pfEaten = TRUE;
		lout << "Try to eat key down " << wParam << " " << lParam << endl;
		putChar(pic, phoneticTable.at(wParam));
	}
	else
		*pfEaten = FALSE;
	return S_OK;
}

STDMETHODIMP Squirrel::OnKeyUp(ITfContext *pic, WPARAM wParam, LPARAM lParam, BOOL *pfEaten)
{
	lout << "OnKeyUp" << endl;
	*pfEaten = FALSE;
	return S_OK;
}

STDMETHODIMP Squirrel::OnPreservedKey(ITfContext *pic, REFGUID rguid, BOOL *pfEaten)
{
	lout << "OnPreservedKey" << endl;
	return E_FAIL;
}

STDMETHODIMP Squirrel::OnSetFocus(BOOL fForeground)
{
	lout << "OnSetFocus" << endl;
	return E_FAIL;
}

STDMETHODIMP Squirrel::OnTestKeyDown(ITfContext *pic, WPARAM wParam, LPARAM lParam, BOOL *pfEaten)
{
	lout << "OnTestKeyDown" << endl;
	if (wParam==16)
	{
		enabled = !enabled;
		langBarItemSink->OnUpdate(TF_LBI_BTNALL);
	}
	if (enabled && phoneticTable.count(wParam))
	{
		*pfEaten = TRUE;
		lout << "Try to eat test key down " << wParam << " " << lParam << endl;
	}
	else
	{
		*pfEaten = FALSE;
		lout << "Try not to eat test key down " << wParam << " " << lParam << endl;
	}
	return S_OK;
}

STDMETHODIMP Squirrel::OnTestKeyUp(ITfContext *pic, WPARAM wParam, LPARAM lParam, BOOL *pfEaten)
{
	lout << "OnTestKeyUp" << endl;
	*pfEaten = FALSE;
	return S_OK;
}

HRESULT __stdcall Squirrel::DoEditSession(TfEditCookie ec)
{
	lout << "DoEditSession" << endl;
	HRESULT hr;
	ITfRange *range = NULL;
	hr = pic->GetEnd(ec, &range);
	lprintf("GetEnd %08x\n", hr);
	range->SetText(ec, 0, &textToSet, 1);
	lout << "DoEditSession done" << endl;
	return S_OK;
}
