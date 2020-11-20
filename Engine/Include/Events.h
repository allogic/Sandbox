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
* Context state handling.
*/

template<typename Context> u32   WindowStatus(Context const& context)
{
  return context.mStatus;
}
template<typename Context> r32v2 WindowSize(Context const& context)
{
  return { context.mWidth, context.mHeight };
}
template<typename Context> r32   WindowAspect(Context const& context)
{
  return (r32)context.mWidth / context.mHeight;
}
template<typename Context> r32v2 MousePosition(Context const& context)
{
  return context.mMousePosition;
}
template<typename Context> r32v2 MouseScroll(Context const& context)
{
  return context.mMouseScroll;
}
template<typename Context> u32   MouseDown(Context const& context, u32 key)
{
  return context.mMouseKeyStates[key].mActionCurr == eEventDown;
}
template<typename Context> u32   MouseHeld(Context const& context, u32 key)
{
  return context.mMouseKeyStates[key].mActionCurr == eEventHeld;
}
template<typename Context> u32   MouseUp(Context const& context, u32 key)
{
  return context.mMouseKeyStates[key].mActionCurr == eEventUp;
}
template<typename Context> u32   KeyDown(Context const& context, u32 key)
{
  return context.mKeyboardKeyStates[key].mActionCurr == eEventDown;
}
template<typename Context> u32   KeyHeld(Context const& context, u32 key)
{
  return context.mKeyboardKeyStates[key].mActionCurr == eEventHeld;
}
template<typename Context> u32   KeyUp(Context const& context, u32 key)
{
  return context.mKeyboardKeyStates[key].mActionCurr == eEventUp;
}