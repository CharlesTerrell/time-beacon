| Wireless | Tracker | V2  |
| -------- | ------- | --- |
WiFi/BLE/LoRa/GNSS
| Development Board |     |     |
| ----------------- | --- | --- |
https://heltec.org
Documents Rev1.1 P1/16 Mar.2026 Heltec Automation © Limited standard files

| Document | Version  |             |        |
| -------- | -------- | ----------- | ------ |
| Version  | Time     | Description | Remark |
| V2.3     | 2025-9-5 |             | Ashley |
TXpowerupgradedfrom21±1dBm
to28±1dBm.
https://heltec.org
Documents Rev1.1 P2/16 Mar.2026 HeltecAutomation©Limitedstandardfiles

Copyright Notice
All contents in the files are protected by copyright law, and all copyrights are reserved by Chengdu
Heltec Automation Technology Co., Ltd. (hereinafter referred to as Heltec). Without written
permission, all commercial use of the files from Heltec are forbidden, such as copy, distribute,
reproduce the files, etc., but non-commercial purpose, downloaded or printed by individual are
welcome.
Disclaimer
Chengdu Heltec Automation Technology Co., Ltd. reserves the right to change, modify or improve
Theseinstructionsareintendedforyouuse.
https://heltec.org
Documents Rev1.1 P3/16 Mar.2026 HeltecAutomation©Limitedstandardfiles

https://heltec.org
Documents Rev1.1 P4/16 Mar.2026 HeltecAutomation©Limitedstandardfiles
1.Description
1.1 Overview
Wireless Tracker V2 integrates the Semtech SX1262 LoRa transceiver and UC6580 GNSS module,
powered by the high-performance ESP32-S3FN8 platform to deliver reliable long-range
communicationandhigh-precisionpositioninginacompact,developer-friendlyIoTsolution.
WithitsmaximumTXpowerincreasedto28±1dBm(upgradedfrom21±1dBm),combinedwithan
upgraded RF front-end architecture and optimized PCB layout, Wireless Tracker V2 achieves
extended communication range, enhanced signal penetration, improved link robustness, and
greater overall system stability—making it ideal for demanding and large-scale tracking
deployments.
https://heltec.org
Documents Rev1.1 P5/16 Mar.2026 HeltecAutomation©Limitedstandardfiles
1.2 Upgrade and Enhancement
 Maximum TX Power: Increased from 21 ± 1 dBm to 28 ± 1 dBm, enabling extended
communicationrangeandimprovedlinkreliability.
 FEMUpgrade: Replaced theRF front-endmodule tosupporthigher output power andenhance
transmissionefficiencyandreceiversensitivity.
 RFFilterAdded:AddedanRFfiltertoimproveEMCperformance.
 GNSS Active Antenna Upgrade: Enhanced with an upgraded low-noise amplifier (LNA) to
achievestrongersignalreceptionandhigherpositioningaccuracy.
 SX1262GroundOptimization:ImplementedfullgroundshieldingaroundtheSX1262toimprove
RFstabilityandreduceinterference.
 Ground Structure Redesign: Adopted a unified ground plane to optimize return paths and
enhanceanti-interferenceperformance.
 LoRa Pin Layout Optimization: Adjusted GPIO33 and GPIO34 routing to reduce layer crossover
andminimizesignalinterferenceneartheLoRadriverpins.
 Power Layout Optimization: Improved power trace routing and minimized adjacent-layer
crossovertoenhancesystemstability.
 Header Footprint Update: Reduced the package size of the last four header pins and updated
thePCBfootprinttoimprovemanufacturability.
 Display Protection: Equipped with a dedicated plastic bracket to provide additional mechanical
protectionforthescreenduringoperation.
 Solar Interface Added: Newly added solar power interface to support sustainable outdoor
