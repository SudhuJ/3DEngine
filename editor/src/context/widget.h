#pragma once

#include "events.h"
#include "inputs.h"

struct GuiContext;

struct IWidget {
    FLOW_INLINE IWidget(GuiContext*) {};
    FLOW_INLINE virtual ~IWidget() = default;
    FLOW_INLINE virtual void onSelect(Entity){}
    FLOW_INLINE virtual void onShow(GuiContext*){}
    FLOW_INLINE virtual void setTitle(const char*){}
};

using Widget = std::unique_ptr<IWidget>;
