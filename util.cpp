#include <iostream>
#include <fstream>
#include <cstdarg>
#include <string>
#include <codecvt>
#include <windows.h>
#include "util.h"
using namespace std;

#ifdef _DEBUG
char path[MAX_PATH];
bool init()
{
	ExpandEnvironmentStringsA("%APPDATA%\\squirrel.log", path, MAX_PATH);
	return true;
}
bool isInit = init();
#endif

// TODO
int lprintf(const char *fmt, ...)
{
#ifdef _DEBUG
	static FILE *fout = fopen(path, "a");
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
// TODO
ofstream lout(path, ios_base::out|ios_base::app);
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

wstring fromString(string str)
{
	wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
	return converter.from_bytes(str);
}
