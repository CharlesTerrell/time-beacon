| WiFi | LoRa | 32 V4 |
| ---- | ---- | ----- |
Wi-Fi/Bluetooth/LoRa
| Battery  | Management |     |
| -------- | ---------- | --- |
| 0.96inch | OLED       |     |
Documents Rev1.4 P1/17 August,2025 Heltec Automation © Limited standard files

|         |            | Document       | Version         |         |
| ------- | ---------- | -------------- | --------------- | ------- |
| Version | Time       | Description    |                 | Remark  |
| V4.1.0  | 2025-08-11 | Internal beta  | version         | Richard |
| V4.2.0  | 2025-09-11 | First Official | Release Version | Richard |
Documents Rev1.4 P2/17 Sep.2025 HelTecAutomation©Limitedstandardfileshttps://heltec.org

| Copyright |     | Notice |     |     |     |     | Disclaimer |
| --------- | --- | ------ | --- | --- | --- | --- | ---------- |
All contents in the files are protected by Chengdu Heltec Automation Technology Co.,
copyright law, and all copyrights are reserved Ltd. reserves the right to change, modify or
byChengdu HeltecAutomation TechnologyCo., improve the document and product described
Ltd.(hereinafter referredto asHeltec). Without herein. Its contents are subject to change
written permission, all commercial use of the without notice. These instructions are intended
| files from     | Heltec | are       | forbidden, |            | such as | copy, | foryouuse. |
| -------------- | ------ | --------- | ---------- | ---------- | ------- | ----- | ---------- |
| distribute,    |        | reproduce | the        | files,     | etc.,   | but   |            |
| non-commercial |        |           | purpose,   | downloaded |         | or    |            |
printedbyindividualarewelcome.
Documents Rev1.4 P3/17 August,2025 Heltec Automation © Limited standard files

