#ifndef SPCT_SYSTEM_H
#define SPCT_SYSTEM_H

#include <stdint.h>


#define COMPONENT_LOG_TAG "component"

#define SPCT_COMPONENT_MAX_NUM  8


#define SPCT_COMPONENT_INIT_STACK_DEPTH 2048
#define SPCT_COMPONENT_DEINIT_STACK_DEPTH 1024
#define SPCT_COMPONENT_HANDLER_STACK_DEPTH 4096


#define SPCT_DECLARE_COMPONENT(name) \
    extern spct_component_handle_t name

#define SPCT_DEFINE_COMPONENT(name, i, d, c, s) \
    static spct_component_t __spct_##name##_component = { .init = i, .deinit = d, .cb = c, .id = s }; \
    spct_component_handle_t name = &__spct_##name##_component


/*
 *  component event handler callback
 */
typedef bool spct_system_inited_t;

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
typedef spct_ret_t (*spct_system_init_deinit_t)();


/*
 *  component identifier
 */
typedef const char* spct_component_id_t;

/*
 *  component structure
 */
typedef struct {
    /*
     *  pointer to component initialisation task
     */
    spct_system_init_deinit_t init;

    /*
     *  pointer to component deinitialisation task
     */
    spct_system_init_deinit_t deinit;

    /*
     *  pointer to component callback handler function
     */
    spct_component_cb_t cb;

    /*
     *  string identifier, used as esp_event_base_t and in logging
     */
    spct_component_id_t id;

    /*
     *  boolean to signal initialisation of component
     */
    spct_system_inited_t inited;

    /*
     *  memory allocated for event argument so it never goes out of scope
     */
    spct_component_evt_t evt;

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
spct_ret_t spct_system_evt_dispatch(spct_component_handle_t, int32_t);


#endif  /* SPCT_SYSTEM_H */
