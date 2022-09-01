#include "spct_global.h"
#include "spct_system.h"

#include "spct_touch.h"

void app_main() {
    spct_system_init();

    spct_component_register(touch);

    spct_component_subscribe_to(touch, touch);

    spct_init_components();
ets_delay_us(100000);
    spct_deinit_components();

    spct_system_deinit();

    spct_system_init();
    spct_component_register(touch);

    spct_init_components();

}
