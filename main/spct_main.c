#include "spct_global.h"
#include "spct_system.h"
#include "spct_component.h"

#include "spct_touch.h"
#include "spct_bt.h"

void app_main() {
    spct_system_init();

    spct_component_register(bluetooth);
    spct_component_register(touch);

    spct_init_components();
}
