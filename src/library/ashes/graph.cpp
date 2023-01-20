#include "graph.h"
#include <tchar.h>
#include <cassert>
#include <algorithm>

namespace ashes { namespace graph {

//==============================================================================
// Color
//==============================================================================

WORD MakeNullColor()
{
    return 0x0300;
}

WORD MakeFGColor(WORD color)
{
    return (color & 0x000F) | 0x0200;
}

WORD MakeBGColor(WORD color)
{
    return (color & 0x00F0) | 0x0100;
}

WORD FlipColor(WORD color)
{
    WORD bits1 = ((color & 0x000F) << 4) | ((color & 0x00F0) >> 4);
    WORD bits2 = ((color & 0x0100) << 1) | ((color & 0x0200) >> 1);
    return bits2 | bits1;
}

WORD CrossColor(WORD fgcolor, WORD bgcolor)
{
    return (fgcolor & 0x010F) | (bgcolor & 0x02F0);
}

WORD BlendColor(WORD dest, WORD src)
{
    static const WORD kMasks[] = {0x00FF, 0x00F0, 0x000F, 0x0000};
    WORD mask = kMasks[(src & 0x0300) >> 8];
    return (dest & ~mask) | (src & mask);
}

//==============================================================================
// Border
//==============================================================================

const Border& BorderPlusSign()
{
    static const Border kBorder = {TEXT("+"), TEXT("+"), 
        TEXT("+"), TEXT("+"), TEXT("+"), TEXT("+"), 1, 0x200};
    return kBorder;
}

const Border& BorderThinLine()
{
    static const Border kBorder = {TEXT("|"), TEXT("-"), 
        TEXT("+"), TEXT("+"), TEXT("+"), TEXT("+"), 1, 0x200};
    return kBorder;
}

const Border& BorderThickDoubleLine()
{
    static const Border kBorder = {TEXT("║"), TEXT("═"), 
        TEXT("╔"), TEXT("╗"), TEXT("╚"), TEXT("╝"), 2, 0x200};
    return kBorder;
}

const Border& BorderThickSolidLine()
{
    static const Border kBorder = {TEXT("┃"), TEXT("━"), 
        TEXT("┏"), TEXT("┓"), TEXT("┗"), TEXT("┛"), 2, 0x200};
    return kBorder;
}

//==============================================================================
// Others
//==============================================================================

String ToHalfWidthDigit(int number, int min_num_digits)
{
    assert(number >= 0);

    TCHAR buffer[10];
    int num_digits = ::_stprintf_s(buffer, TEXT("%d"), number);
    assert(num_digits > 0);

    int str_len = (std::max)(num_digits, min_num_digits);
    String str(str_len, '0');
    str.replace(str_len - num_digits, num_digits, buffer);

    return str;
}

String ToFullWidthDigit(int number, int min_num_digits)
{
    static const TCHAR* kFullWidthDigit[] = { 
        TEXT("０"), TEXT("１"), TEXT("２"), TEXT("３"), TEXT("４"),
        TEXT("５"), TEXT("６"), TEXT("７"), TEXT("８"), TEXT("９")};

    assert(number >= 0);
    String halfs = ToHalfWidthDigit(number, min_num_digits);
    String fulls;
    std::for_each(halfs.cbegin(), halfs.cend(), [&fulls](TCHAR ch) {
        return fulls += kFullWidthDigit[ch - '0']; });

    return fulls;
}

}}