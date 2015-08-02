#include <windows.h>

extern "C" __declspec(dllexport) HRESULT DllRegisterServer()
{
	return S_OK;
}

extern "C" __declspec(dllexport) HRESULT DllUnregisterServer()
{
	return S_OK;
}
