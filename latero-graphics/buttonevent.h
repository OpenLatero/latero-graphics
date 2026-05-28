#pragma once

/**
 * TODO: probably not the best way to do this...
 * move to latero?
 */

namespace latero::graphics {

typedef struct {
    bool button0_up_event;
    bool button0_down_event;
    bool button1_up_event;
    bool button1_down_event;
} ButtonEvent;

} // namespace