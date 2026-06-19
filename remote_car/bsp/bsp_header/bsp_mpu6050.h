#ifndef BSP_MPU6050_H
#define BSP_MPU6050_H

#include "esp_err.h"
#include "esp_log.h"

#include "driver/i2c_master.h"

#define BSP_MPU6050_SCL_NUM GPIO_NUM_9
#define BSP_MPU6050_SDA_NUM GPIO_NUM_8

#define BSP_MPU6050_INTERNAL_PULLUP false
#define BSP_MPU6050_I2C_SPEED 400000
#define BSP_MPU6050_I2C_NO_ACK_CHECK false

#define BSP_MPU6050_ADDRESS 0x68

#define PWR_MGMT_1_REG 0x6B
#define PWR_MGMT_1_CONF 0x01
#define CONFIG_REG 0x1A
#define CONFIG_CONF 0x03

#define ACCEL_XOUT_H 0x3B

#define BSP_MPU6050_RAW_DATA_BYTES 14

typedef struct 
{
    float accel_x_f;
    float accel_y_f;
    float accel_z_f;

    float gyro_x_f;
    float gyro_y_f;
    float gyro_z_f;
}bsp_mpu6050_angle;

void bsp_mpu6050_init(void);

esp_err_t bsp_mpu6050_get_angle(bsp_mpu6050_angle* curr_angle);

#endif /* BSP_MPU6050_H */