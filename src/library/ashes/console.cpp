#include "console.h"
#include <cctype>
#include <cassert>
#include <algorithm>
#include "winapi.h"

namespace ashes {

namespace {

CONSOLE_SCREEN_BUFFER_INFOEX GetConsoleBufferInfoEX(HANDLE console)
{
    CONSOLE_SCREEN_BUFFER_INFOEX info = {sizeof(info)};
    BOOL success = ::GetConsoleScreenBufferInfoEx(console, &info);
    assert(success);
    return info;
}

bool ResizeConsoleBuffer(HANDLE console, Coord size)
{
    CONSOLE_SCREEN_BUFFER_INFOEX info = GetConsoleBufferInfoEX(console);
    info.dwSize = size;
    info.dwMaximumWindowSize = size;
    info.srWindow = Rect({0, 0}, size);
    info.srWindow.Bottom += 1;
    return ::SetConsoleScreenBufferInfoEx(console, &info);
}

void ModifyCharInfo(CHAR_INFO& ci, wchar_t ch, WORD lvb_attributes, WORD color)
{
    ci.Char.UnicodeChar = ch;
    ci.Attributes = (ci.Attributes & 0xFF) | lvb_attributes;
    ci.Attributes = graph::BlendColor(ci.Attributes, color);
}

}

//==============================================================================
// Console
//==============================================================================

Coord Console::Size() const
{
    return window_size_;
}

Rect Console::Region() const
{
    return Rect({0, 0}, window_size_);
}

bool Console::Resize(const Coord& size)
{
    if (!ResizeConsoleBuffer(window_output_, size))
        return false;

    if (!EnableDoubleBuffer(IsDoubleBufferEnabled()))
        return false;

    window_size_ = size;
    return true;
}

void Console::Center()
{
    BOOL success = TRUE;
    HWND window = ::GetConsoleWindow();
    assert(window != NULL);

    // window size
    RECT window_rect;
    success = ::GetWindowRect(window, &window_rect);
    assert(success);
    LONG width = window_rect.right - window_rect.left;
    LONG height = window_rect.bottom - window_rect.top;

    // work area size
    RECT work_area;
    success = ::SystemParametersInfo(SPI_GETWORKAREA, 0, &work_area, 0);
    assert(success);
    LONG work_area_width = work_area.right - work_area.left;
    LONG work_area_height = work_area.bottom - work_area.top;

    // center
    LONG left = work_area.left + (work_area_width  - width)  / 2;
    LONG top  = work_area.top  + (work_area_height - height) / 2;
    success = ::SetWindowPos(window, HWND_TOPMOST, left, top, 0, 0, SWP_NOSIZE);
    assert(success);
}

void Console::SetUnresizable()
{
    HWND window = ::GetConsoleWindow();
    assert(window != NULL);
    LONG style = ::GetWindowLong(window, GWL_STYLE);
    style &= ~(WS_MAXIMIZEBOX | WS_SIZEBOX);
    ::SetWindowLong(window, GWL_STYLE, style);
}

void Console::SetCodePage(UINT code_page)
{
    BOOL success = ::SetConsoleOutputCP(code_page);
    assert(success);
}

void Console::SetFont(const std::wstring& face_name, const Coord& size, UINT weight)
{
    CONSOLE_FONT_INFOEX info = {0};
    info.cbSize = sizeof(info);
    info.FontWeight = weight;
    info.dwFontSize = size;
    ::wcscpy_s(info.FaceName, face_name.c_str());
    BOOL success = ::SetCurrentConsoleFontEx(window_output_, FALSE, &info);
    assert(success);
}

void Console::SetTitle(const String& title)
{
    BOOL success = ::SetConsoleTitle(title.c_str());
    assert(success);
}

void Console::SetUneditable()
{
    BOOL success = TRUE;

    // disable quick edit mode.
    DWORD mode = 0;
    success = ::GetConsoleMode(input_, &mode);
    assert(success);
    success = ::SetConsoleMode(input_, mode & ~ENABLE_QUICK_EDIT_MODE);
    assert(success);

    // hide console cursor.
    CONSOLE_CURSOR_INFO cursor_info = {1, FALSE};
    success = ::SetConsoleCursorInfo(window_output_, &cursor_info);
    assert(success);
}

void Console::SetDefaultBorder(ashes::graph::Border border)
{
    assert(1 <= border.width && border.width <= 2);
    assert(MeasureWidth(border.line_vert.data()) == border.width);
    assert(MeasureWidth(border.line_hori.data()) == border.width);
    assert(MeasureWidth(border.corner_tl.data()) == border.width);
    assert(MeasureWidth(border.corner_tr.data()) == border.width);
    assert(MeasureWidth(border.corner_bl.data()) == border.width);
    assert(MeasureWidth(border.corner_br.data()) == border.width);
    default_border_ = std::move(border);
}

void Console::SetDefaultBorderColor(WORD color)
{
    default_border_.color = color;
}

int Console::CountEvent() const
{
    DWORD number = 0;
    BOOL success = ::GetNumberOfConsoleInputEvents(input_, &number);
    assert(success);
    return static_cast<int>(number);
}

bool Console::ReadEvent(INPUT_RECORD& event)
{
    if (CountEvent() > 0)
    {
        WaitEvent(event);
        return true;
    }
    return false;
}

void Console::WaitEvent(INPUT_RECORD& event)
{
    DWORD readed_number = 0;
    ::ReadConsoleInput(input_, &event, 1, &readed_number);
    assert(readed_number == 1);
}

SHORT Console::MeasureWidth(wchar_t ch)
{
    return ::iswascii(ch) ? 1 : 2;
}

SHORT Console::MeasureWidth(const char* str)
{
    return MeasureWidth(win::A2W(str, -1, ::GetConsoleCP()).data());
}

SHORT Console::MeasureWidth(const wchar_t* str)
{
    SHORT width = 0;
    for (; *str; ++str) { width += MeasureWidth(*str); };
    return width;
}

void Console::DrawColor(WORD color, const Rect& rect)
{
    const Rect  clamped_rect = Rect::Intersect(rect, Region());
    const Coord clamped_size = clamped_rect.Size();

    if (!rect.IsEmpty() && !clamped_rect.IsEmpty())
    {
        BOOL success = TRUE;
        elem_buffer_.resize(clamped_size.X * clamped_size.Y);

        // read char infos inside rect region.
        Rect read_region = clamped_rect;
        success = ::ReadConsoleOutputW(output_, elem_buffer_.data(),
            clamped_size, {0, 0}, &read_region);
        assert(success);
    
        // modify color attributes in char infos.
        std::for_each(elem_buffer_.begin(), elem_buffer_.end(), 
            [color](CHAR_INFO& ci) {
                ci.Attributes = graph::BlendColor(ci.Attributes, color); });

        // write char infos back to rect region.
        Rect write_region = clamped_rect;
        success = ::WriteConsoleOutputW(output_, elem_buffer_.data(),
            clamped_size, {0, 0}, &write_region);
        assert(success);
    }
}

Rect Console::DrawString(const String& str, const Coord& coord, WORD color)
{
#ifdef UNICODE
    return DrawStringW(str, coord, color);
#else
    return DrawStringA(str, coord, color);
#endif
}

Rect Console::DrawString(const String& str, const Rect& rect, WORD color)
{
#ifdef UNICODE
    return DrawStringW(str, rect, color);
#else
    return DrawStringA(str, rect, color);
#endif
}

void Console::DrawBlank(const Rect& rect, WORD color)
{
    const Rect  clamped_rect = Rect::Intersect(rect, Region());
    const Coord clamped_size = clamped_rect.Size();

    if (!rect.IsEmpty() && !clamped_rect.IsEmpty())
    {
        elem_buffer_.resize(clamped_size.X * clamped_size.Y);
        BOOL success = TRUE;

        // read char infos inside rect region.
        Rect read_region = clamped_rect;
        success = ::ReadConsoleOutputW(output_, elem_buffer_.data(),
            clamped_size, {0, 0}, &read_region);
        assert(success);

        // modify character and color attributes in char infos.
        std::for_each(elem_buffer_.begin(), elem_buffer_.end(), 
            [color](CHAR_INFO& ci) { ModifyCharInfo(ci, L' ', 0, color); });

        // write char infos back to rect region.
        Rect write_region = clamped_rect;
        success = ::WriteConsoleOutputW(output_, elem_buffer_.data(),
            clamped_size, {0, 0}, &write_region);
        assert(success);
    }
}

void Console::DrawRectInnerBorder(
    const Rect& rect, 
    const graph::Border* specified_border)
{
    const auto& border = GetBorder(specified_border);
    const SHORT lastx  = rect.Right - border.width + 1;

    if (rect.Width() >= 2 * border.width && rect.Height() >= 2)
    {
        // draw corner.
        DrawString(border.corner_tl, {rect.Left, rect.Top},    border.color);
        DrawString(border.corner_bl, {rect.Left, rect.Bottom}, border.color);
        DrawString(border.corner_tr, {lastx,     rect.Top},    border.color);
        DrawString(border.corner_br, {lastx,     rect.Bottom}, border.color);

        // draw vertical line.
        for (SHORT y = rect.Top + 1 ; y < rect.Bottom ; ++y)
        {
            DrawString(border.line_vert, {rect.Left, y}, border.color);
            DrawString(border.line_vert, {lastx,     y}, border.color);
        }
        
        // draw horizontal line.
        for (SHORT x = rect.Left + border.width; x < lastx; x += border.width)
        {
            DrawString(border.line_hori, {x, rect.Top},    border.color);
            DrawString(border.line_hori, {x, rect.Bottom}, border.color);
        }
    }
}

void Console::DrawRectOuterBorder(
    const Rect& rect,
    const graph::Border* specified_border)
{
    if (!rect.IsEmpty())
    {
        const graph::Border& border = GetBorder(specified_border);
        Rect outer_rect = Rect(rect).Extend({border.width, 1});
        outer_rect.Right += (rect.Width() % border.width == 0 ? 0 : 1);
        DrawRectInnerBorder(outer_rect, &border);
    }
}

Rect Console::DrawStrings(
    const StringVector& strs,
    const Rect& rect,
    WORD color)
{
#ifdef UNICODE
    return DrawStringsW(strs, rect, color);
#else
    return DrawStringsA(strs, rect, color);
#endif
}

Rect Console::DrawFramedStrings(
    const StringVector& strs,
    const Rect& rect,
    WORD color,
    const graph::Border* specified_border)
{
    Rect str_region = DrawStrings(strs, rect, color);
    DrawRectOuterBorder(str_region, specified_border);
    return str_region;
}

bool Console::IsDoubleBufferEnabled() const
{
    return output_ == memory_output_;
}

bool Console::EnableDoubleBuffer(bool enable)
{
    output_ = window_output_;

    if (enable && CreateDoubleBuffer())
    {
        output_ = memory_output_;
    }

    return IsDoubleBufferEnabled() == enable;
}

void Console::FlushDoubleBuffer()
{
    // allocate char infos buffer.
    const Coord buffer_size = Size();
    elem_buffer_.resize(buffer_size.X * buffer_size.Y);
    BOOL success = TRUE;

    // read all char info from memory double buffer.
    Rect read_region = {{0, 0}, buffer_size};
    success = ::ReadConsoleOutputW(memory_output_, elem_buffer_.data(),
        buffer_size, {0, 0}, &read_region);
    assert(success);

    // write all char info to console window buffer.
    Rect write_region = {{0, 0}, buffer_size};
    success = ::WriteConsoleOutputW(window_output_, elem_buffer_.data(),
        buffer_size, {0, 0}, &write_region);
    assert(success);
}

Console::Console()
{
    input_ = ::GetStdHandle(STD_INPUT_HANDLE);
    output_ = ::GetStdHandle(STD_OUTPUT_HANDLE);

    assert(input_ != INVALID_HANDLE_VALUE);
    assert(output_ != INVALID_HANDLE_VALUE);

    window_output_ = output_;
    window_size_ = GetConsoleBufferInfoEX(window_output_).dwSize;
    default_border_ = graph::BorderThickDoubleLine();
}

Console::~Console()
{
    DestroyDoubleBuffer();
}

void Console::HoriLeftAlignClamp(
    const std::wstring& str,
    CoordRange& anchor,
    const CoordRange& clip,
    CoordRange& indexes)
{
    struct IndexAndXCoord { std::size_t i; SHORT x; };

    // clamp anchor by clip.
    const SHORT clamped_left  = (std::max)(anchor.first, clip.first);
    const SHORT clamped_right = (std::min)(anchor.second, clip.second);

    // find first character of truncated string.
    IndexAndXCoord char1 = {0, anchor.first};
    for (; char1.i < str.size(); ++char1.i)
    {
        SHORT char_width = MeasureWidth(str[char1.i]);
        if (char1.x >= clamped_left && char_width > 0) { break; }
        char1.x += char_width;
    }

    // find last character of truncated string.
    IndexAndXCoord char2 = char1;
    for (; char2.i < str.size(); ++char2.i)
    {
        SHORT char_width = MeasureWidth(str[char2.i]);
        if (char2.x + char_width - 1 > clamped_right) { break; }
        char2.x += char_width;
    }

    // compute clamped index range and coord range.
    indexes.first = static_cast<SHORT>(char1.i);
    indexes.second = static_cast<SHORT>(char2.i - 1);
    anchor.first = char1.x;
    anchor.second = char2.x - 1;
}

void Console::HoriMidAlignClamp(
    const std::wstring& str,
    CoordRange& anchor,
    const CoordRange& clip,
    CoordRange& indexes)
{
    const SHORT str_width  = MeasureWidth(str.data());
    const SHORT anchor_mid = (anchor.first + anchor.second) / 2;
    const SHORT mid_offset = 2 * anchor_mid - (anchor.first + anchor.second);

    // combine anchor range to clip.
    SHORT combined_first  = (std::max)(anchor.first,  clip.first);
    SHORT combined_second = (std::min)(anchor.second, clip.second);

    // convert mid align anchor to left align anchor.
    anchor.first  = anchor_mid - (str_width + mid_offset) / 2;
    anchor.second = anchor.first + str_width - 1;

    HoriLeftAlignClamp(str, anchor, {combined_first, combined_second}, indexes);
}

void Console::VertMidAlignClamp(
    std::size_t num_strs,
    CoordRange& anchor,
    CoordRange& indexes)
{
    const SHORT anchor_mid = (anchor.first + anchor.second) / 2;
    const SHORT mid_offset = 2 * anchor_mid - (anchor.first + anchor.second);
    const SHORT str_number = static_cast<SHORT>(num_strs);

    SHORT first  = anchor_mid - (str_number + mid_offset) / 2;
    SHORT second = first + str_number - 1;

    anchor.first  = (std::max)(first,  anchor.first);
    anchor.second = (std::min)(second, anchor.second);

    indexes.first  = anchor.first - first;
    indexes.second = indexes.first + (anchor.second - anchor.first);
}

const graph::Border& Console::GetBorder(const graph::Border* specified_border) const
{
    return specified_border == nullptr ? default_border_ : *specified_border;
}

Rect Console::DrawStringA(const std::string& str, const Coord& coord, WORD color)
{
    return DrawStringW(win::A2W(str, ::GetConsoleCP()), coord, color);
}

Rect Console::DrawStringA(const std::string& str, const Rect& rect, WORD color)
{
    return DrawStringW(win::A2W(str, ::GetConsoleCP()), rect, color);
}

Rect Console::DrawStringsA(
    const std::vector<std::string>& strs,
    const Rect& rect,
    WORD color)
{
    const Rect clip = Region();
    CoordRange xrange = {rect.Right, rect.Left};

    CoordRange yanchor = {rect.Top, rect.Bottom};
    CoordRange yindexes;
    VertMidAlignClamp(strs.size(), yanchor, yindexes);

    for (SHORT y = yanchor.first ; y <= yanchor.second ; ++y)
    {
        const std::string& str = strs[y - yanchor.first + yindexes.first];
        std::wstring wstr = win::A2W(str, ::GetConsoleCP());
        CoordRange xanchor = {rect.Left, rect.Right};
        CoordRange xindexes;
        HoriMidAlignClamp(wstr, xanchor, {clip.Left, clip.Right}, xindexes);

        if (xindexes.first <= xindexes.second)
        {
            // consider x-range of current string line, even we can't show it.
            xrange.first  = (std::min)(xrange.first,  xanchor.first);
            xrange.second = (std::max)(xrange.second, xanchor.second);
            
            if (clip.Top <= y && y <= clip.Bottom)
            {
                WriteOutput(wstr, xindexes, xanchor, y, color);
            }
        }
    }

    return {xrange.first, yanchor.first, xrange.second, yanchor.second};
}

Rect Console::DrawStringW(const std::wstring& str, const Coord& coord, WORD color)
{
    const Rect clip = Region();

    if (!str.empty() && (clip.Top <= coord.Y && coord.Y <= clip.Bottom))
    {
        CoordRange anchor = {coord.X, clip.Right};
        CoordRange indexes;
        HoriLeftAlignClamp(str, anchor, {clip.Left, clip.Right}, indexes);

        if (indexes.first <= indexes.second)
        {
            WriteOutput(str, indexes, anchor, coord.Y, color);
            return {anchor.first, coord.Y, anchor.second, coord.Y};
        }
    }
    
    return {{0, 0}, {0, 0}};
}

Rect Console::DrawStringW(const std::wstring& str, const Rect& rect, WORD color)
{
    const Rect clip = Region();
    const SHORT y = (rect.Top + rect.Bottom) / 2;

    if (!str.empty() && !rect.IsEmpty() && (clip.Top <= y && y <= clip.Bottom))
    {
        CoordRange anchor = {rect.Left, rect.Right};
        CoordRange indexes;
        HoriMidAlignClamp(str, anchor, {clip.Left, clip.Right}, indexes);

        if (indexes.first <= indexes.second)
        {
            WriteOutput(str, indexes, anchor, y, color);
            return {anchor.first, y, anchor.second, y};
        }
    }

    return {{0, 0}, {0, 0}};
}

Rect Console::DrawStringsW(
    const std::vector<std::wstring>& strs,
    const Rect& rect,
    WORD color)
{
    const Rect clip = Region();
    CoordRange xrange = {rect.Right, rect.Left};

    CoordRange yanchor = {rect.Top, rect.Bottom};
    CoordRange yindexes;
    VertMidAlignClamp(strs.size(), yanchor, yindexes);

    for (SHORT y = yanchor.first ; y <= yanchor.second ; ++y)
    {
        const std::wstring& str = strs[y - yanchor.first + yindexes.first];
        CoordRange xanchor = {rect.Left, rect.Right};
        CoordRange xindexes;
        HoriMidAlignClamp(str, xanchor, {clip.Left, clip.Right}, xindexes);

        if (xindexes.first <= xindexes.second)
        {
            // consider x-range of current string line, even we can't show it.
            xrange.first  = (std::min)(xrange.first,  xanchor.first);
            xrange.second = (std::max)(xrange.second, xanchor.second);
            
            if (clip.Top <= y && y <= clip.Bottom)
            {
                WriteOutput(str, xindexes, xanchor, y, color);
            }
        }
    }

    return {xrange.first, yanchor.first, xrange.second, yanchor.second};
}

void Console::WriteOutput(
    const std::wstring& str,
    const CoordRange& indexes,
    const CoordRange& xrange,
    SHORT y,
    WORD color)
{
    BOOL success = TRUE;

    // allocate char infos buffer.
    const Rect region = {xrange.first, y, xrange.second, y};
    elem_buffer_.resize(region.Width());
    CHAR_INFO* elem = elem_buffer_.data();

    // read char infos at line coord.Y.
    Rect read_region = region;
    success = ::ReadConsoleOutputW(output_, elem_buffer_.data(),
        region.Size(), {0, 0}, &read_region);
    assert(success);

    // modify character and color attributes in char infos.    
    for (SHORT idx = indexes.first; idx <= indexes.second; ++idx)
    {
        const wchar_t ch = str[idx];
        const SHORT width = MeasureWidth(ch);

        if (width == 2)
        {
            ModifyCharInfo(*elem++, ch, COMMON_LVB_LEADING_BYTE, color);
            ModifyCharInfo(*elem++, ch, COMMON_LVB_TRAILING_BYTE, color);
        }
        else if (width == 1)
        {
            ModifyCharInfo(*elem++, ch, 0, color);
        }
    }

    // write char infos back to line coord.Y
    Rect write_region = region;
    success = ::WriteConsoleOutputW(output_, elem_buffer_.data(), 
        region.Size(), {0, 0}, &write_region);
    assert(success);
}

bool Console::CreateDoubleBuffer()
{
    // create console screen buffer.
    if (memory_output_ == INVALID_HANDLE_VALUE)
    {
        memory_output_ = ::CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 
            0, nullptr, CONSOLE_TEXTMODE_BUFFER, nullptr);
        if (memory_output_ == INVALID_HANDLE_VALUE)
            return false;
    }
    
    // resize console screen buffer
    Coord window_size = GetConsoleBufferInfoEX(window_output_).dwSize;
    Coord memory_size = GetConsoleBufferInfoEX(memory_output_).dwSize;
    if (memory_size != window_size)
    {
        if (!ResizeConsoleBuffer(memory_output_, window_size))
            return false;
    }

    return true;
}

void Console::DestroyDoubleBuffer()
{
    if (memory_output_ != INVALID_HANDLE_VALUE)
    {
        ::CloseHandle(memory_output_);
        memory_output_ = INVALID_HANDLE_VALUE;
    }
}

Console& GConsole()
{
    static Console instance;
    return instance;
}

}