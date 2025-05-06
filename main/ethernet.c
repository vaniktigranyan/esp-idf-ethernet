#include "esp_event.h"
#include "esp_log.h"
#include "esp_eth.h"
#include "esp_netif.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_netif_ip_addr.h"
#include "driver/gpio.h"


static const char *TAG = "MY Ethernet >> ";
EventGroupHandle_t eth_event_group;

void eth_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    switch (event_id) {
    case ETHERNET_EVENT_CONNECTED:
        ESP_LOGI(TAG, "Ethernet Link Up");
        break;
    case ETHERNET_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "Ethernet Link Down");
        break;
    case ETHERNET_EVENT_START:
        ESP_LOGI(TAG, "Ethernet Started");
        break;
    case ETHERNET_EVENT_STOP:
        ESP_LOGI(TAG, "Ethernet Stopped");
        break;
    default:
        ESP_LOGW(TAG, "Unknown Ethernet event: %" PRId32, event_id);
        break;
    }
}
// IP event handler
void got_ip_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    const esp_netif_ip_info_t *ip_info = &event->ip_info;

    ESP_LOGI(TAG, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
    ESP_LOGI(TAG, "Got IP Address:");
    ESP_LOGI(TAG, "IP: " IPSTR, IP2STR(&ip_info->ip));
    ESP_LOGI(TAG, "Netmask: " IPSTR, IP2STR(&ip_info->netmask));
    ESP_LOGI(TAG, "Gateway: " IPSTR, IP2STR(&ip_info->gw));
    ESP_LOGI(TAG, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
}
void init_ethernet(void)
{
    ESP_LOGI(TAG, "Initializing Ethernet");
    // Initialize the TCP/IP stack (required before using esp_netif)
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    eth_event_group = xEventGroupCreate();

    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_ETH();  // Default Ethernet interface config
    esp_netif_t *eth_netif = esp_netif_new(&cfg);      // Create a new network interface

    ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &got_ip_event_handler, NULL));

    // Configure the ESP32's built-in MAC controller and specify SMI GPIO pins
    eth_esp32_emac_config_t mac_config = ETH_ESP32_EMAC_DEFAULT_CONFIG();
    mac_config.smi_mdc_gpio_num = 23;   // MDC GPIO pin for Ethernet (clock line)
    mac_config.smi_mdio_gpio_num = 18;  // MDIO GPIO pin for Ethernet (data line)

    eth_mac_config_t default_mac_config = ETH_MAC_DEFAULT_CONFIG();
    
    // Configure the external PHY chip (e.g., IP101)
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
    phy_config.phy_addr = 0;           // PHY address on MDIO bus (commonly 0)
    phy_config.reset_gpio_num = 5;     // Reset GPIO for PHY (use -1 if not used)

    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&mac_config, &default_mac_config);
    esp_eth_phy_t *phy = esp_eth_phy_new_ip101(&phy_config);

    esp_eth_config_t eth_config = ETH_DEFAULT_CONFIG(mac, phy);
    esp_eth_handle_t eth_handle = NULL;

    ESP_ERROR_CHECK(esp_eth_driver_install(&eth_config, &eth_handle));// Install the Ethernet driver
    ESP_ERROR_CHECK(esp_netif_attach(eth_netif, esp_eth_new_netif_glue(eth_handle)));// Attach the network interface to the Ethernet driver


    // example static IP Address
    // ESP_ERROR_CHECK(esp_netif_dhcpc_stop(eth_netif));
    // esp_netif_ip_info_t ip_info = {
    //     .ip.addr = esp_ip4addr_aton("192.168.2.10"),
    //     .netmask.addr = esp_ip4addr_aton("255.255.255.0"),
    //     .gw.addr = esp_ip4addr_aton("192.168.2.1")
    // };
    // ESP_ERROR_CHECK(esp_netif_set_ip_info(eth_netif, &ip_info));


    ESP_ERROR_CHECK(esp_eth_start(eth_handle));// Start the Ethernet driver (initializes PHY, establishes link, etc.)
}
