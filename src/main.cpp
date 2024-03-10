
#include "sandbox.h"

#include <stdio.h>

static QuartzInitInfo quartzInfo = {
  Vec2U{ 800, 600 }, // extents
  Vec2I{ 100, 100 }, // position
  "Sandbox"
};

QUARTZ_ENTRY(Sandbox, quartzInfo)
