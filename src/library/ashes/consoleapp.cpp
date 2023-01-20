#include "consoleapp.h"
#include "console.h"

namespace ashes {

SimpleConsoleApp::SimpleConsoleApp()
{
}

SimpleConsoleApp::~SimpleConsoleApp()
{
}

void SimpleConsoleApp::Run()
{
    OnInit();

    for (pending_exit_ = false; !pending_exit_; )
    {
        for (INPUT_RECORD event; GConsole().ReadEvent(event);)
        {
            DealEvent(event);
        }

        timer_manager_.Tick(time::Msecs(15));

        if (GConsole().IsDoubleBufferEnabled())
        {
            GConsole().FlushDoubleBuffer();
        }
    }

    OnUninit();
}

void SimpleConsoleApp::Exit()
{
    pending_exit_ = true;
}

SimpleConsoleApp::WidgetID SimpleConsoleApp::GetFocusWidget() const
{
    return focus_widget_;
}

TimerManager& SimpleConsoleApp::GetTimerManager()
{
    return timer_manager_;
}

void SimpleConsoleApp::DealEvent(const INPUT_RECORD& event)
{
    if (event.EventType == MOUSE_EVENT)
    {
        const MOUSE_EVENT_RECORD& mouse_event = event.Event.MouseEvent;

        if (mouse_event.dwEventFlags == MOUSE_MOVED)
        {
            DealMouseMoveEvent(mouse_event.dwMousePosition);
        }
        else if (mouse_event.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED)
        {
            DealMouseClickEvent();
        }
    }
    else if (event.EventType == KEY_EVENT)
    {
        const KEY_EVENT_RECORD& key_event = event.Event.KeyEvent;

        DealKeyEvent(key_event.wVirtualKeyCode, !!key_event.bKeyDown);
    }
}

void SimpleConsoleApp::DealMouseMoveEvent(const COORD& mouse_coord)
{
    const WidgetID kOldFocusWidgetID = focus_widget_;

    focus_widget_ = FindFocusWidget(mouse_coord);
    
    if (focus_widget_ == kOldFocusWidgetID)
    {
        if (focus_widget_ != kNullWidgetID)
        {
            OnMouseMove(focus_widget_);
        }
    }
    else
    {
        if (kOldFocusWidgetID != kNullWidgetID)
        {
            OnMouseLeave(kOldFocusWidgetID);
        }

        if (focus_widget_ != kNullWidgetID)
        {
            OnMouseEnter(focus_widget_);
        }
    }
}

void SimpleConsoleApp::DealMouseClickEvent()
{
    if (focus_widget_ != kNullWidgetID)
    {
        OnMouseClick(focus_widget_);
    }
}

SimpleConsoleApp::WidgetID SimpleConsoleApp::FindFocusWidget(
    const COORD&) const
{
    return kNullWidgetID;
}

}
    