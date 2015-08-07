#include <iostream>
#include <fstream>
#include <cstdarg>
#include <string>
#include <codecvt>
#include <windows.h>
#include "util.h"
using namespace std;

// TODO
int lprintf(const char *fmt, ...)
{
#ifdef _DEBUG
	static FILE *fout = fopen("c:\\squirrel.log", "a");
	setbuf(fout, NULL);
	va_list args;
	va_start(args, fmt);
	int ret = vfprintf(fout, fmt, args);
	va_end(args);
	return ret;
#else
	return 0;
#endif
}

#ifdef _DEBUG
ofstream lout("c:\\squirrel.log", ios_base::out|ios_base::app);
#else
ostream lout(0);
#endif

string getSelfPath()
{
	TCHAR path[_MAX_PATH];
	GetModuleFileName((HINSTANCE) &__ImageBase, path, _MAX_PATH);
	return toString(wstring(path));
}

string toString(wstring str)
{
	wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
	return converter.to_bytes(str);
}
