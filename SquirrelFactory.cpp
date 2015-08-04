#include "SquirrelFactory.h"
#include "Squirrel.h"

SquirrelFactory::SquirrelFactory() : count(0)
{
	
}

HRESULT __stdcall SquirrelFactory::QueryInterface(REFIID iid, void **ret)
{
	if (iid==IID_IUnknown)
	{
		this->AddRef();
		*ret = (IUnknown *) this;
		return S_OK;
	}
	if (iid==IID_IClassFactory)
	{
		this->AddRef();
		*ret = (IClassFactory *) this;
		return S_OK;
	}
	return E_NOINTERFACE;
}

ULONG __stdcall SquirrelFactory::AddRef()
{
	count++;
	return count;
}

ULONG __stdcall SquirrelFactory::Release()
{
	count--;
	if (count==0)
		delete this;
	return count;
}

HRESULT __stdcall SquirrelFactory::CreateInstance(IUnknown *p, const IID &iid, void **ret)
{
	if (p!=NULL)
		return CLASS_E_NOAGGREGATION;
	Squirrel *squirrel = new Squirrel();
	return squirrel->QueryInterface(iid, ret);
}

HRESULT __stdcall SquirrelFactory::LockServer(BOOL b)
{
	return E_FAIL;
}
