//==============================================================================
// Console: 
// 
// Console is a wrap class of windows console apis.
// If use that class, you should't use windows console apis yet!
//
// The coordinate system is about screen space, the unit is not character!
//   we use character width to refer the occupied screen width of a character.
//   we always think the width of ascii character is 1, and others is 2.
//   so don't use characters which is not meet the above assumed!
//==============================================================================

#ifndef ASHES_CONSOLE_H
#define ASHES_CONSOLE_H

#include <string>
#include <vector>
#include <utility>
#include <windows.h>
#include "rect.h"
#include "coord.h"
#include "graph.h"

namespace ashes {

class Console
{
    friend Console& GConsole();

public:

    // Typedefs
    typedef std::vector<String>     StringVector;
    typedef std::pair<SHORT, SHORT> CoordRange;

    // Window
    Coord Size() const;
    Rect Region() const;
    bool Resize(const Coord& size);
    void Center();
    void SetUnresizable();

    // Text
    void SetCodePage(UINT code_page);
    void SetFont(const std::wstring& face_name, const Coord& size, UINT weight);
    void SetTitle(const String& title);
    void SetUneditable();
    
    // Preference
    void SetDefaultBorder(graph::Border border);
    void SetDefaultBorderColor(WORD color);

    // Event
    int CountEvent() const;
    bool ReadEvent(INPUT_RECORD& event);
    void WaitEvent(INPUT_RECORD& event);
    
    // Measure
    static SHORT MeasureWidth(wchar_t ch);
    static SHORT MeasureWidth(const char* str);
    static SHORT MeasureWidth(const wchar_t* str);

    // Draw
    void DrawColor(WORD color, const Rect& rect);
    Rect DrawString(const String& str, const Coord& coord, WORD color);
    Rect DrawString(const String& str, const Rect& rect, WORD color);
    void DrawBlank(const Rect& rect, WORD color);
    void DrawRectInnerBorder(const Rect& rect, const graph::Border* border = nullptr);
    void DrawRectOuterBorder(const Rect& rect, const graph::Border* border = nullptr);
    Rect DrawStrings(const StringVector& strs, const Rect& rect, WORD color);
    Rect DrawFramedStrings(const StringVector& strs, const Rect& rect, 
        WORD color, const graph::Border* border = nullptr);

    // Double buffer
    bool IsDoubleBufferEnabled() const;
    bool EnableDoubleBuffer(bool enable);
    void FlushDoubleBuffer();

private:

    //==========================================================================
    // Special functions
    //==========================================================================

    Console();
    Console(const Console&) = delete;
    ~Console();
    Console& operator = (const Console&) = delete;

    //==========================================================================
    // Align and clamp: align and clamp by anchor, aslo clamp by clip,
    // output clamped index/coord range to indexes/anchor.
    //==========================================================================

    static void HoriLeftAlignClamp(const std::wstring& str, CoordRange& anchor,
        const CoordRange& clip, CoordRange& indexes);

    static void HoriMidAlignClamp(const std::wstring& str, CoordRange& anchor,
        const CoordRange& clip, CoordRange& indexes);

    static void VertMidAlignClamp(std::size_t num_strs, CoordRange& anchor,
        CoordRange& indexes);

    //==========================================================================
    // Draw implementation: use wstring for underlying implementation always.
    //==========================================================================

    const graph::Border& GetBorder(const graph::Border* specified_border) const;

    Rect DrawStringA(const std::string& str, const Coord& coord, WORD color);
    Rect DrawStringA(const std::string& str, const Rect& rect, WORD color);
    Rect DrawStringsA(const std::vector<std::string>& strs,
        const Rect& rect, WORD color);

    Rect DrawStringW(const std::wstring& str, const Coord& coord, WORD color);
    Rect DrawStringW(const std::wstring& str, const Rect& rect, WORD color);
    Rect DrawStringsW(const std::vector<std::wstring>& strs,
        const Rect& rect, WORD color);

    void WriteOutput(const std::wstring& str, const CoordRange& indexes,
        const CoordRange& xrange, SHORT y, WORD color);

    //==========================================================================
    // Double buffer implementation
    //==========================================================================

    bool CreateDoubleBuffer();
    void DestroyDoubleBuffer();

private:

    HANDLE input_ = INVALID_HANDLE_VALUE;          // the current input buffer
    HANDLE output_ = INVALID_HANDLE_VALUE;         // the current output buffer
    HANDLE window_output_ = INVALID_HANDLE_VALUE;  // the console window output buffer
    HANDLE memory_output_ = INVALID_HANDLE_VALUE;  // the memory double output buffer

    Coord                  window_size_;     // console window size
    graph::Border          default_border_;  // default border used for draw functions
    std::vector<CHAR_INFO> elem_buffer_;     // buffer used for read data
};

Console& GConsole();

}

#endif
