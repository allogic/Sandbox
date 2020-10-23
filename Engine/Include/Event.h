#pragma once

#include <Types.h>

enum struct EventType : u32
{
  None,
  Down,
  Held,
  Up,
};

struct Event
{
  EventType mActionCurr{ EventType::None };
  EventType mActionPrev{ EventType::None };
};