#include <iostream>
#include <fstream>
#include <cstdarg>
#include <string>
#include <windows.h>
#include "util.h"
using namespace std;

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

// TODO
int lprintf(const char *fmt, ...)
{
#ifdef _DEBUG
	static FILE *fout = fopen("c:\\squirrel.log", "a");
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
	return string(path);
}
