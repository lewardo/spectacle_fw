#ifndef SPCT_APP_H
#define SPCT_APP_H

#include "spct_global.h"
#include "spct_component.h"

#define APP_LOG_TAG "spct_app"

#define SPCT_APP_MAX_NUM  8

// #define SPCT_COMPONENT_INIT_STACK_DEPTH 2048
// #define SPCT_COMPONENT_DEINIT_STACK_DEPTH 1024
// #define SPCT_COMPONENT_HANDLER_STACK_DEPTH 4096

#define SPCT_DECLARE_APP(object) \
    extern spct_app_handle_t object

#define SPCT_DEFINE_APP(object, initialise, deinitialise, callback, string, ...) \
    static spct_component_t __spct_##object##_app = { .init = initialise, .deinit = deinitialise, .cb = callback, .deps = { __VA_ARGS__ }, .name = string }; \
    spct_component_handle_t object = &__spct_##object##_app

/*
 *  app event handler callback
 */
typedef spct_ret_t (*spct_app_cb_t)(spct_component_evt_t);

/*
 *  app initialiser/deinitialisation
 */
typedef spct_ret_t (*spct_app_init_deinit_t)();

/*
 *  app identifier
 */
typedef const char* spct_app_name_t;

/*
 *  component dependencies
 */
typedef uint32_t spct_app_deps_t;

/*
 *  component structure
 */
typedef struct {
    /*
     *  pointer to component initialisation task
     */
    spct_app_init_deinit_t init;

    /*
     *  pointer to component deinitialisation task
     */
    spct_app_init_deinit_t deinit;

    /*
    *  app event handler callback
    */
    spct_app_cb_t cb;

    /*
     *  string identifier
     */
    spct_app_name_t name;

    /*
     *  component dependencies
     */
    spct_app_deps_t deps;

    /*
     *  bitfield for component broadcast subscriptions
     */
    spct_field_t subscriptions;
} spct_app_t;

/*
 *  pointer to component as reference, mechanism to access through argument
 */
typedef spct_app_t* spct_app_handle_t;


extern spct_app_handle_t apps[SPCT_APP_MAX_NUM];
extern spct_app_handle_t current_app;

spct_ret_t spct_app_install(spct_app_handle_t);

spct_ret_t spct_app_run(spct_app_handle_t);
spct_ret_t spct_app_quit(); // go back to app index 0

#endif