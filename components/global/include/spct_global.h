#ifndef SPCT_BASE_H
#define SPCT_BASE_H

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "esp_err.h"
#include "esp_log.h"


/*
 *  void pointer dereferencing macro for clarification (alleviating hyperparenthesis)
 */
#define pv_deref(pv, t) ((t) *((t*) (pv)))

/*
 *  macro for negated if to help clarification (alleviating hyperparenthesis)
 */
#define ifnt(condition) if(!(condition))

/*
 *  macro for negated while to help clarification (alleviating hyperparenthesis)
 */
#define whilnt(condition) while(!(condition))

/*
 *  macro to assert correct return of spct_(.+) functions that fails with message
 */
#define SPCT_ERROR_CHECK(expr, msg) assert(((void) (msg), ((expr) == SPCT_OK)))

/*
 *  take return value of esp idf function and translate it into spct_ret_t
 */
#define SPCT_FORWARD_ESP_RETURN(condition) (((condition) != ESP_OK) ? (SPCT_ERR) : (SPCT_OK))

/*
 *  info logging functions that forward to ESP_LOGI with additional context information
 */
#define SPCT_LOGI(tag, fmt, ...) ESP_LOGI("\t"tag, "\t(%s) " fmt, __func__, ##__VA_ARGS__);

/*
 *  warning logging functions that forward to ESP_LOGW with additional context information
 */
#define SPCT_LOGW(tag, fmt, ...) ESP_LOGW("\t"tag, "\t(%s) " fmt, __func__, ##__VA_ARGS__);

/*
 *  error logging functions that forward to ESP_LOGE with additional context information
 */
#define SPCT_LOGE(tag, fmt, ...) ESP_LOGE("\t"tag, "\t(%s) " fmt, __func__, ##__VA_ARGS__);

/*
 *  simple macro to get bit from long
 */
#define SPCT_FIELD_GET_BIT(field, bit) (((field) >> (bit)) & true)

/*
 *  simple macro to set bit in long
 */
#define SPCT_FIELD_SET_BIT(field, bit) ((void) ((field) |= (1 << (bit))))

/*
 *  simple macro to clear bit in long
 */
#define SPCT_FIELD_CLEAR_BIT(field, bit) ((void) ((field) &= ~(1 << (bit))))

/*
 *  simple macro to toggle bit in long
 */
#define SPCT_FIELD_TOGGLE_BIT(field, bit) ((void) ((field) ^= (1 << (bit))))


/*
 *  give counting semaphore
 */
#define SPCT_ACCUMTR_INC(accumulator) ((void) (++accumulator))

/*
 *  get count of semaphore
 */
#define SPCT_ACCUMTR_GET(accumulator) (accumulator)

/*
 *  take semaphore and return true if available, else return false
 */
#define SPCT_ACCUMTR_DEC(accumulator) (((accumulator) > 0) ? --(accumulator) : (void) false)

/*
 *  check if semaphore is zero
 */
#define SPCT_ACCUMTR_ZERO(accumulator) (((accumulator) > 0) ? false : true)


/*
 *  return type for spct_(.+) functions
 */
typedef enum {
    SPCT_OK = 0,
    SPCT_ERR
} spct_ret_t;

/*
 *  fast deterministic ISR safe, multithreading unsafe event group without needing pxHigherPriorityTaskWoken functionality
 */
typedef long spct_field_t;

/*
 *  fast detrminitstic ISR safe, multithreading unsafe semaphore without needing pxHigherPriorityTaskWoken functionality
 */
typedef long spct_accumtr_t;




#endif