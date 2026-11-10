SPECIFICATIONS AND FEATURES
DATASHEET
WWW.UNICORECOMM.COM
UFirebirdII-UC6580
Dual-frequency GNSS Positioning Chip
Copyright© 2009-2023, Unicore Communications, Inc.

UC6580 Datasheet
Revision History
| Version   | Revision History  | Date       |
| --------- | ----------------- | ---------- |
| R1.0      | First Release     | Sep.,2023  |
Update the working voltage in technical specifications
| R1.1  | Update the IO power domain description  | Oct., 2023  |
| ----- | --------------------------------------- | ----------- |
Update Table 7-3

i

Legal right notice
This manual provides information and details on the products of Unicore
Communication, Inc. (“Unicore”) referred to herein.
All rights, title and interest to this document and the information such as data, designs,
layouts contained in this manual are fully reserved, including but not limited to the
copyrights, patents, trademarks and other proprietary rights as relevant governing laws
may grant, and such rights may evolve and be approved, registered or granted from the
whole information aforesaid or any part(s) of it or any combination of those parts.
Unicore holds the trademarks of “和芯星通”,“UNICORECOMM” and other trade name,
trademark, icon, logo, brand name and/or service mark of Unicore products or their
product serial referred to in this manual (collectively “Unicore Trademarks”).
This manual or any part of it, shall not be deemed as, either expressly, implied, by
estoppel or any other form, the granting or transferring of Unicore rights and/or
interests (including but not limited to the aforementioned trademark rights), in whole or
in part.
Disclaimer
The information contained in this manual is provided “as is” and is believed to be true
and correct at the time of its publication or revision. This manual does not represent,
and in any case, shall not be construed as a commitments or warranty on the part of
Unicore with respect to the fitness for a particular purpose/use, the accuracy, reliability
and correctness of the information contained herein.
Information, such as product specifications, descriptions, features and user guide in this
manual, are subject to change by Unicore at any time without prior notice, which may
not be completely consistent with such information of the specific product you
purchase.
Should you purchase our product and encounter any inconsistency, please contact us or
our local authorized distributor for the most up-to-date version of this manual along
with any addenda or corrigenda.
ii

UC6580 Datasheet
Foreword
This datasheet provides information on the hardware features and performance
specifications of UC6580 positioning chip.
Target Readers
This datasheet applies to technicians who have knowledge in the GNSS field but not to
general readers.
iii

I
UC6580 Datasheet
II
1  Product Introduction
| 1.1  Overview    |     |     |     |
| ---------------- | --- | --- | --- |
UC6580 is a dual-frequency multi-constellation positioning SoC developed by Unicore
Communications, with sub-meter level accuracy, supporting BDS-3 signals. It adopts 22
nm process, low-power design, compact size, RF-baseband integrated technology, and
supports multi-path mitigation, anti-jamming and high precision GNSS joint positioning,
which performs well in the power and size sensitive scenarios.
UC6580 is suitable for global applications. It has 96 tracking channels, supports GPS,
GLONASS, BDS, Galileo, NAVIC and QZSS multi-constellation joint positioning, as well as
SBAS signal reception processing, providing fast and accurate positioning experience
with high performance.
UC6580 supports L1 + L5/L2 dual-frequency single point positioning and multi-system
raw data output. The former is suitable for wearables, handheld devices and walking
navigation, significantly improving users’ experience compared with single-frequency
solution especially in urban multi-path environment, while the latter is suitable for
vehicle navigation, robotic and UAV applications.
UC6580 has two models including automotive grade and industrial grade (see Table
1-1).
Table 1-1 UC6580 models
| Model               | Grade                 | Package  |     |
| ------------------- | --------------------- | -------- | --- |
| UC6580A             | Automotive            | QFN40    |     |
| UC6580I             | Industrial            | QFN40    |     |
|                     |                       |          |     |
| UC-01-C43 EN R1.1   | Product Introduction  |          | 1   |
UC6580 Datasheet
1.2 Product Features
 22 nm dual-frequency multi-constellation GNSS SoC, with low power consumption
and compact size
 Concurrent acquisition and tracking of dual frequencies from multiple
constellations, including BDS-3 signals; supports:
- BDS B1I/B1C + B2a or B1I/B1C* + B2I
- GPS L1 + L5 or L1 + L2
- Galileo E1 + E5a or E1 + E5b
- GLONASS G1 or G1+G2
- QZSS L1 + L5 or L1 + L2
- SBAS L1
- NAVIC L5*
 Real-time wideband and narrowband anti-jamming technology: detection and
removal of wideband and narrowband jamming of no less than -75 dBm
 Supports L1 + L5/L2 dual-frequency single point positioning, with excellent multi-
path mitigation algorithm
 Supports L1 + L5/L2 dual-frequency multi-constellation raw data output, provides
centimeter level RTK positioning and sub-meter level RTD positioning
 RF and baseband design with ultra-high sensitivity: acquisition sensitivity better
than -148 dBm, tracking sensitivity better than -165 dBm
 Supports AGNSS
 Supports secure boot
 Automotive grade and industrial grade with QFN40 package (See the section 10.3
Ordering Information for more details)
 Conforms to the requirement of AEC-Q100 Grade2 (UC6580A)
 Supported by specific firmware.
2 Product Introduction UC-01-C43 EN R1.1
1.3
Technical Specifications
Table 1-2 Technical Specifications
Basic Information
| Channels     | 96 channels                   |                 |     |                 |     |
| ------------ | ----------------------------- | --------------- | --- | --------------- | --- |
| Update Rate  | 10 Hz (max.)                  |                 |     |                 |     |
| Data Format  | NMEA-0183, Unicore, RTCM 3.x  |                 |     |                 |     |
|              |                               | Mode 1          |     | Mode 2*         |     |
|              | BDS                           | B1I/B1C + B2a  |     | B1I/B1C* + B2I  |     |
|              | GPS                           | L1 + L5         |     | L1 + L2         |     |
|              | Galileo                       | E1 + E5a        |     | E1 + E5b        |     |
Frequencies
|     | GLONASS  | G1       |     | G1 + G2  |     |
| --- | -------- | -------- | --- | -------- | --- |
|     | QZSS     | L1 + L5  |     | L1 + L2  |     |
|     | NAVIC    | L5*      |     | -        |     |
|     | SBAS     | L1       |     | L1       |     |
Observation Accuracy
| Horizontal Accuracy  | Single point positioning: 1.5 m       |     |     |     |     |
| -------------------- | ------------------------------------- | --- | --- | --- | --- |
| (RMS)                | RTK1: 1 cm + 1ppm                     |     |     |     |     |
| Vertical Accuracy    | Single point positioning: 2.5 m       |     |     |     |     |
| (RMS)                | RTK1: 2 cm + 1ppm                     |     |     |     |     |
| Time Accuracy (RMS)  | 5 ns, peak-to-peak value 30 ns (24h)  |     |     |     |     |
| Velocity Accuracy2   | 0.02 m/s                              |     |     |     |     |
TTFF3
| Cold Start      | 26s  |     |     |     |     |
| --------------- | ---- | --- | --- | --- | --- |
| Hot Start       | 2 s  |     |     |     |     |
| Reacquisition   | 1 s  |     |     |     |     |
Sensitivity4,5              GNSS
| Cold Start  | -148 dBm  |     |     |     |     |
| ----------- | --------- | --- | --- | --- | --- |
| Hot Start   | -156 dBm  |     |     |     |     |
| Tracking    | -162 dBm  |     |     |     |     |
 Supported by specific firmware.
