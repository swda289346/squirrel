#pragma once

#include <iostream>
#include <fstream>
#include <cstdarg>
#include <string>
using namespace std;

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

#ifdef _DEBUG
extern ofstream lout;
#else
extern ostream lout;
#endif

int lprintf(const char *fmt, ...);
string getSelfPath();
