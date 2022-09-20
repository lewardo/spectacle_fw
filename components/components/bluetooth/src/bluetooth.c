#include "spct_global.h"
#include "spct_system.h"

#include "spct_bt.h"

static spct_ret_t bluetooth_init();
static spct_ret_t bluetooth_deinit();

// static spct_ret_t bluetooth_event(spct_component_evt_t);

SPCT_DEFINE_COMPONENT(bluetooth, bluetooth_init, bluetooth_deinit, "bluetooth");


static spct_ret_t bluetooth_init() {
    SPCT_LOGI(BLUETOOTH_LOG_TAG, "INIT");

    return SPCT_OK;

};
static spct_ret_t bluetooth_deinit() {
    SPCT_LOGI(BLUETOOTH_LOG_TAG, "DEINIT");

    return SPCT_OK;
};

// static spct_ret_t bluetooth_event(spct_component_evt_t evt) {
//     SPCT_LOGI(BLUETOOTH_LOG_TAG, "EVT %d", evt);

//     return SPCT_OK;
// }