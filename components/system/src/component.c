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

spct_component_handle_t components[SPCT_COMPONENT_MAX_NUM];
uint32_t component_num = 0;

/* 
 *  register new component to the event mechanism
 */
spct_ret_t spct_component_register(spct_component_handle_t pt_component) {
    spct_component_handle_t component = NULL;
    uint32_t current_index = 0;

    if(event_loop_handle == NULL) {
        SPCT_LOGE(SYSTEM_LOG_TAG, "event handler mechanism uninitialised");
        return SPCT_ERR;
    }

    if(pt_component == NULL) {
        SPCT_LOGE(SYSTEM_LOG_TAG, "component undefined");
        return SPCT_ERR;
    }

    if(component_num >= SPCT_COMPONENT_MAX_NUM) {
        SPCT_LOGE(SYSTEM_LOG_TAG, "max numer of components registered, change config");
        return SPCT_ERR;
    }

    while(current_index < component_num) {
        component = components[current_index++];

        ifnt(strcmp(pt_component->name, component->name) == 0) {
            SPCT_LOGE(SYSTEM_LOG_TAG, "cannot register component %s twice", pt_component->name);
            return SPCT_ERR;
        }
    }

    pt_component->inited = false;
    pt_component->id = component_num;
    components[component_num++] = pt_component;

    SPCT_LOGI(SYSTEM_LOG_TAG, "registering component %s", pt_component->name);

    return SPCT_FORWARD_ESP_RETURN( esp_event_handler_instance_register_with(event_loop_handle, pt_component->name, ESP_EVENT_ANY_ID, event_handler_dispatcher, (void*) pt_component, NULL) );
};

/*
 *  subscribe component to recieve other's broadcasts
 */
spct_ret_t spct_component_subscribe_to(spct_component_handle_t pt_subscriber, spct_component_handle_t pt_broadcaster) {
    SPCT_LOGI(SYSTEM_LOG_TAG, "subscribing component %s to %s", pt_subscriber->name, pt_subscriber->name);

    SPCT_FIELD_SET_BIT(pt_subscriber->subscriptions, pt_broadcaster->id);

    return SPCT_OK;
};

/*
 *  initialise components, only callable once
 */
spct_ret_t spct_init_components() {
    spct_component_handle_t component = NULL;
    uint32_t current_index = 0;

    if(event_loop_handle == NULL) {
        SPCT_LOGE(SYSTEM_LOG_TAG, "event handler mechanism uninitialised");
        return SPCT_ERR;
    }

    while(current_index < component_num) {
        component = components[current_index++];

        SPCT_LOGI(SYSTEM_LOG_TAG, "dispatching init task for component %s", component->name);
        ifnt(component->inited) assert(pdPASS == xTaskCreate(init_rtos_wrapper, component->name, SPCT_COMPONENT_INIT_STACK_DEPTH, (void*) component->init, tskIDLE_PRIORITY, NULL));

        component->inited = true;
    }

    return SPCT_OK;
};

/*
 *  initialise components, only callable once
 */
spct_ret_t spct_deinit_components() {
    spct_component_handle_t component = NULL;
    size_t current_index = 0;

    while(current_index < component_num) {
        component = components[current_index++];

        SPCT_LOGI(SYSTEM_LOG_TAG, "deiniting task for component %s", component->name);
        if(component->inited) component->deinit(); // don't dispatch asynchronously so finalisation is easier

        component->inited = false;
    }

    return SPCT_OK;
};