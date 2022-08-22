#include <stdlib.h>

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


static esp_event_loop_handle_t event_loop_handle = NULL;

static spct_component_handle_t components[SPCT_COMPONENT_MAX_NUM];
static uint32_t component_num = 0;

static spct_field_t sys_field;
static spct_accumtr_t tasks_running;

static enum {
    SYS_SLEEP_BIT = 0,
    SYS_SHUTDOWN_BIT
};

static void event_handler_dispatcher(void*, const char *, int32_t, void*);
static void init_rtos_wrapper(void*);
static void handler_rtos_wrapper(void*);

/*
 *  initialise the component event mechanism
 */
spct_ret_t spct_system_init() {
    esp_event_loop_args_t event_loop_args = {
        .queue_size = 12,
        .task_name = "evtloop",
        .task_priority = tskIDLE_PRIORITY + 1,
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
        SPCT_LOGE(COMPONENT_LOG_TAG, "event handler mechanism uninitialised");
        return SPCT_ERR;
    }

    return SPCT_FORWARD_ESP_RETURN( esp_event_loop_delete(event_loop_handle) );
};

/* 
 *  register new component to the event mechanism
 */
spct_ret_t spct_component_register(spct_component_handle_t pt_component) {
    if(event_loop_handle == NULL) {
        SPCT_LOGE(COMPONENT_LOG_TAG, "event handler mechanism uninitialised");
        return SPCT_ERR;
    }

    if(pt_component == NULL) {
        SPCT_LOGE(COMPONENT_LOG_TAG, "component undefined");
        return SPCT_ERR;
    }

    if(component_num >= SPCT_COMPONENT_MAX_NUM) {
        SPCT_LOGE(COMPONENT_LOG_TAG, "max numer of components registered, change config");
        return SPCT_ERR;
    }

    pt_component->inited = false;
    pt_component->id = component_num;
    components[component_num++] = pt_component;

    SPCT_LOGI(COMPONENT_LOG_TAG, "registering component %s", pt_component->name);

    return SPCT_FORWARD_ESP_RETURN( esp_event_handler_instance_register_with(event_loop_handle, pt_component->name, ESP_EVENT_ANY_ID, event_handler_dispatcher, (void*) pt_component, NULL) );
};

/*
 *  initialise components, only callable once
 */
spct_ret_t spct_init_components() {
    spct_component_handle_t component = NULL;
    uint32_t current_index = 0;

    if(event_loop_handle == NULL) {
        SPCT_LOGE(COMPONENT_LOG_TAG, "event handler mechanism uninitialised");
        return SPCT_ERR;
    }

    while(current_index < component_num) {
        component = components[current_index++];

        SPCT_LOGI(COMPONENT_LOG_TAG, "dispatching init task for component %s", component->name);
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

        SPCT_LOGI(COMPONENT_LOG_TAG, "deiniting task for component %s", component->name);
        if(component->inited) component->deinit(); // don't dispatch asynchronously so finalisation is easier

        component->inited = false;
    }

    return SPCT_OK;
};

/*
 *  dispatch event to component
 */
spct_ret_t spct_system_dispatch_evt(spct_component_handle_t pt_component, spct_component_evt_t i_evt) {
    SPCT_LOGI(COMPONENT_LOG_TAG, "dispatching evt %d to component %s", i_evt, pt_component->name);

    return SPCT_FORWARD_ESP_RETURN( esp_event_post_to(event_loop_handle, pt_component->name, SPCT_EVENT(pt_component, i_evt), NULL, 0, (TickType_t) portMAX_DELAY) );
};


/*
 *  broadcast event to subscribers
 */
spct_ret_t spct_system_broadcast_evt(spct_component_handle_t pt_component, spct_component_evt_t i_evt) {
    spct_component_handle_t component = NULL;
    size_t current_index = 0;

    SPCT_LOGI(COMPONENT_LOG_TAG, "broadcasting evt %d from component %s", i_evt, pt_component->name);

    while(current_index < component_num) {
        component = components[current_index++];

        if(SPCT_FIELD_GET_BIT(component->subscriptions, pt_component->id)) {
            ifnt(SPCT_OK == SPCT_FORWARD_ESP_RETURN(esp_event_post_to(event_loop_handle, component->name, SPCT_EVENT(pt_component, i_evt), NULL, 0, (TickType_t) portMAX_DELAY))) {
                SPCT_LOGE(COMPONENT_LOG_TAG, "failed to broadcast to component %d", current_index);
            }
        } 
    }

    return SPCT_OK;
};

