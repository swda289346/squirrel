#include <windows.h>
#include <winuser.h>
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
		if (getSuperKeyCount()>1)
			ans = true;
		else if (lastSuperKey.at(c)+1==count)
			ans = false;
		else
			ans = true;
	}
	else
	{
		if (getSuperKeyCount())
			ans = true;
		else
			ans = false;
	}
	lout << "KeyState::isCombinedKey " << int(c) << " " << ans << endl;
	lout << "KeyState::isCombinedKey superKeyCount = " << getSuperKeyCount() << endl;
	return ans;
}

bool KeyState::isShift() const
{
	return GetKeyState(VK_SHIFT)>>15;
}

bool KeyState::isOnlyShift() const
{
	return getSuperKeyCount()==1&&isShift();
}

int KeyState::getSuperKeyCount() const
{
	BYTE tmp[256];
	int count = 0;
	GetKeyboardState(tmp);
	if (tmp[VK_SHIFT]>>7)
		count++;
	if (tmp[VK_CONTROL]>>7)
		count++;
	if (tmp[VK_MENU]>>7)
		count++;
	return count;
}

bool KeyState::isSuperKey(wchar_t c)
{
	return SuperKey.count(c);
}
