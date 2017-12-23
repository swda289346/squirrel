#include "SquirrelLangBarItemButton.h"
#include "Squirrel.h"
#include "Dll.h"
#include "util.h"

SquirrelLangBarItemButton::SquirrelLangBarItemButton(Squirrel *parent, GUID type) : count(0), parent(parent), langBarItemInfo{guid, type, TF_LBI_STYLE_BTN_BUTTON|TF_LBI_STYLE_SHOWNINTRAY, 0, SquirrelName}, langBarItemSink(NULL)
{
	objectCounter++;
	lout << "Create SquirrelLangBarItemButton" << endl;
	AddRef();
}

SquirrelLangBarItemButton::~SquirrelLangBarItemButton()
{
	if (langBarItemSink)
		langBarItemSink->Release();
}

bool SquirrelLangBarItemButton::isDisabled() const
{
	return parent->disabled;
}

void SquirrelLangBarItemButton::switchEnabled()
{
	if (!isDisabled())
		parent->enabled = !parent->enabled;
}

bool SquirrelLangBarItemButton::isEnabled() const
{
	return parent->enabled;
}

void SquirrelLangBarItemButton::update()
{
	if (langBarItemSink)
		langBarItemSink->OnUpdate(TF_LBI_BTNALL);
}

HRESULT __stdcall SquirrelLangBarItemButton::QueryInterface(REFIID iid, void **ret)
{
	if (iid==IID_IUnknown)
	{
		this->AddRef();
		*ret = (IUnknown *) this;
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
	return E_NOINTERFACE;
}

ULONG __stdcall SquirrelLangBarItemButton::AddRef()
{
	count++;
	return count;
}

ULONG __stdcall SquirrelLangBarItemButton::Release()
{
	count--;
	if (count==0)
	{
		objectCounter--;
		lout << "Delete SquirrelLangBarItemButton" << endl;
		delete this;
	}
	return count;
}

HRESULT __stdcall SquirrelLangBarItemButton::AdviseSink(REFIID riid, IUnknown *punk, DWORD *pdwCookie)
{
	lout << "AdviseSink" << endl;
	if (langBarItemSink)
		return E_FAIL;
	punk->QueryInterface(IID_ITfLangBarItemSink, (void **) &langBarItemSink);
	*pdwCookie = 1;
	return S_OK;
}

HRESULT __stdcall SquirrelLangBarItemButton::UnadviseSink(DWORD pdwCookie)
{
	lout << "UnadviseSink" << endl;
	if (pdwCookie!=1||!langBarItemSink)
		return E_FAIL;
	langBarItemSink->Release();
	langBarItemSink = NULL;
	return S_OK;
}

HRESULT __stdcall SquirrelLangBarItemButton::GetInfo(TF_LANGBARITEMINFO *pInfo)
{
	lout << "GetInfo" << endl;
	*pInfo = langBarItemInfo;
	return S_OK;
}

HRESULT __stdcall SquirrelLangBarItemButton::GetStatus(DWORD *pdwStatus)
{
	lout << "GetStatus" << endl;
	*pdwStatus = 0;
	// TODO
//	if (isDisabled())
//		*pdwStatus |= TF_LBI_STATUS_DISABLED;
	return S_OK;
}

HRESULT __stdcall SquirrelLangBarItemButton::GetTooltipString(BSTR *pbstrToolTip)
{
	lout << "GetTooltipString" << endl;
	if (isDisabled())
		*pbstrToolTip = SysAllocString(L"Disabled");
	else if (isEnabled())
		*pbstrToolTip = SysAllocString(L"Squirrel");
	else
		*pbstrToolTip = SysAllocString(L"English");
	return S_OK;
}

HRESULT __stdcall SquirrelLangBarItemButton::Show(BOOL fShow)
{
	lout << "Show" << endl;
	return E_NOTIMPL;
}

HRESULT __stdcall SquirrelLangBarItemButton::GetIcon(HICON *phIcon)
{
	lout << "GetIcon" << endl;
	if (isDisabled())
		*phIcon = LoadIcon(NULL, IDI_ERROR);
	else if (isEnabled())
		*phIcon = LoadIcon((HINSTANCE) &__ImageBase, L"ICON");
	else
		*phIcon = LoadIcon(NULL, IDI_QUESTION);
	return S_OK;
}

HRESULT __stdcall SquirrelLangBarItemButton::GetText(BSTR *pbstrText)
{
	lout << "GetText" << endl;
	if (isDisabled())
		*pbstrText = SysAllocString(L"Disabled");
	else if (isEnabled())
		*pbstrText = SysAllocString(L"Squirrel");
	else
		*pbstrText = SysAllocString(L"English");
	return S_OK;
}

HRESULT __stdcall SquirrelLangBarItemButton::InitMenu(ITfMenu *pMenu)
{
	lout << "InitMenu" << endl;
	return E_FAIL;
}

HRESULT __stdcall SquirrelLangBarItemButton::OnClick(TfLBIClick click, POINT pt, const RECT *prcArea)
{
	lout << "OnClick" << endl;
	switchEnabled();
	update();
	if (!isEnabled())
		parent->disable();
	return S_OK;
}

HRESULT __stdcall SquirrelLangBarItemButton::OnMenuSelect(UINT wID)
{
	lout << "OnMenuSelect" << endl;
	return E_FAIL;
}
