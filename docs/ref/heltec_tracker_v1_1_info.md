| Wireless | Tracker | V1.1 |
| -------- | ------- | ---- |
Wi-Fi/LoRa/BLE/GNSS(L5)
| Development | Kit |     |
| ----------- | --- | --- |
https://heltec.org
Documents Rev1.1 P1/17 May2023 Heltec Automation © Limited standard files

| Document | version    |                         |         |
| -------- | ---------- | ----------------------- | ------- |
| Version  | Time       | Description             | Remark  |
| V1.0     | 2023-05-16 | Documentscreating       | Richard |
| V1.1     | 2023-05-21 | Documentstructureupdate | Richard |
V1.1.1 2024-10-10 UpdatetheGNSSmoduleparameterdescription Richard
| Copyright | Notice |     |     |
| --------- | ------ | --- | --- |
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
Documents Rev1.1 P2/17 May2023 HeltecAutomation©Limitedstandardfiles

Content
WirelessTracker...................................................................................................................................1
Documentversion.................................................................................................................................2
CopyrightNotice...................................................................................................................................2
Disclaimer.............................................................................................................................................2
Content..................................................................................................................................................3
1.Description........................................................................................................................................4
1.1Overview....................................................................................................................................4
1.2Productfeatures.........................................................................................................................5
2.PinDefinition.....................................................................................................................................6
2.1Pinassignment...........................................................................................................................6
2.2Pindescription............................................................................................................................7
3.Specifications..................................................................................................................................10
3.1Generalspecifications..............................................................................................................10
3.2Powersupply............................................................................................................................11
3.3Poweroutput............................................................................................................................11
3.4Powercharacteristics...............................................................................................................12
3.5LoRaRFcharacteristics............................................................................................................12
3.6GNSSCharacteristics................................................................................................................14
4.Typicalhardwareconnections........................................................................错误！未定义书签。
4.1Physicaldimensions.................................................................................................................16
5.Resource..........................................................................................................................................16
5.1RelevantResource....................................................................................................................16
5.2ContactInformation.................................................................................................................16
https://heltec.org
Documents Rev1.1 P3/17 May2023 HeltecAutomation©Limitedstandardfiles

1. Description
1.1Overview
WirelessTrackerisadevelopment kitbased onESP32-S3FN8.It integratesbothSX1262 and
UC6580 toprovidefastGNSS solutionforIoT.Collaboratewiththesample programsand
developmenttoolsweprovide,youcantrackanyobjectandthenuploadthatdatawirelessly
| byWi-Fi,Bluetooth, | LoRa. |     |     |     |     |     |
| ------------------ | ----- | --- | --- | --- | --- | --- |
WirelessTrackersupportsL1 +L5 andsupportsGPS,GLONASS,BDS,Galileo,NAVIC,QZSS
multi-systemjointpositioning.It isperfectlycompatible withArduino,can bewidelyused in
developmentsuch asbicyclesharing services,tracking petsorlivestock,locating vehicles,
tracking children,etc.
WirelessTrackerareavailableintwoproductvariants:
Table1.1:Productmodellist
| No. | Model | Description |              |            |          |       |
| --- | ----- | ----------- | ------------ | ---------- | -------- | ----- |
|     |       | 470~510MHz  | working LoRa | frequency, | used for | China |
1 WirelessTracker-LF
|     |     | mainland   | (CN470)LPW band. |        |              |     |
| --- | --- | ---------- | ---------------- | ------ | ------------ | --- |
|     |     | For EU868, | IN865, US915,    | AU915, | AS923, KR920 | and |
2 WirelessTracker-HF other LPW networks with operating frequencies between
863~928MHz.
https://heltec.org
Documents Rev1.1 P4/17 May2023 HeltecAutomation©Limitedstandardfiles

| 1.2Product | features |     |
| ---------- | -------- | --- |
 ESP32-S3FN8+SX1262+UC6580Chipset,supportsWi-Fi,LoRa,Bluetooth,GNSS.
 Low powerdesign ofdual-frequencymulti-systembased on22nm technology.
 Supports L1 + L5, supports GPS, GLONASS, BDS, Galileo, NAVIC, QZSS multi-system