1 RTK accuracy depends on the customer’s algorithm capability, and the reference data here is given
according to Unicore self-developed algorithms.
2 Uniform linear motion of -33 mps using a simulator.
3 Satellite signal strength @ -130 dbm.
4 To get the sensitivity index, CN0 needs to achieve 41 dB (The performance might be updated).
5 Connect to a matched external LNA to ensure superior performance.
| UC-01-C43 EN R1.1   | Product Introduction  |     |     |     | 3   |
| ------------------- | --------------------- | --- | --- | --- | --- |
UC6580 Datasheet
| Reacquisition   | -159 dBm  |     |     |     |     |
| --------------- | --------- | --- | --- | --- | --- |
Power Consumption6 (@25℃)
DCDC Mode  Acquisition: 40 mA@3 V            Tracking : 40 mA@3 V
| Backup Mode  | 5 μA @ 3 V  |     |     |     |     |
| ------------ | ----------- | --- | --- | --- | --- |
Working Voltage
| Main Power Supply    | 2.7 V ~ 3.6 V  |     |     |     |     |
| -------------------- | -------------- | --- | --- | --- | --- |
| IO Power Supply      | 2.7 V ~ 3.6 V  |     |     |     |     |
| Backup Power Supply  | 1.7 V ~3.6 V   |     |     |     |     |
Communication Interfaces
UART ×  2
I2C × 2
SPI ×  2
Reliability Test and Certificates
Conforms to JESD47 standard (UC6580I)
Reliability
Conforms to ACE-Q100 standard (UC6580A)
| Certificates  | Conforms to RoHS and REACH requirements  |     |     |     |     |
| ------------- | ---------------------------------------- | --- | --- | --- | --- |
|     |     |     |     |     |     |
| --- | --- | --- | --- | --- | --- |
6 Depends on the firmware version.
| 4   | Product Introduction  |     |     | UC-01-C43 EN R1.1  |     |
| --- | --------------------- | --- | --- | ------------------ | --- |
2  Pin Definition
| 2.1  | Pin Assignment  |     |     |     |     |     |     |     |     |
| ---- | --------------- | --- | --- | --- | --- | --- | --- | --- | --- |
UC6580A and UC6580I have the same pins, and Figure 2-1 gives the diagram of
UC6580A as an example.
                                 QFN40 TOP VIEW (Marking Side)
|     |     |     |         |         |             |         |     |     |     |
| --- | --- | --- | ------- | ------- | ----------- | ------- | --- | --- | --- |
|     |     | 30  | 29  28  | 27  26  | 25  24  23  | 22  21  |     |     |     |
  T
