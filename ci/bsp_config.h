#ifndef BSP_CONFIG_H
#define BSP_CONFIG_H

/* Build-only placeholders. Never flash CI firmware onto a vehicle. */
#define BSP_CONFIG_WIFI_SSID "ci-no-network"
#define BSP_CONFIG_WIFI_PASSWORD "ci-placeholder-only"
#define BSP_CONFIG_WIFI_AUTH WIFI_AUTH_WPA2_PSK
#define BSP_CONFIG_MQTT_HOSTNAME "mqtt.invalid"
#define BSP_CONFIG_MQTT_PORT 8883
#define BSP_CONFIG_MQTT_SKIP_COMMONAME false
#define BSP_CONFIG_MQTT_USERNAME "ci"
#define BSP_CONFIG_MQTT_PASSWORD "ci-placeholder-only"
#define HTTPS_POSITION_URL "https://telemetry.invalid/api/telemetry"
#define HTTPS_OTA_UPDATE_IP_ADDR "https://ota.invalid/firmware.bin"
#define LOGS_HOST_IP_ADDR "127.0.0.1"
#define LOGS_HOST_PORT_ADDR 5005

#endif /* BSP_CONFIG_H */
