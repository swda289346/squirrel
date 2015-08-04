#pragma once

#include <iostream>
#include <fstream>
#include <cstdarg>
#include <string>
using namespace std;

#ifdef _DEBUG
extern ofstream lout;
#else
extern ostream lout;
#endif

int lprintf(const char *fmt, ...);
string getSelfPath();