jointpositioning.1
 Type-C USB interface with a complete voltage regulator, ESD protection, short circuit
protection,RFshielding,andotherprotection measures.
 Onboard SH1.25-2 battery interface, integrated lithium battery management system
(charge and discharge management, overcharge protection, battery power detection,
| USB/batterypowerautomatic |     | switching). |
| ------------------------- | --- | ----------- |
 Onboard Wi-Fi, Bluetooth dedicated 2.4GHz metal spring antenna,reserved IPEX (U.FL)
| interfaceforLoRa | andGNSS | use. |
| ---------------- | ------- | ---- |
 Onboard 0.96-inch 80(H) x 160(V) RGB TFT-LCD display, which can be used to display
| debugging | information,battery | power,andotherinformation. |
| --------- | ------------------- | -------------------------- |

| SupporttheArduinodevelopment |     | environment. |
| ---------------------------- | --- | ------------ |
1 SeetheGNSSmodulemanualfordetailsonsupportedprojects:UFirebird_StandardPositioningProducts
ProtocolSpecification
https://heltec.org
Documents Rev1.1 P5/17 May2023 HeltecAutomation©Limitedstandardfiles

2. Pin Definition
2.1Pin assignment
https://heltec.org
Documents Rev1.1 P6/17 May2023 HeltecAutomation©Limitedstandardfiles

2.2Pin description
HeaderJ2
Table2-2-1:Pindescription
| No. Name | Type Function   |      |     |     |
| -------- | --------------- | ---- | --- | --- |
| 1 5V     | P 5VPowerSupply |      |     |     |
| 2 GND    | P Ground        |      |     |     |
| 3 3V3    | P Output        | 3.3V |     |     |
| 4 GND    | P Ground        |      |     |     |
| 5 3V3    | P Output        | 3.3V |     |     |
GND
| 6      | P Ground                             |                      |             |             |
| ------ | ------------------------------------ | -------------------- | ----------- | ----------- |
| 7 Vext | P Output                             | 3.3V,powersupply     | forbuilt-in | TFTand GNSS |
| 8 GND  | P Ground                             |                      |             |             |
| 1 RST  | P RST_SW                             |                      |             |             |
| 2 0    | I/O GPIO0,USER_SW                    |                      |             |             |
| 3 1    | I/O GPIO1,Vbat_Read2,Touch1,ADC1_CH0 |                      |             |             |
| 4 2    | I/O GPIO2,ADC                        | Ctrl,Touch1,ADC1_CH0 |             |             |
| 5     | I/O GPIO3,VextCtrl,Touch1,ADC1_CH0        |     |     |     |
| ----- | ----------------------------------------- | --- | --- | --- |
| 6 19  | I/O GPIO19,USB_D-,CLK_OUT2,ADC2_CH8,U1RTS |     |     |     |
| 7 20  | I GPIO20,USB_D+,CLK_OUT1,ADC2_CH9,U1CTS   |     |     |     |
| 8 21  | I/O GPIO21,TFT_LED_K                      |     |     |     |
| 9 26  | I/O GPIO26,SPICS1                         |     |     |     |
| 10 48 | I/O GPIO48,D_SEL                          |     |     |     |
2 VBAT=Vbat_Read*4.9
https://heltec.org
Documents Rev1.1 P7/17 May2023 HeltecAutomation©Limitedstandardfiles

