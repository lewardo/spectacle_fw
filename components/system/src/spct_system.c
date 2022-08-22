#include <stdlib.h>

#include "esp_system.h"
#include "esp_assert.h"
#include "esp_err.h"
#include "esp_log.h"

#include "esp_event.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "spct_global.h"
#include "spct_system.h"


static esp_event_loop_handle_t event_loop_handle = NULL;

static spct_submodule_ptr_t submodules[SPCT_SUBMODULE_MAX_NUM];
static uint32_t submodule_num = 0;

static void event_handler_dispatcher(void*, const char *, int32_t, void*);
static void init_deinit_rtos_wrapper(void*);
static void handler_rtos_wrapper(void*);

/*
 *  initialise the submodule event mechanism
 */
spct_ret_t spct_submodule_init() {
    esp_event_loop_args_t event_loop_args = {
        .queue_size = 12,
        .task_name = "g_evtloop",
        .task_priority = tskIDLE_PRIORITY + 1,
        .task_stack_size = 2048,
        .task_core_id = 0
    };

    return SPCT_FORWARD_ESP_RETURN( esp_event_loop_create(&event_loop_args, &event_loop_handle) );
};

/*
 *  deinitialise the submodule event mechanism
 */
spct_ret_t spct_submodule_deinit() {
    if(event_loop_handle == NULL) {
        SPCT_LOGE(SUBMODULE_LOG_TAG, "event handler mechanism uninitialised");
        return SPCT_ERR;
    }

    return SPCT_FORWARD_ESP_RETURN( esp_event_loop_delete(event_loop_handle) );
};

/* 
 *  register new submodule to the event mechanism
 */
spct_ret_t spct_submodule_register(spct_submodule_ptr_t pt_component) {
    if(event_loop_handle == NULL) {
        SPCT_LOGE(SUBMODULE_LOG_TAG, "event handler mechanism uninitialised");
        return SPCT_ERR;
    }

    if(pt_component == NULL) {
        SPCT_LOGE(SUBMODULE_LOG_TAG, "component undefined");
        return SPCT_ERR;
    }

    if(submodule_num >= SPCT_SUBMODULE_MAX_NUM) {
        SPCT_LOGE(SUBMODULE_LOG_TAG, "max numer of submodules exceeded");
        return SPCT_ERR;
    }

    pt_component->inited = false;
    submodules[submodule_num++] = pt_component;

    SPCT_LOGI(SUBMODULE_LOG_TAG, "registering submodule %s", pt_component->id);

    return SPCT_FORWARD_ESP_RETURN( esp_event_handler_instance_register_with(event_loop_handle, pt_component->id, ESP_EVENT_ANY_ID, event_handler_dispatcher, (void*) pt_component, NULL) );
};

/*
 *  initialise submodules, only callable once
 */
spct_ret_t spct_submodule_init_registered(void) {
    spct_submodule_ptr_t component = NULL;
    uint32_t current_index = 0;

    if(event_loop_handle == NULL) {
        SPCT_LOGE(SUBMODULE_LOG_TAG, "event handler mechanism uninitialised");
        return SPCT_ERR;
    }

    while(current_index < submodule_num) {
        component = submodules[current_index++];

        SPCT_LOGI(SUBMODULE_LOG_TAG, "dispatching init task for submodule %s", component->id);
        ifnt(component->inited) assert(pdPASS == xTaskCreate(init_deinit_rtos_wrapper, component->id, SPCT_SUBMODULE_INIT_STACK_DEPTH, (void*) component->init, tskIDLE_PRIORITY, NULL));

        component->inited = true;
    }

    return SPCT_OK;
};

/*
 *  initialise submodules, only callable once
 */
spct_ret_t spct_submodule_deinit_registered(void) {
    spct_submodule_ptr_t component = NULL;
    uint32_t current_index = 0;

    while(current_index < submodule_num) {
        component = submodules[current_index++];

        SPCT_LOGI(SUBMODULE_LOG_TAG, "dispatching deinit task for submodule %s", component->id);
        if(component->inited) assert(pdPASS == xTaskCreate(init_deinit_rtos_wrapper, component->id, SPCT_SUBMODULE_DEINIT_STACK_DEPTH, (void*) component->deinit, tskIDLE_PRIORITY, NULL));

        component->inited = false;
    }

    return SPCT_OK;
};

/*
 *  dispatch event to submodule
 */
spct_ret_t spct_submodule_dispatch(spct_submodule_ptr_t pt_component, spct_submodule_evt_t i_evt) {
    SPCT_LOGI(SUBMODULE_LOG_TAG, "dispatching cb for submodule %s", pt_component->id);

    return SPCT_FORWARD_ESP_RETURN( esp_event_post_to(event_loop_handle, pt_component->id, i_evt, NULL, 0, (TickType_t) portMAX_DELAY) );
};

/*
 *  interface for apps to query dependency initialisation status, returns SPCT_OK if initialised
 */
spct_ret_t spct_submodule_is_inited(spct_submodule_ptr_t pt_component) {
    return pt_component->inited ? SPCT_OK : SPCT_ERR;
};

/*
 *
 */
static void event_handler_dispatcher(void* pv_arg, const char * pc_base, int32_t i_evt, void* pv_data) {
    spct_submodule_ptr_t component = (spct_submodule_ptr_t) pv_arg;

    if(component != NULL) {
        component->evt = i_evt;

        SPCT_LOGI(SUBMODULE_LOG_TAG, "calling cb for submodule %s with event %d", component->id, component->evt);
        assert(pdPASS == xTaskCreate(handler_rtos_wrapper, component->id, SPCT_SUBMODULE_HANDLER_STACK_DEPTH, (void*) component, tskIDLE_PRIORITY, NULL));
    } else {
        SPCT_LOGE(SUBMODULE_LOG_TAG, "submodule undefined");
    }
};

/*
 *  FreeRTOS task wrapper for spct_ret_t (*)(void) initialisation/deinitialisation functions
 */
static void init_deinit_rtos_wrapper(void* pv_arg) {
    spct_submodule_init_deinit_t task = (spct_submodule_init_deinit_t) pv_arg;
    
    SPCT_ERROR_CHECK(task(), "failed to run init/deinit event");

    vTaskDelete(NULL);
};

/*
 *  FreeRTOS task wrapper for spct_ret_t (*)(spct_submodule_evt_t) initialisation/deinitialisation functions
 */
static void handler_rtos_wrapper(void* pv_arg) {
    spct_submodule_ptr_t component = (spct_submodule_ptr_t) pv_arg;

    SPCT_ERROR_CHECK(component->cb(component->evt), "failed to run submodule callback");

    vTaskDelete(NULL);
};