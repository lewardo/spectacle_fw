#ifndef SPCT_SYSTEM_H
#define SPCT_SYSTEM_H

#include <stdint.h>

#include "spct_global.h"


#define COMPONENT_LOG_TAG "component"

#define SPCT_COMPONENT_MAX_NUM  8

#define SPCT_COMPONENT_INIT_STACK_DEPTH 2048
#define SPCT_COMPONENT_DEINIT_STACK_DEPTH 1024
#define SPCT_COMPONENT_HANDLER_STACK_DEPTH 4096

#define SPCT_EVENT(component, evt) ((component->id << 8) | evt)

#define SPCT_DECLARE_COMPONENT(object) \
    extern spct_component_handle_t object

#define SPCT_DEFINE_COMPONENT(object, initialise, deinitialise, handler, string) \
    static spct_component_t __spct_##object##_component = { .init = initialise, .deinit = deinitialise, .cb = handler, .name = string }; \
    spct_component_handle_t object = &__spct_##object##_component


/*
 *  component event handler callback
 */
typedef bool spct_component_inited_t;

/*
 *  component event type
 */
typedef int32_t spct_component_evt_t;

/*
 *  component event handler callback
 */
typedef spct_ret_t (*spct_component_cb_t)(spct_component_evt_t);

/*
 *  component initialiser/deinitialisation
 */
typedef spct_ret_t (*spct_component_init_deinit_t)();


/*
 *  component identifier
 */
typedef const char* spct_component_name_t;

/*
 *  component index
 */
typedef uint32_t spct_component_id_t;

/*
 *  component structure
 */
typedef struct {
    /*
     *  pointer to component initialisation task
     */
    spct_component_init_deinit_t init;

    /*
     *  pointer to component deinitialisation task
     */
    spct_component_init_deinit_t deinit;

    /*
     *  pointer to component callback handler function
     */
    spct_component_cb_t cb;

    /*
     *  string identifier, used as esp_event_base_t and in logging
     */
    spct_component_name_t name;

    /*
     *  boolean to signal initialisation of component
     */
    spct_component_inited_t inited;

    /*
     *  memory allocated for event argument so it never goes out of scope
     */
    spct_component_evt_t evt;

    /*
     *  id for subscriptions
     */
    spct_component_id_t id;

    /*
     *  bitfield for broadcast subscriptions
     */
    spct_field_t subscriptions;
} spct_component_t;

/*
 *  pointer to component as reference, mechanism to access through argument
 */
typedef spct_component_t* spct_component_handle_t;

/*
 *  initialise the component event mechanism
 */
spct_ret_t spct_system_init();

/*
 *  deinitialise the component event mechanism
 */
spct_ret_t spct_system_deinit();

/* 
 *  register new component to the event mechanism
 */
spct_ret_t spct_component_register(spct_component_handle_t);

/*
 *  initialise components, only callable once
 */
spct_ret_t spct_init_components();

/*
 *  initialise components, only callable once
 */
spct_ret_t spct_deinit_components();

/*
 *  dispatch event to component
 */
spct_ret_t spct_system_dispatch_evt(spct_component_handle_t, spct_component_evt_t);

/*
 *  broadcast event to subscribers
 */
spct_ret_t spct_system_broadcast_evt(spct_component_handle_t, spct_component_evt_t);

/*
 *  subscribe component to recieve other's broadcasts
 */
spct_ret_t spct_component_subscribe_to(spct_component_handle_t, spct_component_handle_t);

/*
 *  start light sleep, default awoken by touch system
 */
spct_ret_t spct_system_start_sleep();

/*
 *  start deep sleep, disable all modules, require reset to restart (power off-ish senanigans)
 */
spct_ret_t spct_system_shut_down();

#endif  /* SPCT_SYSTEM_H */
