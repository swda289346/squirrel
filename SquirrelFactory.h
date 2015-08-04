#pragma once

#include "msctf.h"

class SquirrelFactory : IClassFactory
{
	public:
		int count;
		
		SquirrelFactory();
		HRESULT __stdcall QueryInterface(REFIID iid, void **ret);
		ULONG __stdcall AddRef();
		ULONG __stdcall Release();
		HRESULT __stdcall CreateInstance(IUnknown *p, const IID &iid, void **ret);
		HRESULT __stdcall LockServer(BOOL b);
};
