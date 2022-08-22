#include "spct_global.h"
#include "spct_system.h"

#include "spct_touch.h"

void app_main() {
    spct_submodule_init();

    spct_submodule_register(touch);
    spct_submodule_init_registered();
}
