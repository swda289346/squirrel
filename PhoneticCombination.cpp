#include <map>
#include "PhoneticCombination.h"
#include "util.h"

PhoneticCombination::PhoneticCombination(wstring text)
{
	lout << toString(text) << endl;
	for (int i=1;i<=4;i++)
		c[i] = 0;
	for (wchar_t sc : text)
		setChar(sc);
}

static const map<wchar_t, int> phoneticTypeTable =
{
	{L'�t', 1},
	{L'�u', 1},
	{L'�v', 1},
	{L'�w', 1},
	{L'�x', 1},
	{L'�y', 1},
	{L'�z', 1},
	{L'�{', 1},
	{L'�|', 1},
	{L'�}', 1},
	{L'�~', 1},
	{L'��', 1},
	{L'��', 1},
	{L'��', 1},
	{L'��', 1},
	{L'��', 1},
	{L'��', 1},
	{L'��', 1},
	{L'��', 1},
	{L'��', 1},
	{L'��', 1},
	{L'��', 2},
	{L'��', 2},
	{L'��', 2},
	{L'��', 3},
	{L'��', 3},
	{L'��', 3},
	{L'��', 3},
	{L'��', 3},
	{L'��', 3},
	{L'��', 3},
	{L'��', 3},
	{L'��', 3},
	{L'��', 3},
	{L'��', 3},
	{L'��', 3},
	{L'��', 3},
	{L' ', 4},
	{L'��', 4},
	{L'��', 4},
	{L'��', 4},
	{L'��', 4},
};

void PhoneticCombination::setChar(wchar_t sc)
{
	if (sc==VK_BACK)
	{
		clearChar();
		return;
	}
	c[phoneticTypeTable.at(sc)] = (sc!=L' '?sc:0);
	if (phoneticTypeTable.at(sc)!=4)
		c[4] = 0;
}

void PhoneticCombination::clearChar()
{
	for (int i=4;i>=1;i--)
		if (c[i])
		{
			c[i] = 0;
			return;
		}
}

void PhoneticCombination::clearPos(int pos)
{
	c[pos] = 0;
}

wstring PhoneticCombination::asString() const
{
	wstring ans;
	for (int i=1;i<=4;i++)
		if (c[i])
			ans += c[i];
	lout << toString(ans) << endl;
	return ans;
}

bool PhoneticCombination::isEmpty() const
{
	for (int i=1;i<=4;i++)
		if (c[i])
			return false;
	return true;
}
