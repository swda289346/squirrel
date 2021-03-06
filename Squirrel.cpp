#include <fstream>
#include <sstream>
#include <map>
#include <set>
#include <string>
#include <windows.h>
#include <VersionHelpers.h>
#include "Dll.h"
#include "Squirrel.h"
#include "util.h"
#include "PhoneticCombination.h"
#include "CandidateWindow.h"
using namespace std;

// {218F1835-8469-4778-8ABF-89D705BE229C}
static const GUID GUID_LBI_BUTTON = 
{ 0x218f1835, 0x8469, 0x4778, { 0x8a, 0xbf, 0x89, 0xd7, 0x5, 0xbe, 0x22, 0x9c } };

static const GUID GUID_LBI_INPUTMODE = 
{ 0x2C77A81E, 0x41CC, 0x4178, { 0xA3, 0xA7, 0x5F, 0x8A, 0x98, 0x75, 0x68, 0xE6}};

string loadTable()
{
	string ans;
	HRSRC hRsrc = FindResource((HINSTANCE) &__ImageBase, L"TABLE", L"TEXT");
	if (hRsrc)
	{
		lout << "FindResource" << endl;
		HGLOBAL hGlob = LoadResource((HINSTANCE) &__ImageBase, hRsrc);
		const char *ptr = (const char *) LockResource(hGlob);
		ans = string(ptr, SizeofResource((HINSTANCE) &__ImageBase, hRsrc));
//		lout << ans << endl;
	}
	return ans;
}

Squirrel::Squirrel() : count(0), enabled(false), composition(NULL), candidateWindow(NULL)
{
	objectCounter++;
	lout << "Create Squirrel" << endl;
	
	string s = loadTable();
	wstring ws = fromString(s);
	wistringstream iss(ws);
	wstring code, words;
	bool needNext = false;
	while (iss >> code >> words)
	{
//		lout << toString(code) << " " << toString(words);
		for (wchar_t c : words)
		{
			if (needNext)
			{
				codeTable[code].back().push_back(c);
				needNext = false;
			}
			else
			{
				if (c<0xd800 || c>0xdfff)
					codeTable[code].emplace_back(1, c);
				else
				{
					codeTable[code].emplace_back(1, c);
					needNext = true;
				}
			}
		}
		needNext = false;
	}
	langBarItemButton.push_back(new SquirrelLangBarItemButton(this, GUID_LBI_BUTTON));
	if (IsWindows8OrGreater())
		langBarItemButton.push_back(new SquirrelLangBarItemButton(this, GUID_LBI_INPUTMODE));
}

