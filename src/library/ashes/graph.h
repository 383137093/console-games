#ifndef ASHES_GRAPH_H
#define ASHES_GRAPH_H

#include <string>
#include <windows.h>

namespace ashes {

typedef std::basic_string<TCHAR> String;

namespace graph {

//==============================================================================
// Color
// https://docs.microsoft.com/en-us/windows/console/char-info-str
// for attributes in CHAR_INFO, the lower byte is color, and the high byte is
// encoding just like COMMON_LVB_LEADING_BYTE.
// i think is needless to use encoding attributes when work with ashes::Console.
// so i re-define this bits for attributes passed to ashes::Console:
// 0x0100 - don't write foreground color.
// 0x0200 - don't write background color.
//==============================================================================

WORD MakeNullColor();
WORD MakeFGColor(WORD color);
WORD MakeBGColor(WORD color);

WORD FlipColor(WORD color);
WORD CrossColor(WORD fgcolor, WORD bgcolor);
WORD BlendColor(WORD dest, WORD src);

//==============================================================================
// Border
//==============================================================================

struct Border
{
    String line_vert;  // character for vertical line.
    String line_hori;  // character for horizontal line.
    String corner_tl;  // character for corner top-left.
    String corner_tr;  // character for corner top-right.
    String corner_bl;  // character for corner bottom-left.
    String corner_br;  // character for corner bottom-right.
    SHORT  width;      // character width at console screen.
    WORD   color;      // character color attributes.
};

const Border& BorderPlusSign();         // + + + + + +
const Border& BorderThinLine();         // | - + + + +
const Border& BorderThickDoubleLine();  // ║ ═ ╔ ╗ ╚ ╝
const Border& BorderThickSolidLine();   // ┃ ━ ┏ ┓ ┗ ┛

//==============================================================================
// Others
//==============================================================================

String ToHalfWidthDigit(int number, int min_num_digits);
String ToFullWidthDigit(int number, int min_num_digits);

}}

#endif