| 11 47 | I/O GPIO47,Boot_Mode                        |     |
| ----- | ------------------------------------------- | --- |
| 12 33 | I/O GPIO33,SPIIO4,FSPIHD,SUBSPIHD,GNSS_TX   |     |
| 13 34 | I/O GPIO34,SPI05,FSPICS0,SUBSPICS0,GNSS_RX  |     |
| 14 35 | I/O GPIO35,SPI06,FSPID,SUBSPID,GNSS_RST     |     |
| 15 36 | I/O GPIO36,SPI07,FSPICLK,SUBSPICLK,GNSS_PPS |     |
| 16 37 | I/O GPIO37,SPIDQS,FSPIQ,SUBSPIQ             |     |
HeaderJ3
Table2-2-2:Pindescription
| No. Name | Type Function                          |           |
| -------- | -------------------------------------- | --------- |
| 1 18     | I/O GPIO18,U1RXD,ADC2_CH7,CLK_OUT3,LED | WriteCtrl |
| 2 17     | I/O GPIO17,U1TXD,ADC2_CH6              |           |
| 3 16     | I/O GPIO16,U0CTS,ADC2_CH5,XTAL_32K_N   |           |
| 4 15     | I/O GPIO15,U0RTS,ADC2_CH4,XTAL_32K_P   |           |
| 5 7      | I/O GPIO7,ADC1_CH6,TOUCH7              |           |
| 6 6      | I/O GPIO6,ADC1_CH5,TOUCH6              |           |
| 7 5      | I/O GPIO5,ADC1_CH4,TOUCH5              |           |
| 8 4      | I/O GPIO4,ADC1_CH3,TOUCH4              |           |
| 1 46     | I/O GPIO46                             |           |
| 2 45     | I/O GPIO45                             |           |
| 4 44     | I/O GPIO44,U0RXD                       |           |
| 5 43     | I/O GPIO43,U0TXD                       |           |
GPIO14,ADC2_CH3,TOUCH14,FSPIDQS,SUBSPIWP,
| 6 14 | I/O |     |
| ---- | --- | --- |
FSPIWP,LoRa_DIO1
https://heltec.org
Documents Rev1.1 P8/17 May2023 HeltecAutomation©Limitedstandardfiles

GPIO13,ADC2_CH2,TOUCH13,FSPIIO7,SUBSPIQ,FSPIQ,
7 13 I/O
LoRa_Busy
GPIO12,ADC2_CH1,TOUCH12,FSPIIO6,SUBSPICLK,
8 12 I/O
FSPICLK,LoRa_RST
GPIO11, ADC2_CH0, TOUCH11, FSPIIO5, SUBSPID, FSPID,
9 11 I/O
LoRa_MISO
GPIO10,ADC1_CH9,TOUCH10,FSPIIO4,SUBSPICS0,
10 10 I/O
FSPICS0,LoRa_MOSI
GPIO9,ADC1_CH8,TOUCH9,SUBSPIHD,FSPIHD,
11 9 I/O
LoRa_SCK
12 8 I/O GPIO8,ADC1_CH7,TOUCH8,SUBSPICS1,LoRa_NSS
13 42 I/O GPIO42,MTMS,TFT_SDIN
14 41 I/O GPIO41,MTDI,TFT_SCLK
15 40 I/O GPIO40,MTMS,TFT_RS
16 39 I/O GPIO39,MTMS,TFT_RES
18 38 I/O GPIO38,FSPIWP,SUBSPIWP,TFT_CS
https://heltec.org
Documents Rev1.1 P9/17 May2023 HeltecAutomation©Limitedstandardfiles

3. Specifications
3.1General specifications
Table3-1:Generalspecifications
| Parameters            |                       |                   | Description     |            |                 |
| --------------------- | --------------------- | ----------------- | --------------- | ---------- | --------------- |
| MasterChip            | ESP32-S3FN8           | (Xtensa®32-bitlx7 |                 | dualcore   | processor)      |
| LoRaChipset           | SX1262                |                   |                 |            |                 |
| GNSS Chipset          | UC6580                |                   |                 |            |                 |
| Frequency             | 470~510MHz,863~928MHz |                   |                 |            |                 |
| MaxTXPower            | 21 ± 1dBm             |                   |                 |            |                 |
| Receiving sensitivity | -135dBm               |                   |                 |            |                 |
| Wi-Fi                 | 802.11 b/g/n          |                   |                 |            |                 |
| Bluetooth             | Bluetooth             | LE:Bluetooth      | 5,Bluetoothmesh |            |                 |
|                       | Type-C                | USB; 2*1.25       | lithium battery | interface; | LoRa ANT(IPEX); |
Interface
GNSS ANT(IPEX)
| Battery               | 3.7Vlithium             | batterypowersupplyand |     | charging |     |
| --------------------- | ----------------------- | --------------------- | --- | -------- | --- |
| Operating temperature | -20~70℃                 |                       |     |          |     |
| Dimensions            | 65.48mm*28.06mm*13.52mm |                       |     |          |     |
https://heltec.org
Documents Rev1.1 P10/17 May2023 HeltecAutomation©Limitedstandardfiles

