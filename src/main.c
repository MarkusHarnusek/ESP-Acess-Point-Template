#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <driver/uart.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_http_server.h"
#include "lwip/err.h"
#include "lwip/sys.h"

static const char *TAG = "WiFi_AP";
static const char *AP_SSID = "ESP Access Point";
static const char *AP_PASSWORD = "password";
static const int AP_MAX_CLIENTS = 100;
static wifi_auth_mode_t AP_AUTHMODE = WIFI_AUTH_WPA3_PSK;

uart_config_t uart_config = {
    .baud_rate = 115200,
    .data_bits = UART_DATA_8_BITS,
};

const char *html_content = "<!DOCTYPE html>\n"
                           "<html>\n"
                           "<head>\n"
                           "    <title>ESP32 Web Server</title>\n"
                           "    <style>\n"
                           "        body { font-family: Arial, sans-serif; text-align: center; margin-top: 50px; background-color: #f0f0f0; }\n"
                           "        .container { background-color: white; padding: 30px; border-radius: 10px; display: inline-block; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }\n"
                           "        h1 { color: #333; }\n"
                           "        p { color: #666; font-size: 16px; }\n"
                           "    </style>\n"
                           "</head>\n"
                           "<body>\n"
                           "    <div class='container'>\n"
                           "        <h1>Welcome to ESP32!</h1>\n"
                           "        <p>Web server is running on your ESP32.</p>\n"
                           "        <p>SSID: ESP32-AP</p>\n"
                           "        <p>Connect and visit http://192.168.4.1</p>\n"
                           "    </div>\n"
                           "</body>\n"
                           "</html>\n";

static esp_err_t root_get_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, html_content, strlen(html_content));
    return ESP_OK;
}

static httpd_uri_t root = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = root_get_handler,
    .user_ctx = NULL};

static httpd_handle_t start_webserver(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    if (httpd_start(&server, &config) == ESP_OK)
    {
        httpd_register_uri_handler(server, &root);
        ESP_LOGI(TAG, "Web server started");
    }
    return server;
}

// WiFi event handler
static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED)
    {
        ESP_LOGI(TAG, "Station connected");
    }
    else if (event_id == WIFI_EVENT_AP_STADISCONNECTED)
    {
        ESP_LOGI(TAG, "Station disconnected");
    }
}

static void wifi_init_softap(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = AP_SSID,
            .ssid_len = strlen(AP_SSID),
            .password = AP_PASSWORD,
            .max_connection = AP_MAX_CLIENTS,
            .authmode = AP_AUTHMODE
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "WiFi Access Point started. SSID: %s", wifi_config.ap.ssid);
}

void app_main(void)
{
    uart_param_config(UART_NUM_2, &uart_config);
    uart_flush(UART_NUM_0);

    esp_log_level_set("wifi", ESP_LOG_WARN);
    esp_log_level_set("esp_netif_handlers", ESP_LOG_WARN);

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "Starting WiFi Access Point");

    wifi_init_softap();

    start_webserver();

    ESP_LOGI(TAG, "ESP32 is running as WiFi Access Point");
    ESP_LOGI(TAG, "Connect to SSID: ESP32-AP, Password: 12345678");
    ESP_LOGI(TAG, "Then visit http://192.168.4.1 in your browser");
}
