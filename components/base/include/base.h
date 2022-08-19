#ifndef SPCT_BASE_H
#define SPCT_BASE_H

#include "esp_err.h"
#include "esp_log.h"

#define pv_deref(pv, t) ((t) *((t*) (pv)))

#define ifnt(condition) if(!(condition))
#define whilnt(condition) while(!(condition))

#define SPCT_FORWARD_ESP_RETURN(condition) (((condition) != ESP_OK) ? (SPCT_ERR) : (SPCT_OK))

#define SPCT_LOGI(tag, fmt, ...) ESP_LOGI(tag, "(%s) " fmt, __func__, ##__VA_ARGS__);
#define SPCT_LOGW(tag, fmt, ...) ESP_LOGW(tag, "(%s) " fmt, __func__, ##__VA_ARGS__);
#define SPCT_LOGE(tag, fmt, ...) ESP_LOGE(tag, "(%s) " fmt, __func__, ##__VA_ARGS__);

typedef enum {
    SPCT_OK = 0,
    SPCT_ERR
} spct_ret_t;


#endif