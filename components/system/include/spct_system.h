#ifndef SPCT_SYSTEM_H
#define SPCT_SYSTEM_H

#include <stdint.h>

#include "spct_global.h"
#include "spct_component.h"


#define SYSTEM_LOG_TAG "spct_system"

#define SPCT_EVENT(component, evt) ((component->id << 8) | evt)

/*
 *  initialise the component event mechanism
 */
spct_ret_t spct_system_init();

/*
 *  deinitialise the component event mechanism
 */
spct_ret_t spct_system_deinit();


// /*
//  *  dispatch event to component
//  */
// spct_ret_t spct_system_dispatch_evt(spct_component_handle_t, spct_component_evt_t);

/*
 *  broadcast event to subscribers
 */
spct_ret_t spct_system_broadcast_evt(spct_component_handle_t, spct_component_evt_t);

// /*
//  *  subscribe component to recieve other's broadcasts
//  */
// spct_ret_t spct_component_subscribe_to(spct_component_handle_t, spct_component_handle_t);

/*
 *  start light sleep, default awoken by touch system
 */
spct_ret_t spct_system_start_sleep();

/*
 *  start deep sleep, disable all modules, require reset to restart (power off-ish senanigans)
 */
spct_ret_t spct_system_shut_down();

#endif  /* SPCT_SYSTEM_H */
