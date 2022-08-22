#ifndef SPCT_TOUCH_H
#define SPCT_TOUCH_H

#include "spct_global.h"
#include "spct_system.h"

#define TOUCH_LOG_TAG "touch"

#define SPCT_TOUCH_PAD TOUCH_PAD_NUM9

typedef enum {
    SPCT_SINGLE_TAP = 0,
    SPCT_DOUBLE_TAP,
    SPCT_LONG_TAP,
    SPCT_HOLD_TAP
} spct_touch_type_t;

SPCT_DECLARE_SUBMODULE(touch);

#endif /* SPCT_TOUCH_H */
