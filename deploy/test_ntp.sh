#!/usr/bin/env bash
#
# Test script to validate Time Beacon NTP Server response.
#
# Usage:
#   ./deploy/test_ntp.sh [TARGET_IP]
#

set -euo pipefail

TARGET_IP="${1:-192.168.1.200}"

echo "============================================================"
echo "Testing Time Beacon NTP Server at: ${TARGET_IP}"
echo "============================================================"

# 1. Ping test
echo -n "--> Checking ICMP reachability... "
if ping -c 1 -W 2 "${TARGET_IP}" >/dev/null 2>&1; then
    echo "OK"
else
    echo "FAILED (Device may be offline or Wi-Fi connecting)"
fi

# 2. Raw UDP 123 probe using python (requires no external packages)
echo "--> Sending RFC 5905 NTP probe to ${TARGET_IP}:123..."
python3 - << EOF
import socket
import struct
import sys
import time

target_ip = "${TARGET_IP}"
port = 123

# NTP client packet: LI=0, VN=4, Mode=3 (client) -> 0x23
client_packet = bytearray(48)
client_packet[0] = 0x23

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.settimeout(3.0)

t1 = time.time()
try:
    sock.sendto(client_packet, (target_ip, port))
    data, addr = sock.recvfrom(1024)
    t4 = time.time()
except socket.timeout:
    print("    [!] Error: No response received from server within 3 seconds.")
    sys.exit(1)
except Exception as e:
    print(f"    [!] Socket error: {e}")
    sys.exit(1)

if len(data) < 48:
    print(f"    [!] Error: Truncated NTP packet received ({len(data)} bytes).")
    sys.exit(1)

# Unpack response header
li_vn_mode, stratum, poll, precision = struct.unpack("!BBbb", data[0:4])
li = (li_vn_mode >> 6) & 0x03
vn = (li_vn_mode >> 3) & 0x07
mode = li_vn_mode & 0x07

root_delay, root_disp, ref_id = struct.unpack("!II4s", data[4:16])
rtt_ms = (t4 - t1) * 1000.0

print(f"    [+] Response received from {addr[0]}:{addr[1]}")
print(f"        Round-Trip Time: {rtt_ms:.2f} ms")
print(f"        Version:        {vn}")
print(f"        Mode:           {mode} (Server)")
print(f"        Leap Indicator: {li} ({'Locked / In Sync' if li == 0 else 'Unsynchronized'})")
print(f"        Stratum:        {stratum} ({'Stratum 1 (Primary GNSS)' if stratum == 1 else 'Stratum ' + str(stratum)})")
print(f"        Reference ID:   {ref_id.decode('ascii', errors='replace')}")

if stratum == 1 and li == 0:
    print("\n[SUCCESS] Time Beacon is active and synchronized to GNSS!")
else:
    print("\n[WARNING] Device responded, but GNSS fix is not yet locked.")
EOF

# 3. Check chrony sources if chronyc is installed
if command -v chronyc >/dev/null 2>&1; then
    echo ""
    echo "--> Local chrony sources report:"
    chronyc sources -v || true
fi
