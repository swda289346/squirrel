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
	{L'£t', 1},
	{L'£u', 1},
	{L'£v', 1},
	{L'£w', 1},
	{L'£x', 1},
	{L'£y', 1},
	{L'£z', 1},
	{L'£{', 1},
	{L'£|', 1},
	{L'£}', 1},
	{L'£~', 1},
	{L'£¡', 1},
	{L'£¢', 1},
	{L'££', 1},
	{L'£¤', 1},
	{L'£¥', 1},
	{L'£¦', 1},
	{L'£§', 1},
	{L'£¨', 1},
	{L'£©', 1},
	{L'£ª', 1},
	{L'£¸', 2},
	{L'£¹', 2},
	{L'£º', 2},
	{L'£«', 3},
	{L'£¬', 3},
	{L'£­', 3},
	{L'£®', 3},
	{L'£¯', 3},
	{L'£°', 3},
	{L'£±', 3},
	{L'£²', 3},
	{L'£³', 3},
	{L'£´', 3},
	{L'£µ', 3},
	{L'£¶', 3},
	{L'£·', 3},
	{L' ', 4},
	{L'£½', 4},
	{L'£¾', 4},
	{L'£¿', 4},
	{L'£»', 4},
};

void PhoneticCombination::setChar(wchar_t sc)
{
	c[phoneticTypeTable.at(sc)] = (sc!=L' '?sc:0);
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
