STANDARD POSITIONING
PROTOCOL
WWW.UNICORECOMM.COM
Standard Positioning
Products
Copyright© 2009-2022, Unicore Communications, Inc.

Revision History
| Version  | Revision History  | Date  |
| -------- | ----------------- | ----- |

Ver. 1.0.0
|     | Initial release revision  | Jul, 2017   |
| --- | ------------------------- | ----------- |
Primary
Ver.2.0.0
|     | Perfect the NMEA protocol  | Aug, 2017  |
| --- | -------------------------- | ---------- |
Alpha release
Ver.3.2.1 Beta
|     | Mass production release  | Dec, 2017  |
| --- | ------------------------ | ---------- |
release
  Add BD3 satellite with the number of 15-37
  Update QZSS ID to 193/194/195/199
Ver.3.3.0 Beta
|     |   Limited to output GGA/GSA/GSV/RMC messages by  | Apr, 2019  |
| --- | ------------------------------------------------- | ---------- |
release
default

Add AIDTIME/AIDPOS/AIDINFO command
| Ver.3.4.0 Beta  |   Add SBAS  |     |
| --------------- | ------------ | --- |
Jun, 2019
release
  Add echo with #...
  Revise table 1.4.2.2
  Revise CFGPRT echo
  Revise CFGDYN echo
| R3.5  |     | 2020-03-30  |
| ----- | --- | ----------- |
  Revise GGA Altref
  Revise RMC spd
  Add CFGGEOID and discrepancy instructions for
multi-version firmware
| R4  |     | 2020-09-12  |
| --- | --- | ----------- |
  Add protocols supported by R3.2.10.0
| R4.1  |    | 2020-12-04  |
| ----- | --- | ----------- |
Add protocols supported by R3.2.10.100
| R4.2  |    | 2020-12-31  |
| ----- | --- | ----------- |
Update parameters of NMEA message
  Add CFGBLK command
  Add FCTATEST command and update talker ID of
GLL/VTG/GST
| R4.3  |   Remove CFGDYN command  | 2021-04-08  |
| ----- | ------------------------- | ----------- |
  Change the bit2 of CFGSAVE mask field to reserve
  Update checksum description in section 1.2
  Update default configuration of NOTICE
i

Standard Positioning Products Protocol Specification
 Add CFGDYN, and revise the parameter description of
CFGMOD
R4.4  Update the field description of ANTSTAT1/ 2021-06-24
CFGLOWPOWER
 Update NAVTIME
 Add ABNORMAL message
R4.5 2021-10-15
 Add the baud rate of R3.4.0.19 version
 Add EPHABNORMAL command;
 Add information about the firmware R3.2.20.100;
R4.6 2022-03-28
 Revise the message syntax, examples, and
descriptions according to the test results
Disclaimer
This manual provides information about the products of Unicore Communications, Inc.
This document does not transfer the patent, trademark, copyright of the company or any
third party, or any right or permission under it by implication, estoppel or otherwise.
Except as stated in the sales terms and conditions of the products, the company shall
not assume any other responsibilities. Furthermore, Unicore Communications, Inc.
makes no warranty, express or implied, for the sale and/or use of its products, including
the suitability of a particular purpose and marketability of products, or liability for
infringement of any patent, copyright or other intellectual property rights. If the
connection or operation is not in accordance with the manual, the company is not liable.
Unicore Communications, Inc. may make changes to product specifications and product
descriptions at any time without prior notice.
The company's products may contain certain design defects or errors, which will be
included in the corrigendum once found, and may therefore result in differences
between the products’ actual specifications and the published ones. Updated
corrigendum is available upon request.
Before placing an order, please contact our company or local distributors for the latest
specifications.
* Unicore Communications, UNICORECOMM, UFirebird and its logo have been applied
for trademark registration by Unicore Communications, Inc.
Other names and brands are the property of their respective owners.
ii