|     |          |     |         |         |            | U    |   N   |     |     |
| --- | -------- | --- | ------- | ------- | ---------- | ---- | ----- | --- | --- |
|     |          |     |         |         |     R E    | C_ O | C_I   |     |     |
|     |          |   3 |   5   1 |   2   4 |   0 O_ C O |      |       |     |     |
|     |          | O   | O O     | O O     | O D C      | C D  | C D   |     |     |
|     |          | PI  | PI PI   | PI PI   | PI L V_    | D    | D     |     |     |
|     |          |     |         |         |            |      |       |     |     |
|     | 31  SCL  |     |         |         |            |      | NC    | 20  |     |
|     | 32  SDA        |     |         |       |           |        | TX      | 19  |     |
| --- | -------------- | --- | ------- | ----- | --------- | ------ | ------- | --- | --- |
|     | 33  D_SEL      |     |         |       |           |        | RX      | 18  |     |
|     | 34  BOOT_MODE  |     |         |       |           |        | RESETN  | 17  |     |
|     | 35  PPS        |     |         |       |           |        | TMS     | 16  |     |
|     | 36  PIO15      |     |         |       |           |        | TCK     | 15  |     |
|     | 37  PIO16      |     |         |       |           |        | PIO13   | 14  |     |
|     | 38  L5_IN      |     |         |       |           |        | PIO14   | 13  |     |
|     | 39  NC         |     |         |       |           |        | RTC_I   | 12  |     |
|     | 40  L1_IN      |     |         |       |           |        | RTC_O   | 11  |     |
|     |                |     |         |       |           |        |         |     |     |
|     |                |     |         |       |           |   T    |         |     |     |
|     |                |     | N       |   X   |   K   O   | N      | E T     |     |     |
|     |                |     |   X O_I | E     | C D_I   T | D_ A   | R       |     |     |
|     |                |  C  | O_ X    |  C O_ | B A D E   |        | O_      |     |     |
|     |                | N   | D T C   | N D   | V_ V D V_ | V D    | D       |     |     |
|     |                |     | L       | L     |           |        | L       |     |     |
|     |   PIN Number   |     |         |       |           |        |         |     |     |
|     |                | 1   | 2  3    | 4  5  | 6  7  8   | 9  10  |         |     |     |
Figure 2-1 UC6580 Pin Diagram
|                     |     |     |     |                 |     |     |     |     |     |
| ------------------- | --- | --- | --- | --------------- | --- | --- | --- | --- | --- |
| UC-01-C43 EN R1.1   |     |     |     | Pin Definition  |     |     |     |     | 5   |
UC6580 Datasheet
2.2  Pin Description
Table 2-1 Description of Power Supply Pin
| Name      | Pin   | Type   |     | Description              |     |
| --------- | ----- | ------ | --- | ------------------------ | --- |
| DCDC_IN   | 21    | Power  |     | DC/DC power input        |     |
| DCDC_OUT  | 22    | Power  |     | DC/DC power output       |     |
| V_CORE    | 23    | Power  |     | Core power input         |     |
| V_BACK    | 6     | Power  |     | Backup power input       |     |
| VDD_IO    | 7     | Power  |     | IO/TCXO power input      |     |
| LDO_C     | 24    | Power  |     | Core LDO voltage output  |     |
| LDO_X     | 2     | Power  |     | TCXO LDO voltage output  |     |
Used by the chip itself, and cannot
| LDO_EX  | 5   | Power  |     |     |     |
| ------- | --- | ------ | --- | --- | --- |
supply power to other circuits
| NC       | 20  | Power  |     | Keep NC                        |     |
| -------- | --- | ------ | --- | ------------------------------ | --- |
| V_DET7   | 8   | Power  |     | Antenna detection power input  |     |
| VDD_ANT  | 9   | Power  |     | Antenna power output           |     |
| LDO_RET  | 10  | Power  |     | Backup power output            |     |
| GND      |     |        |     | Ground                         |     |
Table 2-2 Analog Pin Description
| Name     | Pin   | Type   |     | Description        |     |
| -------- | ----- | ------ | --- | ------------------ | --- |
| L1_IN    | 40    | RF     |     | L1 RF input        |     |
| L5_IN    | 38    | RF     |     | L5 or L2 RF input  |     |
| TCXO_IN  | 3     | Clock  |     | 26 MHz TCXO input  |     |
32.768 kHz crystal or digital waveform
| RTC_I  | 12  | Clock   |     |     |     |
| ------ | --- | ------- | --- | --- | --- |
input
| RTC_O  | 11  | Clock   |     | 32.768 kHz clock output  |     |
| ------ | --- | ------- | --- | ------------------------ | --- |
| NC     | 4   | -       |     | Keep NC                  |     |
| NC     | 39  | -       |     | Keep NC                  |     |
| NC     | 1   | RF      |     | Keep NC                  |     |
Table 2-3 PIO Pin Description
| Name   | Pin   | Type   | IO Reset   |     | Description   |
| ------ | ----- | ------ | ---------- | --- | ------------- |
| PIO0   | 25    | IO     | I/Pull-up  |     | GPIO0         |
| PIO1   | 28    | IO     | I/Pull-up  |     | GPIO1         |
| PIO2   | 27    | IO     | I/Pull-up  |     | GPIO2         |
| PIO3   | 30    | IO     | I/Pull-up  |     | GPIO3         |
| PIO4   | 26    | IO     | I/Pull-up  |     | GPIO4         |
7 Not supported currently.
| 6   |     |     | Pin Definition  |     | UC-01-C43 EN R1.1  |
| --- | --- | --- | --------------- | --- | ------------------ |
| Name                | Pin   Type   | IO Reset        | Description   |     |
| ------------------- | ------------ | --------------- | ------------- | --- |
| PIO5                | 29  IO       | I/Pull-up       | GPIO5         |     |
| TX                  | 19  IO       | I/Pull-up       | GPIO6         |     |
| RX                  | 18  IO       | I/Pull-up       | GPIO7         |     |
| SCL                 | 31  IO       | I/Pull-up       | GPIO8         |     |
| SDA                 | 32  IO       | I/Pull-up       | GPIO9         |     |
| D_SEL               | 33  IO       | I/Pull-up       | GPIO10        |     |
| PPS                 | 35  IO       | I/Pull-up       | GPIO11        |     |
| BOOT_MODE           | 34  IO       | I/Pull-up       | GPIO12        |     |
| PIO13               | 14  IO       | I/Pull-up       | GPIO13        |     |
| PIO14               | 13  IO       | I/Pull-up       | GPIO14        |     |
| PIO15               | 36  IO       | I/Pull-up       | GPIO15        |     |
| PIO16               | 37  IO       | I/Pull-up       | GPIO16        |     |
| TMS                 | 16  IO       | I/Pull-up       | GPIO17        |     |
| TCK                 | 15  IO       | I/Pull-up       | GPIO18        |     |
| RESETN              | 17  IO       | I/Pull-up       | -             |     |
|                     |              |                 |               |     |
| UC-01-C43 EN R1.1   |              | Pin Definition  |               | 7   |
UC6580 Datasheet
3 Chip Structure
3.1 Block Diagram
GNSS Antenna
PMU
GNSS RF
RF_IN1
DCDC V_IO
LNA RF_IN2
LDO V_Main
SAW Down Conversion
AON
V_BCKP
TCXO
26MHz A/D Conversion Retention RAM
RTC 32.768kHz
Digital Interfaces
SRAM UART UART
GNSS BB SPI SPI
ROM
I2C I2C
CPU
GPIO
Flash
GPIO PPS
EXTINT
Figure 3-1 UC6580 Block Diagram
8 Chip Structure UC-01-C43 EN R1.1
3.2 Power Management Unit (PMU)
The Power Management Unit (PMU) provides four power domains that are internally
generated by LDOs and supervised by several voltage monitors:
UC-01-C43 EN R1.1 Chip Structure 9
D C
.7 V
V _1
V D
.7 V
V _
.7 V
D C _ IN
~ 3 .6
C O R E
.0 V
D _ IO
~ 3 .6
B A C K
~ 3 .6
V
V
V
D C D C
L D O _ C
L D O _ E X
L D O _ X
P S W
L D
C
O
Po
_
Mn
R
Utr
E
o l
T
D
C
.8 D C _ O U
1 .0 V
L D O _ C
0 .9 V
L D O _ E
1 .0 V
L D O _ X
V o r 1
L D O _ R
0 .9 V
T
X
.8 E
V
T
Figure 3-2 Power Management Unit (PMU)
 Core
Core domain is the main power domain for the RF and digital part inside the chip. The
subsequent LDO_C converts the V_CORE input to respective voltages, which must be
connected with a decoupling capacitor through the LDO_C pin. LDO_C drives the digital
logic parts.
 IO
IO power domain is powered by VDD_IO, including the chip IO devices, on-chip Flash,
etc. The voltage supply of VDD_IO is 2.7 V ~ 3.6 V. Except IO devices, other PMU devices
are powered by a dedicated LDO_EX. LDO_EX must be connected with a decoupling
capacitor through LDO_EX pin.
 Backup
Backup domain runs the RTC section and Retention RAM. This domain uses VDD_IO and
V_BACK as the voltage sources. When the range of VDD_IO is normal, it uses VDD_IO,
otherwise uses V_BACK. The allowed range of V_BACK is 1.7 V ~ 3.6 V. Therefore, an
ordinary lithium battery or other battery can be directly connected to this pin. If you do
not need the RTC and backup function, you must connect the V_BACK pin to VDD_IO.
UC6580 Datasheet
 TCXO
The clock domain supplies power to TCXO. This domain has a dedicated LDO called
LDO_X, which is also powered by VDD_IO. If TCXO is powered by LDO_X, LDO_X should
be connected to the power pin of TCXO and be decoupled by a capacitor. You can also
choose an external power source other than LDO_X to power TCXO.
Based on the above division of power domains and hardware design, UC6580 has three
modes of power consumption:
 Running mode: Every power source of the chip is normal, CPU runs normally, and