Content
1.Introduction..............................................................................................................................................................5
1.1Overview.............................................................................................................................................................5
1.2Features...............................................................................................................................................................5
1.3ApplicationScenarios.....................................................................................................................................5
1.4Components.......................................................................................................................................................6
1.5Variants...............................................................................................................................................................6
2.PinDefinition...........................................................................................................................................................7
2.1PinLayout...........................................................................................................................................................7
2.2PinDescription.................................................................................................................................................8
2.2.1HeaderJ2........................................................................................................................................................8
2.2.2HeaderJ3........................................................................................................................................................9
2.2.3AdditionalPins..............................................................................................................................................10
2.2.4Peripherals(I2C/SPI/UART/I2S/PWM/etc.)...................................................................................................10
3.Specifications.........................................................................................................................................................11
3.1Generalspecifications..................................................................................................................................11
3.2PowerSupply..................................................................................................................................................12
3.3PowerOutput.................................................................................................................................................12
3.4PowerConsumption(25℃）.......................................................................................................................13
3.5LoRaRFCharacteristics...............................................................................................................................13
3.5.1TransmitPower...........................................................................................................................................13
3.5.2ReceivingSensitivity...................................................................................................................................13
3.6LoRaWANFrequencyPlan..........................................................................................................................14
4.Schematic................................................................................................................................................................15
5.Dimensions..............................................................................................................................................................16
6.Resource...................................................................................................................................................................17
6.1RelevantResource.........................................................................................................................................17
6.2ContactInformation.....................................................................................................................................17
Documents Rev1.4 P4/17 Sep.2025 HelTecAutomation©Limitedstandardfileshttps://heltec.org

1. Introduction
1.1 Overview
Heltec WiFi LoRa 32 V4 is a fully upgraded version of the classic LoRa development board.
Since its initial launch in 2017, this product series has been highly favored by developers and
makers. Building upon the powerful features of its predecessors, the V4 version introduces
comprehensive optimizations in Transmit Power, power management, expandability and
other hardware design. It is suitable for various IoT applications such as smart cities,
agricultural monitoring, smart homes, industrial control, security systems, and wireless meter
reading, providing developers with a more efficient and flexible development experience.
1.2 Features
 BaseonESP32-S3R2& SX-1262, supports Wi-Fib/g/n, BLE, andLoRacommunication.
 2MBPSRAMand16MBexternalFlash, moresuitableforUI andcomplexsystem.
 High-powerversionwith LoRatransmissionpower increasedto28±1dBm.
 Formfactor andpin compatibilitywith WiFiLoRa 32V3.
 PCcasingprotectsthescreenandintegratesFPC2.4Gantenna.
 AddedSH1.25-2Psolar panelinterface.
 Optimizedlithiumbattery management.
 AddedSH1.25-8PinGNSS interface.
 USBType-Cinterfacewithintegratedvoltageregulation,ESDprotection,short-circuitprotection,
andRF isolationdesign;
 Lowpowerconsumptionislessthan20μA.
1.3 Application Scenarios
 EnvironmentalSensor
 IndustrialInternetofThings
 AssetTracking
 RemoteMeter Reading
 Smart Agriculture
 WirelessAlarm
 InnovativeProjectsandGeekApplication
Documents Rev1.4 P5/17 Sep.2025 HelTecAutomation©Limitedstandardfileshttps://heltec.org

1.4 Components
1.5 Variants
Table1.5:Productvariants
TX Power
| No. | Model |     | Description |     |     |
| --- | ----- | --- | ----------- | --- | --- |
[dBm]
|     |              |      | 868~928MHz,   | compatible    | with EU868, |
| --- | ------------ | ---- | ------------- | ------------- | ----------- |
| 1   | HTIT-WB32LAF | 28±1 |               |               |             |
|     |              |      | IN865, US915, | AU915, AS923, | KR920, and  |
±
| 2 HTIT-WB32LAF-N-HF |     | 21 1   |                |            |            |
| ------------------- | --- | ------ | -------------- | ---------- | ---------- |
|                     |     |        | other regional | standards. |            |
| 3 HTIT-WB32LAF-N-LF |     | 21 ± 1 | 470~510MHz,    | compatible | with CN470 |
±
| 4 HTIT-WB32LAF-N-433 |     | 21 1 | 433MHz |     |     |
| -------------------- | --- | ---- | ------ | --- | --- |
Documents Rev1.4 P6/17 Sep.2025 HelTecAutomation©Limitedstandardfileshttps://heltec.org

2.Pin Definition
2.1 Pin Layout
The following figure shows some common information and descriptions of the WiFi LoRa 32
pins. For more detailed functional information about each pin, please refer to the ESP32 chip
manual. ESP32-S3 MCU Datasheet.
Documents Rev1.4 P7/17 Sep.2025 HelTecAutomation©Limitedstandardfileshttps://heltec.org

| 2.2 Pin Description |     |     |     |     |     |     |     |
| ------------------- | --- | --- | --- | --- | --- | --- | --- |
| 2.2.1 Header        | J2  |     |     |     |     |     |     |
Table2.2.1:Pindescription
| No. Name |            | Type |        | Function                                  |                                |           |     |
| -------- | ---------- | ---- | ------ | ----------------------------------------- | ------------------------------ | --------- | --- |
| 1 GND    | G          |      |        | Ground                                    |                                |           |     |
| 2 5V     | P          |      |        | 5V Power                                  | Supply                         |           |     |
| 3 Ve     | P          |      |        | Output                                    | 3.3V                           |           |     |
| 4 Ve     | P          |      |        | Output                                    | 3.3V                           |           |     |
| 5 RX     | I/O        |      | IO MUX | GPIO44,                                   | U0RXD                          |           |     |
| 6 TX     | I/O        |      | IO MUX | GPIO43,                                   | U0TXD                          |           |     |
| 7 RST    | I          |      |        | CHIP_PU,                                  | Reset Button                   |           |     |
| 8 0      | I/O        |      | IO MUX | GPIO0,                                    | PRG Button                     |           |     |
| 9 36     | I/O        |      | IO MUX | GPIO36,SPIIO7,FSPICLK,SUBSPICLK,Vext_Ctrl |                                |           |     |
| 10 35    | I/O        |      | IO MUX | GPIO35,                                   | SPIIO6,FSPID,                  | SUBSPID,  | LED |
| 11 34    | I/O        |      | IO MUX | GPIO34,                                   | SPIIO5,FSPICS0,                | SUBSPICS0 |     |
| 12 33    | I/O        |      | IO MUX | GPIO33,                                   | SPIIO4,FSPIHD,                 | SUBSPIHD  |     |
| 13 47    | I/O        |      | IO MUX | GPIO47,                                   | SPICLK_P_DIFF,SUBSPICLK_P_DIFF |           |     |
| 14 48    | I/O        |      | IO MUX | GPIO48,SPICLK_N_DIFF,SUBSPICLK_N_DIFF     |                                |           |     |
| 15 26    | I/O        |      | IO MUX | GPIO26,                                   | SPICS1                         |           |     |
| 16 21    | I/O        | RTC  | IO MUX | GPIO21,                                   | OLED RST                       |           |     |
| 17 20    | I/O Analog | RTC  | IO MUX | GPIO20,U1CTS,ADC2_CH9,                    |                                | USB_D+    |     |
| 18 19    | I/O Analog | RTC  | IO MUX | GPIO19,U1RTS,ADC2_CH8,                    |                                | USB_D-    |     |
Documents Rev1.4 P8/17 Sep.2025 HelTecAutomation©Limitedstandardfileshttps://heltec.org

| 2.2.2 Header | J3  |     |     |     |     |     |
| ------------ | --- | --- | --- | --- | --- | --- |
Table2.2.2:Pindescription
| No. Name |     | Type |        | Function   |                         |          |
| -------- | --- | ---- | ------ | ---------- | ----------------------- | -------- |
| 1 GND    | P   |      |        | Ground.    |                         |          |
| 2 3V3    | P   |      |        | 3.3V Power | Supply                  |          |
| 3 3V3    | P   |      |        | 3.3V Power | Supply                  |          |
| 4 37     | I/O |      | IO MUX | GPIO37,    | SPIDQS, FSPIQ, SUBSPIQ, | ADC Ctrl |
| 5 46     | I/O |      | IO MUX | GPIO46     |                         |          |
| 6 45     | I/O |      | IO MUX | GPIO45     |                         |          |
| 7 42     | I/O |      | IO MUX | GPIO42,    | MTMS, GNSS_RST          |          |
| 8 41     | I/O |      | IO MUX | GPIO41,    | MTDI, GNSS_PPS          |          |
| 9 40     | I/O |      | IO MUX | GPIO40,    | MTDO, GNSS_Wakeup       |          |
| 10 39    | I/O |      | IO MUX | GPIO39,    | MTCK, GNSS_TX           |          |
| 11 38    | I/O |      | IO MUX | GPIO38,    | FSPIWP, SUBSPIWP,       | GNSS_RX  |
VBAT_Read1
| 12 1 | I/O Analog | RTC | IO MUX | GPIO1, ADC1_CH0, | TOUCH1, |     |
| ---- | ---------- | --- | ------ | ---------------- | ------- | --- |
| 13 2 | I/O Analog | RTC | IO MUX | GPIO2, ADC1_CH1, | TOUCH2  |     |
| 14 3 | I/O Analog | RTC | IO MUX | GPIO3, ADC1_CH2, | TOUCH3  |     |
| 15 4 | I/O Analog | RTC | IO MUX | GPIO4, ADC1_CH3, | TOUCH4  |     |
| 16 5 | I/O Analog | RTC | IO MUX | GPIO5, ADC1_CH4, | TOUCH5  |     |
| 17 6 | I/O Analog | RTC | IO MUX | GPIO6, ADC1_CH5, | TOUCH6  |     |
18 7 I/O Analog RTC IO MUX GPIO7, ADC1_CH6, TOUCH7, V FEM _Control
1 ADC1_CH0isusedtoreadthelithiumbatteryvoltage,theADC_CTRL(37)pinneedstobepulledhigh.thevoltageofthelithiumbatteryis:
VBAT=100/(100+390)*VADC_IN1
Documents Rev1.4 P9/17 Sep.2025 HelTecAutomation©Limitedstandardfileshttps://heltec.org

| 2.2.3 Additional  |        | Pins                        |      |        |                    |              |            |     |
| ----------------- | ------ | --------------------------- | ---- | ------ | ------------------ | ------------ | ---------- | --- |
|                   |        |                             |      |        |                    | Table 2.2.3: | Additional | Pin |
| No. Name          |        |                             | Type |        | Function           |              |            |     |
| 1                 | 18 I/O | Analog                      | RTC  | IO MUX | GPIO18, OLED_SCL   |              |            |     |
| 2                 | 17 I/O | Analog                      | RTC  | IO MUX | GPIO17, OLED_SDA   |              |            |     |
| 3                 | 16 I/O | Analog                      | RTC  | IO MUX | GPIO16, XTAL_32K_N |              |            |     |
| 4                 | 15 I/O | Analog                      | RTC  | IO MUX | GPIO15, XTAL_32K_P |              |            |     |
| 2.2.4 Peripherals |        | (I2C/SPI/UART/I2S/PWM/etc.) |      |        |                    |              |            |     |
ESP32-S3 can integrates a rich set of peripherals. To learn more about on-chip components,
| please | refer to ESP32-S3 | MCU | Datasheet. |     |     |     |     |     |
| ------ | ----------------- | --- | ---------- | --- | --- | --- | --- | --- |
Owing to the GPIO Matrix and IOMUX capabilities, the vast majority of GPIO pins can be
configured for functions such as I2C, SPI, I2S, PWM or UART. For specific pin assignments
and IOMUX correspondences, please refer to the list of pins marked with IOMUX
| functionality | in Appendix | Tables | 2.2.1 | and 2.2.2. |     |     |     |     |
| ------------- | ----------- | ------ | ----- | ---------- | --- | --- | --- | --- |
Note: The original functionality of the reconfigured pins will be affected. Please select
appropriate pins for your application. Such as OLED has occupied one I2C interface, while
| LoRa and | Flash have | each utilized | a   | separate | SPI interface. |     |     |     |
| -------- | ---------- | ------------- | --- | -------- | -------------- | --- | --- | --- |
Documents Rev1.4 P10/17 Sep.2025 HelTecAutomation©Limitedstandardfileshttps://heltec.org

3.Specifications
| 3.1 General    | specifications |                   |                  |            |              |                  |                |
| -------------- | -------------- | ----------------- | ---------------- | ---------- | ------------ | ---------------- | -------------- |
|                |                |                   |                  |            |              | Table3.1:General | specifications |
| Parameters     |                | Description       |                  |            |              |                  |                |
| Master Chip    |                | ESP32-S3R2        |                  |            |              |                  |                |
| LoRa Chip      |                | SX-1262           |                  |            |              |                  |                |
| Frequency      |                | 433~510           | MHz, 863~928     |            | MHz          |                  |                |
| Max. TX Power  |                | 21±1dBm,          | 28±1             | dBm        |              |                  |                |
| Max. Receiving | Sensitivity    | -137 dBm          |                  |            |              |                  |                |
| Wi-Fi          |                | 802.11            | b/g/n, up        | to 150Mbps |              |                  |                |
| Bluetooth      |                | Bluetooth         | LE, Bluetooth    |            | 5, Bluetooth | mesh             |                |
| OLED           |                | SSD1315(0.96Inch, |                  | 128*64     | resolution)  |                  |                |
| Power Supply   |                | 5V@USB/Solar,     | 3.3-4.2V@Battery |            |              |                  |                |
Hardware Resource 7*ADC1 + 2*ADC2, 7*Touch, 3*UART, 2*I2C, 2*I2S, 4*SPI, etc.
|     |     | 384KB | ROM; 512KB | SRAM; | 16KB | RTC SRAM; 16MB | Flash; 2MB |
| --- | --- | ----- | ---------- | ----- | ---- | -------------- | ---------- |
Memory
PSRAM
|     |     | USB Type-C; | SH1.25-2P |     | lithium | battery interface; | SH1.25-2P |
| --- | --- | ----------- | --------- | --- | ------- | ------------------ | --------- |
Interface solar panel interface; 2*IPEX1.0 ANT(LoRa&2.4G); 2*18*2.54
|     |     | Header | Pins, 2*2*2.54 | Header | Pins |     |     |
| --- | --- | ------ | -------------- | ------ | ---- | --- | --- |
℃
Operating Temperature -40~85 (OLED operating temperature: -40~ 70°C)
| Dimensions |     | 51.7 * | 25.4* 10.7mm |     |     |     |     |
| ---------- | --- | ------ | ------------ | --- | --- | --- | --- |
Documents Rev1.4 P11/17 Sep.2025 HelTecAutomation©Limitedstandardfileshttps://heltec.org

| 3.2 Power | Supply |     |     |     |     |     |     |     |     |     |
| --------- | ------ | --- | --- | --- | --- | --- | --- | --- | --- | --- |
USB or 5V can only be connected to one of them. When either USB or 5V input is connected,
the solar panel and lithium battery can be connected simultaneously.
|           |                 |      |     |         |     |         |         | Table3.2: |         | Powersupply |
| --------- | --------------- | ---- | --- | ------- | --- | ------- | ------- | --------- | ------- | ----------- |
| Power     | Supply          | Mode |     | Minimum |     | Typical | Maximum |           | Company |             |
| Type-C    | USB(≥500mA)     |      |     | 4.7     |     | 5       |         | 6         |         | V           |
| Lithium   | battery(≥250mA) |      |     | 3.3     |     | 3.7     |         | 4.2       |         | V           |
|           | 5V pin(≥500mA)  |      |     | 4.7     |     | 5       |         | 6         |         | V           |
|           | 3V3 pin(≥150mA) |      |     | 2.7     |     | 3.3     |         | 3.5       |         | V           |
|           | Solar Panel     |      |     | 4.7     |     | 5       |         | 6         |         | V           |
| 3.3 Power | Output          |      |     |         |     |         |         |           |         |             |
When using VE for external power supply, the VextCtrl(GPIO36) pin needs to be pulled low.
Table3.3:Poweroutput
|     | Pin    |     | Minimum |     | Typical |     | Maximum |     | Company |     |
| --- | ------ | --- | ------- | --- | ------- | --- | ------- | --- | ------- | --- |
|     | 3.3V   |     |         |     |         |     |         | 500 |         | mA  |
|     | 5V@USB |     |         |     |         |     |         | 500 |         | mA  |
|     | Ve     |     |         |     |         |     |         | 500 |         | mA  |
Documents Rev1.4 P12/17 Sep.2025 HelTecAutomation©Limitedstandardfileshttps://heltec.org

| 3.4 Power |      | Consumption(25℃） |          |        |      |     |           |       |             |
| --------- | ---- | ---------------- | -------- | ------ | ---- | --- | --------- | ----- | ----------- |
|           |      |                  |          |        |      |     | Table3.4: | Power | consumption |
| Mode      |      | Condition        |          |        | Min. |     | Typical   | Max.  | Unit        |
| WiFi      | Scan | USB powered      |          |        |      |     | 119       |       | mA          |
| WiFi      | AP   | USB powered      |          |        |      |     | 170       |       | mA          |
| BT        |      | USB powered      |          |        |      |     | 115       |       | mA          |
|           |      | 14dBm,USB        | powered, | 868 TX |      |     | 290       |       | mA          |
|           |      | 17dBm,USB        | powered, | 868 TX |      |     | 330       |       | mA          |
TX
|     |     | 22dBm,USB    | powered, | 868 TX  |     |     | 400 |     | mA  |
| --- | --- | ------------ | -------- | ------- | --- | --- | --- | --- | --- |
|     |     | 27dBm,USB    | powered, | 868 TX  |     |     | 750 |     | mA  |
| RX  |     | TX disabled; | RX       | enabled |     |     | 75  |     | mA  |
|     |     | USB powered  |          |         |     |     | 2   |     | mA  |
μ
| Sleep |     | Battery | powered |     |     |     | 20  |     | A   |
| ----- | --- | ------- | ------- | --- | --- | --- | --- | --- | --- |
μ
|                |     | 3.3V header     | powered |     |     |     | 20  |     | A   |
| -------------- | --- | --------------- | ------- | --- | --- | --- | --- | --- | --- |
| Charging       |     | USB@5V          |         |     |     |     | 500 |     | mA  |
| 3.5 LoRa       | RF  | Characteristics |         |     |     |     |     |     |     |
| 3.5.1 Transmit |     | Power           |         |     |     |     |     |     |     |
Table3.5.1:Transmitpower
|     | Module       |     | Frequency | Band[MHz] |     | Maximum | power[dBm] |     |     |
| --- | ------------ | --- | --------- | --------- | --- | ------- | ---------- | --- | --- |
|     | HTIT-WB32LAF |     |           | 863-928   |     |         | 28±        | 1   |     |
21±1
| HTIT-WB32LAF-N |     |     | 433/470-510/863-928 |     |     |     |     |     |     |
| -------------- | --- | --- | ------------------- | --- | --- | --- | --- | --- | --- |
Documents Rev1.4 P13/17 Sep.2025 HelTecAutomation©Limitedstandardfileshttps://heltec.org

| 3.5.2 | Receiving |       | Sensitivity     |     |             |        |     |     |
| ----- | --------- | ----- | --------------- | --- | ----------- | ------ | --- | --- |
| The   | following | table | gives typically |     | sensitivity | level. |     |     |
Table3.5.2: Receivingsensitivity
|     | Signal  | Bandwidth/[KHz] |           |     | Spreading | Factor | Sensitivity/[dBm] |      |
| --- | ------- | --------------- | --------- | --- | --------- | ------ | ----------------- | ---- |
|     |         |                 | 125       |     |           | SF12   |                   | -137 |
|     |         |                 | 125       |     |           | SF10   |                   | -131 |
|     |         |                 | 125       |     |           | SF7    |                   | -122 |
| 3.6 | LoRaWAN |                 | Frequency |     | Plan      |        |                   |      |
Supports LoRaWAN frequency channels and models corresponding table.
Table3.6:FrequencyPlan
|     | Region |       | Frequency(MHz) |         |     |                                | Model             |     |
| --- | ------ | ----- | -------------- | ------- | --- | ------------------------------ | ----------------- | --- |
|     |        | IN868 |                | 865~867 |     | HTIT-WB32LAF/HTIT-WB32LAF-N-HF |                   |     |
|     |        | EU868 |                | 863~870 |     | HTIT-WB32LAF/HTIT-WB32LAF-N-HF |                   |     |
|     |        | US915 |                | 902~928 |     | HTIT-WB32LAF/HTIT-WB32LAF-N-HF |                   |     |
|     |        | AU915 |                | 915~928 |     | HTIT-WB32LAF/HTIT-WB32LAF-N-HF |                   |     |
|     |        | KR920 |                | 920~923 |     | HTIT-WB32LAF/HTIT-WB32LAF-N-HF |                   |     |
|     |        | AS923 |                | 920~925 |     | HTIT-WB32LAF/HTIT-WB32LAF-N-HF |                   |     |
|     |        | CN470 |                | 470~510 |     |                                | HTIT-WB32LAF-N-LF |     |
Documents Rev1.4 P14/17 Sep.2025 HelTecAutomation©Limitedstandardfileshttps://heltec.org

4. Schematic
(V4.2)

5.Dimensions
Documents Rev1.4 P16/17 August,2025 Heltec Automation © Limited standard files

6.Resource
| 6.1 Relevant | Resource |     |     |     |
| ------------ | -------- | --- | --- | --- |
 User Guide
 SDK
| Heltec ESP   | (ESP32  | & ESP8266) | framework |     |
| ------------ | ------- | ---------- | --------- | --- |
| Heltec ESP32 | library |            |           |     |
 Schematic diagram

| Downloadable | resource    |                   |                   |          |
| ------------ | ----------- | ----------------- | ----------------- | -------- |
| 6.2 Contact  | Information |                   |                   |          |
|              |             | Heltec Automation | Technology        | Co., Ltd |
|              |             | Chengdu,          | Sichuan, China    |          |
|              |             | Email:            | support@heltec.cn |          |
|              |             | Phone:            | +86-028-62374838  |          |
https://heltec.org
Documents Rev4.2.0 P17/17 Sep.2025 HelTecAutomation©Limitedstandardfileshttps://heltec.org
