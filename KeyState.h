#pragma once

#include <map>
#include <set>
using namespace std;

class KeyState
{
	public:
		int count;
		int superKeyCount;
		map<wchar_t, int> lastSuperKey;
		
		KeyState();
		void reset();
		void setKey(wchar_t c);
		void releaseKey(wchar_t c);
		bool isCombinedKey(wchar_t c) const;
		bool isShift() const;
		bool isOnlyShift() const;
		int getSuperKeyCount() const;
		static bool isSuperKey(wchar_t c);
};
