#ifndef ASHES_EVENTDISPATCHER_H
#define ASHES_EVENTDISPATCHER_H

#include <vector>
#include <cassert>
#include <cstdint>
#include <algorithm>
#include <functional>

namespace ashes {

template <class EventType>
class EventDispatcher
{
public:

    typedef std::uintptr_t           ListenerID;
    typedef std::function<EventType> Callback;

    EventDispatcher() = default;
    EventDispatcher(const EventDispatcher&) = delete;
    EventDispatcher& operator = (const EventDispatcher&) = delete;

    void Bind(ListenerID id, Callback cb, bool disposable);
    void Unbind(ListenerID id);
    void UnbindAll();

    template <class... Args>
    void operator () (Args&&... args);

    //==========================================================================
    // Bind listener templates
    //==========================================================================

    // Bind a function as listener, identified as the function.
    template <class Ret, class... Args>
    void Bind(Ret(*func)(Args...))
    {
        Bind(reinterpret_cast<ListenerID>(func), func, false);
    }

    // Bind an object's member function as listener, identified as the object.
    template <class T, class Ret, class... Args>
    void Bind(T* obj, Ret(T::*func)(Args...))
    {
        Bind(reinterpret_cast<ListenerID>(obj), [obj, func](Args... args) {
            return (obj->*func)(std::forward<Args>(args)...); }, false);
    }

    // Bind an object's const member function as listener, identified as the object.
    template <class T, class Ret, class... Args>
    void Bind(T* obj, Ret(T::*func)(Args...) const)
    {
        Bind(reinterpret_cast<ListenerID>(obj), [obj, func](Args... args) {
            return (obj->*func)(std::forward<Args>(args)...); }, false);
    }

    // Bind a callback as listener, identified as specified object.
    template <class T>
    void Bind(T* obj, Callback&& cb)
    {
        Bind(reinterpret_cast<ListenerID>(obj), std::forward<Callback>(cb), false);
    }

    // Bind a callback as listener without identification.
    void Bind(Callback&& cb)
    {
        Bind(0, std::forward<Callback>(cb), false);
    }

    //==========================================================================
    // Bind disposable listener templates
    //==========================================================================

    // Bind a function as listener, identified as the function.
    template <class Ret, class... Args>
    void BindDisposable(Ret(*func)(Args...))
    {
        Bind(reinterpret_cast<ListenerID>(func), func, true);
    }

    // Bind an object's member function as listener, identified as the object.
    template <class T, class Ret, class... Args>
    void BindDisposable(T* obj, Ret(T::*func)(Args...))
    {
        Bind(reinterpret_cast<ListenerID>(obj), [obj, func](Args... args) {
            return (obj->*func)(std::forward<Args>(args)...); }, true);
    }

    // Bind an object's const member function as listener, identified as the object.
    template <class T, class Ret, class... Args>
    void BindDisposable(T* obj, Ret(T::*func)(Args...) const)
    {
        Bind(reinterpret_cast<ListenerID>(obj), [obj, func](Args... args) {
            return (obj->*func)(std::forward<Args>(args)...); }, true);
    }

    // Bind a callback as listener, identified as specified object.
    template <class T>
    void BindDisposable(T* obj, Callback&& cb)
    {
        Bind(reinterpret_cast<ListenerID>(obj), std::forward<Callback>(cb), true);
    }

    // Bind a callback as listener without identification.
    void BindDisposable(Callback&& cb)
    {
        Bind(0, std::forward<Callback>(cb), true);
    }

    //==========================================================================
    // Unbind listener templates
    //==========================================================================

    // Unbind a function.
    template <class Ret, class... Args>
    void Unbind(Ret(*func)(Args...))
    {
        Unbind(reinterpret_cast<ListenerID>(func));
    }

    // Unbind listeners that identified as specified object.
    template <class T>
    void Unbind(T* obj)
    {
        Unbind(reinterpret_cast<ListenerID>(obj));
    }

private:

    //==========================================================================
    // Privates
    //==========================================================================

    struct Listener
    {
        ListenerID id = 0;
        Callback   callback;
        bool       pending_unbind = false;
        bool       disposable = false;
    };

    void MarkPendingUnbind(Listener& listener, bool cond);
    void RemovePendingUnbindListener();

    bool                  in_dispatching_ = false;
    bool                  has_pending_unbind_ = false;
    std::vector<Listener> listeners_;
};

template <class EventType>
void EventDispatcher<EventType>::Bind(ListenerID id, Callback cb, bool disposable)
{
    Listener listener;
    listener.id = id;
    listener.callback = std::move(cb);
    listener.disposable = disposable;
    listeners_.push_back(std::move(listener));
}

template <class EventType>
void EventDispatcher<EventType>::Unbind(ListenerID id)
{
    std::for_each(listeners_.begin(), listeners_.end(),
        [id, this](Listener& x) { MarkPendingUnbind(x, x.id == id); });
    RemovePendingUnbindListener();
}

template <class EventType>
void EventDispatcher<EventType>::UnbindAll()
{
    std::for_each(listeners_.begin(), listeners_.end(),
        [this](Listener& x) { MarkPendingUnbind(x, true); });
    RemovePendingUnbindListener();
}

template <class EventType>
template <class... Args>
void EventDispatcher<EventType>::operator () (Args&&... args)
{
    assert(!in_dispatching_);
    RemovePendingUnbindListener();
    in_dispatching_ = true;

    for (int idx = int(listeners_.size()) - 1; idx >= 0; --idx)
    {
        if (!listeners_[idx].pending_unbind)
        {
            Listener listener = std::move(listeners_[idx]);
            listener.callback(args...);
            MarkPendingUnbind(listener, listener.disposable);
            listeners_[idx] = std::move(listener);
        }
    }

    in_dispatching_ = false;
    RemovePendingUnbindListener();
}

template <class EventType>
void EventDispatcher<EventType>::MarkPendingUnbind(Listener& listener, bool cond)
{
    listener.pending_unbind |= cond;
    has_pending_unbind_ |= cond;
}

template <class EventType>
void EventDispatcher<EventType>::RemovePendingUnbindListener()
{
    if (!in_dispatching_ && has_pending_unbind_)
    {
        listeners_.erase(std::remove_if(listeners_.begin(), listeners_.end(),
            [](const Listener& x) { return x.pending_unbind; }), listeners_.end());
        has_pending_unbind_ = false;
    }
}

}

#endif