| 3.2Power |     | supply |     |     |     |     |     |     |
| -------- | --- | ------ | --- | --- | --- | --- | --- | --- |
ExceptwhenUSBor5VPinisconnectedseparately,lithiumbatterycanbeconnectedtochargeit.
Inothercases,onlyasinglepowersupplycanbeconnected.
Table3-2:Powersupply
|          | Powersupplymode        |             | Minimum |     | Typical | Maximum |     | Company |
| -------- | ---------------------- | ----------- | ------- | --- | ------- | ------- | --- | ------- |
|          | Type-C                 | USB(≥500mA) | 4.7     |     | 5       | 6       |     | V       |
|          | Lithiumbattery(≥250mA) |             | 3.3     |     | 3.7     | 4.2     |     | V       |
|          | 5Vpin(≥500mA)          |             | 4.7     |     | 5       | 6       |     | V       |
|          | 3V3                    | pin(≥150mA) | 2.7     |     | 3.3     | 3.5     |     | V       |
| 3.3Power |                        | output      |         |     |         |         |     |         |
Table3-3:Poweroutput
|       | OutputPin |               |     | electric | current |     | Company |     |
| ----- | --------- | ------------- | --- | -------- | ------- | --- | ------- | --- |
|       |           | 3.3VPin       |     |          | 500     |     | mA      |     |
| 5VPin | (USB      | Powered only) |     |          | 500     |     | mA      |     |
|       |           | VextPin       |     |          | 350     |     | mA      |     |
https://heltec.org
Documents Rev1.1 P11/17 May2023 HeltecAutomation©Limitedstandardfiles

3.4Power characteristics
Table3-4:Powercharacteristics
VBAT/battery
| Mode |     | USBpower |     |     |     | Unit |
| ---- | --- | -------- | --- | --- | --- | ---- |
powered
| Wi-FiScan |       | 100 |     | 74  |     | mA  |
| --------- | ----- | --- | --- | --- | --- | --- |
| Wi-FiAP   |       | 150 |     | 111 |     | mA  |
| BT        |       | 102 |     | 75  |     | mA  |
| GNSS      |       | 120 |     | 89  |     | mA  |
|           | 14dbm |     | 200 | 148 |     | mA  |
| TX        | 17dbm |     | 220 | 163 |     | mA  |
|           | 22dbm |     | 240 | 178 |     | mA  |
TXdisabled;
| RX  |     |     | 80  | 59  |     | mA  |
| --- | --- | --- | --- | --- | --- | --- |
RXenabled
| sleep                      |     | 2mA |     |     | 15μA |     |
| -------------------------- | --- | --- | --- | --- | ---- | --- |
| 3.5LoRa RF characteristics |     |     |     |     |      |     |
| 3.5.1Transmit power        |     |     |     |     |      |     |
Table3-5-1:Transmitpower
| Operatingfrequencyband |     |     | Maximum | powervalue/[dBm] |     |     |
| ---------------------- | --- | --- | ------- | ---------------- | --- | --- |
470~510 21 ± 1
867~870 21 ± 1
902~928 21 ± 1
https://heltec.org
Documents Rev1.1 P12/17 May2023 HeltecAutomation©Limitedstandardfiles

