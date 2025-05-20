#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_http_server.h"
#include "cJSON.h"
#include"http_server.h"
#include "esp_spiffs.h"

static const char *TAG = "MY SERVER >> ";
httpd_handle_t server_handle = NULL;
void delete_server(){
    httpd_stop(&server_handle);
}
void init_spiffs() {
    ESP_LOGI("SPIFFS", "Initializing SPIFFS");
    //init spiffs
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true
    };

    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) {
        ESP_LOGE("SPIFFS", "Failed to mount or format filesystem");
    } else {
        size_t total, used;
        esp_spiffs_info(NULL, &total, &used);
        ESP_LOGI("SPIFFS", "Partition size: total: %d, used: %d", total, used);
    }
}

static esp_err_t get_handler(httpd_req_t *req)
{  
    ESP_LOGI("GET ======================>>", "Received request for URI: %s", req->uri);

    FILE* f = fopen("/spiffs/index.html", "r"); // open html file
    if (f == NULL) {
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }

    char line[128];
    while (fgets(line, sizeof(line), f)) {// file
        httpd_resp_send_chunk(req, line, strlen(line)); // send data in interface
    }
    fclose(f);// close file
    httpd_resp_send_chunk(req, NULL, 0); // end

    return ESP_OK;
}
static esp_err_t post_handler(httpd_req_t *req)
{
    char content[2048];  // buffer
    int content_len = req->content_len;  // lenght

    ESP_LOGI(TAG, "POST request received, content length: %d", content_len);

    // We limit the size to protect against buffer overflow
    if (content_len >= sizeof(content)) {
        ESP_LOGE(TAG, "Content too large, truncating...");
        content_len = sizeof(content) - 1;
    }

    // Reading data from the request body
    int ret = httpd_req_recv(req, content, content_len);
    if (ret <= 0) {
        ESP_LOGE(TAG, "Failed to receive POST content");
        return ESP_FAIL;
    }
    content[ret] = '\0';  
    ESP_LOGI(TAG, "Received POST content:\n%s", content);


    //const char *resp = "POST request processed";
    httpd_resp_send(req, "OK", strlen("OK"));
    return ESP_OK;
}
void server_initiation()
{
    httpd_config_t server_config = HTTPD_DEFAULT_CONFIG();
    server_config.uri_match_fn = httpd_uri_match_wildcard; 
    server_config.stack_size = 8192; // 8KB server stack size
    
    httpd_start(&server_handle, &server_config);

    httpd_uri_t uri_get = {
        .uri = "/",//.uri = "/*",
        .method = HTTP_GET,
        .handler = get_handler,
        .user_ctx = NULL};
    httpd_register_uri_handler(server_handle, &uri_get);

    httpd_uri_t post_devconf_uri = {
        .uri = "/config",
        .method = HTTP_POST,
        .handler = post_handler,
        .user_ctx = NULL};
    httpd_register_uri_handler(server_handle, &post_devconf_uri);
}