operationandextendedbatterylife.
https://heltec.org
Documents Rev1.1 P6/16 Mar.2026 HeltecAutomation©Limitedstandardfiles
1.3 Product Features
 ESP32-S3FN8+SX1262+UC6580Chipset,supportsWi-Fi,LoRa,Bluetooth,GNSS.
 High-powerversionwithLoRatransmissionpowerincreasedto28±1dBm.
 Lowpowerdesignofdual-frequencymulti-systembasedon22nmtechnology.
 Supports L1, supports GPS, GLONASS, BDS, Galileo, NAVIC, QZSS multi-system joint
positioning.1
 Type-CUSBinterfacewithacompletevoltageregulator,ESDprotection,shortcircuitprotection,
RFshielding,andotherprotectionmeasures.
 Onboard SH1.25-2 lithium battery and solar interfaces with integrated management for
charging,overchargeprotection,powermonitoring,andautomaticUSB/batteryswitching.
 Onboard Wi-Fi, Bluetooth dedicated 2.4GHz metal spring antenna, reserved IPEX (U.FL)
interfaceforLoRaandGNSSuse.
 Onboard 0.96-inch 80×160 RGB TFT-LCD for displaying debugging, battery status, and more,
equippedwithadedicatedplasticbracketforaddedprotection.
 The GNSS active antenna is enhanced with an upgraded low-noise amplifier (LNA), providing
