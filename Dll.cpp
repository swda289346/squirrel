#include <iostream>
#include <codecvt>
#include <windows.h>
#include <winreg.h>
#include <msctf.h>
#include "Squirrel.h"
#include "SquirrelFactory.h"
#include "util.h"
using namespace std;

STDAPI DllGetClassObject(const CLSID &clsid, const IID &iid, void **ret)
{
	if (clsid==guid)
	{
		SquirrelFactory *factory = new SquirrelFactory();
		if (factory==NULL)
			return E_OUTOFMEMORY;
		return factory->QueryInterface(iid, ret);
	}
	return CLASS_E_CLASSNOTAVAILABLE;
}

STDAPI DllCanUnloadNow()
{
	return S_OK;
}

HRESULT DllRegisterServer()
{
	lout << "Register server" << endl;
	HRESULT hr;
	
	// Register COM
	HKEY key;
	lout << "Create key \\" << endl;
	hr = RegCreateKeyEx(HKEY_CLASSES_ROOT, "CLSID" "\\{7841FDFF-FBE7-4D1F-9E57-F56CAF7F05A5}", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &key, NULL);
	if (hr!=ERROR_SUCCESS)
	{
		lout << "Fail" << endl;
		return E_FAIL;
	}
	string value = "Squirrel";
	lout << "Write key \\" << endl;
	hr = RegSetValueEx(key, NULL, 0, REG_SZ, (const BYTE *) value.c_str(), value.size());
	if (hr!=ERROR_SUCCESS)
	{
		lout << "Fail" << endl;
		return E_FAIL;
	}
	lout << "Create key \\InprocServer32" << endl;
	hr = RegCreateKeyEx(key, "InprocServer32", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &key, NULL);
	if (hr!=ERROR_SUCCESS)
	{
		lout << "Fail" << endl;
		return E_FAIL;
	}
	value = getSelfPath();
	lout << "Write key \\InprocServer32" << endl;
	hr = RegSetValueEx(key, NULL, 0, REG_SZ, (const BYTE *) value.c_str(), value.size());
	if (hr!=ERROR_SUCCESS)
	{
		lout << "Fail" << endl;
		return E_FAIL;
	}
	
	// Register TS
	ITfInputProcessorProfiles *profile;
	CoInitialize(NULL);
	lout << "Create input profile" << endl;
	hr = CoCreateInstance(CLSID_TF_InputProcessorProfiles, NULL, CLSCTX_INPROC_SERVER, IID_ITfInputProcessorProfiles, (LPVOID *) &profile);
	if (hr!=S_OK)
	{
		lout << "Fail" << endl;
		CoUninitialize();
		return E_FAIL;
	}
	lout << "Register text service" << endl;
	hr = profile->Register(guid);
	if (hr!=S_OK)
	{
		lout << "Fail" << endl;
		profile->Release();
		CoUninitialize();
		return E_FAIL;
	}
	lout << "Add language profile" << endl;
	string path = getSelfPath();
	wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
	wstring wpath = converter.from_bytes(path);
	hr = profile->AddLanguageProfile(guid, 1028, guid, L"Squirrel", -1, wpath.c_str(), -1, 0);
	if (hr!=S_OK)
	{
		lout << "Fail" << endl;
		profile->Release();
		CoUninitialize();
		return E_FAIL;
	}
	profile->Release();
	lout << "Create category manager" << endl;
	ITfCategoryMgr *categoryMgr;
	CoCreateInstance(CLSID_TF_CategoryMgr, NULL, CLSCTX_INPROC_SERVER, IID_ITfCategoryMgr, (void **) &categoryMgr);
	if (hr!=S_OK)
	{
		lout << "Fail" << endl;
		CoUninitialize();
		return E_FAIL;
	}
	lout << "Register category keyboard" << endl;
	hr = categoryMgr->RegisterCategory(guid, GUID_TFCAT_TIP_KEYBOARD, guid);
	if (hr!=S_OK)
	{
		lout << "Fail" << endl;
		CoUninitialize();
		categoryMgr->Release();
		return E_FAIL;
	}
	lout << "Register app support" << endl;
	hr = categoryMgr->RegisterCategory(guid, GUID_TFCAT_TIPCAP_IMMERSIVESUPPORT, guid);
	if (hr!=S_OK)
	{
		lout << "Fail" << endl;
		CoUninitialize();
		categoryMgr->Release();
		return E_FAIL;
	}
	lout << "Register system tray suuport" << endl;
	hr = categoryMgr->RegisterCategory(guid, GUID_TFCAT_TIPCAP_SYSTRAYSUPPORT, guid);
	if (hr!=S_OK)
	{
		lout << "Fail" << endl;
		CoUninitialize();
		categoryMgr->Release();
		return E_FAIL;
	}
	categoryMgr->Release();
	CoUninitialize();
	
	lout << "Register server success" << endl;
	return S_OK;
}

HRESULT DllUnregisterServer()
{
	lout << "Unregister server" << endl;
	HRESULT hr;
	
	// Unregister COM
	lout << "Delete key \\" << endl;
	hr = RegDeleteTree(HKEY_CLASSES_ROOT, "CLSID" "\\{7841FDFF-FBE7-4D1F-9E57-F56CAF7F05A5}");
	if (hr!=ERROR_SUCCESS)
	{
		lout << "Fail" << endl;
		return E_FAIL;
	}
	
	// Unregister TS
	ITfInputProcessorProfiles *profile;
	CoInitialize(NULL);
	lout << "Create input profile" << endl;
	hr = CoCreateInstance(CLSID_TF_InputProcessorProfiles, NULL, CLSCTX_INPROC_SERVER, IID_ITfInputProcessorProfiles, (LPVOID *) &profile);
	if (hr!=S_OK)
	{
		lout << "Fail" << endl;
		CoUninitialize();
		return E_FAIL;
	}
	lout << "Unregister text service" << endl;
	hr = profile->Unregister(guid);
	if (hr!=S_OK)
	{
		lout << "Fail" << endl;
		profile->Release();
		CoUninitialize();
		return E_FAIL;
	}
	profile->Release();
	CoUninitialize();
	
	lout << "Unregister server success" << endl;
	return S_OK;
}
