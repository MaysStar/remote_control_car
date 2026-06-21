#include "app_car_position.h"

/* Private variables and functions */
static const char* TAG = "APP_POS";

static const float RAD_TO_DEG = 57.29578f;
static const float dt = 0.01;

void app_car_position_task(void* pvParameters)
{
    app_car_position curr_car_position = {0};

    while(1)
    {
        bsp_mpu6050_angle curr_mpu6050_angle;

        if(osal_mpu6050_get_angle(&curr_mpu6050_angle) == ESP_OK)
        {
            /* Calculate roll, pitch and yaw for accelerometer */
            float accel_roll = atan2f(curr_mpu6050_angle.accel_y_f, 
                sqrtf(curr_mpu6050_angle.accel_x_f * curr_mpu6050_angle.accel_x_f + curr_mpu6050_angle.accel_z_f * curr_mpu6050_angle.accel_z_f)) * RAD_TO_DEG;

            float accel_pitch = atan2f(-curr_mpu6050_angle.accel_x_f, 
                sqrtf(curr_mpu6050_angle.accel_y_f * curr_mpu6050_angle.accel_y_f + curr_mpu6050_angle.accel_z_f * curr_mpu6050_angle.accel_z_f)) * RAD_TO_DEG;

            float prev_roll = curr_car_position.roll;
            float prev_pitch = curr_car_position.pitch;
            float prev_yaw = curr_car_position.yaw;

            /* Coplementary filter for gyro and accel 
            *  cord_n = ALPHA * S(gyro_n)dt + (1 - ALPHA) * accel_n
            */
            curr_car_position.roll = ALPHA * (curr_car_position.roll + curr_mpu6050_angle.gyro_x_f * dt) + 
                                    (1.0f - ALPHA) * accel_roll;

            curr_car_position.pitch = ALPHA * (curr_car_position.pitch + curr_mpu6050_angle.gyro_y_f * dt) + 
                                    (1.0f - ALPHA) * accel_pitch;
        
            curr_car_position.yaw = curr_car_position.yaw + curr_mpu6050_angle.gyro_z_f * dt;

            /* Hard Constraints for angles */
            if (curr_car_position.pitch > 85.0f)  curr_car_position.pitch = 85.0f;
            if (curr_car_position.pitch < -85.0f) curr_car_position.pitch = -85.0f;
            if (curr_car_position.roll > 85.0f)   curr_car_position.roll = 85.0f;
            if (curr_car_position.roll < -85.0f)  curr_car_position.roll = -85.0f;

            /* Cancel gyroscope drift */
            while (curr_car_position.yaw > 180.0f)  curr_car_position.yaw -= 360.0f;
            while (curr_car_position.yaw < -180.0f) curr_car_position.yaw += 360.0f;

            /* Noise filter */
            if(fabsf(prev_roll - curr_car_position.roll) < 0.2f)
            {
                curr_car_position.roll = prev_roll;
            }

            if(fabsf(prev_pitch - curr_car_position.pitch) < 0.2f)
            {
                curr_car_position.pitch = prev_pitch;
            }

            if(fabsf(prev_yaw - curr_car_position.yaw) < 0.2f)
            {
                curr_car_position.yaw = prev_yaw;
            }

            app_set_position(&curr_car_position);
            app_car_dir_speed curr_dir_speed = app_get_dir_speed();
            ESP_LOGI(TAG, "dir: %d, speed: %d, speed roll: %03.1f, pitch: %03.1f, yaw %03.1f", 
                curr_dir_speed.direction, curr_dir_speed.speed, curr_car_position.roll, curr_car_position.pitch, curr_car_position.yaw);
        }
        else
        {
            ESP_LOGE(TAG, "data is corrupted");
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}