Squirrel::~Squirrel()
{
	for (auto b:langBarItemButton)
		b->Release();
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

void Squirrel::putCharToComposition(wchar_t c)
{
	if (composition)
	{
		ITfRange *range = NULL;
		composition->GetRange(&range);
		ITfContext *context = NULL;
		range->GetContext(&context);
		putChar(context, c);
	}
}

void Squirrel::disable()
{
	if (candidateWindow)
	{
		delete candidateWindow;
		candidateWindow = NULL;
	}
	putCharToComposition(27);
}

void Squirrel::completeComposition(TfEditCookie ec)
{
	composition->EndComposition(ec);
	composition->Release();
	composition = NULL;
	if (candidateWindow)
	{
		delete candidateWindow;
		candidateWindow = NULL;
	}
}

void Squirrel::updateLangBarItem()
{
	for (SquirrelLangBarItemButton *button:langBarItemButton)
		button->update();
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
	if (iid==IID_ITfKeyEventSink)
	{
		this->AddRef();
		*ret = (ITfKeyEventSink *) this;
		return S_OK;
	}
	if (iid==IID_ITfKeyTraceEventSink)
	{
		this->AddRef();
		*ret = (ITfKeyTraceEventSink *) this;
		return S_OK;
	}
	if (iid==IID_ITfEditSession)
	{
		this->AddRef();
		*ret = (ITfEditSession *) this;
		return S_OK;
	}
	if (iid==IID_ITfCompositionSink)
	{
		this->AddRef();
		*ret = (ITfCompositionSink *) this;
		return S_OK;
	}
	if (iid==IID_ITfThreadFocusSink)
	{
		this->AddRef();
		*ret = (ITfThreadFocusSink *) this;
		return S_OK;
	}
	if (iid==IID_ITfThreadMgrEventSink)
	{
		this->AddRef();
		*ret = (ITfThreadMgrEventSink *) this;
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
	lout << "Squirrel::Release count=" << count << "->" << count-1 << endl;
	count--;
	if (count==0)
	{
		objectCounter--;
		lout << "Delete Squirrel" << endl;
		delete this;
	}
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
	for (SquirrelLangBarItemButton *button:langBarItemButton)
	{
		ITfLangBarItemButton *t = (ITfLangBarItemButton *) button;
		hr = langBarItemMgr->AddItem((ITfLangBarItem *) t);
		if (hr!=S_OK)
			lprintf("AddItem fail %08x\n", hr);
	}
	langBarItemMgr->Release();
	keyState.reset();
	ITfSource *source;
	ptim->QueryInterface(IID_ITfSource, (void **) &source);
	DWORD tmp;
	source->AdviseSink(IID_ITfThreadFocusSink, this, &threadFocusSinkCookie);
	source->AdviseSink(IID_ITfThreadMgrEventSink, this, &threadMgrEventSinkCookie);
	source->AdviseSink(IID_ITfKeyTraceEventSink, this, &keyTraceEventSinkCookie);
	source->Release();
	disabled = false;
	ITfDocumentMgr *pdim;
	ptim->GetFocus(&pdim);
	OnSetFocus(pdim, NULL);
	if (pdim)
		pdim->Release();
	lout << "Activate done" << endl;
	return S_OK;
}

STDMETHODIMP Squirrel::Deactivate()
{
	lout << "Deactivate" << endl;
	HRESULT hr;
	disable();
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
	for (SquirrelLangBarItemButton *button:langBarItemButton)
	{
		ITfLangBarItemButton *t = (ITfLangBarItemButton *) button;
		hr = langBarItemMgr->RemoveItem((ITfLangBarItem *) t);
		if (hr!=S_OK)
			lprintf("RemoveItem fail %08x\n", hr);
	}
	langBarItemMgr->Release();
	ITfSource *source;
	ptim->QueryInterface(IID_ITfSource, (void **) &source);
	source->UnadviseSink(threadFocusSinkCookie);
	source->UnadviseSink(threadMgrEventSinkCookie);
	source->UnadviseSink(keyTraceEventSinkCookie);
	source->Release();
	ptim->Release();
	ptim = NULL;
	return S_OK;
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
	{' ', L' '},
};

static const map<char, wchar_t> PunctuationTable =
{
	{'1', L'�I'},
	{'2', L'�I'},
	{'3', L'��'},
	{'4', L'�C'},
	{'5', L'�H'},
	{'6', L'�s'},
	{'7', L'��'},
	{'8', L'��'},
	{'9', L'�]'},
	{'0', L'�^'},
	{189, L'�u'},
	{187, L'�v'},
	{186, L'�F'},
	{222, L'�B'},
	{219, L'�i'},
	{221, L'�j'},
	{188, L'�A'},
	{190, L'�C'},
	{191, L'�H'},
};

static bool isPunctuation(wchar_t c)
{
	for (auto p : PunctuationTable)
		if (p.second==c)
			return true;
	return false;
}

static const map<char, wchar_t> SelectTable =
{
	{'1', '1'},
	{'2', '2'},
	{'3', '3'},
	{'4', '4'},
	{'5', '5'},
	{'6', '6'},
	{'7', '7'},
	{'8', '8'},
	{'9', '9'},
	{VK_NUMPAD1, '1'},
	{VK_NUMPAD2, '2'},
	{VK_NUMPAD3, '3'},
	{VK_NUMPAD4, '4'},
	{VK_NUMPAD5, '5'},
	{VK_NUMPAD6, '6'},
	{VK_NUMPAD7, '7'},
	{VK_NUMPAD8, '8'},
	{VK_NUMPAD9, '9'},
};

STDMETHODIMP Squirrel::OnKeyDown(ITfContext *pic, WPARAM wParam, LPARAM lParam, BOOL *pfEaten)
{
	lout << "OnKeyDown" << endl;
	if (disabled)
	{
		*pfEaten = FALSE;
		return S_OK;
	}
	if (enabled && keyState.isCombinedKey(wchar_t(wParam)) && keyState.isOnlyShift() && PunctuationTable.count(wParam) && composition==NULL)
	{
		*pfEaten = TRUE;
		putChar(pic, PunctuationTable.at(wParam));
		return S_OK;
	}
	if (keyState.isCombinedKey(wchar_t(wParam)))
	{
		*pfEaten = FALSE;
		return S_OK;
	}
	if (composition==NULL && wParam==' ')
	{
		*pfEaten = FALSE;
		return S_OK;
	}
	if (enabled && wParam==27)
	{
		putChar(pic, wchar_t(wParam));
		*pfEaten = FALSE;
		return S_OK;
	}
	if (candidateWindow && (wParam==38||wParam==40||wParam==13||wParam==' '||wParam==VK_BACK||wParam==VK_PRIOR||wParam==VK_NEXT))
	{
		*pfEaten = TRUE;
		putChar(pic, wParam);
		return S_OK;
	}
	if (candidateWindow && SelectTable.count(wParam))
	{
		*pfEaten = TRUE;
		putChar(pic, SelectTable.at(wParam));
		return S_OK;
	}
	if (candidateWindow)
	{
		*pfEaten = TRUE;
		return S_OK;
	}
	if (composition && wParam==VK_BACK)
	{
		*pfEaten = TRUE;
		putChar(pic, wParam);
		return S_OK;
	}
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
	if (disabled)
	{
		*pfEaten = FALSE;
		return S_OK;
	}
	bool isCombinedKey = keyState.isCombinedKey(wchar_t(wParam));
	keyState.releaseKey(wchar_t(wParam));
	if (wParam==16 && !isCombinedKey)
	{
		*pfEaten = TRUE;
		enabled = !enabled;
		updateLangBarItem();
		if (!enabled)
			disable();
		return S_OK;
	}
	*pfEaten = FALSE;
	return S_OK;
}

STDMETHODIMP Squirrel::OnPreservedKey(ITfContext *pic, REFGUID rguid, BOOL *pfEaten)
{
	lout << "OnPreservedKey" << endl;
	lout << "E_FAIL" << endl;
	return E_FAIL;
}

STDMETHODIMP Squirrel::OnSetFocus(BOOL fForeground)
{
	lout << "OnSetFocus" << endl;
	lout << "E_FAIL" << endl;
	return E_FAIL;
}

STDMETHODIMP Squirrel::OnTestKeyDown(ITfContext *pic, WPARAM wParam, LPARAM lParam, BOOL *pfEaten)
{
	lout << "OnTestKeyDown" << endl;
	if (disabled)
	{
		*pfEaten = FALSE;
		return S_OK;
	}
	keyState.setKey(wchar_t(wParam));
	if (enabled && keyState.isCombinedKey(wchar_t(wParam)) && keyState.isOnlyShift() && PunctuationTable.count(wParam) && composition==NULL)
	{
		*pfEaten = TRUE;
		return S_OK;
	}
	if (keyState.isCombinedKey(wchar_t(wParam)))
	{
		*pfEaten = FALSE;
		return S_OK;
	}
	if (composition==NULL && wParam==' ')
	{
		*pfEaten = FALSE;
		return S_OK;
	}
	if (candidateWindow && (wParam==38||wParam==40||wParam==13||wParam==' '||wParam==VK_BACK||wParam==VK_PRIOR||wParam==VK_NEXT))
	{
		*pfEaten = TRUE;
		return S_OK;
	}
	if (candidateWindow && SelectTable.count(wParam))
	{
		*pfEaten = TRUE;
		return S_OK;
	}
	if (candidateWindow)
	{
		*pfEaten = TRUE;
		return S_OK;
	}
	if (composition && wParam==VK_BACK)
	{
		*pfEaten = TRUE;
		return S_OK;
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
	if (disabled)
	{
		*pfEaten = FALSE;
		return S_OK;
	}
	if (wParam==16 && !keyState.isCombinedKey(wchar_t(wParam)))
	{
		*pfEaten = TRUE;
		return S_OK;
	}
	keyState.releaseKey(wchar_t(wParam));
	*pfEaten = FALSE;
	return S_OK;
}

STDMETHODIMP Squirrel::OnKeyTraceDown(WPARAM wParam, LPARAM lParam)
{
	lout << "OnKeyTraceDown" << endl;
	if (disabled && KeyState::isSuperKey(wchar_t(wParam)))
		keyState.setKey(wchar_t(wParam));
	return S_OK;
}

STDMETHODIMP Squirrel::OnKeyTraceUp(WPARAM wParam, LPARAM lParam)
{
	lout << "OnKeyTraceUp" << endl;
	if (disabled && KeyState::isSuperKey(wchar_t(wParam)))
		keyState.releaseKey(wchar_t(wParam));
	return S_OK;
}

static const set<wchar_t> phoneticCompleteSet =
{
	L' ', L'��', L'��', L'��', L'��'
};

HRESULT __stdcall Squirrel::DoEditSession(TfEditCookie ec)
{
	lout << "DoEditSession" << endl;
	HRESULT hr;
	if (textToSet==27)
	{
		lout << "textToSet==" << textToSet << " EndComposition" << endl;
		ITfRange *range = NULL;
		composition->GetRange(&range);
		range->SetText(ec, 0, NULL, 0);
		range->Release();
		completeComposition(ec);
		return S_OK;
	}
	if (isPunctuation(textToSet))
	{
		ITfInsertAtSelection *insertAtSelection = NULL;
		hr = pic->QueryInterface(IID_ITfInsertAtSelection, (void **) &insertAtSelection);
		lprintf("QueryInterface InsertAtSelection %08x\n", hr);
		ITfRange *range = NULL;
		hr = insertAtSelection->InsertTextAtSelection(ec, TF_IAS_QUERYONLY, NULL, 0, &range);
		lprintf("InsertTextAtSelection %08x %08x\n", hr, range);
		insertAtSelection->Release();
		range->SetText(ec, 0, &textToSet, 1);
		range->Release();
		return S_OK;
	}
	if (composition==NULL)
	{
		ITfContextComposition *contextComposition = NULL;
		hr = pic->QueryInterface(IID_ITfContextComposition, (void **) &contextComposition);
		lprintf("QueryInterface ITfContextComposition %08x\n", hr);
		ITfInsertAtSelection *insertAtSelection = NULL;
		hr = pic->QueryInterface(IID_ITfInsertAtSelection, (void **) &insertAtSelection);
		lprintf("QueryInterface InsertAtSelection %08x\n", hr);
		ITfRange *range = NULL;
		hr = insertAtSelection->InsertTextAtSelection(ec, TF_IAS_QUERYONLY, NULL, 0, &range);
		lprintf("InsertTextAtSelection %08x %08x\n", hr, range);
		range->SetText(ec, 0, NULL, 0);
		insertAtSelection->Release();
		hr = contextComposition->StartComposition(ec, range, (ITfCompositionSink *) this, &composition);
		lprintf("StartComposition %08x\n", hr);
		range->Release();
		contextComposition->Release();
	}
	if (candidateWindow)
	{
		if (textToSet==VK_PRIOR)
		{
			candidateWindow->lastPage();
			return S_OK;
		}
		if (textToSet==' ' || textToSet==VK_NEXT)
		{
			candidateWindow->nextPage();
			return S_OK;
		}
		if (textToSet==38)
		{
			candidateWindow->lastItem();
			return S_OK;
		}
		if (textToSet==40)
		{
			candidateWindow->nextItem();
			return S_OK;
		}
		if (textToSet==VK_BACK)
		{
			ITfRange *range = NULL;
			composition->GetRange(&range);
			wchar_t text[1024];
			ULONG len;
			range->GetText(ec, 0, text, 1024, &len);
			text[len] = 0;
			wstring wText(text);
			PhoneticCombination combination(wText);
			combination.clearPos(4);
			wText = combination.asString();
			range->SetText(ec, 0, wText.c_str(), wText.size());
			delete candidateWindow;
			candidateWindow = NULL;
			return S_OK;
		}
		wstring textString = textToSet==13?candidateWindow->getCandidate():candidateWindow->getCandidate(textToSet-'1');
		ITfRange *range = NULL;
		hr = composition->GetRange(&range);
		range->SetText(ec, 0, textString.c_str(), textString.size());
		TF_SELECTION selection;
		selection.range = range;
		range->Collapse(ec, TF_ANCHOR_END);
		selection.style.ase = TF_AE_NONE;
		selection.style.fInterimChar = FALSE;
		pic->SetSelection(ec, 1, &selection);
		range->Release();
		completeComposition(ec);
		return S_OK;
	}
	ITfRange *range = NULL;
	hr = composition->GetRange(&range);
	lprintf("GetRange %08x\n", hr);
	
	// get position
	int x, y;
	HWND parent;
	ITfContextView *contextView = NULL;
	hr = pic->GetActiveView(&contextView);
	lprintf("GetActiveView %08x %08x\n", hr, contextView);
	hr = contextView->GetWnd(&parent);
	lprintf("GetWnd %08x %08x\n", hr, parent);
	RECT rect;
	BOOL clip;
	contextView->GetTextExt(ec, range, &rect, &clip);
	x = rect.left;
	y = rect.bottom;
	contextView->Release();
	
	wchar_t text[1024];
	ULONG len;
	range->GetText(ec, 0, text, 1024, &len);
	text[len] = 0;
	wstring textString(text);
	PhoneticCombination combination(textString);
	combination.setChar(textToSet);
	textString = combination.asString();
	range->SetText(ec, 0, textString.c_str(), textString.size());
	TF_SELECTION selection;
	selection.range = range;
	range->Collapse(ec, TF_ANCHOR_END);
	selection.style.ase = TF_AE_NONE;
	selection.style.fInterimChar = FALSE;
	pic->SetSelection(ec, 1, &selection);
	range->Release();
	if (combination.isEmpty())
		completeComposition(ec);
	if (phoneticCompleteSet.count(textToSet))
	{
		ITfContextView *contextView = NULL;
		hr = pic->GetActiveView(&contextView);
		lprintf("GetActiveView %08x %08x\n", hr, contextView);
		HWND parent, child;
		hr = contextView->GetWnd(&parent);
		lprintf("GetWnd %08x %08x\n", hr, parent);
		vector<wstring> candidates;
		if (codeTable.count(textString))
			candidates = codeTable[textString];
		else
			return S_OK;
		HMONITOR hMonitor = MonitorFromWindow(parent, MONITOR_DEFAULTTONULL);
		if (hMonitor!=NULL)
		{
			MONITORINFO monitorInfo;
			monitorInfo.cbSize = sizeof(MONITORINFO);
			BOOL ret = GetMonitorInfo(hMonitor, &monitorInfo);
			lout << "GetMonitorInfo " << ret << endl;
			lout << monitorInfo.rcMonitor.top << " " << monitorInfo.rcMonitor.bottom << " " << monitorInfo.rcMonitor.left << " " << monitorInfo.rcMonitor.right << endl;
			if (x+70>monitorInfo.rcMonitor.right)
				x = monitorInfo.rcMonitor.right-70;
			if (y+360>monitorInfo.rcMonitor.bottom)
				y = monitorInfo.rcMonitor.bottom-360;
		}
		candidateWindow = new CandidateWindow((HINSTANCE) &__ImageBase, parent, candidates, x, y);
		candidateWindow->onSelect = bind(&Squirrel::putCharToComposition, this, 13);
		contextView->Release();
	}
	lout << "DoEditSession done" << endl;
	return S_OK;
}

HRESULT __stdcall Squirrel::OnCompositionTerminated(TfEditCookie ecWrite, ITfComposition *pComposition)
{
	lout << "OnCompositionTerminated" << endl;
	ITfRange *range = NULL;
	composition->GetRange(&range);
	range->SetText(ecWrite, 0, NULL, 0);
	range->Release();
	composition->Release();
	composition = NULL;
	if (candidateWindow)
	{
		delete candidateWindow;
		candidateWindow = NULL;
	}
	return S_OK;
}

HRESULT __stdcall Squirrel::OnKillThreadFocus()
{
	if (candidateWindow)
		candidateWindow->hide();
	return S_OK;
}

HRESULT __stdcall Squirrel::OnSetThreadFocus()
{
	if (candidateWindow)
		candidateWindow->show();
	return S_OK;
}

HRESULT __stdcall Squirrel::OnInitDocumentMgr(ITfDocumentMgr *pdim)
{
	lout << "OnInitDocumentMgr" << " E_NOTIMPL" << endl;
	return E_NOTIMPL;
}

HRESULT __stdcall Squirrel::OnPopContext(ITfContext *pic)
{
	lout << "OnPopContext" << " E_NOTIMPL" << endl;
	return E_NOTIMPL;
}

HRESULT __stdcall Squirrel::OnPushContext(ITfContext *pic)
{
	lout << "OnPushContext" << " E_NOTIMPL" << endl;
	return E_NOTIMPL;
}

HRESULT __stdcall Squirrel::OnSetFocus(ITfDocumentMgr *pdimFocus, ITfDocumentMgr *pdimPrevFocus)
{
	lout << "OnSetFocus " << pdimFocus << " " << pdimPrevFocus << endl;
	if (!pdimFocus)
	{
		disabled = true;
		updateLangBarItem();
		return S_OK;
	}
	
	HRESULT hr;
	VARIANT var;
	ITfContext *context;
	hr = pdimFocus->GetTop(&context);
	ITfCompartmentMgr *compartmentMgr = NULL;
	hr = context->QueryInterface(IID_ITfCompartmentMgr, (void **) &compartmentMgr);
	lprintf("QueryInterface ITfCompartmentMgr %08x %08x\n", hr, compartmentMgr);
	ITfCompartment *compartment = NULL;
	hr = compartmentMgr->GetCompartment(GUID_COMPARTMENT_KEYBOARD_DISABLED, &compartment);
	lprintf("GetCompartment %08x %08x\n", hr, compartment);
	hr = compartment->GetValue(&var);
	lprintf("GetValue %08x\n", hr);
	context->Release();
	compartment->Release();
	compartmentMgr->Release();
	if (var.vt==VT_I4 && var.lVal)
	{
		disabled = true;
		updateLangBarItem();
		return S_OK;
	}
	
	disabled = false;
	updateLangBarItem();
	return S_OK;
}

HRESULT __stdcall Squirrel::OnUninitDocumentMgr(ITfDocumentMgr *pdim)
{
	lout << "OnUninitDocumentMgr" << " E_NOTIMPL" << endl;
	return E_NOTIMPL;
}
