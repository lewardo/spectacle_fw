#include "esp_system.h"
#include "esp_sleep.h"
#include "esp_attr.h"

#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"

#include "driver/touch_pad.h"

#include "spct_global.h"
#include "spct_system.h"

#include "spct_touch.h"

static spct_ret_t touch_init();
static spct_ret_t touch_deinit();

// static spct_ret_t touch_event(spct_component_evt_t);

static void touch_dispatcher(void*);
static void touch_isr(void*);

static spct_accumtr_t touch_accumtr;
static spct_field_t touch_field;

static time_t isr_time = 0;

enum {
    TOUCH_ACTIVE_BIT = 0,
    TOUCH_HANDLING_BIT
};

SPCT_DEFINE_COMPONENT(touch, touch_init, touch_deinit, "touch");


static spct_ret_t touch_init() {
    uint16_t calib = 0;

    SPCT_LOGI(TOUCH_LOG_TAG, "tp init");

    touch_pad_init();
    touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER);
    touch_pad_set_voltage(TOUCH_HVOLT_2V7, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_1V);


    SPCT_LOGI(TOUCH_LOG_TAG, "tp config");

    touch_pad_config(SPCT_TOUCH_PAD, 0);
    touch_pad_filter_start(10);

    touch_pad_read_filtered(SPCT_TOUCH_PAD, &calib);
    touch_pad_set_thresh(SPCT_TOUCH_PAD, calib * 0.65f);

    SPCT_LOGI(TOUCH_LOG_TAG, "tp isr reg");

    touch_pad_isr_register(touch_isr, NULL);
    touch_pad_intr_enable();

    SPCT_LOGI(TOUCH_LOG_TAG, "tp sleep enable");

    touch_pad_set_trigger_mode(TOUCH_TRIGGER_BELOW);

    esp_sleep_enable_touchpad_wakeup();

    return SPCT_OK;
}

static spct_ret_t touch_deinit() {
    SPCT_LOGI(TOUCH_LOG_TAG, "tp disable");

    esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TOUCHPAD);

    touch_pad_intr_disable();
    touch_pad_isr_deregister(touch_isr, NULL);

    SPCT_LOGI(TOUCH_LOG_TAG, "tp deinit");

    return SPCT_OK;
}

// static spct_ret_t touch_event(spct_component_evt_t evt) {
//     SPCT_LOGI(TOUCH_LOG_TAG, "EVT %d", evt);

//     // if(evt == SPCT_EVENT(touch, SPCT_LONG_TAP)) {
//     //     spct_system_start_sleep();
//     // }

//     // if(evt == SPCT_EVENT(touch, SPCT_HOLD_TAP)) {
//     //     spct_system_shut_down();
//     // }

//     return SPCT_OK;
// }

static void touch_dispatcher(void* pv_arg) {
    time_t time =  (time_t) pv_arg;

    SPCT_FIELD_SET_BIT(touch_field, TOUCH_HANDLING_BIT);
    SPCT_ACCUMTR_DEC(touch_accumtr);

    vTaskDelay(pdMS_TO_TICKS(150));

    ifnt(SPCT_ACCUMTR_ZERO(touch_accumtr)) {
        spct_system_broadcast_evt(touch, SPCT_DOUBLE_TAP);
    } else {
        if(time > 5000) spct_system_shut_down();
        else if (time > 1500) spct_system_broadcast_evt(touch, SPCT_HOLD_TAP);
        else if (time > 500) spct_system_broadcast_evt(touch, SPCT_LONG_TAP);
        else spct_system_broadcast_evt(touch, SPCT_SINGLE_TAP);
    }

    whilnt(SPCT_ACCUMTR_ZERO(touch_accumtr)) SPCT_ACCUMTR_DEC(touch_accumtr);

    SPCT_FIELD_CLEAR_BIT(touch_field, TOUCH_HANDLING_BIT);

    vTaskDelete(NULL);
};

static void IRAM_ATTR touch_isr(void* pv_arg) {
    uint32_t status = touch_pad_get_status();

    touch_pad_clear_status();

    if((status >> SPCT_TOUCH_PAD) & true) {
        SPCT_FIELD_TOGGLE_BIT(touch_field, TOUCH_ACTIVE_BIT);

        if(SPCT_FIELD_GET_BIT(touch_field, TOUCH_ACTIVE_BIT)) {
            touch_pad_set_trigger_mode(TOUCH_TRIGGER_ABOVE);

            if(SPCT_FIELD_GET_BIT(touch_field, TOUCH_HANDLING_BIT)) {
                SPCT_ACCUMTR_INC(touch_accumtr);
            }

            isr_time = esp_timer_get_time();

        } else {
            touch_pad_set_trigger_mode(TOUCH_TRIGGER_BELOW);
            
            isr_time = (esp_timer_get_time() - isr_time) >> 10;

            ifnt(SPCT_FIELD_GET_BIT(touch_field, TOUCH_HANDLING_BIT)) {
                xTaskCreate(touch_dispatcher, "dispatcher", 2048, (void*) isr_time, tskIDLE_PRIORITY, NULL);
                SPCT_ACCUMTR_INC(touch_accumtr);
            }
        }
    }
};
