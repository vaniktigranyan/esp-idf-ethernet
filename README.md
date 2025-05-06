# 🧩Connecting IP101GA to ESP32 and Running an HTTP Server (RMII)

## 🗒️Description

This guide explains how to connect the IP101GA Ethernet PHY chip to an ESP32 microcontroller using the RMII interface, configure the Ethernet connection, and launch an HTTP server using ESP-IDF.

---

## Required Components

- ESP32 (WROOM or WROVER)
- PHY chip: **IP101GA**
- Connector: **RJ45 with transformer**
- Some resistors and capacitors (see IP101GA datasheet)
- 3.3V power supply

---

## IP101GA to ESP32 Wiring (Scheme)

```plaintext

ESP32                      IP101GA (PHY)
+------------+            +----------------------+
|   GPIO23   |---MDC----> |    MDC (Clock)       |
|   GPIO18   |---MDIO---> |    MDIO (Data)       |
|   GPIO5    |--RESET---> |    RESETn            |
|   GPIO21   |---TX_EN--> |    TXEN              |
|   GPIO19   |---TXD0---> |    TXD0              |
|   GPIO22   |---TXD1---> |    TXD1              |
|   GPIO25   |<--RXD0---- |    RXD0              |
|   GPIO26   |<--RXD1---- |    RXD1              |
|   GPIO27   |<--CRS_DV-- |    CRS_DV            |
|   GPIO16   |--CLK_OUT-->|   XI (RMII clock in) |
|   GND      |---GND----> |    GND               |
|   3.3V     |--VCC-----> |    VDDIO, VDD        |
+------------+            +----------------------+
```

# ⚙️ sdkconfig Settings
Enable Ethernet and configure RMII interface:
```bash
CONFIG_LWIP_IRAM_OPTIMIZATION=y
CONFIG_ETH_IRAM_OPTIMIZATION=y
CONFIG_ETH_ENABLED=y
CONFIG_ETH_USE_ESP32_EMAC=y
CONFIG_ETH_PHY_INTERFACE_RMII=y
CONFIG_ETH_RMII_CLK_OUTPUT=y
CONFIG_ETH_RMII_CLK_OUT_GPIO=16
CONFIG_ETH_DMA_BUFFER_SIZE=512
CONFIG_ETH_DMA_RX_BUFFER_NUM=10
CONFIG_ETH_DMA_TX_BUFFER_NUM=10
```

# 🌐 Runtime Output Example

```bash
I (399) MY Ethernet >> : Initializing Ethernet
I (459) esp_eth.netif.netif_glue: 7c:9e:bd:d3:72:43
I (459) esp_eth.netif.netif_glue: ethernet attached to netif
I (2659) MY Ethernet >> : Ethernet Started
I (2659) MY Ethernet >> : Ethernet Link Up
I (2659) main_task: Returned from app_main()
I (3659) MY Ethernet >> : ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
I (3659) MY Ethernet >> : Got IP Address:
I (3659) MY Ethernet >> : IP: 192.168.37.43
I (3659) MY Ethernet >> : Netmask: 255.255.255.0
I (3669) MY Ethernet >> : Gateway: 192.168.37.254
I (3669) MY Ethernet >> : ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
I (3679) esp_netif_handlers: eth ip: 192.168.37.43, mask: 255.255.255.0, gw: 192.168.37.254
```
# 🌐 HTTP Server Log Example
Once the server is running, you can send a POST,GET request using Postman or another client:
```bash
I (41789) MY SERVER >> : POST request received, content length: 36
I (41789) MY SERVER >> : Received POST content:
{
    "msg":"Hello from Postman"
}
```