the power supply of each domain is set by the software. All events, including
external interruption, communication request, timing, etc., can be processed
normally.
 V_BACK mode: The IO and main power supply of the chip is cut off from the outside,
and there is only V_BACK power supply left. At this time, the power consumption of
the chip drops to a very low level, and the specific functions and power
consumption depend on the mode set by the software. It can wake up as soon as it
is powered on.
 Power off mode: All power supplies are cut off from the outside, and the chip does
not work at all.
10 Chip Structure UC-01-C43 EN R1.1
3.3
Clock
The chip requires an external 26 MHz clock, which is generated by TCXO, to provide
reference frequency for RF and baseband PLL. In order to ensure the stable operation of
the PLL when the chip is booted, the 26 MHz clock should work stably within10 ms after
the main and IO domains are powered.
The chip supports RTC crystal input. RTC crystal is usually driven by an on-chip
32.768 kHz oscillator, which connects to an external 32.768 kHz crystal. The chip also
supports external RTC clock input. The input signal amplitude should be 0.9 V to 1.98 V,
and the input signal frequency should be 32.768 kHz. The RTC clock frequency offset
must be less than 20 ppm.
Table 3-1 Clock
|     | Frequency Source  | Frequency  | Remark  |     |     |     |
| --- | ----------------- | ---------- | ------- | --- | --- | --- |
Work stably within10 ms after the
System
|     | TCXO  | 26 MHz  | main and IO domains are  |     |     |     |
| --- | ----- | ------- | ------------------------ | --- | --- | --- |
Clock
powered
Connect an external 32.768 kHz
|        | On-chip oscillator  | 32.768 kHz  |                                   |     |     |     |
| ------ | ------------------- | ----------- | --------------------------------- | --- | --- | --- |
| RTC    |                     |             | crystal                           |     |     |     |
| Clock  | External digital    |             | Input signal amplitude should be  |     |     |     |
32.768 kHz
|     | waveform generator  |     | 0.9 V to 1.98 V  |     |     |     |
| --- | ------------------- | --- | ---------------- | --- | --- | --- |
If the main power supply and IO power supply fail and a backup battery is connected to
V_BACK, the baseband, RF and CPU do not work, while RTC keeps running to provide
time reference for the receiver. This operating mode is called RTC time keeping mode.
Under this mode, the relevant data are saved in Retention RAM for GNSS hot start.
RTC time keeping mode is a prerequisite for GNSS hot start. Under this mode, RTC
provides time information and Retention RAM provides ephemeris and almanac
information. If you do not need GNSS hot start function, connect RTC_O to ground. In the
AGNSS-based system, if time and ephemeris are provided through network as
assistance, RTC is not necessary.
Table 3-2 RTC Timing Keeping Mode
Power  Working Parts
Mode
|                     | Supply  | BB  RF          | CPU  RTC  | Retention RAM  |      |     |
| ------------------- | ------- | --------------- | --------- | -------------- | ---- | --- |
| RTC time keeping    | V_BACK  |                 |          |                |     |     |
|                     |         |                 |           |                |      |     |
| UC-01-C43 EN R1.1   |         | Chip Structure  |           |                |      | 11  |
UC6580 Datasheet
3.4 System Reset
According to the power structure of UC6580, there are two reset domains: Core domain
and Backup domain.
Core domain can be reset by three methods:
 RESETN is the reset pin of the chip. When the voltage level at RESETN is low, the
reset signal will be sent to the Core domain. The duration of RESETN low level
should be more than 10 us.
 The chip’s software reset, which is controlled by the firmware.
 Watchdog RESET.
If any of the above reset sources issues a reset signal, the Core domain is reset.
Backup domain can be reset by two methods:
 When the voltage of V_BACK is lower than 1.2 V, it will trigger the reset.
 The software system sends the RTC RESET signal, which is controlled by the
firmware and only resets the RTC counter.
12 Chip Structure UC-01-C43 EN R1.1
4 RF Subsystem
The RF subsystem of UC6580 adopts dual-frequency dual-channel architecture. The
frequency of the input signal ranges from 1166MHz to 1620MHz. The received GNSS
signals are amplified by a single-ended Low Noise Amplifier (LNA), and then fed to a RF
gain block to be further amplified, thus reducing the noise figure requirements for the
mixer. The RF gain block also provides a single-ended to differential conversion. After
completing the orthogonal down-conversion, multi-GNSS signals are divided into two
channels. Afterwards, the I and Q signals of both channels are low-pass filtered and
amplified by a separate Programmable Gain Amplifier (PGA), after which both I and Q
signals are sent to the 6-bit high-speed ADC section for data conversion.
AGC 1
mixer I Channel
(IF Signals)
LPF
ADC1
GNSS
Signals LNA Gain I/Q I Signals
Gain LPF
ADC1
Q Channel
L1 Frequency
(IF Signals)
AGC 1
Q Signals
AGC 1
mixer I Channel
(IF Signals)
LPF
ADC1
GNSS
Signals LNA Gain I/Q I Signals
Gain LPF
ADC1
Q Channel
L5/L2 Frequency
(IF Signals)
AGC 1
Q Signals
Figure 4-1 RF Subsystem
UC-01-C43 EN R1.1 RF Subsystem 13
UC6580 Datasheet
The RF subsystem of UC6580 supports any mode below:
 Dual-frequency L1+L5
 Dual-frequency L1+L2
 L1 single-frequency multi-constellation mode.
