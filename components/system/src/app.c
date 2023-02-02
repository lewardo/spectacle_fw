#include <stdlib.h>
#include <string.h>

#include "esp_system.h"
#include "esp_event.h"
#include "esp_sleep.h"
#include "esp_assert.h"
#include "esp_err.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "spct_global.h"
#include "spct_system.h"
#include "spct_component.h"
#include "spct_app.h"


spct_app_handle_t apps[SPCT_APP_MAX_NUM];
spct_app_handle_t current_app = NULL;
uint32_t app_num = 0;

spct_ret_t spct_app_install_launcher(spct_app_handle_t launcher) {

    return SPCT_OK;
};

spct_ret_t spct_app_install(spct_app_handle_t app) {

    return SPCT_OK;
};

spct_ret_t spct_app_launch(spct_app_handle_t app) {

    return SPCT_OK;
};

spct_ret_t spct_app_quit() {

    return SPCT_OK;
};