I
Standard Positioning Products Protocol Specification
1 General Protocol
1.1 Messages
In the Unicore protocol, input and output statements are collectively called messages.
Each message is a string of full ASCII characters.
The basic format of the message is:
$MSGNAME,data1,data2,data3,…[*CC]\r\n
All messages contain three data blocks:
The first data block is the message header, which starts with ‘$’ (0x24）.
The second data block is the data field consisting of a number of parameters or data. The
message header and data field are separated by ‘,’ (0x2C).
The last data block is an optional checksum, which is separated from the previous data
with ‘*'(0x2A).
The input message ends with ‘\r' (0x0D) or ‘\n' (0x0A) or any combination of the two.
The output message ends with ‘\r\n'. The total length of each message does not exceed
128 bytes.
Message header and parameters, as well as letters in checksums are not case-sensitive.
Certain parameters of certain input commands can be omitted (marked as optional in the
command description). These parameters can be empty, that is, there is no character
between the two commas.
Then, if there is no special instruction, this parameter will be ignored and the options it
controls will remain unchanged.
Most of the message headers can be used for both input commands and output
messages. The same message header is used as input to set parameters or to query the
current configuration, and as output to output receiver information or configuration.
1.2 Checksum
The two characters after ‘*'(0x2A) in the message are the checksum, which is calculated
as the xor of all characters (excluding ‘$' and ‘*') from ‘$' to ‘*', in hexadecimal (calculate
the checksum according to the uppercase letters corresponding to the input
information).
The checksum in the input command is optional. If the input statement contains ‘*'
followed by the two characters, the checksum is examined. If the examination result is
wrong, the command is not executed, and the receiver outputs the $fail message, in
which a checksum error is indicated. If the statement does not contain a checksum, the
command is executed directly.
If the parameters of the input message are empty and a checksum needs to be added, it
should be followed by ‘,'. It's not allowed to add an extra ‘,' when the parameter is not
null.
Example: $PDTINFO,*62
The output message always contains a checksum. The description of the checksum in
the Unicore protocol will be omitted in the following message definition.
1.3 Formats
In the Unicore protocol, the data in the message contains the following types:
String (STR)
The string consists of up to 32 ASCII characters except ‘\ r' and ‘\ n', such as GPSL1.
Unsigned Integer（UINT）
Unsigned integers range from 0 to 4294967295, and are defined in both decimal and
hexadecimal. A decimal unsigned integer consists of ASCII characters from 0 to 9. Such
as 123, 4291075193. A hexadecimal unsigned integer starts with the character h or H,
followed by a string of 0 to 9 and a-f (or A-F), with a maximum of 8 characters
(excluding the starting h or H). Such as hE10, hE41BA7C0.
Unsigned Long Integer (UINT64)
Unsigned long integers range from 0~18446744073709551615, and are defined in both
decimal and hexadecimal. A decimal unsigned long integer consists of ASCII characters
from 0 to 9. Such as 123 and 4291075193. A hexadecimal unsigned long integer starts
with h, or H, or 0x, or 0X, followed by a string of characters from 0 to 9 and a-f (or A-F),
with a maximum of 16 characters (excluding the starting h, H, 0x, or 0X). Such as hE10
and hE41BA7C0, or 0xFFFFFF and 0XFFFFF.
Standard Positioning Products Protocol Specification
Signed Integer (INT)
Signed integers are composed of ASCII characters from 0 to 9 and a negative sign, with
a range of-2147483648 to 2147483647. Such as 123217754, -245278.
DOUBLE
Double-precision floating-point data consists of ASCII characters from 0 to 9, with a
negative sign and a decimal point, ranging from -2^1023 to 2^1023. Such as 3.1415926,
-9024.12367225
1.4 Coordinates
The position value observed by the product is based on the WGS84 coordinate system.
If the position value of the output message is based on other coordinate system, it will
be noted in the message explanation of this documentation. If the user expects to use a
different reference coordinate, the position is very likely to be off by tens or even
hundreds of meters.
1.5 Message Definition
1.5.1 Common Message
1.5.1.1 PDTINFO
Read Product Information
Syntax $PDTINFO
Example $PDTINFO
Read product information, the receiver outputs PDTINFO message after
Description
receiving this command
Input/Output Input
No parameters
Output the Product Information
Syntax $PDTINFO,pdtName,config,hwVer,fwVer,PN,SN
$PDTINFO,UM220,G1B1,V4.1,R3.0Build13260,080101000001,000101114
Example
Description  The receiver outputs the message of the product information
| Input/Output  | Output  |     |
| ------------- | ------- | --- |
Parameter Definition
| Parameter  | Format  | Description            |
| ---------- | ------- | ---------------------- |
| pdtName    | STR     | Product name           |
| Config     | STR     | Product configuration  |
| hwVer      | STR     | Hardware version       |
| fwVer      | STR     | Firmware version       |
| PN         | STR     | Product number         |
| SN         | STR     | Product serial number  |
| 1.5.1.2       | RESET                      |     |
| ------------- | -------------------------- | --- |
| Syntax        | $RESET,type,clrMask        |     |
| Example       | $RESET,0,h01 (Warm start)  |     |
| Description   | Receiver reset             |     |
| Input/Output  | Input                      |     |
Parameter Definition
| Parameter  | Format  | Description  |
| ---------- | ------- | ------------ |
Reset type
0 - Software reset
UINT
| type  |     | 1 - Chip-level reset (watchdog reset)  |
| ----- | --- | -------------------------------------- |
(optional)
2 - Board-level reset
3 - Receiver stopped
UINT
Reset to clear the receiver's saved information. Setting the
clrMask
|     | (optional)  | bit to 1 clears the signal upon reset.  |
| --- | ----------- | --------------------------------------- |
Standard Positioning Products Protocol Specification
bit0 – Clear ephemeris
bit1 – Reserve0
bit2 – Clear receiver position and time
bit3 – Reserve1
bit4 – Clear ionospheric correction and UTC parameters
bit5 – Reserve2
bit6 – Reserve3
bit7 – Clear almanac
Several commonly used startup methods, the parameters
are listed as follow:
h00 – Hot start
h01 – Warm start
h85 – Cold start
The parameter of the cold start reset command is h85, and the mismatch of the reset
parameters will cause the receiver to start in an incorrect state
When a leap second occurs, the receiver may take up to 25 minutes to sync to UTC time
after a cold start reset.
1.5.1.3 Command Echo(1)
Syntax #User command
Example #CFGPRT,1,h0,115200,3,35
Description Display the current Unicore command input by users
Input/Output Output
No parameters
(1): This command is only supported by the firmware of R3.2.10.0, R3.2.20.0, R3.4.0.0 and
above.
| 1.5.1.4  | OK   |     |
| -------- | ---- | --- |
| Syntax   | $OK  |     |
| Example  | $OK  |     |
A response that the receiver executes command correctly
Description
The message is only output on the port where the command is received
| Input/Output  | Output  |     |
| ------------- | ------- | --- |
No parameters
| 1.5.1.5  | FAIL             |     |
| -------- | ---------------- | --- |
| Syntax   | $FAIL,errorCode  |     |
| Example  | $FAIL,0          |     |
A response that the input command is incorrect
Description
The message is only output on the port where the command is received
| Input/Output  | Output  |     |
| ------------- | ------- | --- |
Parameter Definition
| Parameter  | Format  | Description  |
| ---------- | ------- | ------------ |
Error code
errorCode  UINT  0 –Illegal instruction or incorrect parameter
1 –Checksum error
|     |     |     |
| --- | --- | --- |
Standard Positioning Products Protocol Specification
1.5.2  Config Message
| 1.5.2.1  | CFGPRT  |     |
| -------- | ------- | --- |
Read Port Configuration
| Syntax   | $CFGPRT,portID  |     |
| -------- | --------------- | --- |
| Example  | $CFGPRT,1       |     |
Read the receiver port configuration. The receiver outputs a CFGPRT
Description
message after receiving this command.
| Input/Output  | Input  |     |
| ------------- | ------ | --- |
Parameter Definition
| Parameter  | Format  | Description      |
| ---------- | ------- | ---------------- |
|            | UINT    | Port number,1~2  |
portID
(optional)
If empty, output the current port configuration
Set/Output the Port Configuration
| Syntax        | $CFGPRT,portID,addr,baud,inProto,outProto  |     |
| ------------- | ------------------------------------------ | --- |
| Example       | $CFGPRT,1,h0,115200,1,3                    |     |
| Description   | Set or output the port configuration       |     |
| Input/Output  | Input/output                               |     |
Parameter Definition
| Parameter  | Format  | Description  |
| ---------- | ------- | ------------ |
Port number:
1 – UART1
UINT
portID
(optional)
2 – UART2
If empty, configure the current UART
| addr  | UINT  | H0                             |
| ----- | ----- | ------------------------------ |
| baud  | UINT  | Optional baud rate for UART:   |
(optional) 9600/115200/230400/460800(1)
Meaningless if the output port is not UART.
Input protocol of the port, enabled by setting the
corresponding bit to 1
bit0 – UNICORE(2)
UINT
bit5 - Reserve
inProto
(optional)
bit7 - RTCM3.2
bit9 - Reserve
bit10 - Reserve
Output protocol of the port, enabled by setting the
corresponding bit to 1
bit0 - UNICORE
UINT
outProto
bit1 - NMEA
(optional)
bit2 - RTCM3.2 (Set the baud rate to 115200 and above.)
bit5 - Command echo(3)
(1) The baud rate of 460800 is only supported by firmware R3.4.0.19.
(2) COM1 cannot disable the Unicore input protocol.
(3) Only supported by firmware R3.2.10.0, R3.2.20.0, and R3.4.0.0.
The format of A-GNSS assisted positioning data complies with RTCM3.2.
Concurrent output of RTCM 3.2 protocol at two serial ports is not supported.
When continuously sending CFGPRT command, the time interval should be larger than 1s.
1.5.2.2 CFGMSG
Read Message Output Configuration
Syntax $CFGMSG,msgClass,msgID
Example $CFGMSG,0,1
Standard Positioning Products Protocol Specification
Read the message configuration. The receiver outputs CFGMSG
Description
configuration information after receiving this command.
| Input/Output  | input  |     |     |
| ------------- | ------ | --- | --- |
Parameter Definition
| Parameter  | Format  | Description    |     |
| ---------- | ------- | -------------- | --- |
| msgClass   | UINT    | Message class  |     |
| msgID      | UINT    | Message ID     |     |
Set/Output Message Output Configuration
| Syntax        | $CFGMSG,msgClass,msgID,rate                     |     |     |
| ------------- | ----------------------------------------------- | --- | --- |
| Example       | $CFGMSG,0,1,1                                   |     |     |
| Description   | Set or output the message output configuration  |     |     |
| Input/Output  | Input/output                                    |     |     |
Parameter Definition
| Parameter  | Format  | Description    |     |
| ---------- | ------- | -------------- | --- |
| msgClass   | UINT    | Message class  |     |
UINT
| msgID  |     | Message ID  |     |
| ------ | --- | ----------- | --- |
(optional)
| switch  | UINT  | Message control  |     |
| ------- | ----- | ---------------- | --- |
| Message  | Class  | ID  | Message Control  |
| -------- | ------ | --- | ---------------- |
0: disable
0  0
GGA
1: enable
0: disable
0  1
GLL
1: enable
0: disable
0  2
GSA
1: enable
0: disable
|     | 0   | 3   |     |
| --- | --- | --- | --- |
GSV
1: enable
0: disable
|     | 0   | 4   |     |
| --- | --- | --- | --- |
RMC
1: enable
0: disable
|     | 0   | 5   |     |
| --- | --- | --- | --- |
VTG
1: enable
0: disable
|     | 0   | 6   |     |
| --- | --- | --- | --- |
ZDA
1: enable
0: disable
| GST  | 0   | 7   |     |
| ---- | --- | --- | --- |
1: enable
0: disable
| NAVPOS  | 1   | 0   |     |
| ------- | --- | --- | --- |
1: enable
0: disable
| NAVVEL  | 1   | 1   |     |
| ------- | --- | --- | --- |
1: enable
0: disable
|     | 1   | 2   |     |
| --- | --- | --- | --- |
NAVTIME
1: enable
0: disable
|     | 1   | 3   |     |
| --- | --- | --- | --- |
NAVACC
1: enable
0: disable
|     | 3   | 0   |     |
| --- | --- | --- | --- |
LSF
1: enable
0: disable
|     | 3   | 1   |     |
| --- | --- | --- | --- |
ANTSTAT
1: enable
| Reserved  | Reserved  | Reserved  | Reserved    |
| --------- | --------- | --------- | ----------- |
|           | 3         | 3         | 0: disable  |
ANTSTAT1
1: enable
0: disable
|     | 6   | 0   |     |
| --- | --- | --- | --- |
NOTICE(1)
1: enable
0: disable
|     | 6   | 4   |     |
| --- | --- | --- | --- |
ABNORMAL(2)
5: enable
Standard Positioning Products Protocol Specification
0: disable
| EPHABNORMAL(3)  | 6   | 2   |
| --------------- | --- | --- |
10: enable
(1): Only supported by the firmware R3.2.10.0, R3.2.10.100 and R3.2.20.100.
(2): Only supported by the firmware R3.4.0.19.
(3): Only supported by the firmware R3.2.20.0.
| 1.5.2.3  | CFGTP  |     |
| -------- | ------ | --- |
Read Timing Pulse Configuration
| Syntax   | $CFGTP  |     |
| -------- | ------- | --- |
| Example  | $CFGTP  |     |
Read the current timing pulse configuration. The receiver outputs a CFGTP
Description
message after receiving this command.
| Input/Output  | Input  |     |
| ------------- | ------ | --- |
No parameters
Set Timing Pulse Configuration
| Syntax        | $CFGTP,interval,length,flag,antDelay,rfDelay,usrDelay  |     |
| ------------- | ------------------------------------------------------ | --- |
| Example       | $CFGTP,1000000,500000,1,0,800,0                        |     |
| Description   | Set or output timing pulse configuration               |     |
| Input/Output  | Input/output                                           |     |
Parameter Definition
| Parameter  | Format  | Description  |
| ---------- | ------- | ------------ |
UINT
interval  Timing pulse frequency, unit: μs; set to 1000000 by default
(optional)
Timing pulse width, unit: μs, and the maximum value is no
UINT
more than interval – 1 μs. (High-level width when the rising
length
|     | (optional)  | edge is aligned with the integral timing pulse frequency ,  |
| --- | ----------- | ----------------------------------------------------------- |
and low-level width when the falling edge is aligned with
the integral timing pulse frequency )
Configuration of the timing pulse:
bit0
0 – Disable the time pulse output
1 – Enable the time pulse output
bit1
0 – Rising edge aligned to an integral number of seconds
UINT
flag 1 – Falling edge aligned to an integral number of seconds
(optional)
bit2
0 – Output when the timing is reliable
1 – Always output timing pulses
bit3
0 –Disable TIMTP
1 – Enable TIMTP
INT
antDelay Antenna delay, unit: ns; (-32768 ~ 32767)
(optional)
INT
rfDelay RF unit delay, unit: ns; (-32768~32767)
(optional)
User-set delay, unit: ns, range: -32768~32767
INT
Set the latency to a negative value leads the delay of timing
usrDelay
(optional) pulse edge. Modify the delay may decrease the precision of
the pulse in the completely tuning period.
Standard Positioning Products Protocol Specification
| 1.5.2.4  | CFGNMEA  |     |
| -------- | -------- | --- |
Read NMEA Configuration
| Syntax   | $CFGNMEA  |     |
| -------- | --------- | --- |
| Example  | $CFGNMEA  |     |
Read the NMEA configuration. The receiver outputs a CFGNMEA message
Description
after receiving this command.
| Input/Output  | Input  |     |
| ------------- | ------ | --- |
No parameters
Set/Output NMEA Configuration
| Syntax        | $CFGNMEA,nmeaVer                      |     |
| ------------- | ------------------------------------- | --- |
| Example       | $CFGNMEA,h30                          |     |
| Description   | Set or output the NMEA configuration  |     |
| Input/Output  | Input/output                          |     |
Parameter Definition
| Parameter  | Format  | Description  |
| ---------- | ------- | ------------ |
Output NMEA protocol version
h30 – Extend Beidou related statements based on NMEA
| nmeaVer  | UINT  | standard version 3.0 (NMEA 3.0)  |
| -------- | ----- | -------------------------------- |
h51 –Extend Beidou related statements based on standard
NMEA 4.1 (NMEA 4.1)
| 1.5.2.5  | CFGSYS  |     |
| -------- | ------- | --- |
Read Satellite System Configuration
| Syntax   | $CFGSYS  |     |
| -------- | -------- | --- |
| Example  | $CFGSYS  |     |
Read the current satellite system configuration. The receiver outputs a
Description
CFGSYS message after receiving this command.
| Input/Output  | Input  |     |
| ------------- | ------ | --- |
No parameters
Set/Output Satellite System Configuration
| Syntax   | $CFGSYS,sysMask  |     |
| -------- | ---------------- | --- |
| Example  | $CFGSYS,H11      |     |
Set or output satellite system configuration.
Description
The receiver automatically reset while receiving the command, the
configuration of enabling the satellite system works after the reset.
| Input/Output  | Input/output  |     |
| ------------- | ------------- | --- |
Parameter Definition
| Parameter  | Format  | Description  |
| ---------- | ------- | ------------ |
H01(1) –GPS L1+SBAS+QZSS joint positioning
H10 – BDS B1
| sysMask  | UINT  | H101(2)– GPS+GLONASS+GALILEO+SBAS+QZSS joint  |
| -------- | ----- | --------------------------------------------- |
positioning
H11(3)– GPS+BDS+GALILEO+SBAS+QZSS joint positioning
(1): Firmware below R3.4.0.0 only supports GPS L1++QZSS joint positioning, R3.4.0.0 and
above support GPS L1 + SBAS + QZSS joint positioning
(2): Only supported by the firmware with the version of R3.4.0.0 and above
(3): Firmware below R3.4.0.0 only supports GPS L1+BDS B1+QZSS, R3.4.0 and above
support GPS+BDS+GALILEO+SBAS+QZSS joint positioning.
| 1.5.2.6  | CFGDYN  |     |
| -------- | ------- | --- |
Read Dynamic Configuration
| Syntax   | $CFGDYN  |     |
| -------- | -------- | --- |
| Example  | $CFGDYN  |     |
Standard Positioning Products Protocol Specification
|     | Read  the  current  | dynamic  configuration.  | The  receiver  | outputs  a  |
| --- | ------------------- | ------------------------ | -------------- | ----------- |
Description
CFGDYN message after receiving this command.
| Input/Output  | Input  |     |     |     |
| ------------- | ------ | --- | --- | --- |
No parameters
Set/Output Dynamic Configuration
| Syntax   | $CFGDYN,mask,dynModel,staticHoldThresh  |     |     |     |
| -------- | --------------------------------------- | --- | --- | --- |
| Example  | $CFGDYN,h01,0,1000                      |     |     |     |
Set/output dynamic configuration
Description
| Input/Output  | Input/output  |     |     |     |
| ------------- | ------------- | --- | --- | --- |
Parameter Definition
| Parameter  | Format  | Description  |     |     |
| ---------- | ------- | ------------ | --- | --- |
Domain to be configured, enabled by
|     | UINT  | setting the corresponding bit to 1   |     |     |
| --- | ----- | ------------------------------------ | --- | --- |
Mask
bit0 – dynModel
(optional)
bit1 - staticHoldThresh
Dynamic model
0 – Portable
| dynModel  | UINT  |     |     |     |
| --------- | ----- | --- | --- | --- |
1 – Static
The current default configuration is 0
|     |     | Speed  threshold  | in  cm/s  | in  static  hold  |
| --- | --- | ----------------- | --------- | ----------------- |
mode
| staticHoldThresh  | UINT  |     |     |     |
| ----------------- | ----- | --- | --- | --- |
If this value is 0, static hold mode is off
1.5.2.7  CFGGEOID
Read Elevation Configuration
| Syntax  $CFGGEOID   |     |     |
| ------------------- | --- | --- |
| Example  $CFGGEOID  |     |     |
Read the current elevation configuration. The receiver outputs a CFGGEOID
Description
message after receiving this command.
| Input/Output  Input  |     |     |
| -------------------- | --- | --- |
No parameters
Set/Output Elevation Configuration
| Syntax        | $CFGGEOID,Model                     |     |
| ------------- | ----------------------------------- | --- |
| Example       | $CFGGEOID,0                         |     |
| Description   | Set/Output elevation Configuration  |     |
| Input/Output  | Input/output                        |     |
Parameter Definition
| Parameter  | Format  | Description                        |
| ---------- | ------- | ---------------------------------- |
|            | UINT    | 0 - Elevation output is ellipsoid  |
Model
|     | (optional)  | 1 - Elevation output is altitude  |
| --- | ----------- | --------------------------------- |
1.5.2.8  CFGSAVE
| Syntax  $CFGSAVE   |     |     |
| ------------------ | --- | --- |
| Example  $CFGSAVE  |     |     |
Description  Save the current receiver configuration, which is stored in NOR Flash
| Input/Output  Input  |     |     |
| -------------------- | --- | --- |
No parameters
Standard Positioning Products Protocol Specification
Do NOT cut off the power within one second after inputting $cfgsave. Power off will
damage the configuration of the receiver, and the receiver will be restored to factory
settings.
1.5.2.9 CFGCLR
Syntax $CFGCLR
Example $CFGCLR
Clear the current receiver configuration, the current configuration and
Description that saved in Flash are restored to factory settings concurrently, which
takes effect after the receiver is reboot or powered on again.
Input/Output Input
No parameters
The configuration modified by this command takes effect after resetting the receiver.
For R3.4.0.0 version, the configuration will not be cleared after sending this command.
1.5.2.10 CFGCWOUT
Query Interference Detection Command Configuration
Syntax $CFGCWOUT
Example $CFGCWOUT
Query the output configuration of the interference detection command
Description
CWOUT.
Input/Output Input
No parameters
Set Interference Detection Command Configuration
Syntax $CFGCWOUT,CWOutCtrl
Example $CFGCWOUT,1
Description Control the output of interference detection command
| Input/Output  | Input/output  |     |
| ------------- | ------------- | --- |
Parameter Definition
| Parameter  | Format  | Description  |
| ---------- | ------- | ------------ |
1: Enable CWOUT output
| CWOutCtrl  | UINT  |     |
| ---------- | ----- | --- |
0: Disable CWOUT output
| 1.5.2.11  | AIDTIME  |     |
| --------- | -------- | --- |
Syntax  $AIDTIME,year,month,day,hour,minute,second,millisecond
| Example       | $AIDTIME,2018,4,9,17,41,36,200               |     |
| ------------- | -------------------------------------------- | --- |
| Description   | Input time assistance information, UTC time  |     |
| Input/Output  | Input                                        |     |
Parameter Definition
| Parameter    | Format  | Description         |
| ------------ | ------- | ------------------- |
| year         | UINT    | Year, > 1980        |
| month        | UINT    | Month, 1~12         |
| day          | UINT    | Day, 1~31           |
| hour         | UINT    | Hour, 0~23          |
| minute       | UINT    | Minute, 0~59        |
| second       | UINT    | Second, 0~59        |
| millisecond  | UINT    | Millisecond, 0~999  |
 The input information should be in accordance with the current time. Wrong input will
cause the function to fail.
Standard Positioning Products Protocol Specification
| 1.5.2.12      | AIDPOS                                        |     |
| ------------- | --------------------------------------------- | --- |
| Syntax        | $AIDPOS,Latitude,N,Longitude,E,altitude       |     |
| Example       | $AIDPOS,4002.229934,N,11618.096855,E,37.254   |     |
| Description   | Input position assistance information         |     |
| Input/Output  | Input                                         |     |
Parameter Definition
| Parameter  | Format  | Description  |
| ---------- | ------- | ------------ |
Latitude in the format of ddmm.mmmmmm
| Latitude   | DOUBLE  | dd - Degree  |
| ---------- | ------- | ------------ |
mm.mmmmmm - Minute
North or South latitude indication
| N   | STRING  | N - North latitude  |
| --- | ------- | ------------------- |
S - South latitude
Longitude in the format of dddmm.mmmmmm
| Longitude  | DOUBLE  | ddd - Degree  |
| ---------- | ------- | ------------- |
mm.mmmmmm - Minute
East longitude or west longitude indication
| E   | STRING  | E - East longitude  |
| --- | ------- | ------------------- |
W - West longitude
| altitude  | DOUBLE  | Ellipsoid height in the unit of meter  |
| --------- | ------- | -------------------------------------- |
| 1.5.2.13  | AIDINFO  |     |
| --------- | -------- | --- |
Query the Status of Auxiliary Data
| Syntax   | $AIDINFO  |     |
| -------- | --------- | --- |
| Example  | $AIDINFO  |     |
|     | Query  | the  status  of  auxiliary  | data,  the  receiver  | outputs  $AIDINFO  |
| --- | ------ | --------------------------- | --------------------- | ------------------ |
Description
message after receiving this command
| Input/Output  | Input  |     |     |     |
| ------------- | ------ | --- | --- | --- |
No parameters
Output the Status and Types of Auxiliary Data
Syntax  $AIDINFO,GPSRS,GPSUS,BDSRS,BDSUS,GALRS,GALUS,GLORS,GLOUS,AType
| Example       | $AIDINFO,0x0FF7FFFBFF,0x0FF7FFFBFF,,,,,,,0x0311*27  |     |     |     |
| ------------- | --------------------------------------------------- | --- | --- | --- |
| Description   | Output status and types of auxiliary data           |     |     |     |
| Input/Output  | Output                                              |     |     |     |
Parameter Definition
| Parameter  | Format  | Description  |     |     |
| ---------- | ------- | ------------ | --- | --- |
Receiving state of GPS ephemeris, as long as the received data
is verified, the corresponding bit is set to 1.
| GPSRS  | UINT64  |     |     |     |
| ------ | ------- | --- | --- | --- |
If the GPS system is not enabled, this field is empty
GPS ephemeris is valid and can be used for positioning, the
GPSUS  UINT64  corresponding bit is set to 1, if the GPS system is not enabled,
this field is empty
Receiving state of BDS ephemeris, as long as the received data
is verified, the corresponding bit is set to 1.
| BDSRS  | UINT64  |     |     |     |
| ------ | ------- | --- | --- | --- |
If the BDS system is not enabled, this field is empty
BDS ephemeris is valid and can be used for positioning, the
BDSUS  UINT64  corresponding bit is set to 1, if the BDS system is not enabled,
this field is empty
Receiving state of GAL ephemeris, as long as the received data
is verified, the corresponding bit is set to 1.
| GALRS  | UINT64  |     |     |     |
| ------ | ------- | --- | --- | --- |
If the GAL system is not enabled, this field is empty
GAL ephemeris is valid and can be used for positioning, the
| GALUS  | UINT64  |     |     |     |
| ------ | ------- | --- | --- | --- |
corresponding bit is set to 1, if the GAL system is not enabled,
Standard Positioning Products Protocol Specification
this field is empty
Receiving state of GLO ephemeris, as long as the received data
is verified, the corresponding bit is set to 1.
| GLORS  UINT64  |     |     |
| -------------- | --- | --- |
If the GLO system is not enabled, this field is empty
GLO ephemeris is valid and can be used for positioning, the
GLOUS  UINT64  corresponding bit is set to 1, if the GLO system is not enabled,
this field is empty
Auxiliary Type:
bit0-3: with GPS/BDS/GAL/GLO ephemeris assistance
bit4: auxiliary position is valid
Bit5: use auxiliary position
| Atype  UINT  |     |     |
| ------------ | --- | --- |
Bit6-7: reserve
Bit8: auxiliary time is valid
Bit9: use auxiliary time
Bit10-15: reserve
1.5.2.14  CFGMOD
Set to Static Application Mode
| Syntax        | $CFGMOD, Model                           |     |
| ------------- | ---------------------------------------- | --- |
| Example       | $CFGMOD,0                                |     |
| Description   | Set or output positioning configuration  |     |
| Input/Output  | Input                                    |     |
Parameter Definition
| Parameter  | Format  | Description  |
| ---------- | ------- | ------------ |
0 – Disable static application mode
| Model  | UINT  |     |
| ------ | ----- | --- |
1 – Enable static application mode
 Only supported by the firmware with the version of R3.2.10.100 and R3.2.20.100
This mode cannot be used in conjunction with dynamic speed threshold mode (CFGDYN)
1.5.3 NMEA Message
The message format described in this section is for the versions shown as below:
 The version of Beidou related messages extended on the basis of NMEA 3.0
(nmeaVer in CFGNMEA command is h30)
 The version of Beidou related messages extended on the basis of standard NMEA
4.1 ($GBGSA, nmeaVer in CFGNMEA command is h51)
BD3 satellites are involved in NMEA4.1 to support BDS satellites with number of 1~37.
With the increasing of satellites, only output GGA, GSV, GSA and RMC messages by
default to prevent data loss at the rate of 9600. When the amount of data at 9600 baud
rate is allowed, the maximum number of satellites will be output, but it is limited to the
amount of output data at 9600 baud rate. Under the strong sky signal, there will be a
phenomenon of incomplete output of the number of satellites. If the user inputs other
commands, priority is given to ensuring the complete output of the message added by
the user. The number of satellites and satellite information in the GSV message will be
reduced accordingly.
The baud rate of 115200 is supported, which is able to output all satellite information,
and the default output messages include GGA, GSV, GSA and RMC after switching. If
other messages are required, send the command separately.
1.5.3.1 NmeaVer h51
GGA
Syntax $--GGA,time,Lat,N,Lon,E,FS,NoSV,HDOP,msl,M,Altref,M,DiffAge,DiffStation*cs
Example $GPGGA,060845.00,4004.74005,N,11614.19613,E,1,10,0.85,53.5,M,,M,,*7B
Description GNSS positioning data
Input/Output Output
Parameter Definition
Standard Positioning Products Protocol Specification
| Parameter  | Format  | Description  |
| ---------- | ------- | ------------ |
Positioning system flag
GP - GPS+SBAS(1)+QZSS joint positioning
GB - BDS system standalone positioning
| --  | STR  |     |
| --- | ---- | --- |
GA - Galileo system standalone positioning
GL - GLONASS system standalone positioning
GN - Multiple system joint positioning
UTC time, in the format of hhmmss.ss
hh – Hour
| time  | STR  |     |
| ----- | ---- | --- |
mm – Minutes
ss.ss – Seconds
Latitude, in the format of ddmm.mmmmm
| Lat  | STR  | dd - Degrees   |
| ---- | ---- | -------------- |
mm.mmmmm - Minutes
North or south latitude indicator
| N   | STR  | N - North latitude  |
| --- | ---- | ------------------- |
S - South latitude
Longitude, in the format of dddmm.mmmmm
| Lon  | STR  | ddd - Degrees  |
| ---- | ---- | -------------- |
mm.mmmmm - Minutes
East longitude or west longitude indicator
| E   | STR  | E - East longitude  |
| --- | ---- | ------------------- |
W - West longitude
Positioning status indicator
| FS  | UINT  | 0-Invalid  |
| --- | ----- | ---------- |
1-Point positioning
2-Differential positioning
6-Recursive positioning(2)
NoSV UINT Number of satellites participating in positioning
Horizontal dilution of precision, 0.00 ~ 99.99, the value is 99.99
HDOP DOUBLE
when not positioning
Ellipsoid height, fixed output one decimal place or altitude
msl DOUBLE
(CFGGEOID is set to 1)
Unit of ellipsoid height or altitude, specified to constant M. The field
M STR
is empty when not positioning.
Sea level separation, only valid when CFGGEOID is set to 1,
Altref DOUBLE
otherwise it is fixed to empty.
Unit of Sea level separation, specified to constant M. The field is
M STR
empty when not positioning.
Differential correction latency in seconds
DiffAge DOUBLE
Null for non-differential positioning
Reference station ID
DiffStation DOUBLE
Null for non-differential positioning
Checksum
cs STR
A hexadecimal number obtained by calculating an XOR of all
characters from '$' to '*' in this statement
(1): SBAS is only supported by firmware with the version of R3.4.0.0 or above.
(2): Only supported by R3.2.10.0.
GLL
Syntax $--GLL,Lat,N,Lon,E,time,Valid,Mode*cs
Example $GPGLL,4004.74005,N,11614.19613,E,060845.00,A,A*6F
Description Geographic longitude/latitude
Standard Positioning Products Protocol Specification
| Input/Output  | Output  |     |
| ------------- | ------- | --- |
Parameter Definition
| Parameter  | Format  | Description  |
| ---------- | ------- | ------------ |
Positioning system flag
GP - GPS+SBAS(1)+QZSS joint positioning
GB - BDS system standalone positioning
| --  | STR  |     |
| --- | ---- | --- |
GA - Galileo system standalone positioning
GL - GLONASS system standalone positioning
GN - Multiple system joint positioning
Latitude, in the format of ddmm.mmmmm
| Lat  | STR  | dd - Degrees   |
| ---- | ---- | -------------- |
mm.mmmmm - Minutes
North or south latitude indicator
| N   | STR  | N – North latitude  |
| --- | ---- | ------------------- |
S – South latitude
Longitude, in the format of dddmm.mmmmm
| Lon  | STR  | ddd - Degrees  |
| ---- | ---- | -------------- |
mm.mmmmm - Minutes
East longitude or west longitude indicator
| E   | STR  | E – East longitude  |
| --- | ---- | ------------------- |
W – West longitude
UTC time, in the format of hhmmss.ss
hh - Hours
| time  | STR  |     |
| ----- | ---- | --- |
mm - Minutes
ss.ss - Seconds
| Valid  | STR  | Position valid indicator  |
| ------ | ---- | ------------------------- |
V – Invalid
A – Valid
Positioning system mode indicator
N – Not positioning
Mode STR
A – Point positioning
D – Differential positioning
Checksum
cs STR
A hexadecimal number obtained by calculating an XOR of all
characters from '$' to '*' in this statement
(1): SBAS is only supported by firmware with the version of R3.4.0.0 or above.
GSA
$--
Syntax GSA,Smode,FS,sv1,sv2,sv3,sv4,sv5,sv6,sv7,sv8,sv9,sv10,sv11,sv12,PDOP,H
DOP,VDOP,systemID*cs
Example $GPGSA,A,3,02,03,06,09,12,17,19,23,28,25,,,1.34,0.85,1.04,1*1E
Description GNSS dilution of precision and effective satellite information
Input/Output Output
Parameter Definition
Parameter Format Description
Positioning system flag
GP - GPS+SBAS(1)+QZSS joint positioning
GB - BDS system standalone positioning
-- STR
GA - Galileo system standalone positioning
GL - GLONASS system standalone positioning
GN - Multiple system joint positioning
Standard Positioning Products Protocol Specification
Positioning mode specified states
Smode STR M– Manually specify 2D or 3D positioning
A– Automatically switch to 2D or 3D positioning
Positioning mode
1– Not positioning
FS UINT
2– 2D positioning
3– 3D positioning
Participating satellite ID
When there are less than 12 satellites participating in the
positioning, the insufficient area is filled in empty and it only
outputs the first 12 satellites if there are more than 12 satellites
GPS satellite ID is 01 ~ 32
sv1 ~ sv12 UINT
BDS satellite ID is 01 ~ 37
GLO satellite ID is 65 ~ 92(2)
GAL satellite ID is 01 ~ 36(3)
SBAS satellite ID is 33~51(4)
QZSS satellite ID: 193, 194, 195, 199
Position dilution of precision, 0.00~99.99, the value is 99.99
PDOP DOUBLE
when not positioning
Horizontal dilution of precision, 0.00 ~ 99.99, the value is 99.99
HDOP DOUBLE
when not positioning
Vertical dilution of precision, 0.00 ~ 99.99, the value is 99.99
VDOP DOUBLE
when not positioning
GNSS system ID as defined by the NMEA protocol
1 - GPS system ID
systemID UINT 2 - GLO system ID(5)
3 - GAL System ID (6)
4 - BDS System ID
Checksum
cs STR
A hexadecimal number obtained by calculating an XOR of all
characters from '$' to '*' in this statement
(1): SBAS is only supported by firmware with the version of R3.4.0.0 or above.
(2) (3) (4) (5) (6): Only supported by firmware with the version of R3.4.0.0 or above.
GSV
$--
Syntax GSV,NoMsg,MsgNo,NoSv,sv1,elv1,az1,cn01,sv2,elv2,az2,cn02,sv3,elv3,az3,
cn03,sv4,elv4,az4,cn04, signalID*cs
$GPGSV,3,01,11,02,34,277,41,03,16,043,35,05,04,215,35,06,69,333,48,0*57
$GPGSV,3,02,11,09,25,110,41,12,31,305,43,17,55,116,46,19,76,088,46,0*56
$GPGSV,3,03,11,23,23,077,40,25,04,328,32,28,05,171,36,0*67
Example
$GBGSV,3,01,12,01,37,145,42,02,34,225,39,03,44,188,42,04,25,123,37,0*4C
$GBGSV,3,02,12,05,17,249,36,06,30,169,38,07,03,188,31,08,69,027,43,0*4E
$GBGSV,3,03,12,09,09,186,34,10,15,211,36,12,26,306,40,13,60,316,44,0*48
Visible GNSS satellites
Each GSV message contains information for only 4 satellites. When the
Description
number of satellites exceeds 4, the receiver sends multiple GSV messages
continuously
Input/Output Output
Parameter Definition
Parameter Format Description
System identification
GP-GPS satellite information
-- STR
GB-BDS satellite information
GL-GLO satellite Information(1)
Standard Positioning Products Protocol Specification
GA-GAL satellite Information(2)
Total number of GSV messages, the minimum value is 1
NoMsg is the total number of GSV messages in this system, for
NoMsg UINT example:
NoMsg in GPGSV is the total number of GPGSV messages,
excluding the number of GBGSV messages
Number of this GSV message. The minimum value is 1.
MsgNo UINT
MsgNo is the number of the GSV message in this system
NoSv UINT Total number of visible satellites in this system
Satellite number of the first to fourth satellite
GPS satellite number is 01 ~ 32
BDS satellite number is 01 ~ 37
sv1 ~ sv4 UINT GLO satellite number is 65 ~ 92 (3)
GAL satellite number is 01 ~ 36 (4)
QZSS satellites are 193, 194, 195, 199
SBAS(5) satellite number is 33~51
Elevation angle of the first to fourth satellite (0 ~ 90 degrees),
elv1 ~ elv4 UINT
fixed output of 2 digits, add zero up front if less than 2 digits
Azimuth of the first to fourth satellite (0 ~ 359 degrees), fixed
az1 ~ az4 UINT
output of 3 digits, add zero up front if less than 3 digits
CNR of the first to fourth satellite (0 ~ 90dBHz), fixed output of 2
digits, add zero up front if less than 2 digits
cn01~cn04 UINT
fill null for untracked satellites
signalID UINT Signal ID defined by NMEA protocol (fixedly output 0)
Checksum
cs STR
A hexadecimal number obtained by calculating an XOR of all
characters from '$' to '*' in this statement
(1) (2) (3) (4) (5): Only supported by firmware with the version of R3.4.0.0 or above.
 Due to the excessive number of satellites in GN mode, at 9600 baud rate, GSV will have
the problem of incomplete printing of satellite information. For complete satellite
information, please switch the baud rate to 115200
RMC
Syntax  $--RMC,time,status,Lat,N,Lon,E,spd,cog,date,mv,mvE,mode,navStates*cs
Example  $GPRMC,060845.00,A,4004.74005,N,11614.19613,E,,,180817,,,A,V*0B
| Description   | The recommended minimum data  |     |
| ------------- | ----------------------------- | --- |
| Input/Output  | Output                        |     |
Parameter Definition
| Parameter  | Format  | Description  |
| ---------- | ------- | ------------ |
Positioning system flag
GP - GPS+ SBAS (1)+QZSS joint positioning
GB - BDS system standalone positioning
| --  | STR  |     |
| --- | ---- | --- |
GA - Galileo system standalone positioning
GL - GLONASS system standalone positioning
GN - Multiple system joint positioning
UTC time, in the format of hhmmss.ss
hh - Hours
| time  | STR  |     |
| ----- | ---- | --- |
mm - Minutes
ss.ss - Seconds
Position valid indicator
| status  | STR  | V – Invalid  |
| ------- | ---- | ------------ |
A – Valid
Latitude, in the format of ddmm.mmmmm
| Lat  | STR  |     |
| ---- | ---- | --- |
dd - Degrees
Standard Positioning Products Protocol Specification
mm.mmmmm - Minutes
North or south latitude indicator
| N   | STR  | N - North latitude  |
| --- | ---- | ------------------- |
S – South latitude
Longitude, in the format of dddmm.mmmmm
| Lon  | STR  | ddd - Degrees  |
| ---- | ---- | -------------- |
mm.mmmmm - Minutes
East longitude or west longitude indicator
| E   | STR  | E – East longitude  |
| --- | ---- | ------------------- |
W – West longitude
| spd  | DOUBLE  | Speed over ground, unit: knot  |
| ---- | ------- | ------------------------------ |
Course over ground, unit: degree
| cog  | DOUBLE  |     |
| ---- | ------- | --- |
Calculated clockwise from north. If the speed measurement fails
or the static scene speed is extremely small, the output is empty.
UTC date, in the format of ddmmyy
dd - Day
| date  | STR  |     |
| ----- | ---- | --- |
mm - Month
yy - Year
| mv   | DOUBLE  | Magnetic declination, specified to null            |
| ---- | ------- | -------------------------------------------------- |
| mvE  | STR     | Magnetic declination direction, specified to null  |
Positioning mode
N – Not positioning
| mode  | STR  |     |
| ----- | ---- | --- |
A – Point positioning
D – Differential positioning
Navigation states flag, fixedly output ‘V’
| navStates  | STR  |     |
| ---------- | ---- | --- |
V-Device does not provide navigation state information
Checksum
cs STR
A hexadecimal number obtained by calculating an XOR of all
characters from '$' to '*' in this statement
(1): SBAS is only supported by firmware with the version of R3.4.0.0 or above.
VTG
Syntax $--VTG,cogt,T,cogm,M,sog,N,kph,K,mode*cs
Example $GPVTG,,T,,M,0.000,N,0.000,K,A*23
Description Course over ground and ground speed
Input/Output Output
Parameter Definition
Parameter Format Description
Positioning system flag
GP - GPS+SBAS (1)+QZSS joint positioning
GB - BDS system standalone positioning
-- STR
GA - Galileo system standalone positioning
GL - GLONASS system standalone positioning
GN - Multiple system joint positioning
Course over ground with reference to true north (0.00 ~ 359.99
cogt DOUBLE degrees). If the speed measurement fails or the static scene
speed is extremely small, the output is empty.
T STR Course flag, specified to constant T
Course over ground with reference to MN (0.000 ~ 359.999
cogm DOUBLE
degrees). The field is empty by default.
M STR Course flag, specified to constant M
sog DOUBLE Speed over ground, unit: knot
N STR Unit of speed, specified to constant N
Standard Positioning Products Protocol Specification
kph DOUBLE Speed over ground, unit: km/h
K STR Unit of speed, specified to constant K
Positioning mode
N – Not positioning
mode STR
A – Point positioning
D – Differential positioning
Checksum
cs STR
A hexadecimal number obtained by calculating an XOR of all
characters from '$' to '*' in this statement
(1): SBAS is only supported by firmware with the version of R3.4.0.0 or above.
ZDA
Syntax $--ZDA,time,day,mon,year,ltzh,ltzn*cs
Example $GPZDA,060845.00,18,08,2017,00,00*6C
Description Date and time
Input/Output Output
Parameter Definition
Parameter Format Description
Positioning system flag
GP - GPS+SBAS (1)+QZSS joint positioning
GB - BDS system standalone positioning
-- STR
GA - Galileo system standalone positioning
GL - GLONASS system standalone positioning
GN - Multiple system joint positioning
UTC time, in the format of hhmmss.ss
time STR
hh - Hours
mm - Minutes
ss.ss - Seconds
day UINT UTC day with two digits, 01 ~ 31
mon UINT UTC month with two digits, 01 ~ 12
year UINT UTC year with four digits
ltzh UINT Hours in local time zone (fixed output 00)
ltzn UINT Minutes in local time zone (fixed output 00)
Checksum
cs STR
A hexadecimal number obtained by calculating an XOR of all
characters from '$' to '*' in this statement
(1): SBAS is only supported by firmware with the version of R3.4.0.0 or above.
GST
Syntax $--GST,time,rngRMS,stdMajor,stdMinor,hdg,stdLat,stdLon,stdAlt*cs
Example $GPGST,060845.00,0.6,,,,0.07,0.09,0.09*47
Description GNSS pseudorange error statistics
Input/Output Output
Parameter Definition
Parameter Format Description
Positioning system flag
GP - GPS+SBAS (1)+QZSS joint positioning
GB - BDS system standalone positioning
-- STR
GA - Galileo system standalone positioning
GL - GLONASS system standalone positioning
GN - Multiple system joint positioning
time STR UTC time, in the format of hhmmss.ss
Standard Positioning Products Protocol Specification
hh - Hours
mm - Minutes
ss.ss - Seconds
Standard deviation of pseudorange error in meters, with a
rngRMS DOUBLE
maximum of 3750000
Semi-major axis of the error ellipse, in meters.
stdMajor DOUBLE
Specified to null
Semi-minor axis of the error ellipse, in meters.
stdMinor DOUBLE
Specified to null
Semi-major axis direction of the error ellipse in degrees,
hdg DOUBLE
clockwise from north. Specified to null
stdLat DOUBLE Standard deviation of latitude error, in meters
stdLon DOUBLE Standard deviation of longitude error, in meters
stdAlt DOUBLE Standard deviation of altitude error, in meters
Checksum
cs STR
A hexadecimal number obtained by calculating an XOR of all
characters from '$' to '*' in this statement
(1): SBAS is only supported by firmware with the version of R3.4.0.0 or above.
1.5.3.2 NmeaVer h30
GGA
$--
Syntax
GGA,time,Lat,N,Lon,E,FS,NoSV,HDOP,msl,M,Altref,M,DiffAge,DiffStation*cs
$GPGGA,063952.000,4002.229934,N,11618.096855,E,1,4,2.788,37.254,M,0
Example
,M,,*71
Description GNSS positioning data
Input/Output Output
Parameter Definition
| Parameter  | Format  | Description  |
| ---------- | ------- | ------------ |
Positioning system flag
GP - GPS+SBAS(1)+QZSS joint positioning
BD - BDS system standalone positioning
| --  | STR  |     |
| --- | ---- | --- |
GA - Galileo system standalone positioning
GL - GLONASS system standalone positioning
GN - Multiple system joint positioning
UTC time, in the format of hhmmss.sss
hh - Hours
| time  | STR  |     |
| ----- | ---- | --- |
mm - Minutes
ss.sss - Seconds
Latitude, in the format of ddmm.mmmmmm
| Lat  | STR  | dd - Degrees  |
| ---- | ---- | ------------- |
mm.mmmmmm - Minutes
North or south latitude indicator
| N   | STR  | N – North latitude  |
| --- | ---- | ------------------- |
S – South latitude
Longitude, in the format of dddmm.mmmmmm
| Lon  | STR  | ddd - Degrees  |
| ---- | ---- | -------------- |
mm.mmmmmm - Minutes
East longitude or west longitude indicator
| E   | STR  | E – East longitude  |
| --- | ---- | ------------------- |
W – West longitude
| FS  | UINT  | Positioning status indicator  |
| --- | ----- | ----------------------------- |
Standard Positioning Products Protocol Specification
0-Invalid
1-Point positioning
2-Differential positioning
6-Recursive positioning(2)
NoSV  UINT  Number of satellites participating in positioning
| HDOP  | DOUBLE  | Horizontal dilution of precision, 0.0~127.000  |
| ----- | ------- | ---------------------------------------------- |
Ellipsoid height, fixed output one decimal place or altitude
| Msl  | DOUBLE  |     |
| ---- | ------- | --- |
(CFGGEOID is set to 1)
Unit of ellipsoid height or altitude, specified to constant M. The
| M   | STR  |     |
| --- | ---- | --- |
field is empty when not positioning.
Sea level separation, only valid when CFGGEOID is set to 1,
| Altref  | DOUBLE  |     |
| ------- | ------- | --- |
otherwise it is fixed to empty.
Unit of Sea level separation, specified to constant M. The field
| M   | STR  |     |
| --- | ---- | --- |
is empty when not positioning.
Differential correction latency in seconds
| DiffAge  | DOUBLE  |     |
| -------- | ------- | --- |
Null for non-differential positioning
Reference station ID
| DiffStation  | DOUBLE  |     |
| ------------ | ------- | --- |
Null for non-differential positioning
Checksum
| cs  | STR  |     |
| --- | ---- | --- |
A hexadecimal number obtained by calculating an XOR of all
characters from '$' to '*' in this statement
(1): SBAS is only supported by firmware with the version of R3.4.0.0 or above.
(2): Only supported by R3.2.10.0.
GLL
| Syntax  | $--GLL,Lat,N,Lon,E,time,Valid,Mode*cs  |     |
| ------- | -------------------------------------- | --- |
Example  $GPGLL,4002.217867,N,11618.105743,E,123400.000,A,A*5B
| Description   | Geographic longitude/latitude  |     |
| ------------- | ------------------------------ | --- |
| Input/Output  | Output                         |     |
Parameter Definition
| Parameter  | Format  | Description  |
| ---------- | ------- | ------------ |
Positioning system flag
GP - GPS+SBAS(1)+QZSS joint positioning
BD - BDS system standalone positioning
| --  | STR  |     |
| --- | ---- | --- |
GA - Galileo system standalone positioning
GL - GLONASS system standalone positioning
GN - Multiple system joint positioning
Latitude, in the format of ddmm.mmmmmm
| Lat  | STR  | dd - Degrees  |
| ---- | ---- | ------------- |
mm.mmmmmm - Minutes
North or south latitude indicator
| N   | STR  | N – North latitude  |
| --- | ---- | ------------------- |
S – South latitude
Longitude, in the format of dddmm.mmmmmm
| Lon  | STR  | ddd - Degrees  |
| ---- | ---- | -------------- |
mm.mmmmmm - Minutes
East longitude or west longitude indicator
| E   | STR  | E – East longitude  |
| --- | ---- | ------------------- |
W – West longitude
UTC time, in the format of hhmmss.sss
| time  | STR  | hh - Hours  |
| ----- | ---- | ----------- |
mm - Minutes
Standard Positioning Products Protocol Specification
ss.sss - Seconds
Position valid indicator
| Valid  | STR  | V – Invalid  |
| ------ | ---- | ------------ |
A – Valid
Positioning mode
N – Not positioning
| Mode  | STR  |     |
| ----- | ---- | --- |
A – Point positioning
D – Differential positioning
Checksum
| cs  | STR  |     |
| --- | ---- | --- |
A hexadecimal number obtained by calculating an XOR of all
characters from '$' to '*' in this statement
(1): SBAS is only supported by firmware with the version of R3.4.0.0 or above.
GSA
$--
Syntax  GSA,Smode,FS,sv1,sv2,sv3,sv4,sv5,sv6,sv7,sv8,sv9,sv10,sv11,sv12,PDOP,H
DOP,VDOP*cs
| Example  | $GPGSA,A,3,14,22,18,31,,,,,,,,,5.572,2.788,4.824*36  |     |
| -------- | ---------------------------------------------------- | --- |
Description  GNSS dilution of precision and effective satellite information
| Input/Output  | Output  |     |
| ------------- | ------- | --- |
Parameter Definition
| Parameter  | Format  Description  |     |
| ---------- | -------------------- | --- |
Positioning system flag
GP - GPS+SBAS(1)+QZSS joint positioning
| --  | STR  BD - BDS system standalone positioning  |     |
| --- | -------------------------------------------- | --- |
GA - Galileo system standalone positioning
GL - GLONASS system standalone positioning
GN - Multiple system joint positioning
Positioning mode specified states
Smode STR M – Manually specify 2D or 3D positioning
A – Automatically switch to 2D or 3D positioning
Positioning mode
1– Not positioning
FS UINT
2– 2D positioning
3– 3D positioning
Participating satellite ID
When there are less than 12 satellites participating in the
positioning, the insufficient area is filled in empty and it only
output the first 12 satellites if there are more than 12 satellites
GPS satellite ID is 1 ~ 32
sv1 ~ sv12 UINT
BDS satellite ID is 161 ~ 197(160 +BDS PRN)
GLONASS satellite ID is 65 ~ 92(2)
Galileo satellite ID is 101~136(3)
QZSS satellite ID is 193, 194 , 195, 199
SBAS satellite ID is 33~51(4)
PDOP DOUBLE Position dilution of precision, 0.0~127.000
HDOP DOUBLE Horizontal dilution of precision, 0.0~127.000
VDOP DOUBLE Vertical dilution of precision, 0.0~127.000
Checksum
cs STR
A hexadecimal number obtained by calculating an XOR of all
characters from '$' to '*' in this statement
(1): SBAS is only supported by firmware with the version of R3.4.0.0 or above.
(2) (3) (4): Only supported by firmware with the version of R3.4.0.0 or above.
Standard Positioning Products Protocol Specification
GSV
$--
Syntax GSV,NoMsg,MsgNo,NoSv,sv1,elv1,az1,cn01,sv2,elv2,az2,cn02,sv3,elv3,az3,
cn03,sv4,elv4,az4,cn04*cs
$GPGSV,3,1,11,3,82,133,50,6,70,73,50,7,21,311,45,13,46,275,50*75
$GPGSV,3,2,11,16,52,51,49,19,52,194,49,21,12,49,37,23,40,222,49*7C
Example $GPGSV,3,3,11,30,31,69,46,31,8,127,19,1,5,,44*77
$BDGSV,2,1,5,161,35,140,47,163,33,224,47,164,24,124,43,167,47,73,48*54
$BDGSV,2,2,5,168,5,,50*52
Visible GNSS satellites
Each GSV message contains information for only 4 satellites. When the
Description
number of satellites exceeds 4, the receiver sends multiple GSV messages
continuously
Input/Output Output
Parameter Definition
Parameter Format Description
Positioning system flag
GP - GPS+SBAS(1)+QZSS satellite information
-- STR BD - BDS satellite information
GA - Galileo satellite information
GL - GLONASS satellite information
Total number of GSV messages, the minimum value is 1
NoMsg is the total number of GSV messages in this system,
NoMsg UINT for example:
NoMsg in GPGSV is the total number of GPGSV messages,
excluding the number of BDGSV messages
Number of this GSV message. The minimum value is 1.
MsgNo UINT
MsgNo is the number of this GSV message in this system.
Continuous output GPGSV and BDGSV are numbered
separately
NoSv UINT Total number of visible satellites in this system
Satellite number of the first to fourth satellite
GPS satellite number is 1 ~ 32
BDS satellite number is 161 ~ 197 (160 + BDS PRN)
sv1 ~ sv4 UINT GLONASS satellite number is 65 ~ 92(2)
Galileo satellite number is 101~136(3)
QZSS satellite number is 193, 194 , 195 , 199
SBAS(4) satellite number is 33~51
elv1 ~ elv4 UINT Elevation of the first to fourth satellite (0 ~ 90 degrees)
az1 ~ az4 UINT Azimuth of the first to fourth satellite (0 ~ 359 degrees)
CNR of the 1st to 4th satellites (0 ~ 90dBHz), fill null for
cn01~cn04 UINT
untracked satellites
Checksum
cs STR
A hexadecimal number obtained by calculating an XOR of all
characters from '$' to '*' in this statement
(1): SBAS is only supported by firmware with the version of R3.4.0.0 or above.
(2) (3) (4): Only supported by firmware with the version of R3.4.0.0 or above.
Due to the excessive number of satellites in GN mode, GSV at 9600 baud rate will have
the problem of incomplete printing of satellite information. For complete satellite
information, please switch the baud rate to 115200
RMC
Syntax $--RMC,time,status,Lat,N,Lon,E,spd,cog,date,mv,mvE,mode*cs
$GPRMC,123400.000,A,4002.217821,N,11618.105743,E,0.026,181.631,180
Example
411,,E,A*2C
Standard Positioning Products Protocol Specification
Description The minimum recommended data
Input/Output Output
Parameter Definition
Parameter Format Description
Positioning system flag
GP - GPS+SBAS(1)+QZSS joint positioning
BD - BDS system standalone positioning
-- STR
GA - Galileo system standalone positioning
GL - GLONASS system standalone positioning
GN - Multiple system joint positioning
UTC time, in the format of hhmmss.sss
hh - Hours
time STR
mm - Minutes
ss.sss - Seconds
Position valid indicator
status STR V – Invalid
A – Valid
Latitude, in the format of ddmm.mmmmmm
Lat STR dd - Degrees
mm.mmmmmm - Minutes
North or south latitude indicator
N STR N – North latitude
S – South latitude
Longitude, in the format of dddmm.mmmmmm
Lon STR ddd - Degrees
mm.mmmmmm - Minutes
East longitude or west longitude indicator
E STR E – East longitude
W – West longitude
spd DOUBLE Speed over ground, unit: knot
Course over ground, unit: degree
cog DOUBLE
Calculated clockwise from north. If the speed measurement fails
or the static scene speed is extremely small, the output is empty.
UTC date, in the format of ddmmyy
dd - Day
mm - Month
date STR
yy - Year
If the exact year, month, and day are not parsed, the date part
appears blank
mv DOUBLE Magnetic declination, specified to null
mvE STR Magnetic declination direction, specified to constant E
Positioning mode
N – Not positioning
mode STR
A – Point positioning
D – Differential positioning
Checksum
cs STR
A hexadecimal number obtained by calculating an XOR of all
characters from '$' to '*' in this statement
(1): SBAS is only supported by firmware with the version of R3.4.0.0 or above.
Standard Positioning Products Protocol Specification
VTG
| Syntax        | $--VTG,cogt,T,cogm,M,sog,N,kph,K,mode*cs  |     |
| ------------- | ----------------------------------------- | --- |
| Example       | $GNVTG,0.000,T,,M,0.000,N,0.000,K,A*13    |     |
| Description   | Course over groundand ground speed        |     |
| Input/Output  | Output                                    |     |
Parameter Definition
| Parameter  | Format  | Description  |
| ---------- | ------- | ------------ |
Positioning system flag
GP - GPS+SBAS(1)+QZSS joint positioning
BD - BDS system standalone positioning
| --  | STR  |     |
| --- | ---- | --- |
GA - Galileo system standalone positioning
GL - GLONASS system standalone positioning
GN - Multiple system joint positioning
Course over ground with reference to true north (0.000 ~
cogt  DOUBLE  359.999 degrees). If the speed measurement fails or the
static scene speed is extremely small, the output is empty.
| T   | STR  | Course flag, specified to constant T  |
| --- | ---- | ------------------------------------- |
Course over ground with reference to MN (0.000 ~ 359.999
| cogm  | DOUBLE  |     |
| ----- | ------- | --- |
degrees). The field is empty by default.
| M    | STR     | Course flag, specified to constant M    |
| ---- | ------- | --------------------------------------- |
| sog  | DOUBLE  | Speed over ground, unit: knot           |
| N    | STR     | Unit of speed, specified to constant N  |
| kph  | DOUBLE  | Speed over ground, unit: km/h           |
| K    | STR     | Unit of speed, specified to constant K  |
Positioning mode
| mode  | STR  | N – Not positioning  |
| ----- | ---- | -------------------- |
A – Point positioning
Checksum
| cs  | STR  |     |
| --- | ---- | --- |
A hexadecimal number obtained by calculating an XOR of all
characters from '$' to '*' in this statement
(1): SBAS is only supported by firmware with the version of R3.4.0.0 or above.
ZDA
| Syntax        | $--ZDA,time,day,mon,year,ltzh,ltzn*cs  |     |
| ------------- | -------------------------------------- | --- |
| Example       | $GNZDA,083927.000,21,11,2013,00,00*4C  |     |
| Description   | Date and time                          |     |
| Input/Output  | Output                                 |     |
Parameter Definition
| Parameter  | Format  | Description  |
| ---------- | ------- | ------------ |
Positioning system flag
GP - GPS+SBAS(1)+QZSS joint positioning
BD - BDS system standalone positioning
| --  | STR  |     |
| --- | ---- | --- |
GA - Galileo system standalone positioning
GL - GLONASS system standalone positioning
GN - Multiple system joint positioning
UTC time, in the format of hhmmss.sss
hh - Hours
| time  | STR  |     |
| ----- | ---- | --- |
mm - Minutes
ss.sss - Seconds
| day   | UINT  | UTC day with two digits, 01 ~ 31              |
| ----- | ----- | --------------------------------------------- |
| mon   | UINT  | UTC month with two digits, 01 ~ 12            |
| year  | UINT  | UTC year with four digits                     |
| ltzh  | UINT  | Hours in local time zone (fixedly output 00)  |
Standard Positioning Products Protocol Specification
| ltzn  | UINT  | Minutes in local time zone (fixedly output 00)  |
| ----- | ----- | ----------------------------------------------- |
Checksum
| cs  | STR  |     |
| --- | ---- | --- |
A hexadecimal number obtained by calculating an XOR of all
characters from '$' to '*' in this statement
(1): SBAS is only supported by firmware with the version of R3.4.0.0 or above.
1.5.4  Navigation Result Message
| 1.5.4.1  | NAVPOS                                               |     |
| -------- | ---------------------------------------------------- | --- |
| Syntax   | $NAVPOS,time,system,quality,X,Y,Z,lat,lon,height*cs  |     |
$NAVPOS,282201000,5,3,-
Example  2160481.168,4383619.182,4084735.203,40.078998,116.236534,52.84384
7*1C
| Description   | Output the receiver position information  |     |
| ------------- | ----------------------------------------- | --- |
| Input/Output  | Output                                    |     |
Parameter Definition
| Parameter  | Format  | Description  |
| ---------- | ------- | ------------ |
Time corresponding to the positioning solution
time  UINT  The time definition depends on the current positioning
system, the priority is GPS>BDS>GAL>GLO
Current positioning system
bit0-GPS
| system  | UINT  | bit2-BDS  |
| ------- | ----- | --------- |
bit5-GAL
bit4-GLO
Current positioning quality
| quality  | UINT  | 0 - Invalid  |
| -------- | ----- | ------------ |
1 - External configuration
2 - Coarse
3 - Precise
| X   | DOUBLE  | X of ECEF, in meters  |
| --- | ------- | --------------------- |
| Y   | DOUBLE  | Y of ECEF, in meters  |
| Z   | DOUBLE  | Z of ECEF, in meters  |
The latitude of the receiver, which is positive in north latitude
| lat  | DOUBLE  |     |
| ---- | ------- | --- |
and negative in south latitude, in degrees
The longitude of the receiver, which is positive in east
| lon  | DOUBLE  |     |
| ---- | ------- | --- |
longitude and negative in west longitude, in degrees
height  DOUBLE  The ellipsoidal height of the receiver, in meters
Checksum
| cs  | STR  |     |
| --- | ---- | --- |
A hexadecimal number obtained by calculating an XOR of all
characters from '$' to '*' in this statement
| 1.5.4.2       | NAVVEL                                              |     |
| ------------- | --------------------------------------------------- | --- |
| Syntax        | $NAVVEL,time,system,quality,Vx,Vy,Vz,clockDrift*cs  |     |
| Example       | $NAVVEL,282201000,5,3,0.000,0.000,0.000,31.785*2F   |     |
| Description   | Output the receiver velocity information            |     |
| Input/Output  | Output                                              |     |
Parameter Definition
| Parameter  | Format  | Description                            |
| ---------- | ------- | -------------------------------------- |
| time       | UINT    | Same as time definition in NAVPOS      |
| system     | UINT    | Same as system definition in NAVPOS    |
| quality    | UINT    | Same as quality definition in NAVPOS   |
| Vx         | DOUBLE  | Vx of ECEF coordinate system, in m/s   |
| Vy         | DOUBLE  | Vy of ECEF coordinate system, in m/s   |
Standard Positioning Products Protocol Specification
| Vz          | DOUBLE  | Vz of ECEF coordinate system, in m/s       |
| ----------- | ------- | ------------------------------------------ |
| clockDrift  | DOUBLE  | Equivalent speed of crystal drift, in m/s  |
Checksum
cs  STR  A hexadecimal number obtained by calculating an XOR of all
characters from '$' to '*' in this statement
| 1.5.4.3  | NAVTIME  |     |
| -------- | -------- | --- |
$NAVTIME,GPSW,GPST,GPSQ,GLOY,GLOD,GLOT,GLOQ,BDW,BDT,BDQ,BDG
Syntax
PSDiff,GLOGPSDiff*cs
$NAVTIME,1848,282201.000291049,3,0,0,0.000000000,0,492,282187.00
Example
0291134,3,0.000000085,0.000000000*63
| Description   | Output the receiver time information  |     |
| ------------- | ------------------------------------- | --- |
| Input/Output  | Output                                |     |
Parameter Definition
| Parameter  | Format  | Description          |
| ---------- | ------- | -------------------- |
| GPSW       | UINT    | GPS week             |
| GPST       | DOUBLE  | GPS seconds of week  |
GPS time quality
0 - Invalid
| GPSQ  | UINT  | 1 - External configuration  |
| ----- | ----- | --------------------------- |
2 - Coarse
3 - Precise
| GLOY  | UINT    | GLONASS year            |
| ----- | ------- | ----------------------- |
| GLOD  | UINT    | GLONASS day             |
| GLOT  | DOUBLE  | GLONASS seconds of day  |
GLONASS time quality, the definition is same as that of
| GLOQ  | UINT  |     |
| ----- | ----- | --- |
GPSQ
| BDW  | UINT    | BDS week             |
| ---- | ------- | -------------------- |
| BDT  | DOUBLE  | BDS seconds of week  |
BDQ  UINT  BDS time quality, the definition is same as that of GPSQ
Time difference between BDS time and GPS time, in
| BDGPSDiff  | DOUBLE   |     |
| ---------- | -------- | --- |
seconds
Time difference between GLONASS time and GPS time, in
| GLOGPSDiff  | DOUBLE   |     |
| ----------- | -------- | --- |
seconds
Checksum
| cs  | STR  |     |
| --- | ---- | --- |
A hexadecimal number obtained by calculating an XOR of
all characters from '$' to '*' in this statement
| 1.5.4.4  | NAVACC                                 |     |
| -------- | -------------------------------------- | --- |
| Syntax   | $NAVACC,time,status,pAcc,vAcc,cAcc*cs  |     |
| Example  | $NAVACC,085206.00,A,2480, 70,1250*7D   |     |
Description  Output accuracy information of receiver positioning speed measurement
| Input/Output  | Output  |     |
| ------------- | ------- | --- |
Parameter Definition
| Parameter  | Format  | Description  |
| ---------- | ------- | ------------ |
UTC time, in the format of hhmmss.sss
hh - Hours
| time  | STR  |     |
| ----- | ---- | --- |
mm - Minutes
ss.sss - Seconds
Data validity identification
| status  | UINT  | V - Invalid  |
| ------- | ----- | ------------ |
A - Valid
Standard Positioning Products Protocol Specification
Horizontal positioning accuracy, standard deviation of two-
| pAcc  | UINT  |     |
| ----- | ----- | --- |
dimensional horizontal positioning error, in 0.001m
Accuracy of horizontal velocity measurement, standard
vAcc  UINT  deviation of two-dimensional horizontal velocity error, unit:
0.001 m/s
| cAcc  | UINT  | Ground course accuracy, in 0.001 degrees  |
| ----- | ----- | ----------------------------------------- |
Checksum
| cs  | STR  |     |
| --- | ---- | --- |
A hexadecimal number obtained by calculating an XOR of all
characters from '$' to '*' in this statement
1.5.5  Misc Message
| 1.5.5.1  | ANTSTAT  |     |
| -------- | -------- | --- |
Query Antenna Detection Information
| Syntax       | $ANTSTAT,antType                     |     |
| ------------ | ------------------------------------ | --- |
| Example      | $ANTSTAT,1                           |     |
| Description  | Query antenna detection information  |     |
antType:
Null: External
Type
0: External
1: Internal
No parameters
Output Antenna Detection Information
| Syntax   | $ANTSTAT,status1,status2  |     |
| -------- | ------------------------- | --- |
| Example  | $ANTSTAT,0,0              |     |
Description  Output antenna detection information and antenna type
| Input/Output  | Output  |     |
| ------------- | ------- | --- |
Parameter Definition
| Parameter  | Format  | Description  |     |
| ---------- | ------- | ------------ | --- |
Antenna detection status, as defined below:
$ANTSTAT,0,0     Normal, active antenna
| status1,Status2  | INT  | $ANTSTAT,0,1     Short-circuit  |     |
| ---------------- | ---- | ------------------------------- | --- |
$ANTSTAT,1,0     Open-circuit or passive antenna
$ANTSTAT,1,1     Hardware abnormality
 When the external antenna detection circuit does not exist, the information of the antenna
detection output (type: external) is invalid.
 For the antenna detection circuit, please refer to the hardware reference design.
|                              |     | PIO_14 (ANT_OPEN)       | PIO_15 (ANT_SHORT)      |
| ---------------------------- | --- | ----------------------- | ----------------------- |
| GPIO Status                  |     | Input- Level triggered  | Input- Level triggered  |
| No active antenna connected  |     | 1                       | 0                       |
| Active antenna connected     |     | 0                       | 0                       |
| Active Antenna shorted out   |     | 0                       | 1                       |
1.5.5.2
LSF
Query Leap Seconds Forecast Information
| Syntax   | $LSF,system  |     |     |
| -------- | ------------ | --- | --- |
| Example  | $LSF,0       |     |     |
Query leap seconds forecast information of the specified satellite system,
Description
the receiver outputs LSF message after receiving the command
| Input/Output  | Input  |     |     |
| ------------- | ------ | --- | --- |
Parameter Definition
| Parameter  | Format  | Description  |     |
| ---------- | ------- | ------------ | --- |
Standard Positioning Products Protocol Specification
Query the system corresponding to the leap seconds forecast
information
0: GPS
system UINT
1: BDS
2: GLO
3: GAL
Output Leap Seconds Forecast Information
$LSF,system,flag,utcTLS,utcTLSF,utcTOT,utcWN,utcDN,utcWNLSF,
Syntax
utcA0,utcA1
Example $LSF,0,1,15,16,462836,82,6,86,7811626,14
Description Input/output leap seconds forecast information
Input/Output Input/output
Parameter Definition
Parameter Format Description
The system corresponding to the output of leap second
System UINT forecast information, which is the same as the query
instruction parameter
Valid flag of leap second forecast information
Flag UINT 0: Invalid
1: Valid
Time difference between UTC and system before a leap second
event, in seconds;
utcTLS UINT
GLO system does not have this parameter
Time difference between UTC and system after a leap second
event, in seconds;
utcTLSF UINT
GLO system does not have this parameter；
UTC reference seconds of week, in seconds(BDS system
parameter is 0)
utcTOT UINT
GLO system: the parameter corresponds to GLO UTC A0;
Number of UTC reference week, in weeks (BDS system
parameter is 0)
utcWN UINT
GLO system: the parameter corresponds to GLO UTC A1;
Days of week when leap second event occurs, in days.
utcDN UINT
GLO system: the parameter corresponds to GLO UTC DN;
Number of UTC week when leap second event occurs, in weeks
utcWNLSF UINT
GLO system: the parameter corresponds to GLO UTC KP;
Constant coefficient A0 of UTC polynomial (scale factor 2-30) ,
in s
utcA0 INT
GLO system: the parameter corresponds to GLO UTC tc;
First-order coefficient A1 of UTC polynomial (scale factor 2-
50) , in s/s
utcA1 INT
GLO system: the parameter corresponds to GLO UTC tg;
GPS Week (GPS Week) is the time system adopted in the GPS system. Time Zero is
defined as: 0 a.m. on January 6, 1980. Every 1024 weeks (7168 days) is a cycle. The first
GPS cycle point is 0000:00 and 000:00 on August 22, 1999. That is, from this moment
on, the number of weeks starts again from zero. The rule for counting the number of
weeks is: Sunday is 1, and in turn is 1-7.
The Beidou satellite navigation time system starts at 000:00 and 000:00 UTC ON 1
January 2006. Use Week and seconds into the week count. The rule for counting the
number of weeks is: Sunday is 0, and in turn is 0-6
utcWNLSF: A decimal number converted from the lower eight bits of the binary week
when a leap second occurs. For example: A leap second occurs in the 900th week
(binary: 1110000100) and it is broadcast in 132 (Binary: 10000100).
Conversion method of GPS leap second occurrence week:
STEP1: Convert GPSW in Navtime to binary, set the lower eight bits to zero, and then
Standard Positioning Products Protocol Specification
convert to decimal.
STEP2: Add the number in STEP1 to utcWNLSF to get the week when a leap second
occurs.
 Conversion method of BDS leap second occurrence week:
STEP1: Convert the BDW in Navtime to binary, set the lower eight bits to zero, and then
convert to decimal.
STEP2: Add utcWNLSF to the number in STEP1 to get the week when a leap second
occurs.
 UTCDN: Days of the week in which a leap seconds occurs: GPS: 1-7 from Sunday to
Saturday; BDS: 0-6 from Sunday to Saturday
 Leap seconds occur at 23:59:59
| 1.5.5.3  | CWOUT  |     |
| -------- | ------ | --- |
Output Interference Detection Information
| Syntax        | $CWOUT,CWFlagOut,CWRatioOut                |     |
| ------------- | ------------------------------------------ | --- |
| Example       | $CWOUT,1,0                                 |     |
| Description   | Output interference detection information  |     |
| Input/Output  | Output                                     |     |
Parameter Definition
| Parameter  | Format  | Description  |
| ---------- | ------- | ------------ |
Interference
1: No interference
| CWFlagOut  | UINT  |     |
| ---------- | ----- | --- |
2: Interference detected
3: Strong interference, has affected the receiver positioning
Interference intensity, 0~255, 0 means no interference, 255
| CWRatioOut  | UINT  |     |
| ----------- | ----- | --- |
means strong interference
| 1.5.5.4  FCTATEST  |     |     |
| ------------------ | --- | --- |
Production Test Mode
| Syntax        | $fctatest,Model              |     |
| ------------- | ---------------------------- | --- |
| Example       | $fctatest,1                  |     |
| Description   | Enable test mode of CW wave  |     |
| Input/Output  | Input                        |     |
Parameter Definition
| Parameter  | Format  | Description  |
| ---------- | ------- | ------------ |
0: disable
| Model  | UINT  |     |
| ------ | ----- | --- |
1: enable
 This test mode only supports carrier-to-noise ratio detection of signals modulated with
carrier and pseudo code, and the command can only be used for production line test. In
actual positioning application, abnormal positioning may occur when sending the
command, and power off the receiver or disable the command to recover
| 1.5.5.5  ABNORMAL  |     |     |
| ------------------ | --- | --- |
Query the Output Configuration of the Injected Ephemeris
| Syntax    | $CFGMSG,msgClass,msgID  |     |
| --------- | ----------------------- | --- |
| Example   | $CFGMSG,6,4             |     |
Description   Read the output configuration of $ABNORMAL message
| Input/Output  | Input   |     |
| ------------- | ------- | --- |
No parameters
Output the Status of the Injected Ephemeris
| Syntax    | $ABNORMAL,DataLen,AbnLevel  |     |
| --------- | --------------------------- | --- |
| Example   | $ABNORMAL,0,3*13            |     |
Standard Positioning Products Protocol Specification
| Description   | Output the status of the injected ephemeris  |     |
| ------------- | -------------------------------------------- | --- |
| Input/Output  | Output                                       |     |
Parameter Definition
| Parameter   | Format   | Description   |
| ----------- | -------- | ------------- |
DataLen  INT  Input data length from the serial port after starting up.
The absence level of the number of predicted ephemerides
AbnLevel  INT  that have been injected, of which the value could be 0, 1, 2, 3.
The larger the value is, the more absent satellites there are.
 Note: This statement is output every 5s. If the satellite absence level is 0, it will not be
output. Only supported by R3.4.0.19 version.
| 1.5.5.6  | EPHABNORMAL  |     |
| -------- | ------------ | --- |
Query the Status of the Injected Ephemeris
| Syntax    | $CFGMSG,msgClass,msgID  |     |
| --------- | ----------------------- | --- |
| Example   | $CFGMSG,6,2             |     |
Description   Read the output configuration of $EPHABNORMAL message
| Input/Output  | Input  |     |
| ------------- | ------ | --- |
No parameters
Output the Status of the Injected Ephemeris
| Syntax        | $EPHABNORMAL,status                          |     |
| ------------- | -------------------------------------------- | --- |
| Example       | $EPHABNORMAL,1*50                            |     |
| Description   | Output the status of the injected ephemeris  |     |
| Input/Output  | Output                                       |     |
Parameter Definition
| Parameter   | Format   | Description                          |
| ----------- | -------- | ------------------------------------ |
| status      | INT      | Value 1 means incomplete ephemeris.  |
Note: The value is fixed at 1. The output of the message indicates the incompleteness of
the ephemeris injection, and the default output frequency is once every 10s.
This command is only applicable to version R3.2.20.100, and only when the ephemeris
injection is incomplete will this message be output.
1.6 Default Configuration
1.6.1 Serial Port Configuration (CFGPRT)
Default
Parameter Description
Configuration
UART1
‒ 115200: Default baud rate for the firmware with the
version of R3.2.0.12, R3.2.0.0, R3.4.0.0, R3.2.10.100,
R3.2.10.0 and R3.2.20.100
baud 115200/9600 ‒ 9600: Default baud rate for the firmware with the
version of R3.1.10.0
‒ 460800: Default baud rate for the firmware with the
version of R3.4.0.19
‒ 161: Default configuration for the firmware with the
version of R3.2.0.12, R3.2.0.0, R3.4.0.0, R3.1.10.0 and
R3.4.0.19, input UNICORE +RTCM3.2 + RTCM2.3
inProto 161/129 protocol
‒ 129: Default configuration for the firmware with the
version of R3.2.10.0, R3.2.10.100 and R3.2.20.100,
input UNICORE +RTCM3.2 protocol
‒ 35: Default configuration for the firmware with the
version of R3.4.0.0, R3.2.10.0, R3.2.10.100,
R3.2.20.100 and R3.4.0.19, output
outProto 35/3 UNICORE+NMEA+command echo
‒ 3: Default configuration for the firmware with the
version of R3.1.10.0, R3.2.0.12and R3.2.0.0, output
UNICORE+NMEA protocol
Standard Positioning Products Protocol Specification
UART2
‒ 115200: Default baud rate for the firmware with the
version of R3.2.0.12, R3.2.0.0, R3.4.0.0, and R3.2.10.0
| baud  | 115200/9600  |     |
| ----- | ------------ | --- |
‒  9600: Default baud rate for the firmware with the
version of R3.1.10.0
| inProto  | 1   | Input UNICORE protocol  |
| -------- | --- | ----------------------- |
‒ 35: Default configuration for the firmware with the
version of R3.4.0.0 and R3.2.10.0, output UNICORE
+NMEA +command echo protocol
| outProto  | 35/3  |     |
| --------- | ----- | --- |
‒ 3: Default configuration for the firmware with the
version of  R3.1.10.0, R3.2.0.12 and R3.2.0.0, output
UNICORE+NMEA protocol
1.6.2
Message Configuration (CFGMSG)
Message Output Frequency
Default
| Parameter  |     | Description  |
| ---------- | --- | ------------ |
Configuration
NMEA Message
| GGA  | 1   | Output at 1Hz  |
| ---- | --- | -------------- |
‒ 0: Firmware with the version of R3.2.0.0, R3.2.10.0,
R3.1.10.0, R3.4.0.0, R3.2.10.100, R3.2.20.100 and
| GLL  | 0/1  | R3.4.0.19 defaults to 0, disabled   |
| ---- | ---- | ----------------------------------- |
‒ 1: Firmware with the version of R3.2.0.12 defaults to
1, Output at 1Hz
| GSA  | 1   | Output at 1Hz  |
| ---- | --- | -------------- |
| GSV  | 1   | Output at 1Hz  |
| RMC  | 1   | Output at 1Hz  |
‒ 0: Firmware with the version of R3.1.10.0, R3.2.10.0,
R3.2.0.0, R3.4.0.0, R3.2.10.100, R3.2.20.100 and
| VTG  | 0/1  |     |
| ---- | ---- | --- |
R3.4.0.19 defaults to 0, disabled
‒ 1: Firmware with the version of R3.2.0.12 defaults to
1, Output at 1Hz
| ZDA  | 0   | Disabled  |
| ---- | --- | --------- |
| GST  | 0   | Disabled  |
Navigation Result Message
| POS   | 0   | Disabled  |
| ----- | --- | --------- |
| VEL   | 0   | Disabled  |
| TIME  | 0   | Disabled  |
| ACC   | 0   | Disabled  |
Maintenance and Detection Message
| NOTICE  | 0   | Output at 1Hz  |
| ------- | --- | -------------- |
Output once every 10s by default, and only when the
EPHABNORMAL  10  ephemeris injection is incomplete will this message be
output.
1.6.3
NMEA Configuration (CFGNMEA)
Default
| Parameter  |     | Description  |
| ---------- | --- | ------------ |
Configuration
Extended BEIDOU messages Based on NMEA standard
| nmeaVer  | H51  |     |
| -------- | ---- | --- |
version 4.1
1.6.4  Satellite System Configuration (CFGSYS)
Default
| Parameter  |     | Description  |
| ---------- | --- | ------------ |
Configuration
‒ Firmware with the version of R3.4.0.0 and R3.4.0.19
defaults to support GPS+BDS+Galileo+SBAS+QZSS
sysMask  H11  ‒ Firmware with the version of ‒ R3.1.10.0, R3.2.0.12,
R3.2.0.0, R3.2.10.0, R3.2.10.100 and R3.2.20.100
defaults to support GPS+BDS+QZSS
Standard Positioning Products Protocol Specification
1.6.5  Interference Detection Configuration (CFGCWOUT)
| Parameter  | Default Configuration  | Description                         |
| ---------- | ---------------------- | ----------------------------------- |
| CWOutCtrl  | 0                      | Disable the interference detection  |
1.6.6  Dynamic Configuration (CFGDYN)
| Parameter  | Default Configuration  | Description                           |
| ---------- | ---------------------- | ------------------------------------- |
| mask       |                        | Domain to be configured, disabled by  |
h00
setting the corresponding bit to 0
| dynModel          | 0   | Set the dynamic mode to portable  |
| ----------------- | --- | --------------------------------- |
| staticHoldThresh  | 0   | Disable the static hold mode      |
1.6.7  Elevation Configuration (CFGGEOID)
| Parameter  | Default Configuration  | Description                      |
| ---------- | ---------------------- | -------------------------------- |
| Model      | 0                      | Elevation is ellipsoidal height  |
1.6.8  Static Mode (CFGMOD)
| Parameter  | Default Configuration  | Description              |
| ---------- | ---------------------- | ------------------------ |
| Model      | 0                      | Disable the static mode  |
和芯星通科技（北京）有限公司
Unicore Communications, Inc.
北京市海淀区丰贤东路7号北斗星通大厦三层
F3, No.7, Fengxian East Road, Haidian, Beijing, P.R.China,
1 00094
www.unicorecomm.com
Phone: 86-10-69939800
Fax: 86-10-69939888
info@unicorecomm.com