4.1 LNA
The low noise amplifier (LNA) makes use of a single stage configuration and requires
external matching to function satisfactorily. For improved performance, an external LNA
should be added, of which the gain range is recommended to be within 17dB~50dB. In
an environment with complex interference, it is necessary to use an external SAW filter
to suppress out-of-band interference.
4.2 Gain Block
A single stage differential amplifier follows the LNA providing further amplification and
conversion from single-ended signals to differential signals.
4.3 Mixer
UC6580 uses the active I/Q mixer to first convert the multi-GNSS signals to an
intermediate frequency signals. At this stage, the signals are split into two IF channels
after down-conversion.
4.4 IF Filter
UC6580 integrates an I/Q low-pass filter to remove the out-of-band noise after RF
down-conversion, which improves the noise performance of the RF system.
14 RF Subsystem UC-01-C43 EN R1.1
4.5 AGC
UC6580 supports Automatic Gain Control (AGC), which reduces the convergence time
and computing cost. AGC controls the gain configuration of each module in the radio
frequency data link according to the signal energy required by the RF system.
4.6 PGA and ADC
UC6580 integrates Programmable Gain Amplifier (PGA) and high-speed Analog Digital
Convertor (ADC). The gain value of PGA is configured by AGC to ensure that the signal
energy output by ADC remains unchanged when the RF input signal energy changes
within a certain range, thereby ensuring that the output of the high-speed ADC does not
saturate. The high-speed ADC supports the output of I/Q complex sampling signals.
UC-01-C43 EN R1.1 RF Subsystem 15
UC6580 Datasheet
5 Baseband Subsystem
UC6580 provides multiple interfaces for data communication or access to external
devices, such as UART, SPI, I2C, GPIO, etc.
5.1 Interfaces
5.1.1 UART
UC6580 makes use of two UART interfaces: UART1 and UART2. Both of them can be
used for communication with a host.
UART1By default, PIO6/PIO7 corresponds to UART1, which serves as the main UART in
standard firmware version. The communication interface of UC6580 can be mapped to
different PIO interfaces via BOOT_MODE. PIO6/PIO7 can also be used as SPI, and in this
case, there is no UART1 function. See the description in section 6.2 for the use of
BOOT_MODE and the corresponding communication interface mapping.
UART2 can usePIO15/PIO16. It is mainly used for transmitting or debugging auxiliary
information.
5.1.2 SPI Slave Interface
UC6580 uses SPI slave interface as an optional way to communicate with the host to
transfer data. At the same time, it supports loading firmware via the SPI slave interface.
The maximum transmission rate using SPI slave is 8 Mbps, and the maximum SPI clock
frequency is 8 MHz. When the SPI slave loads the firmware, the maximum transmission
rate is 4 Mbps.
The SPI slave interface shares PIO6/PIO7 and PIO8/PIO9 with UART1 and I2C
respectively. Users can select the communication interface via D_SEL and BOOT_MODE.
If PIO6/PIO7/PIO8/PIO9 is used as SPI slave interface, there are no UART1 and I2C1
functions; if PIO6/PIO7 and PIO8/PIO9 are used as UART1 and I2C1, there is no SPI slave
interface.
When the SPI slave interface is used for host communication, PIO14 should be used as
the SRDY (Slave Ready) signal to indicate whether the SPI slave is ready.
16 Baseband Subsystem UC-01-C43 EN R1.1
5.1.3 SPI Master Interface
UC6580 provides SPI master interface by configuring PIO0/PIO1/PIO3/PIO4, which can
be used to communicate with or control other SPI slave devices. The maximum
transmission rate of the SPI master interface is 16 Mbps, and the maximum SPI clock
frequency is 16 MHz.
The SPI master interface is disabled by default.
5.1.4 I2C
UC6580 provides two I2C interfaces (I2C1 and I2C2) to communicate with the host or
sensor, of which the I2C1 interface works in slave mode for firmware loading and
communicating with the host, and I2C2 works in master mode to connect to the external
sensor. The I2C interfaces are compatible with the I2C protocol, supporting the
transmission rates of 100 Kbps, 400 Kbps, and 3.4 Mbps.
By default, the I2C1 interface uses PIO8/PIO9, and the BOOT_MODE pin should be in
pull-up or open-circuit state when booting. For more information, please refer to the
description in section 5.2. I2C2 can be mapped to PIO13/PIO14 through customized
firmware.
5.1.5 Serial Flash Interface
Serial Flash interface is used to connect UC6580 with external SPI Flash. SPI Flash can
be used for firmware storage and update.
The serial Flash interface uses PIO0/PIO1/PIO2/PIO3/PIO4/PIO5 only when the
BOOT_MODE pin is in pull-up or open-circuit state when booting; otherwise, the serial
Flash interface is invalid.
5.2 PIO Functions
The PIO module may be configured as GPIO or as the aforementioned communication
interfaces. The following table describes all PIO functions.
Table 5-1 PIO Functions
Default
PIO # I/O Description Alternate Function
Function
SPI master MISO
0 GPIO I/O -
SPI flash D0
SPI master MOSI
1 GPIO I/O -
SPI flash D1
UC-01-C43 EN R1.1 Baseband Subsystem 17
UC6580 Datasheet
Default
PIO # I/O Description Alternate Function
Function
PWM0
2 GPIO I/O - UART2 RXD
SPI flash WP
PWM1
3 GPIO I/O - UAR1 TXD
SPI flash HOLD
SPI master CLK
4 GPIO I/O -
SPI flash CLK
SPI master CSN
5 GPIO I/O -
SPI flash CSN
Controlled by BOOT_MODE
when booting:
UART1 TXD (if BOOT_MODE
UART1 TXD
6 GPIO I/O is high when booting)
SPI slave MISO
SPI slave MISO (if
BOOT_MODE is low when
booting)
Controlled by BOOT_MODE
when booting:
UART1_RXD (if BOOT_MODE
UART1 RXD
7 GPIO I/O is high when booting)
SPI slave MOSI
SPI slave MOSI (if
BOOT_MODE is low when
booting)
Controlled by BOOT_MODE
when booting:
I2C1 SCL (if BOOT_MODE is
I2C1 SCL
8 GPIO I/O high when booting)
SPI slave CLK
SPI slave CLK (if
BOOT_MODE is low when
booting)
Controlled by BOOT_MODE
when booting:
I2C1 SDA (if BOOT_MODE is
I2C1 SDA
9 GPIO I/O high when booting)
SPI slave CSN
SPI slave CSN (if
BOOT_MODE is low when
booting)
18 Baseband Subsystem UC-01-C43 EN R1.1
Default
| PIO #  | I/O  Description   |     | Alternate Function  |     |
| ------ | ------------------ | --- | ------------------- | --- |
Function
Communication interface
selection pin. Select from
PPS
PIO6 to PIO9. Only valid
| 10  GPIO  | I/O  |     | D_SEL  |     |
| --------- | ---- | --- | ------ | --- |
when booting.
32.768 kHz clock
This pin is pulled up if it is
not connected.
PPS
| 11  GPIO   | I/O  -  |     | EVENT  |     |
| ---------- | ------- | --- | ------ | --- |
UART1 RXD
Bootstrap mode selection
pin. Select firmware loading
BOOT MODE
address, external/internal
PPS
| 12  GPIO   | I/O  Flash or SPI interface. Only  |     |     |     |
| ---------- | ---------------------------------- | --- | --- | --- |
RF_READY
valid when booting.
UART1 TXD
This pin is pulled up if it is
not connected.
I2C2 SCL
| 13  GPIO   | I/O  -  |     | ODO_DIR  |     |
| ---------- | ------- | --- | -------- | --- |
EVENT
I2C2_SDA
| 14  GPIO   | I/O  -  |     | ODO_CNT  |     |
| ---------- | ------- | --- | -------- | --- |
EVENT
UART2 TXD
| 15  GPIO   | I/O  -  |     |     |     |
| ---------- | ------- | --- | --- | --- |
LO1_DET
UART2 RXD
| 16  GPIO   | I/O  -  |     | BLK  |     |
| ---------- | ------- | --- | ---- | --- |
LO2_DET
ODO_DIR
| 17  TMS  | I/O  Debug interface  |     |     |     |
| -------- | --------------------- | --- | --- | --- |
GPIO
ODO_CNT
| 18  TCK  | I/O  Debug interface  |     |     |     |
| -------- | --------------------- | --- | --- | --- |
GPIO

