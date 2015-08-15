#include <windows.h>
#include "KeyState.h"
#include "util.h"

static const wchar_t KEY_CTRL = 17;
static const wchar_t KEY_SHIFT = 16;

static const set<wchar_t> SuperKey =
{
	KEY_CTRL,
	KEY_SHIFT,
};

KeyState::KeyState()
{
	reset();
}

void KeyState::reset()
{
	count = 0;
	superKeyCount = 0;
	for (wchar_t c : SuperKey)
	{
		lastSuperKey[c] = -1;
	}
}

void KeyState::setKey(wchar_t c)
{
	lout << "KeyState::setKey " << int(c) << endl;
	if (SuperKey.count(c))
	{
		if (lastSuperKey[c]==-1)
		{
			superKeyCount++;
			lastSuperKey[c] = count;
		}
	}
	count++;
}

void KeyState::releaseKey(wchar_t c)
{
	lout << "KeyState::releaseKey " << int(c) << endl;
	if (SuperKey.count(c))
	{
		if (lastSuperKey[c]!=-1)
			superKeyCount--;
		lastSuperKey[c] = -1;
	}
}

bool KeyState::isCombinedKey(wchar_t c) const
{
	bool ans;
	if (SuperKey.count(c))
	{
		if (superKeyCount>1)
			ans = true;
		else if (lastSuperKey.at(c)+1==count)
			ans = false;
		else
			ans = true;
	}
	else
	{
		if (superKeyCount)
			ans = true;
		else
			ans = false;
	}
	lout << "KeyState::isCombinedKey " << int(c) << " " << ans << endl;
	lout << "KeyState::isCombinedKey superKeyCount = " << superKeyCount << endl;
	return ans;
}

bool KeyState::isShift() const
{
	return lastSuperKey.at(VK_SHIFT)!=-1;
}

bool KeyState::isOnlyShift() const
{
	return superKeyCount==1&&isShift();
}
