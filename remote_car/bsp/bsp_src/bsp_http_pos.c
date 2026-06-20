#include "bsp_http_pos.h"

/* Private variables and functions */
static const char* TAG = "APP_HTTP";

static esp_http_client_handle_t esp_http_client_handle = NULL;

void bsp_http_pos_init(void)
{
    /* Configure HTTPS and headers */
    esp_http_client_config_t esp_http_client_config = 
    {
        .url = HTTPS_POSITION_URL,
        .method = HTTP_METHOD_POST,
        .transport_type = HTTP_TRANSPORT_OVER_SSL,

        .keep_alive_enable = true,

        .crt_bundle_attach = esp_crt_bundle_attach,
    };

    esp_http_client_handle = esp_http_client_init(&esp_http_client_config);
    if (esp_http_client_handle == NULL) {
        ESP_LOGE(TAG, "Failed to init HTTP client");
    }

    esp_http_client_set_header(esp_http_client_handle, "Content-Type", "application/json");
}

void bsp_http_post_pos(float roll, float pitch, float yaw)
{
    if(esp_http_client_handle != NULL)
    {
        /* Parse data into JSON type */
        char json_payload[128];
        snprintf(json_payload, sizeof(json_payload), "{\"roll\":%.1f,\"pitch\":%.1f,\"yaw\":%.1f}", roll, pitch, yaw);

        /* POST data in HTTP website */
        ESP_ERROR_CHECK(esp_http_client_set_post_field(esp_http_client_handle, json_payload, strlen(json_payload)));

        int res = esp_http_client_perform(esp_http_client_handle);

        /* Get feedback */
        if(res == ESP_OK)
        {
            int stat_code = esp_http_client_get_status_code(esp_http_client_handle);

            if(stat_code != 200)
            {
                ESP_LOGE(TAG, "HTTP POST failed status code: %d", stat_code);
            }
        }
        else {
            ESP_LOGE(TAG, "HTTP POST failed: %s", esp_err_to_name(res));
        }
    }
    else
    {
        ESP_LOGE(TAG, "Failed to init HTTP client");
    }
}

void bsp_http_cleanup(void)
{
    /* Clean up in the end of communication */
    if(esp_http_client_handle != NULL)
    {
        esp_err_t res = esp_http_client_cleanup(esp_http_client_handle);

        if(res != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to cleanup HTTP client");
        }
    }
    else
    {
        ESP_LOGE(TAG, "Failed to init HTTP client");
    }
}