If you want to change the I/O alternate function, please contact the UNICORECOMM
FAE.
|                     |     |                     |     |     |
| ------------------- | --- | ------------------- | --- | --- |
| UC-01-C43 EN R1.1   |     | Baseband Subsystem  |     | 19  |
UC6580 Datasheet
5.3 Time Management Unit
The Time Management Unit (TMU) manages all clock sources in the baseband, using
more accurate clocks to calibrate less accurate clocks.
5.4 Watchdog
UC6580 contains two watchdog timers which prevent the system-lockup caused by the
software deadlock. During normal operation, the firmware resets the watchdog's internal
counter at regular intervals before the timer overflow occurs.
5.5 Timer Counter
The timer counter has an EVENT input and a PPS output. EVENT can be input via PIO11,
PIO13 or PIO14, but only one EVENT can be input at a time. Event input is the external
timestamp event relative to GPS time.

EVENT function is disabled by default. Please contact Unicore FAE if necessary.
PPS can be output via PIO11. PPS outputs pulse sequence synchronized with GPS or
UTC time grid, and the time interval can be configured over a wide range of frequency.
All input and output signals are synchronized with the internal clock frequency of the
receiver, so that the inherent maximum quantization error of the input and output
signals reaches ±10 ns.
20 Baseband Subsystem UC-01-C43 EN R1.1
6 System Configuration
6.1 Power Supply Scheme
UC6580 supports two power supply schemes, including internal DC-DC mode and LDO
mode.

If you do not use the hot start and backup function, connect the V_BACK to VDD_IO
6.1.1 DC-DC Mode
In this mode, the main power (V_Main) connects to the pin DCDC_IN, and the output of
the DCDC module DCDC_Out provides the power to the rest circuits. At this time, the
system is powered by the internal DC-DC:
 DCDC_IN and VDD_IO use the same power.
 V_BACK can use an independent power, or use the same power as DCDC_IN and
VDD_IO.
 The system power supply is input by DCDC_IN, and DCDC_Out is connected to the
V_CORE input pin.
 TCXO is powered by LDO_X. The voltage could be 1.8 V or 2.8 V.
6.1.2 LDO Mode
In this mode, the main power (V_Main) connects to DCDC_IN while DCDC_IN and
DCDC_Out are short-circuited and the internal DC-DC is bypassed, so that the V_Main
provides the power to the rest circuits directly. At this time:
 DCDC_IN and DCDC_OUT are short-circuited together.
 DCDC_IN and VDD_IO use the same power.
 V_BACK can use an independent power, or use the same power as DCDC_IN and
VDD_IO.
 The system power supply is input by VDD_IO and output to V_CORE through
LDO_EX.
 TCXO is powered by LDO_X. The voltage could be 1.8 V or 2.8 V.
UC-01-C43 EN R1.1 System Configuration 21
UC6580 Datasheet
For specific design scheme of the above modes, please refer to UC6580 Hardware
Reference Design.
6.2  BOOT Mode
The boot mode of UC6580 is a standalone mode which is controlled by D_SEL (PIO 10)
and Boot_Mode (PIO 12). According to the two PIO’s status, the mode divides into three
situations:
  Boots from UART1 (PIO6 and PIO7) and I2C1 (PIO8 and PIO9)
  Boots from UART1 (PIO11 and PIO12)
  Boots from SPI slave (PIO6 to PIO9).
