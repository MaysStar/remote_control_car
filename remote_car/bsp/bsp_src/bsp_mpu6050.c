#include "bsp_mpu6050.h"

/* Private variables and functions */
static const char* TAG = "BSP_MPU6050";

static i2c_master_dev_handle_t mpu6050_i2c_handle = NULL;
static uint8_t mpu6050_raw_data[BSP_MPU6050_RAW_DATA_BYTES];

void bsp_mpu6050_init(void)
{
    /* Configure ESP I2C for communication with MPU-6050 */
    i2c_master_bus_config_t i2c_master_bus_config = 
    {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_NUM_0,
        .scl_io_num = BSP_MPU6050_SCL_NUM,
        .sda_io_num = BSP_MPU6050_SDA_NUM,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = BSP_MPU6050_INTERNAL_PULLUP,
    };

    i2c_master_bus_handle_t i2c_master_bus_handle = NULL;
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_master_bus_config, &i2c_master_bus_handle));
    ESP_LOGI(TAG, "GPIO config successful");

    /* Configure MPU-6050 device I2C bus */
    i2c_device_config_t i2c_mpu6050_config = 
    {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = BSP_MPU6050_ADDRESS,
        .scl_speed_hz = BSP_MPU6050_I2C_SPEED,
        .flags.disable_ack_check = BSP_MPU6050_I2C_NO_ACK_CHECK,
        .scl_wait_us = 10000,
    };

    ESP_ERROR_CHECK(i2c_master_bus_add_device(i2c_master_bus_handle, &i2c_mpu6050_config, &mpu6050_i2c_handle));

    /* Configure MPU6050  */

    /* Register PWR_MGMT_1 as 0x01, so make PLL with X axis gyroscope reference and sleep mode off */
    uint8_t reg_conf[2];
    reg_conf[0] = PWR_MGMT_1_REG;
    reg_conf[1] = PWR_MGMT_1_CONF;
    ESP_ERROR_CHECK(i2c_master_transmit(mpu6050_i2c_handle, reg_conf, 2, 1000));

    /* Register CONFIG as 0x03, so set filter for gyroscope and accelerometer */
    reg_conf[0] = CONFIG_REG;
    reg_conf[1] = CONFIG_CONF;
    ESP_ERROR_CHECK(i2c_master_transmit(mpu6050_i2c_handle, reg_conf, 2, 1000));

    ESP_LOGI(TAG, "I2C and MPU6050 config successful");
}

esp_err_t bsp_mpu6050_get_angle(bsp_mpu6050_angle* curr_angle)
{
    uint8_t data_reg = ACCEL_XOUT_H;

    esp_err_t ret = i2c_master_transmit_receive(mpu6050_i2c_handle, &data_reg, 1, mpu6050_raw_data, BSP_MPU6050_RAW_DATA_BYTES, 1000);

    /* Cast data into int16_t type */
    int16_t accel_x = (int16_t)((mpu6050_raw_data[0] << 8) | mpu6050_raw_data[1]);
    int16_t accel_y = (int16_t)((mpu6050_raw_data[2] << 8) | mpu6050_raw_data[3]);
    int16_t accel_z = (int16_t)((mpu6050_raw_data[4] << 8) | mpu6050_raw_data[5]);

    int16_t gyro_x = (int16_t)((mpu6050_raw_data[8] << 8) | mpu6050_raw_data[9]);
    int16_t gyro_y = (int16_t)((mpu6050_raw_data[10] << 8) | mpu6050_raw_data[11]);
    int16_t gyro_z = (int16_t)((mpu6050_raw_data[12] << 8) | mpu6050_raw_data[13]);

    /* Normalize data ( divide by LSB )*/ 
    curr_angle->accel_x_f = ((float)accel_x / 16384.0f);
    curr_angle->accel_y_f = ((float)accel_y / 16384.0f);
    curr_angle->accel_z_f = ((float)accel_z / 16384.0f);

    curr_angle->gyro_x_f = ((float)gyro_x / 131.0f);
    curr_angle->gyro_y_f = ((float)gyro_y / 131.0f);
    curr_angle->gyro_z_f = ((float)gyro_z / 131.0f);

    return ret;
}