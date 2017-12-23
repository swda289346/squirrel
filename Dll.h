#pragma once

#include <atomic>
using namespace std;

#ifdef _DEBUG
#define SquirrelName L"Squirrel - Debug"
#define SquirrelGUID L"{AA95DE4F-99FA-41CF-844C-57D01901E740}"
#else
#define SquirrelName L"Squirrel"
#define SquirrelGUID L"{7841FDFF-FBE7-4D1F-9E57-F56CAF7F05A5}"
#endif

extern atomic<int> objectCounter;
