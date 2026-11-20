# Ethernet & Zero-Radio Environment Porting Plan

> [!NOTE]
> **Status: Future Architectural Roadmap**
> Production firmware targets wireless ESP32-S3 devices (Wi-Fi, BLE, LoRa). This document details an architectural adaptation for high-security, SCADA, data center, or air-gapped environments where all wireless RF emissions are strictly prohibited and physical Ethernet (RJ45 / PoE) is required.

## 1. Executive Summary & Objective

In secure facilities, financial infrastructure, and industrial SCADA networks, radio emissions (Wi-Fi, Bluetooth, LoRa) are frequently forbidden by policy. A Stratum-1 reference clock in these environments must operate strictly over physical Ethernet while continuing to discipline its internal clock via an external GNSS antenna.

Because Time Beacon's core timekeeping, hardware 1PPS discipline, and UDP NTP server are modular and decoupled from the radio stack, adapting the project to a wired Ethernet architecture requires **zero alterations to the core synchronization logic**.

---

## 2. Hardware Architecture Options

ESP-IDF natively supports two primary physical Ethernet paradigms:

### Option A: SPI Ethernet on ESP32-S3 (e.g. WIZnet W5500)
* **Architecture**: ESP32-S3 + SPI Ethernet Controller (W5500 or ENC28J60) + GNSS module.
* **Why**: The ESP32-S3 does not integrate an internal Ethernet MAC (EMAC). A standalone SPI Ethernet controller provides 10/100 Ethernet over the standard SPI bus.
* **Pin Requirements (6 GPIOs)**:
  - SPI Bus: MOSI, MISO, SCLK, CS
  - Control: Interrupt (`INT` GPIO with falling-edge IRQ) and Reset (`RST` GPIO).
* **Throughput**: ~10–15 Mbps sustained over high-speed SPI. More than sufficient to service 10,000+ NTP queries per second.
* **Off-the-shelf reference hardware**: LilyGO T-ETH-Lite (ESP32-S3 + W5500) paired with an external u-blox or Unicore GNSS receiver.

### Option B: Dedicated RMII Ethernet on ESP32 with PoE (e.g. LAN8720)
* **Architecture**: Classic ESP32 (Xtensa dual-core) + external RMII PHY + GNSS module.
* **Why**: The classic ESP32 includes an internal 10/100 EMAC peripheral, connecting to an external physical transceiver (LAN8720A, RTL8201, or IP101) via a reduced 50 MHz RMII bus.
* **Power over Ethernet (PoE)**: Allows a single RJ45 cable to provide both high-speed network connectivity and device operating power (IEEE 802.3af PoE).
* **Off-the-shelf reference hardware**:
  - **Olimex ESP32-POE-ISO**: 10/100 Ethernet with 3000V galvanic isolation and 802.3af PoE.
  - External GNSS module (u-blox NEO-M8N / NEO-M9N / UC6580) wired to UART1 + 1PPS GPIO.

---

## 3. Firmware Adaptations & Zero-RF Guarantees

### 3.1 Complete Radio Deactivation (Zero RF)
To guarantee strict regulatory and security compliance, wireless radios are excised at both the compile and runtime levels:
1. **Kconfig Disables**:
   ```ini
   CONFIG_ESP_WIFI_ENABLED=n
   CONFIG_BT_ENABLED=n
   ```
2. **LoRa Radio Isolation**:
   - The SX1262 driver initialization and beacon task loops are bypassed entirely (`enable_meshtastic = false`, or omitted from CMake build).
   - No RF transceivers are energized.

### 3.2 Network Layer: Replacing Wi-Fi with `eth_manager`
The only network subsystem replacement is substituting `wifi_manager.c` with an Ethernet driver (`eth_manager.c`):

```c
// Initialize ESP-IDF default Ethernet netif
esp_netif_config_t cfg = ESP_NETIF_DEFAULT_ETH();
esp_netif_t *eth_netif = esp_netif_new(&cfg);

// Install SPI Ethernet driver (e.g. W5500)
eth_w5500_config_t w5500_config = ETH_W5500_DEFAULT_CONFIG(SPI2_HOST, &spi_devcfg);
w5500_config.int_gpio_num = PIN_ETH_INT;
esp_eth_mac_t *mac = esp_eth_mac_new_w5500(&w5500_config, &mac_config);
esp_eth_phy_t *phy = esp_eth_phy_new_w5500(&phy_config);

// Install and start driver
esp_eth_config_t eth_config = ETH_DEFAULT_CONFIG(mac, phy);
esp_eth_handle_t eth_handle = NULL;
esp_eth_driver_install(&eth_config, &eth_handle);
esp_netif_attach(eth_netif, esp_eth_new_netif_glue(eth_handle));
esp_eth_start(eth_handle);
```

### 3.3 Core Subsystems (100% Unchanged)
* **NTP Server (`ntp_server.c`)**: Binds to `INADDR_ANY` on UDP port 123. Operates seamlessly over Ethernet without line-of-code changes.
* **GNSS Driver (`gnss_driver.c`, `gnss_task.c`, `nmea_parser.c`)**: Evaluates incoming satellite sentences and feeds hardware timestamps to timekeeper.
* **Time Discipline (`time_sync.c`)**: Maintains hardware 1PPS discipline, PLL phase locking, and microsecond reference clock accuracy.
* **Configuration Drive (`usb_msc.c`)**: If running on ESP32-S3, USB flash drive configuration (`config.ini`) remains functional for setting static IP or DHCP.

---

## 4. Engineering Effort & Implementation Estimate

* **Porting Effort**: **Low to Moderate (~1 to 2 days)**.
* **Verification**: Connect to an isolated test bench switch, verify DHCP/static IP acquisition, and benchmark timestamp response latency using Chrony or `sntp` over Ethernet.
