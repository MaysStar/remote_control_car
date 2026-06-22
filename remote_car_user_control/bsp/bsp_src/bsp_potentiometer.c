#include "bsp_potentiometer.h"

/* Private variables and functions */
static const char* TAG = "BSP_ADC";

static adc_continuous_handle_t adc_handle = NULL;

void bsp_potentiometer_adc_init(void)
{
    /* Configure pattern and handle */
    adc_digi_pattern_config_t adc_digi_pattern_config = 
    {
        .atten = BSP_POTENTIOMETER_ADC_ATTEN, 
        .channel = BSP_POTENTIOMETER_ADC_CHANNEL,
        .unit = BSP_POTENTIOMETER_ADC_UNIT,
        .bit_width = BSP_POTENTIOMETER_ADC_BIT_WIDTH,
    };

    /* Set DMA remory size */
    adc_continuous_handle_cfg_t adc_continuous_handle_cfg = 
    {
        .max_store_buf_size = 1024,
        .conv_frame_size = 256,
    };

    ESP_ERROR_CHECK(adc_continuous_new_handle(&adc_continuous_handle_cfg, &adc_handle));

    /* Connect pattern with ADC handle */
    adc_continuous_config_t adc_continuous_config_potmtr = 
    {
        .adc_pattern = &adc_digi_pattern_config,
        .conv_mode = ADC_CONV_SINGLE_UNIT_1,
        .format = ADC_DIGI_OUTPUT_FORMAT_TYPE1,
        .pattern_num = 1,
        .sample_freq_hz =  20 * 1000 // 20kHz
    };

    ESP_ERROR_CHECK(adc_continuous_config(adc_handle, &adc_continuous_config_potmtr));

    ESP_ERROR_CHECK(adc_continuous_start(adc_handle));

    ESP_LOGI(TAG, "starting is successful");
}

uint8_t bsp_potentiometer_adc_get_value(void)
{
    uint8_t adc_data[256];
    uint32_t bytes_read = 0;

    float curr_value_in_percent = 0.0f;

    if(adc_handle != NULL)
    {
        /* Read data from ADC */
        uint32_t data_sum = 0; 
        uint32_t actual_samples = 0;

        esp_err_t res = adc_continuous_read(adc_handle, adc_data, sizeof(adc_data), &bytes_read, 10);

        if(res == ESP_OK)
        {
            /* Summarize data and calculate average */
            for(uint32_t i = 0; i < bytes_read; i += SOC_ADC_DIGI_RESULT_BYTES)
            {
                adc_digi_output_data_t* p_adc_data = (adc_digi_output_data_t*)&adc_data[i];

                data_sum += p_adc_data->type1.data;
                actual_samples++;
            }

            if(actual_samples > 0)
            {
                float avg_raw_value = ((float)data_sum / (float)actual_samples);
                curr_value_in_percent = ((float)avg_raw_value / BSP_POTENTIOMETER_ADC_BIT_VALUE) * 100.0f;
            }
        }
        else
        {
            ESP_LOGE(TAG, "timeout");
        }
    }
    else 
    {
        ESP_LOGE(TAG, "init needed");
    }

    /* Set limitation */
    if (curr_value_in_percent > 100.0f) curr_value_in_percent = 100.0f;
    if (curr_value_in_percent < 0.0f)   curr_value_in_percent = 0.0f;

    return (uint8_t)curr_value_in_percent;
}