strongersignalreceptionandimprovedpositioningaccuracy.
1 SeetheGNSSmodulemanualfordetailsonsupportedprojects.
https://heltec.org
Documents Rev1.1 P7/16 Mar.2026 HeltecAutomation©Limitedstandardfiles
2.Pin Definition
2.1 Pin Assignment
https://heltec.org
Documents Rev1.1 P8/16 Mar.2026 HeltecAutomation©Limitedstandardfiles
| 2.2 Pin | Interface |     |     |
| ------- | --------- | --- | --- |
2.2.1 HeaderJ1
Table2-2-1:Pindescription
| NO. | NAME | TYPE | FUNCTION |
| --- | ---- | ---- | -------- |
| 1   |      | P    | Ground   |
GND
| 2   |     | P   | PowerSupply5V |
| --- | --- | --- | ------------- |
5V
| 3   |     | P   | Ground |
| --- | --- | --- | ------ |
GND
| 4   |     | P   | Powersupplyforbuilt-inTFTandGNSS,3.3V |
| --- | --- | --- | ------------------------------------- |
Vext
| 5   |     | P   | Ground |
| --- | --- | --- | ------ |
GND
| 6   |     | P   | Powersupplyforbuilt-inTFTandGNSS,3.3V |
| --- | --- | --- | ------------------------------------- |
Vext
| 7   | GPIO15 | I/O | U0RTS,ADC2_CH4,XTAL_32K_P              |
| --- | ------ | --- | -------------------------------------- |
| 8   | GPIO16 | I/O | U0CTS,ADC2_CH5,XTAL_32K_N              |
| 9   | GPIO19 | I/O | CLK_OUT2,ADC2_CH8,U1RTS,USB_N          |
| 10  | GPIO20 | I/O | CLK_OUT1,ADC2_CH9,U1CTS,USB_P          |
| 11  | GPIO18 | I/O | U1RXD,ADC2_CH7,CLK_OUT3,LED_Write_Ctrl |
| 12  | GPIO17 | I/O | U1TXD,ADC2_CH6                         |
| 13  | GPIO35 | I/O | SPI06,FSPID,SUBSPID,GNSS_RST           |
| 14  | GPIO36 | I/O | SPI07,FSPICLK,SUBSPICLK,PPS            |
| 15  |        | I/O | SPIDQS,FSPIQ,SUBSPIQ                   |
GPIO37
| 16  | GPIO21 | I/O | TFT_LED_K |
| --- | ------ | --- | --------- |
| 17  | GPIO48 | I/O | XTAL_P    |
| 18  |        | I/O | XTAL_N    |
GPIO47
| 19  | GPIO33 | I/O | SPIIO4,FSPIHD,SUBSPIHD,GNSS_TX  |
| --- | ------ | --- | ------------------------------- |
| 20  | GPIO34 |     | SPI05,FSPICS0,SUBSPICS0,GNSS_RX |
I/O
https://heltec.org
Documents Rev1.1 P9/16 Mar.2026 HeltecAutomation©Limitedstandardfiles
| 2.2.2 Header | J2  |     |     |
| ------------ | --- | --- | --- |
Table2-2-2:Pindescription
| NO. | NAME | TYPE | FUNCTION |
| --- | ---- | ---- | -------- |
Ground
| 1   | GND    | P   |                           |
| --- | ------ | --- | ------------------------- |
| 2   | 3V3    | P   | Output3.3V                |
| 3   | GPIO7  | I/O | ADC1_CH6,TOUCH7           |
| 4   | GPIO6  | I/O | ADC1_CH5,TOUCH6           |
| 5   | GPIO5  | I/O | ADC1_CH4,TOUCH5           |
| 6   | GPIO4  | I/O | ADC1_CH3,TOUCH4,PA_CSD    |
| 7   | GPIO3  | I/O | ADC1_CH2,TOUCH3,Vext_Ctrl |
| 8   | GPIO2  | I/O | ADC1_CH1,TOUCH2,ADC_Ctrl  |
| 9   | GPIO1  | I/O | ADC1_CH0,TOUCH1,Vbat_Read |
| 10  | GPIO0  | I/O | USER_SW                   |
| 11  | RST    | I/O | RST_SW                    |
| 12  | GPIO38 | I/O | FSPIWP,SUBSPIWP,TFT_CS    |
| 13  | GPIO41 | I/O | MTDI,TFT_SCLK             |
| 14  | GPIO42 | I/O | MTMS,TFT_SDIN             |
| 15  | GPIO40 | I/O | MTMS,TFT_RS               |
| 16  | GPIO39 | I/O | MTMS,TFT_RES              |
| 17  | GPIO46 | I/O | PA_CPS                    |
| 18  | GPIO45 | I/O |                           |
| 19  | GPIO44 | I/O | U0RXD                     |
| 20  | GPIO43 | I/O | U0TXD                     |
https://heltec.org
Documents Rev1.1 P10/16 Mar.2026 HeltecAutomation©Limitedstandardfiles
3.Specifications
3.1 General Specifications
Table3-1:Generalspecifications
Parameters Description
MCU ESP32-S3FN8(Xtensa®32-bitLX7dual-core
processor)
LoRaChip SX1262
GNSSChip UC6580
ChargingIC CN3165
Frequency 863~928MHz
MaxTXPower 28±1dBm
Receivingsensitivity -135dBm
Wi-Fi IEEE802.11b/g/n
Bluetooth Bluetooth5.0,BluetoothLE,BluetoothMesh
USBType-C;1.25mmLithiumBattery
Connector;1.25mmSolarPanelConnector;
Interface
LoRaAntenna(IPEX/U.FL);GNSSAntenna(IPEX
/U.FL)
Battery Supports3.7VLithiumBatteryandSolarPanel
Operatingtemperature -20~70℃
Dimensions 53.00mm*25.40mm*9.37mm
https://heltec.org
Documents Rev1.1 P11/16 Mar.2026 HeltecAutomation©Limitedstandardfiles
| 3.2 | Power Supply |     |     |     |     |     |
| --- | ------------ | --- | --- | --- | --- | --- |
The Lithium battery can be charged when USB or 5V is not connected. Only one power source
shouldbeusedatatime.
Table3-2:PowerSupply
|     |     | Minimum |     | Typical | Maximum | Unit |
| --- | --- | ------- | --- | ------- | ------- | ---- |
PowerSupplyMode
| USBType-C(≥500mA)      |                | 4.7 |     | 5   | 6   | V   |
| ---------------------- | -------------- | --- | --- | --- | --- | --- |
| LithiumBattery(≥250ma) |                | 3.3 |     | 3.7 | 4.2 | V   |
|                        | 5VPin(≥500ma)  | 4.7 |     | 5   | 6   | V   |
|                        | 3V3Pin(≥150ma) | 2.7 |     | 3.3 | 3.5 | V   |
| 3.3                    | Power Output   |     |     |     |     |     |
Table3-3:PowerOutput
|                       | OutputPin |     | Current |     |     | Unit |
| --------------------- | --------- | --- | ------- | --- | --- | ---- |
|                       | 3v3Pin    |     |         | 500 |     | mA   |
| 5VPin(USBPoweredOnly) |           |     |         | 500 |     | mA   |
|                       | VextPin   |     |         | 350 |     | mA   |
https://heltec.org
Documents Rev1.1 P12/16 Mar.2026 HeltecAutomation©Limitedstandardfiles
| 3.4 Power | Characteristics |     |     |     |     |     |
| --------- | --------------- | --- | --- | --- | --- | --- |
Table3-4:PowerCharacteristics
VBAT/battery
|     | Mode |     | USB power |     |     |     |
| --- | ---- | --- | --------- | --- | --- | --- |
Unit
powered
|          | Wi-FiScan |             | 79    | 75  |      | mA  |
| -------- | --------- | ----------- | ----- | --- | ---- | --- |
|          | Wi-FiAP   |             | 140   | 139 |      | mA  |
|          |           |             |       | 64  |      | mA  |
|          | BT        |             | 66    |     |      |     |
|          | GNSS      |             | 130   | 127 |      | mA  |
|          | Sleep     |             | 1.2mA |     | 21μA |     |
| 3.5 LoRa | Receiving | Sensitivity |       |     |      |     |
ThefollowingtablegivestypicallysensitivityleveloftheWirelessTrackerr-(L/H).
Table3-5:ReceivingSensitivity
| Signalbandwidth/[kHz] |     | Spreadingfactor |      | Sensitivity/[dBm] |      |     |
| --------------------- | --- | --------------- | ---- | ----------------- | ---- | --- |
|                       | 125 |                 | SF12 |                   | -135 |     |
|                       | 125 |                 | SF10 |                   | -130 |     |
|                       | 125 |                 | SF7  |                   | -124 |     |
https://heltec.org
Documents Rev1.1 P13/16 Mar.2026 HeltecAutomation©Limitedstandardfiles
3.6 Operation Frequencies
WirelessTrackersupportsLoRaWANfrequencychannelsandmodelscorrespondingtable.
Table3-6:OperationFrequencies
| Region | Frequency(MHz) | Model              |
| ------ | -------------- | ------------------ |
| IN868  | 865~867        | WirelessTracker-HF |
| EU868  | 863~870        | WirelessTracker-HF |
| US915  | 902~928        | WirelessTracker-HF |
| AU915  | 915~928        | WirelessTracker-HF |
| KR920  | 920~923        | WirelessTracker-HF |
| AS923  | 920~925        | WirelessTracker-HF |
3.7 GNSS Characteristics
BasicInformationoftheGNSSModule
Table3-7:Basicinformation
Channel 96
Updatefrequency Max10Hz
| Dataformat | NMEA-0183，Unicore，RTCM3.x |     |
| ---------- | ------------------------- | --- |
BDS:B2a
GPS:L1
GLONASS:G1
Frequencypoint Galileo:E1+E5a
QZSS:L1
SBAS:L1
https://heltec.org
Documents Rev1.1 P14/16 Mar.2026 HeltecAutomation©Limitedstandardfiles
| 4.Mechanical | Specification | (mm) |
| ------------ | ------------- | ---- |
https://heltec.org
Documents Rev1.1 P15/16 Mar.2026 HeltecAutomation©Limitedstandardfiles
5.Resource
5.1 Relevant Resource
 Heltec_ESP32Framework
 HeltecLoRaWANtestserverbasedonSnapEmu
 UserGuide

GNSSDatasheet
 DisplayDatasheet
5.2 Contact Information
| Heltec   | Automation              | Technology               | Co., Ltd |
| -------- | ----------------------- | ------------------------ | -------- |
| Chengdu, | Sichuan, China          | Email: support@heltec.cn |          |
|          | Phone: +86-028-62374838 |                          |          |
https://heltec.org
https://heltec.org
Documents Rev1.1 P16/16 Mar.2026 HeltecAutomation©Limitedstandardfiles
