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

#include "system_component.h"

esp_event_loop_handle_t event_loop_handle = NULL;
spct_accumtr_t tasks_running;


spct_ret_t spct_system_init() {
    esp_event_loop_args_t event_loop_args = {
        .queue_size = 12,
        .task_name = "evtloop",
        .task_priority = tskIDLE_PRIORITY + 2,
        .task_stack_size = 2048,
        .task_core_id = 0
    };

    return SPCT_FORWARD_ESP_RETURN( esp_event_loop_create(&event_loop_args, &event_loop_handle) );
};

/*
 *  deinitialise the component event mechanism
 */
spct_ret_t spct_system_deinit() {
    if(event_loop_handle == NULL) {
        SPCT_LOGE(SYSTEM_LOG_TAG, "event handler mechanism uninitialised");
        return SPCT_ERR;
    }

    return SPCT_FORWARD_ESP_RETURN( esp_event_loop_delete(event_loop_handle) );
};

/*
 *  dispatch event to component
 */
spct_ret_t spct_system_dispatch_evt(spct_component_handle_t pt_component, spct_component_evt_t i_evt) {
    SPCT_LOGI(SYSTEM_LOG_TAG, "dispatching evt %d to component %s", i_evt, pt_component->name);

    return SPCT_FORWARD_ESP_RETURN( esp_event_post_to(event_loop_handle, pt_component->name, SPCT_EVENT(pt_component, i_evt), NULL, 0, (TickType_t) portMAX_DELAY) );
};


/*
 *  broadcast event to subscribers
 */
spct_ret_t spct_system_broadcast_evt(spct_component_handle_t pt_component, spct_component_evt_t i_evt) {
    spct_component_handle_t component = NULL;
    size_t current_index = 0;

    SPCT_LOGI(SYSTEM_LOG_TAG, "broadcasting evt %d from component %s", i_evt, pt_component->name);

    while(current_index < component_num) {
        component = components[current_index++];

        if(SPCT_FIELD_GET_BIT(component->subscriptions, pt_component->id)) {
            ifnt(SPCT_OK == SPCT_FORWARD_ESP_RETURN(esp_event_post_to(event_loop_handle, component->name, SPCT_EVENT(pt_component, i_evt), NULL, 0, (TickType_t) portMAX_DELAY))) {
                SPCT_LOGE(SYSTEM_LOG_TAG, "failed to broadcast to component %d", current_index);
            }
        } 
    }

    return SPCT_OK;
};

/*
 *  catch all dispatcher for events and broadcasts
 */
void event_handler_dispatcher(void* pv_arg, const char * pc_base, int32_t i_evt, void* pv_data) {
    spct_component_handle_t component = (spct_component_handle_t) pv_arg;

    if(component != NULL) {
        component->evt = i_evt;

        SPCT_LOGI(SYSTEM_LOG_TAG, "calling cb for component %s with event %d", component->name, component->evt);
        assert(pdPASS == xTaskCreate(handler_rtos_wrapper, component->name, SPCT_COMPONENT_HANDLER_STACK_DEPTH, (void*) component, tskIDLE_PRIORITY + 1, NULL));
    } else {
        SPCT_LOGE(SYSTEM_LOG_TAG, "component undefined");
    }
};

/*
 *  FreeRTOS task wrapper for spct_ret_t (*)(void) initialisation/deinitialisation functions
 */
void init_rtos_wrapper(void* pv_arg) {
    spct_component_init_deinit_t task = (spct_component_init_deinit_t) pv_arg;

    SPCT_ACCUMTR_INC(tasks_running);
    SPCT_ERROR_CHECK(task(), "failed to run init/deinit event");
    SPCT_ACCUMTR_DEC(tasks_running);

    vTaskDelete(NULL);
};

/*
 *  FreeRTOS task wrapper for spct_ret_t (*)(spct_component_evt_t) initialisation/deinitialisation functions
 */
void handler_rtos_wrapper(void* pv_arg) {
    spct_component_handle_t component = (spct_component_handle_t) pv_arg;

    SPCT_ACCUMTR_INC(tasks_running);
    SPCT_ERROR_CHECK(component->cb(component->evt), "failed to run component callback");
    SPCT_ACCUMTR_DEC(tasks_running);

    vTaskDelete(NULL);
};