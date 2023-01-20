#ifndef ASHES_CONSOLEAPP_H
#define ASHES_CONSOLEAPP_H

#include <string>
#include <vector>
#include <cstdint>
#include <windows.h>
#include "timermanager.h"

namespace ashes {

class SimpleConsoleApp
{

public:

    SimpleConsoleApp();
    SimpleConsoleApp(const SimpleConsoleApp&) = delete;
    virtual ~SimpleConsoleApp() = 0;
    SimpleConsoleApp& operator = (const SimpleConsoleApp&) = delete;

    void Run();
    void Exit();

protected:

    typedef std::uint32_t WidgetID;

    WidgetID GetFocusWidget() const;
    TimerManager& GetTimerManager();

    virtual void OnInit() {}
    virtual void OnUninit() {}
    
    //==========================================================================
    // Event dispatch
    //==========================================================================

    virtual void DealEvent(const INPUT_RECORD& event);
    virtual void DealMouseMoveEvent(const COORD& mouse_coord);
    virtual void DealMouseClickEvent();
    virtual void DealKeyEvent(WORD /*key*/, bool /*pressed*/) {}

    //==========================================================================
    // Widget routing
    //==========================================================================

    static const WidgetID kNullWidgetID = 0;

    virtual WidgetID FindFocusWidget(const COORD& mouse_coord) const;
    virtual void OnMouseEnter(WidgetID /*widget*/) {}
    virtual void OnMouseMove(WidgetID /*widget*/) {}
    virtual void OnMouseLeave(WidgetID /*widget*/) {}
    virtual void OnMouseClick(WidgetID /*widget*/) {}

private:
    
    bool         pending_exit_ = false;
    WidgetID     focus_widget_ = kNullWidgetID;
    TimerManager timer_manager_;
};

}

#endif