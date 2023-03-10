#ifndef SPCT_SYS_COMPONENT_H
#define SPCT_SYS_COMPONENT_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "spct_global.h"
#include "spct_system.h"
#include "spct_component.h"

extern esp_event_loop_handle_t event_loop_handle;

extern spct_component_handle_t components[SPCT_COMPONENT_MAX_NUM];
extern uint32_t component_num;

extern spct_accumtr_t tasks_running;
extern spct_field_t system_field;

enum {
    SYSTEM_INITED_BIT = 0
};

void event_handler_dispatcher(void*, const char *, int32_t, void*);
void init_rtos_wrapper(void*);
// void handler_rtos_wrapper(void*);

#endif // SPCT_SYS_COMPONENT_H
