#pragma once

#include <Core.h>
#include <Types.h>

/*
* Event state handling.
*/

enum EventType : u32
{
  eEventNone,
  eEventDown,
  eEventHeld,
  eEventUp,
};

struct Event
{
  EventType mActionCurr{ eEventNone };
  EventType mActionPrev{ eEventNone };
};

/*
* Window state handling.
*/

template<typename Window> u32   WindowStatus(Window const& window)
{
  return window.mStatus;
}
template<typename Window> r32v2 WindowSize(Window const& window)
{
  return { window.mWidth, window.mHeight };
}
template<typename Window> r32   WindowAspect(Window const& window)
{
  return (r32)window.mWidth / window.mHeight;
}
template<typename Window> r32v2 MousePosition(Window const& window)
{
  return window.mMousePosition;
}
template<typename Window> r32v2 MouseScroll(Window const& window)
{
  return window.mMouseScroll;
}
template<typename Window> u32   MouseDown(Window const& window, u32 key)
{
  return window.mMouseKeyStates[key].mActionCurr == eEventDown;
}
template<typename Window> u32   MouseHeld(Window const& window, u32 key)
{
  return window.mMouseKeyStates[key].mActionCurr == eEventHeld;
}
template<typename Window> u32   MouseUp(Window const& window, u32 key)
{
  return window.mMouseKeyStates[key].mActionCurr == eEventUp;
}
template<typename Window> u32   KeyDown(Window const& window, u32 key)
{
  return window.mKeyboardKeyStates[key].mActionCurr == eEventDown;
}
template<typename Window> u32   KeyHeld(Window const& window, u32 key)
{
  return window.mKeyboardKeyStates[key].mActionCurr == eEventHeld;
}
template<typename Window> u32   KeyUp(Window const& window, u32 key)
{
  return window.mKeyboardKeyStates[key].mActionCurr == eEventUp;
}