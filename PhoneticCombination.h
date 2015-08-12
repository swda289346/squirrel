#pragma once

#include <string>
using namespace std;

class PhoneticCombination
{
	public:
		wchar_t c[5];
		
		PhoneticCombination(wstring text);
		void setChar(wchar_t sc);
		void clearPos(int pos);
		wstring asString() const;
};