3.5.2Receivingsensitivity
ThefollowingtablegivestypicallysensitivityleveloftheWirelessTrackerr-(L/H).
Table3-5-2:Receivingsensitivity
| Signal | Bandwidth/[KHz] | SpreadingFactor |      | Sensitivity/[dBm] |
| ------ | --------------- | --------------- | ---- | ----------------- |
|        | 125             |                 | SF12 | -135              |
|        | 125             |                 | SF10 | -130              |
|        | 125             |                 | SF7  | -124              |
3.5.3OperationFrequencies
Wireless Tracker supports LoRaWAN frequency channels and models corresponding
table.
Table3-5-3:OperationFrequencies
| Region |     | Frequency(MHz)  |     | Model              |
| ------ | --- | --------------- | --- | ------------------ |
| EU433  |     | 433.175~434.665 |     | WirelessTracker-LF |
| CN470  |     | 470~510         |     | WirelessTracker-LF |
| IN868  |     | 865~867         |     | WirelessTracker-HF |
| EU868  |     | 863~870         |     | WirelessTracker-HF |
| US915  |     | 902~928         |     | WirelessTracker-HF |
| AU915  |     | 915~928         |     | WirelessTracker-HF |
| KR920  |     | 920~923         |     | WirelessTracker-HF |
| AS923  |     | 920~925         |     | WirelessTracker-HF |
https://heltec.org
Documents Rev1.1 P13/17 May2023 HeltecAutomation©Limitedstandardfiles

3.6GNSS Characteristics
3.6.1 Basicinformation
Table3-6-1:Basicinformation
| Channel         |     | 96      |
| --------------- | --- | ------- |
| Updatefrequency |     | Max10Hz |
Dataformat NMEA-0183，Unicore，RTCM3.x
BDS:B2a
GPS:L1+L5
GLONASS:G1
Frequencypoint Galileo:E1+E5a
QZSS:L1+L5
SBAS:L1
NAVIC:L5*(Specificfirmware)
TTFF²
3.6.2Accuracyand
Table3-6-2:AccuracyandTTFF²
| Horizontalposition            | accuracy(RMS) | 1.5m |
| ----------------------------- | ------------- | ---- |
| Verticalpositionaccuracy(RMS) |               | 2.5m |
| Timeaccuracy(RMS)             |               | 5ns  |
| Speed | accuracy | 0.02m/s |
| ----- | -------- | ------- |
| Cold  | boot     | ＜26s    |
3 -33mpslinearuniformmotionsceneunderthesimulator
https://heltec.org
Documents Rev1.1 P14/17 May2023 HeltecAutomation©Limitedstandardfiles

|     | Warmboot  |     |     | ＜2s |     |
| --- | --------- | --- | --- | --- | --- |
|     | Recapture |     |     | 1s  |     |
3.6.3Sensitivity(Unit:dBm)
Talbe3-6-2:Sensitivity
| Sensitivity | GNSS | BDS  | GPS  | GAL  | GLONASS |
| ----------- | ---- | ---- | ---- | ---- | ------- |
| Cold boot   | -148 | -146 | -148 | -144 | -144    |
| Warmboot    | -156 | -155 | -155 | -154 | -148    |
| Trace       | -165 | -163 | -165 | -163 | -158    |
| Recapture   | -156 | -154 | -156 | -154 | -152    |
https://heltec.org
Documents Rev1.1 P15/17 May2023 HeltecAutomation©Limitedstandardfiles

4. Physical dimensions
5. Resource
5.1Relevant Resource
 Heltec ESP (ESP32 & ESP8266) framework (Already included Heltec ESP32
LoRaWANlibrary)
 HeltecLoRaWANtestserverbasedonSnapEmu
 UserManualDocument
5.2Contact Information
HeltecAutomationTechnologyCo.,Ltd
https://heltec.org
Documents Rev1.1 P16/17 May2023 HeltecAutomation©Limitedstandardfiles

Chengdu,Sichuan,China
Email:support@heltec.cn
Phone:+86-028-62374838
https://heltec.org
https://heltec.org
Documents Rev1.1 P17/17 May2023 HeltecAutomation©Limitedstandardfiles
