#include "app_global_state.h"
#include <assert.h>
#include <stdio.h>
TickType_t test_tick;
int main(void)
{
    assert(app_get_dir_speed().direction == APP_MQTT_COMMUNICATION_CAR_DIRECTION_STOP);
    app_global_state_init();
    app_set_speed(70);
    assert(app_get_dir_speed().speed == 0);
    app_set_speed(70);
    app_set_dir(APP_MQTT_COMMUNICATION_CAR_DIRECTION_FORWARD);
    test_tick = 499;
    assert(app_get_dir_speed().speed == 70);
    test_tick = 500;
    assert(app_get_dir_speed().speed == 0);
    app_set_dir(APP_MQTT_COMMUNICATION_CAR_DIRECTION_FORWARD);
    assert(app_get_dir_speed().speed == 0);
    app_set_speed(50);
    app_stop_motion();
    assert(app_get_dir_speed().direction == APP_MQTT_COMMUNICATION_CAR_DIRECTION_STOP);
    test_tick = UINT32_MAX - 100;
    app_set_dir(APP_MQTT_COMMUNICATION_CAR_DIRECTION_FORWARD);
    app_set_speed(30);
    test_tick = 398;
    assert(app_get_dir_speed().speed == 30);
    test_tick = 399;
    assert(app_get_dir_speed().speed == 0);
    test_tick = 1000;
    app_set_dir(APP_MQTT_COMMUNICATION_CAR_DIRECTION_FORWARD);
    app_set_speed(40);
    test_tick = 1400;
    app_set_dir(APP_MQTT_COMMUNICATION_CAR_DIRECTION_FORWARD);
    test_tick = 1500;
    assert(app_get_dir_speed().speed == 0);

    app_set_dir(APP_MQTT_COMMUNICATION_CAR_DIRECTION_FORWARD);
    app_set_speed(40);
    app_set_dir((app_car_dir_state)99);
    assert(app_get_dir_speed().speed == 0);

    app_set_dir(APP_MQTT_COMMUNICATION_CAR_DIRECTION_FORWARD);
    app_set_speed(255);
    assert(app_get_dir_speed().speed == 0);
    app_set_position(NULL);
    puts("command state tests passed");
}
