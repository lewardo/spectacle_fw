#ifndef SPCT_TOUCH_H
#define SPCT_TOUCH_H

#include "spct_global.h"
#include "spct_system.h"

#define TOUCH_LOG_TAG "touch"

#define SPCT_TOUCH_PAD TOUCH_PAD_NUM9

typedef enum {
    SPCT_SINGLE_TAP = 0,    // tap
    SPCT_DOUBLE_TAP,        // tap tap
    SPCT_LONG_TAP,          // taaaap
    SPCT_HOLD_TAP,          // taaaaaaaap
    SPCT_POWEROFF_TAP       // taaaaaaaaaathisisprobablylongenoughaaaaaaap
} spct_touch_type_t;

/*
 *  extern spct_component_handle_t touch;
 */
SPCT_DECLARE_COMPONENT(touch);

#endif /* SPCT_TOUCH_H */