See Table 6-1 to Table 6-5 for the details of the pin function configuration at boot and
the boot mode description.
BOOT_MODE is valid only at power-on or before the RESETN signal is sent. After the
RESETN signal is sent, the BOOT_MODE pin can be used as an ordinary PIO pin.
Table 6-1 Pin Function Configuration at Boot
Boot Time
BOOT_ Boot
| D_SEL   |       |         |                   | After Boot  | Remark  |
| ------- | ----- | ------- | ----------------- | ----------- | ------- |
| MODE    | Mode  | PIO6，7  | PIO8/9  PIO11/12  |             |         |
Outputs the
|       | Boots      |        |          | positioning    |     |
| ----- | ---------- | ------ | -------- | -------------- | --- |
|       | from       |        |          | information    |     |
| 1  X  |            | UART1  | I2C1  X  |                |     |
|       | UART1      |        |          | through UART1  |     |
|       | and I2C1   |        |          | (PIO 6 and     |     |
PIO7)
Outputs the
Boots from
positioning
|     | Boots  |     |     |     | UART1 (PIO  |
| --- | ------ | --- | --- | --- | ----------- |
information
| 0  1  | from  | X   | UART1  |     | 11 and  |
| ----- | ----- | --- | ------ | --- | ------- |
through SPI
|     | UART1   |     |     |     | PIO12), no  |
| --- | ------- | --- | --- | --- | ----------- |
slave   (PIO 6
I2C1 boot
to PIO9)
Outputs the
positioning
information
| 0  0  | SPI slave  | SPI slave  | X   |     |     |
| ----- | ---------- | ---------- | --- | --- | --- |
through SPI
slave (PIO 6 to
PIO9)
The boot steps are as follows:
| 22   |     | System Configuration  |     | UC-01-C43 EN R1.1  |     |
| ---- | --- | --------------------- | --- | ------------------ | --- |
1.  CPU detects the firmware upgrade request from the interfaces.
Table 6-2 UART1 and I2C1 Boot Mode
| If   | Then  |     |
| ---- | ----- | --- |
CPU detects a firmware upgrade  CPU starts to adapt the baud rate and upgrade the
request from UART1 within 20 ms  firmware. After the upgrade, run the firmware.
after power-on or reset.
CPU starts to adapt the I2C1 clock and upgrade the
CPU detects a firmware upgrade
request from I2C1 within 20 ms after  firmware. After the upgrade, run the firmware.
power-on or reset.
| CPU does not detect a firmware  | Do step 2  |     |
| ------------------------------- | ---------- | --- |
upgrade request from UART1 or I2C1
within 20 ms after power-on or reset.
Table 6-3 UART1 Boot Mode
| If   | Then  |     |
| ---- | ----- | --- |
CPU detects a firmware upgrade  CPU starts to adapt the baud rate and upgrade the
request from UART1 within 20 ms  firmware. After the upgrade, run the firmware.
after power-on or reset.
| CPU does not detect a firmware  | Do step 2  |     |
| ------------------------------- | ---------- | --- |
upgrade request from UART1 within
20 ms after power-on or reset.
Table 6-4 SPI Slave Boot Mode
| If   | Then  |     |
| ---- | ----- | --- |
CPU detects a firmware upgrade  CPU upgrades the firmware and runs it after the
| request from SPI slave within 20 ms  | upgrade.  |     |
| ------------------------------------ | --------- | --- |
after power-on or reset.
| CPU does not detect a firmware  | Do step 2  |     |
| ------------------------------- | ---------- | --- |
upgrade request from SPI slave within
20 ms after power-on or reset.
2.  CPU detects built-in flash and external flash in order.
Table 6-5 CPU Detects Flash
| If   | Then  |     |
| ---- | ----- | --- |
CPU detects a firmware in the built-in  CPU reads the firmware and runs it.
flash.
CPU does not detect the firmware in  CPU tries to read the firmware in external flash and
| the built-in flash.   | runs it.              |     |
| --------------------- | --------------------- | --- |
|                       |                       |     |
| UC-01-C43 EN R1.1     | System Configuration  | 23  |
UC6580 Datasheet
7  Electrical Specifications
7.1  DC Electrical Specifications
7.1.1  Absolute Maximum Rating
Table 7-1 Absolute Maximum Rating
| Symbol   | Parameter   |     | Min.  | Max.  | Unit  |
| -------- | ----------- | --- | ----- | ----- | ----- |
Input voltage of the internal
| DCDC_IN  |     |     | -0.2  | 3.6  | V   |
| -------- | --- | --- | ----- | ---- | --- |
DC/DC converter
Supply voltage of baseband main
| V_CORE  |     |     | -0.2  | 1.05  | V   |
| ------- | --- | --- | ----- | ----- | --- |
core and RF
Output voltage of the internal
| DCDC_OUT  |     |     | -0.2  | 1.05  | V   |
| --------- | --- | --- | ----- | ----- | --- |
DC/DC converter
Input voltage of I/O, LDO_X and
| VDD_IO  |     |     | -0.2  | 3.6  | V   |
| ------- | --- | --- | ----- | ---- | --- |
flash
Supply voltage of backup
| V_BACK  |     |     | -0.2  | 3.6  | V   |
| ------- | --- | --- | ----- | ---- | --- |
domain
| TCXO_IN  | Input voltage on TCXO_IN  |     | -0.2  | 2.9   | V    |
| -------- | ------------------------- | --- | ----- | ----- | ---- |
| RTC_I    | Input voltage on RTC_I    |     | -0.2  | 1.98  | V    |
| Vidig    | Input voltage on PIO      |     | -0.2  | 3.6   | V    |
| Prfin    | RF input power on LNA_IN  |     |       | +15   | dBm  |
360 (@room
| Ptot  | Total power  |     |     |     | mW  |
| ----- | ------------ | --- | --- | --- | --- |
temperature)
| Tjun  | Junction temperature   |     | -40  | +125  | °C  |
| ----- | ---------------------- | --- | ---- | ----- | --- |
| Ts    | Storage temperature    |     | -50  | +150  | °C  |

The ripple voltage of all the input voltages must be within 50 mV.
|      |     |                            |     |                    |     |
| ---- | --- | -------------------------- | --- | ------------------ | --- |
| 24   |     | Electrical Specifications  |     | UC-01-C43 EN R1.1  |     |
| 7.1.2  Recommended Working Conditions  |     |     |     |     |     |     |
| -------------------------------------- | --- | --- | --- | --- | --- | --- |
Table 7-2 Recommended Working Conditions
| Symbol   | Parameter   |     | Min.  | Typical   | Max.  Unit   |     |
| -------- | ----------- | --- | ----- | --------- | ------------ | --- |
Input voltage of internal
| DCDC_IN  |     |     | 2.7  | 3.3  | 3.6  V  |     |
| -------- | --- | --- | ---- | ---- | ------- | --- |
DC/DC converter
Supply voltage of baseband
| V_CORE  |     |     | 0.9  | 1.0  | 1.05  V  |     |
| ------- | --- | --- | ---- | ---- | -------- | --- |
main core and RF
Input voltage of I/O, LDO_X
| VDD_IO  |     |     | 2.7  | 3.3  | 3.6  V  |     |
| ------- | --- | --- | ---- | ---- | ------- | --- |
and flash
Supply voltage of backup
| V_BACK  |     |     | 1.7  | 3.3  | 3.6  V  |     |
| ------- | --- | --- | ---- | ---- | ------- | --- |
domain
| 7.2  Analog Electrical Specifications  |     |     |     |     |     |     |
| -------------------------------------- | --- | --- | --- | --- | --- | --- |
Table 7-3 Analog Electrical Specifications 1
| Symbol   | Parameter   |     | Min.  | Typical   | Max.  Unit   |     |
| -------- | ----------- | --- | ----- | --------- | ------------ | --- |
LDO_X output voltage
|     |     |     | 1.75  | 1.8  | 1.95  V  |     |
| --- | --- | --- | ----- | ---- | -------- | --- |
(1.8 V TCXO)
LDO_X
LDO_X output voltage
|     |     |     | 2.75  | 2.8  | 2.95  V  |     |
| --- | --- | --- | ----- | ---- | -------- | --- |
(2.8 V/2.9 V TCXO)
| LDO_RET   | LDO_RET output voltage   |     | 0.6   | 0.9  | 0.95  V  |     |
| --------- | ------------------------ | --- | ----- | ---- | -------- | --- |
| LDO_C     | LDO_C output voltage     |     | 0.85  | 0.9  | 0.95  V  |     |
| LDO_EX    | LDO_EX output voltage    |     | 0.95  | 1.0  | 1.05  V  |     |
| V_DET8    | Antenna detection input  |     | 2.7   | 3.3  | 3.6  V   |     |
| VDD_ANT9  | Antenna power output     |     | 2.7   | 3.3  | 3.6  V   |     |
|           | 32.768 kHz crystal or    |     |       |      |          |     |
| RTC_I     |                          |     |       |      | 1.98  V  |     |
digital waveform input
| RTC_O  | 32.768 kHz clock output  |     |     |     | 1.98  V  |     |
| ------ | ------------------------ | --- | --- | --- | -------- | --- |
Output voltage of the
| DCDC_OUT  |     |     | 0.9  | 1.0  | 1.05  V  |     |
| --------- | --- | --- | ---- | ---- | -------- | --- |
internal DC/DC converter
TCXO input peak-to-peak
| TCXO_IN_Vpp  |     |     | 0.5   | 0.6  | 1.98  Vpp  |     |
| ------------ | --- | --- | ----- | ---- | ---------- | --- |
voltage
Table 7-4 Analog Electrical Specifications 2: RTC Specifications
Symbol   Parameter   Condition   Min.   Typical   Max.  Unit
RTC crystal oscillator
| RTC_Fxtal  |     |     |     | 32768  |   Hz  |     |
| ---------- | --- | --- | --- | ------ | ----- | --- |
resonate frequency
8 Not supported currently.
9 The output voltage of VDD_ANT = V_DET- (antenna current) * (10 Ω).
| UC-01-C43 EN R1.1   |     | Electrical Specifications  |     |     |     | 25  |
| ------------------- | --- | -------------------------- | --- | --- | --- | --- |
UC6580 Datasheet
Symbol   Parameter   Condition   Min.   Typical   Max.  Unit
| RTC_T_start  | RTC startup time      |              | 0.2  1  | 2   | s   |
| ------------ | --------------------- | ------------ | ------- | --- | --- |
| RTC_CL       | RTC load capacitance  | ESR = 80 kΩ  |   12.5  |     | pF  |
Shared RTC
RTC low level input
| RTC_Vil  |     | oscillator  | 0.0    | 0.2  | V   |
| -------- | --- | ----------- | ------ | ---- | --- |
voltage
input
Shared RTC
RTC high level input
| RTC_Vih  |     | oscillator  | 0.9    | 1.98  | V   |
| -------- | --- | ----------- | ------ | ----- | --- |
voltage
input
| 7.3  RF Electrical Specifications  |     |     |     |     |     |
| ---------------------------------- | --- | --- | --- | --- | --- |
Table 7-5 RF Electrical Specifications
Symbol   Parameter   Condition   Min.  Typical   Max.   Unit
Receiver input
| L1_IN  |     |     |           |           |            |
| ------ | --- | --- | --------- | --------- | ---------- |
|        |     |     | 1559.098  | 1561.098  | 1606  MHz  |
frequency
Receiver input
| L5_IN  |     |     | 1166.45  | 1176.45  | 1217.14  MHz  |
| ------ | --- | --- | -------- | -------- | ------------- |
frequency
LNA input
| LNA_IN  |     |     |     | 50  |   Ω  |
| ------- | --- | --- | --- | --- | ---- |
impedance
LNA_S11  LNA input return loss  50Ω environment    -10    dB
Receiver cascaded
| NFtot  |     | 50Ω environment  |     | 5   |   dB  |
| ------ | --- | ---------------- | --- | --- | ----- |
noise figure
Ext_Gain  External LNA gain  50Ω environment  15  17  60  dB
| TCXO_Freq  | TCXO frequency  | 0.5ppm  |     | 26  |   MHz  |
| ---------- | --------------- | ------- | --- | --- | ------ |
|     |     |     |     |     |     |
| --- | --- | --- | --- | --- | --- |
 When the external LNA gain falls into this range, the system’s CN0 fluctuates by 1dB.
