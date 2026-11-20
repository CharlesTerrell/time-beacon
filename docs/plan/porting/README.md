# Multi-Architecture Porting Roadmaps

> [!NOTE]
> **Status: Future Architectural Roadmaps**
> Production firmware is implemented in pure ESP-IDF (v5.x/v6.x) for the ESP32-S3. The documents in this directory capture architectural explorations and migration plans for potential future hardware variants.

## Porting Scenarios

| Scenario | Target Platforms | Key Frameworks | Effort | Document |
| :--- | :--- | :--- | :--- | :--- |
| **1. Non-Espressif Targets** | Heltec Mesh Node T096 (nRF52840), Nordic ARM Cortex-M | PlatformIO, Zephyr RTOS, Nordic nrfx | High (Requires HAL abstraction for radio, BLE, & power) | [PlatformIO / nRF52 Roadmap](platformio_nrf52.md) |
| **2. Espressif RISC-V Targets** | ESP32-C3, ESP32-C6, ESP32-P4 | Pure ESP-IDF (unified HAL) | Low (~5% delta: pinout, ADC cal, USB MSC constraints) | [Espressif RISC-V Migration](espressif_riscv.md) |
