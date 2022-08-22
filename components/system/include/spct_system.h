#ifndef SPCT_SUBMODULE_H
#define SPCT_SUBMODULE_H

#include <stdint.h>


#define SUBMODULE_LOG_TAG "submodule"

#define SPCT_SUBMODULE_MAX_NUM  8


#define SPCT_SUBMODULE_INIT_STACK_DEPTH 2048
#define SPCT_SUBMODULE_DEINIT_STACK_DEPTH 1024
#define SPCT_SUBMODULE_HANDLER_STACK_DEPTH 4096


#define SPCT_DECLARE_SUBMODULE(name) \
    extern spct_submodule_ptr_t name

#define SPCT_DEFINE_SUBMODULE(name, i, d, c, s) \
    static spct_submodule_t __spct_##name##_submodule = { .init = i, .deinit = d, .cb = c, .id = s }; \
    spct_submodule_ptr_t name = &__spct_##name##_submodule


/*
 *  submodule event handler callback
 */
typedef bool spct_submodule_inited_t;

/*
 *  submodule event type
 */
typedef int32_t spct_submodule_evt_t;

/*
 *  submodule event handler callback
 */
typedef spct_ret_t (*spct_submodule_cb_t)(spct_submodule_evt_t);

/*
 *  submodule initialiser/deinitialisation
 */
typedef spct_ret_t (*spct_submodule_init_deinit_t)();


/*
 *  submodule identifier
 */
typedef const char* spct_submodule_id_t;

/*
 *  submodule structure
 */
typedef struct {
    /*
     *  pointer to submodule initialisation task
     */
    spct_submodule_init_deinit_t init;

    /*
     *  pointer to submodule deinitialisation task
     */
    spct_submodule_init_deinit_t deinit;

    /*
     *  pointer to submodule callback handler function
     */
    spct_submodule_cb_t cb;

    /*
     *  string identifier, used as esp_event_base_t and in logging
     */
    spct_submodule_id_t id;

    /*
     *  boolean to signal initialisation of submodule
     */
    spct_submodule_inited_t inited;

    /*
     *  memory allocated for event argument so it never goes out of scope
     */
    spct_submodule_evt_t evt;

} spct_submodule_t;

/*
 *  pointer to submodule as reference, mechanism to access through argument
 */
typedef spct_submodule_t* spct_submodule_ptr_t;

/*
 *  initialise the submodule event mechanism
 */
spct_ret_t spct_submodule_init();

/*
 *  deinitialise the submodule event mechanism
 */
spct_ret_t spct_submodule_deinit();

/* 
 *  register new submodule to the event mechanism
 */
spct_ret_t spct_submodule_register(spct_submodule_ptr_t);

/*
 *  initialise submodules, only callable once
 */
spct_ret_t spct_submodule_init_registered();

/*
 *  initialise submodules, only callable once
 */
spct_ret_t spct_submodule_deinit_registered();

/*
 *  dispatch event to submodule
 */
spct_ret_t spct_submodule_dispatch(spct_submodule_ptr_t, int32_t);

/*
 *  interface for apps to query dependency initialisation status, returns SPCT_OK if initialised
 */
spct_ret_t spct_submodule_is_inited(spct_submodule_ptr_t);

#endif  /* SPCT_SUBMODULE_H */
