#pragma once

#include <iostream>
#include <fstream>
#include <cstdarg>
#include <string>
#include <windows.h>
using namespace std;

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

#ifdef _DEBUG
extern ofstream lout;
#else
extern ostream lout;
#endif

int lprintf(const char *fmt, ...);
string getSelfPath();
string toString(wstring str);
wstring fromString(string str);
