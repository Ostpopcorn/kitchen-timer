#include "custom_exception.h"


#ifdef USE_EXCEPTION
#include <stdexcept>
#else
#include "esp_log.h"
#endif
void exception_throw(){
    #ifdef USE_EXCEPTION
        throw std::invalid_argument("Invalid type");
    #else
        ESP_LOGE("MODEL","ERROR, dunno what, sorry. ");
    #endif
}
