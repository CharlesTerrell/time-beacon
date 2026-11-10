`
| Mesh       | Node |             | T096 |
| ---------- | ---- | ----------- | ---- |
| Bluetooth, | LoRa | & GNSS,0.96 | TFT  |
https://heltec.org
Documents Rev.1.0 P1/12 May2023 Heltec Automation © Limited standard files

| Document  | Version   |             |         |
| --------- | --------- | ----------- | ------- |
| Version   | Time      | Description | Remark  |
| Rev.1.0.0 | 2026-3-25 |             | Richard |
| Copyright | Notice    |             |         |
All contents in the files are protected by copyright law, and all copyrights are
reservedbyChengduHeltecAutomationTechnologyCo.,Ltd.(hereinafterreferredto
as Heltec). Without written permission, all commercial use of the files from Heltec
are forbidden, such ascopy, distribute, reproduce the files, etc., but non-commercial
purpose,downloadedorprintedbyindividualarewelcome.
Disclaimer
Chengdu Heltec Automation Technology Co., Ltd. reserves the right to change,
modify or improve the document and product described herein. Its contents are
subjecttochangewithoutnotice.Theseinstructionsareintendedforyouuse.
https://heltec.org
Documents Rev.1.0 P2/12 May2023 Heltec Automation © Limited standard files

Content
MeshNodeT096................................................................................................................................................1
1 Description..........................................................................................................................................4
1.1 Overview...................................................................................................................................................4
1.2 Product Features...................................................................................................................................4
1.3 Typical Application Scenarios...........................................................................................................4
2 Pin Definition.....................................................................................................................................5
2.1 P2................................................................................................................................................................6
2.2 P3...............................................................................................................................................................7
2.3 Unroutedpins........................................................................................................................................8
3 Specifications.....................................................................................................................................9
3.1 GeneralSpecification...........................................................................................................................9
3.2 PowerConsumption.............................................................................................................................9
3.3 TransmitPower...................................................................................................................................10
3.4 Operation Frequencies......................................................................................................................10
4 Physical Dimensions......................................................................................................................11
5 Resource............................................................................................................................................11
6 Heltec Contact Information........................................................................................................12
https://heltec.org
Documents Rev.1.0 P3/12 May2023 Heltec Automation © Limited standard files

1 Description
1.1 Overview
Mesh Node T096 is a low-power development board that integrates Bluetooth, LoRa, and GNSS. At its core, the
nRF52840 MCU provides ample processing power and Bluetooth connectivity, while the SX1262 LoRa
transceiver—integrated with PA—delivers transmit power of 28±1 dBm, ensuring exceptional range and link reliability.
The UC6580 GNSS chip enables precise global positioning, making the device ideal for decentralized mesh networks,
assettracking, andremotesensing.
UtilizingLaserDirectStructuring(LDS)technologyforbothGNSSandBluetoothantennas,theT096achievesacompact,
durable, and highly efficient antenna solution. Engineered for low-power operation, it is perfectly suited for
battery-powered deployments. Fully supported by the Arduino development environment and compatible with
open-sourcemeshprojectssuchasMeshtasticandMeshCore,theMeshNodeT096offersdevelopersandenthusiastsa
versatile,ready-to-useplatformforbuildingresilient,scalablemeshnetworks.
1.2 Product Features
 Based on nRF52840 (Bluetooth), SX1262(LoRa), and  CN3165 lithium battery charge management IC
UC6580(GNSS). features built-in adaptive current regulation for
 Lowpowerconsumption,13μAindeepsleep. efficientandsafecharging.
 Onboard0.96inchTFT-LCDdisplaycontains  Various Interfaces: 2*1.25 LiPo connector, 2*1.25
80(H)RGBx160(V)dots. Solarpanelconnector.
 Type-C USB interface with a complete voltage  Compatible with Arduino,and we provide Arduino
regulator,ESDprotection,shortcircuitprotection,RF developmentframeworksandlibraries.
shielding,andotherprotectionmeasures.
1.3 Typical Application Scenarios
 DecentralizedOutdoorMeshCommunication  EnvironmentalMonitoringandRemoteSensing
 AssetTrackingandPositionMonitoring  Open-SourceMeshExperimentationandEducation
 EmergencyandDisasterCommunicationNetworks  PortablePersonalCommunicationDevice
https://heltec.org
Documents Rev.1.0 P4/12 May2023 Heltec Automation © Limited standard files

2 Pin Definition
About SPI, I2C, UART, PWM, QPSI, I2S, PDM, QDEC: Each IO pin is preconfigured with IO_MUX functionality,
whichmeans youcan useanyGPIOas peripheralpins.Youjustneedtodefinethesepins inyourcode.
However,it's important to note that once a pin is used, it may affect other functions associated with that pin. For
information,please refertotheSchemaicdiagram.
https://heltec.org
Documents Rev.1.0 P5/12 May2023 Heltec Automation © Limited standard files

P2
2.1
| PHY | Name   | Type | Description      |
| --- | ------ | ---- | ---------------- |
| 1   | GND    | P    | Ground           |
| 2   | 5V     | P    | 5VPower          |
| 3   | GND    | P    | Ground           |
| 4   | Vext   | P    | 3V3output        |
| 5   | GND    | P    | Ground           |
| 6   | Vext   | P    | 3V3output        |
| 7   | P0.07  | I/O  | GPIO0.07         |
| 8   | P0.08  | I/O  | GPIO0.08         |
| 9   | SWDCLK | I/O  | SWDClock         |
| 10  | SWDIO  | I/O  | SWDData          |
| 11  | P0.27  | I/O  | GPIO0.27         |
| 12  | P0.04  | I/O  | GPIO0.04,ADC2    |
| 13  | P1.03  | I/O  | GPIO1.03         |
| 14  | P1.05  | I/O  | GPIO1.05         |
| 15  | P1.07  | I/O  | GPIO1.07         |
| 16  | P1.06  | I/O  | GPIO1.06         |
| 17  | P1.02  | I/O  | GPIO1.02         |
| 18  | P1.04  | I/O  | GPIO1.04         |
| 19  | P0.23  | I/O  | GPIO0.23,GNSS_TX |
| 20  | P0.25  | I/O  | GPIO0.25,GNSS_RX |
https://heltec.org
Documents Rev.1.0 P6/12 May2023 Heltec Automation © Limited standard files

2.2P3
| PHY | Name  | Type | Description            |         |
| --- | ----- | ---- | ---------------------- | ------- |
| 1   | GND   | P    | Ground                 |         |
| 2   | VDD   | P    | VDD_nRF                |         |
| 3   | P0.31 | I/O  | GPIO0.31,ADC7          |         |
| 4   | P0.29 | I/O  | GPIO0.29,ADC6          |         |
| 5   | P0.02 | I/O  | GPIO0.02,ADC0          |         |
| 6   | P1.15 | I/O  | GPIO1.15,ADC_Ctrl(high | enable) |
| 7   | P1.13 | I/O  | GPIO1.13               |         |
| 8   | P0.10 | I/O  | GPIO0.10               |         |
| 9   | P0.09 | I/O  | GPIO0.09               |         |
| 10  | P1.10 | I/O  | GPIO1.10,UserButton    |         |
| 11  | P0.18 | I/O  | GPIO0.18,Reset         |         |
| 12  | P0.22 | I/O  | GPIO0.22,CS            |         |
| 13  | P1.11 | I/O  | GPIO1.11,PPS           |         |
| 14  | P0.20 | I/O  | GPIO0.20,SCLK          |         |
| 15  | P0.17 | I/O  | GPIO0.17,SDIN          |         |
| 16  | P0.15 | I/O  | GPIO0.15,RS            |         |
| 17  | P0.13 | I/O  | GPIO0.13,RES           |         |
| 18  | P1.01 | I/O  | GPIO1.01               |         |
| 19  | P1.00 | I/O  | GPIO1.00               |         |
| 20  | P0.24 | I/O  | GPIO0.24               |         |
https://heltec.org
Documents Rev.1.0 P7/12 May2023 HeltecAutomation©Limitedstandardfiles

2.3Unrouted pins
| Name  | Number   | Function      |         |
| ----- | -------- | ------------- | ------- |
| P0.26 | GPIO0.26 | VextCtrl(high | enable) |
| P0.03 | GPIO0.03 | ADCIN         |         |
| P0.21 | GPIO0.21 | DIO1          |         |
| P0.05 | GPIO0.05 | LoRaNSS       |         |
| P1.08 | GPIO1.08 | LoRa_SCK      |         |
| P0.11 | GPIO0.11 | LoRa_MOSI     |         |
| P0.14 | GPIO0.14 | LoRa_MISO     |         |
| P0.16 | GPIO0.16 |               |         |
LoRa_RST
| P0.19 | GPIO0.19 | LoRa_BUSY      |         |
| ----- | -------- | -------------- | ------- |
| P0.06 | GPIO0.06 | VGNSS_Ctrl(low | enable) |
| P1.14 | GPIO1.14 | GNSS_RST       |         |
| P0.30 | GPIO0.30 | FEM_Ctrl       |         |
| P0.12 | GPIO0.12 | PA_CSD         |         |
| P0.28 | GPIO0.28 | WhiteLED       |         |
| P1.12 | GPIO1.12 | DisplayLED     |         |
| P1.09 | GPIO1.09 | PACTX          |         |
https://heltec.org
Documents Rev.1.0 P8/12 May2023 HeltecAutomation©Limitedstandardfiles

3 Specifications
3.1 General Specification
Table3.1:Generalspecification
Parameters Description
MCU nRF52840
LoRaChipset SX1262
GNSS UC6580
Memory 1MROM;256KBSRAM
Bluetooth Bluetooth5,Bluetoothmesh,BLE.
Storagetemperature -30~80℃
Operatingtemperature -20~70℃
OperatingHumidity 90%(Nocondensing)
PowerSupply 3~5.5V(USB),3~4.2(Battery)
Display ST7735S,0.96Inch,80(H)RGBx160(V)
ActiveArea 10.8mm(H)×21.7(V)mm
USB 2.0, 2x LEDs, 2 xButtons,4 x SPI, 2x TWI, 2 x UART,4 x PWM, QSPI, I2S,
HardwareResource
PDM,QDECEtc.
Type-CUSB,1.25-2Plithiumbatteryconnector,1.25-2Psolarpanelconnector,
Interface
LoRaANT(IPEX1.0),GNSSANT(IPEX1.0),2x13x2.54HeaderPins
Dimensions 52*25.4*10.2mm
3.2Power Consumption
Table3.2:Workingcurrent
Consumption@3.7V_BAT
Mode Condition
470MHz 868MHz 915MHz
10dBm 152mA 152mA
LoRa_TX
16dBm 234mA 237mA
https://heltec.org
Documents Rev.1.0 P9/12 May2023 HeltecAutomation©Limitedstandardfiles

|     | 22dBm | 407mA | 415mA |
| --- | ----- | ----- | ----- |
|     | 29dBm | 776mA | 765mA |
|     | UART  | 10mA  |       |
BT
|       | Scan  | 2.6mA |     |
| ----- | ----- | ----- | --- |
| Sleep |       | 13uA  |     |
| GNSS  |       | 40mA  |     |
|       | Solar | 500mA |     |
Charging
|     | USB-C | 500mA |     |
| --- | ----- | ----- | --- |
3.3Transmit Power
Table3.3:Transmitpower
Operatingfrequencyband(MHz) Maximumpowervalue/[dBm]
| 863~870 |     | 28±1 |     |
| ------- | --- | ---- | --- |
| 902~928 |     | 28±1 |     |
3.4Operation Frequencies
Table3.4:OperationFrequencies
| Region | Frequency(MHz) | Model        |     |
| ------ | -------------- | ------------ | --- |
| IN868  | 865~867        | HT-n5262G-HF |     |
| EU868  | 863~870        | HT-n5262G-HF |     |
| US915  | 902~928        | HT-n5262G-HF |     |
| AU915  | 915~928        | HT-n5262G-HF |     |
| KR920  | 920~923        | HT-n5262G-HF |     |
| AS923  | 920~925        | HT-n5262G-HF |     |
https://heltec.org
Documents Rev.1.0 P10/12 May2023 HeltecAutomation©Limitedstandardfiles

4 Physical Dimensions
5 Resource
MeshNodeT096SchematicDiagram
SDK:HeltecnRF52frameworkandLib
SnapEmuLoRaWANIoTPlatform
MeshNodeT096ManualDocument
TFT-LCDDatasheet
GNSSDocuments
https://heltec.org
Documents Rev.1.0 P11/12 May2023 HeltecAutomation©Limitedstandardfiles

6 Heltec Contact Information
HeltecAutomationTechnologyCo.,Ltd
Chengdu,Sichuan,China
Email:support@heltec.cn
Phone:+86-028-62374838
https://heltec.org
https://heltec.org
Documents Rev.1.0 P12/12 May2023 HeltecAutomation©Limitedstandardfiles
