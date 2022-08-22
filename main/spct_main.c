#include "spct_global.h"
#include "spct_system.h"

#include "spct_touch.h"

void app_main() {
    spct_system_init();

    spct_component_register(touch);
    spct_init_components();
}
