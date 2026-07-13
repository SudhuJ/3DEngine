#pragma once

#include "helpers.h"

struct SelectEvent {
  FLOW_INLINE SelectEvent(entityID entity):EnttID(entity) {

  }
  entityID EnttID;
};
