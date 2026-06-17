#ifndef APP_CAR_POSITION_H
#define APP_CAR_POSITION_H

#include "math.h"

#include "osal.h"

#include "app_global_state.h"

#define ALPHA 0.98f

void app_car_position_task(void* pvParameters);

#endif /* APP_CAR_POSITION_H */