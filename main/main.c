#include "base.h"
#include "submodule.h"

#include "touch.h"

void app_main() {
    spct_submodule_init();

    spct_submodule_register(touch);
    spct_submodule_init_registered();

}