| 26   |     | Electrical Specifications  |     | UC-01-C43 EN R1.1  |     |
| ---- | --- | -------------------------- | --- | ------------------ | --- |
8  Mechanical Dimensions
8.1  UC6580A (QFN40 Automotive)
Figure 8-1 UC6580A (QFN40 Automotive) Mechanical Dimensions
|                     |                        |     |
| ------------------- | ---------------------- | --- |
| UC-01-C43 EN R1.1   | Mechanical Dimensions  | 27  |
UC6580 Datasheet
8.2  UC6580I (QFN40 Industrial)
Figure 8-2 UC6580I (QFN40 Industrial) Mechanical Dimensions
|      |                        |                    |
| ---- | ---------------------- | ------------------ |
| 28   | Mechanical Dimensions  | UC-01-C43 EN R1.1  |
9  Reflow Soldering
The reflow soldering temperature curve is recommended as shown in Figure 9-1 below
(M705-GRN360 is recommended for solder paste).
Figure 9-1 Reflow Soldering Temperature Curve (QFN40)
|                     |                   |     |
| ------------------- | ----------------- | --- |
| UC-01-C43 EN R1.1   | Reflow Soldering  | 29  |
UC6580 Datasheet
Product Appearance and Packaging
10.1  Product Appearance
UC6580A (QFN40 Automotive)               UC6580I (QFN40 Industrial)
Figure 10-1 UC6580 Product Appearance
10.2
Label
|     | L o t ID | P r o d u c t N | a m e |
| --- | -------- | --------------- | ----- |
D a t e  C o d e
R e s e r v e d  C o d e
P in 1  M a r k
Figure 10-2 Label Description
Table 10-1 Code Description
| Code    | Description       |     |     |
| ------- | ----------------- | --- | --- |
| UC6580  | Product model     |     |     |
| A       | Automotive grade  |     |     |
| I       | Industrial grade  |     |     |
| 30   | Product Appearance and Packaging  |     | UC-01-C43 EN R1.1  |
| ---- | --------------------------------- | --- | ------------------ |
|     | 10.3     |                               |          |                       |              |                |
| --- | -------- | ----------------------------- | -------- | --------------------- | ------------ | -------------- |
|     |          | Ordering Information          |          |                       |              |                |
|     | Table    |   10-2   Ordering Information |          |                       |              |                |
|     |          |                               |          | Built-in  Operating   |              | Product        |
|     | Model    | Chip Scale Package            |          |                       | Grade        |                |
|     |          |                               |          | Flash  Temperature    |              | Package        |
|     |          | QFN40                         |          | -40 °C to             |              | Tape & Reel,   |
|     | UC6580A  |                               |          | Yes                   | Automotive   |                |
|     |          | 5 mm × 5 mm ×                 |  0.85mm  | 105 °C                |              | 3000 pcs/reel  |
|     |          | QFN40                         |          |                       |              | Tape & Reel,   |
|     | UC6580I  |                               |          | Yes  -40 °C to 85 °C  | Industrial   |                |
|     |          | 5 mm × 5 mm ×                 |  0.85mm  |                       |              | 3000 pcs/reel  |
|     | UC-01-C43 EN R1.1   |     | Product Appearance and Packaging  |     |     | 31  |
| --- | ------------------- | --- | --------------------------------- | --- | --- | --- |
和芯星通科技（北京）有限公司
Unicore Communications, Inc.
北京市海淀区丰贤东路7号北斗星通大厦三层
F3, No.7, Fengxian East Road, Haidian, Beijing, P.R.China,
www.unicorecomm.com
Phone: 86-10-69939800
Fax: 86-10-69939888
info@unicorecomm.com
www.unicorecomm.com