/*
 *  subscribe component to recieve other's broadcasts
 */
spct_ret_t spct_component_subscribe_to(spct_component_handle_t pt_subscriber, spct_component_handle_t pt_broadcaster) {
    SPCT_LOGI(COMPONENT_LOG_TAG, "subscribing component %s to %s", pt_subscriber->name, pt_subscriber->name);

    SPCT_FIELD_SET_BIT(pt_subscriber->subscriptions, pt_broadcaster->id);

    return SPCT_OK;
};

/*
 *  start light sleep, default awoken by touch system
 */
spct_ret_t spct_system_start_sleep() {
    SPCT_FIELD_SET_BIT(sys_field, SYS_SLEEP_BIT);

    SPCT_LOGW(COMPONENT_LOG_TAG, "sleep scheduled, all events will be buffered");
    SPCT_LOGI(COMPONENT_LOG_TAG, "sleep start, gn hun");

    // TODO: automatically flush buffers instead of delay
    vTaskDelay(pdMS_TO_TICKS(500));

    esp_light_sleep_start(); 

    SPCT_LOGI(COMPONENT_LOG_TAG, "awoken, good morning dear");

    SPCT_FIELD_CLEAR_BIT(sys_field, SYS_SLEEP_BIT);

    return SPCT_OK;
};

/*
 *  start deep sleep, disable all modules, require reset to restart (power off-ish senanigans)
 */
spct_ret_t spct_system_shut_down() {
    SPCT_FIELD_SET_BIT(sys_field, SYS_SHUTDOWN_BIT);

    SPCT_LOGW(COMPONENT_LOG_TAG, "SHUTTING DOWN, ALL EVENTS FROM NOW WILL NOT BE HANDLED");
    
    spct_deinit_components();

    SPCT_LOGI(COMPONENT_LOG_TAG, "shudown, bye!");

    // TODO: automatically flush buffers instead of delay
    vTaskDelay(500);

    esp_deep_sleep_start(); // never gets past here

    return SPCT_OK;
};


/*
 *  catch all dispatcher for events and broadcasts
 */
static void event_handler_dispatcher(void* pv_arg, const char * pc_base, int32_t i_evt, void* pv_data) {
    spct_component_handle_t component = (spct_component_handle_t) pv_arg;

    if(component != NULL) {
        component->evt = i_evt;

        SPCT_LOGI(COMPONENT_LOG_TAG, "calling cb for component %s with event %d", component->name, component->evt);
        assert(pdPASS == xTaskCreate(handler_rtos_wrapper, component->name, SPCT_COMPONENT_HANDLER_STACK_DEPTH, (void*) component, tskIDLE_PRIORITY + 2, NULL));
    } else {
        SPCT_LOGE(COMPONENT_LOG_TAG, "component undefined");
    }
};

/*
 *  FreeRTOS task wrapper for spct_ret_t (*)(void) initialisation/deinitialisation functions
 */
static void init_rtos_wrapper(void* pv_arg) {
    spct_component_init_deinit_t task = (spct_component_init_deinit_t) pv_arg;

    SPCT_ACCUMTR_INC(tasks_running);
    SPCT_ERROR_CHECK(task(), "failed to run init/deinit event");
    SPCT_ACCUMTR_DEC(tasks_running);

    vTaskDelete(NULL);
};

/*
 *  FreeRTOS task wrapper for spct_ret_t (*)(spct_component_evt_t) initialisation/deinitialisation functions
 */
static void handler_rtos_wrapper(void* pv_arg) {
    spct_component_handle_t component = (spct_component_handle_t) pv_arg;

    SPCT_ACCUMTR_INC(tasks_running);
    SPCT_ERROR_CHECK(component->cb(component->evt), "failed to run component callback");
    SPCT_ACCUMTR_DEC(tasks_running);

    vTaskDelete(NULL);
};