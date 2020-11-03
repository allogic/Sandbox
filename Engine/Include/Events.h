#pragma once

#include <Types.h>

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