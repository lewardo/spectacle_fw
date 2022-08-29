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


enum {
    SYS_SLEEP_BIT = 0,
    SYS_SHUTDOWN_BIT
};

/*
 *  start light sleep, default awoken by touch system
 */
spct_ret_t spct_system_start_sleep() {
    SPCT_LOGW(SYSTEM_LOG_TAG, "sleep scheduled, all events will be buffered");
    SPCT_LOGI(SYSTEM_LOG_TAG, "sleep start, gn hun");

    // TODO: automatically flush buffers instead of delay
    vTaskDelay(pdMS_TO_TICKS(500));

    esp_light_sleep_start(); 

    SPCT_LOGI(SYSTEM_LOG_TAG, "awoken, good morning dear");

    return SPCT_OK;
};

/*
 *  start deep sleep, disable all modules, require reset to restart (power off-ish senanigans)
 */
spct_ret_t spct_system_shut_down() {
    SPCT_LOGW(SYSTEM_LOG_TAG, "SHUTTING DOWN, ALL EVENTS FROM NOW WILL NOT BE HANDLED");
    
    spct_deinit_components();

    SPCT_LOGI(SYSTEM_LOG_TAG, "shudown, bye!");

    // TODO: automatically flush buffers instead of delay
    vTaskDelay(500);

    esp_deep_sleep_start(); // never gets past here

    return SPCT_OK;
};