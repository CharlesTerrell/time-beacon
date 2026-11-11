# MeshCore Integration & Time Synchronization Specification

This document details the MeshCore (v1.x) packet protocol, cryptographic requirements, why stock MeshCore nodes do not passively sync time from ambient broadcasts, and how a future trusted remote mesh time server architecture could be implemented.

---

## 1. Local Radio Parameters (US Standard)

For local MeshCore compatibility in the US915 band:
- **Frequency**: `910.525 MHz` (`910525000 Hz`)
- **Bandwidth**: `62.5 kHz` (`SX1262_BW_62_5_KHZ = 0x03`)
- **Spreading Factor**: `SF7`
- **Coding Rate**: `CR 4/5` (`SX1262_CR_4_5`)
- **TX Power**: `+22 dBm`
- **Preamble Length**: `32 symbols` (MeshCore standard for $SF \le 8$; $SF > 8$ uses 16 symbols)
- **Sync Word**: `0x1424` (Private sync word / `SX1262_SYNC_PUBLIC`)

---

## 2. MeshCore Wire Protocol

MeshCore packets follow this structure:
```text
[Header: 1B] [Transport Codes: 0 or 4B] [Path Length: 1B] [Path: N*HashSize] [Payload: M Bytes]
```

### 2.1 Header Byte (`0bVVPPPPRR`)
- **Bits 0–1 (`Route Type`)**:
  - `0x00` (`0b00`): `ROUTE_TYPE_TRANSPORT_FLOOD` (4 transport bytes follow)
  - `0x01` (`0b01`): `ROUTE_TYPE_FLOOD` (Standard flood routing)
  - `0x02` (`0b10`): `ROUTE_TYPE_DIRECT` (Direct source-routed path)
  - `0x03` (`0b11`): `ROUTE_TYPE_TRANSPORT_DIRECT`
- **Bits 2–5 (`Payload Type`)**:
  - `0x00`: `PAYLOAD_TYPE_REQ`
  - `0x01`: `PAYLOAD_TYPE_RESPONSE`
  - `0x02`: `PAYLOAD_TYPE_TXT_MSG`
  - `0x03`: `PAYLOAD_TYPE_ACK`
  - `0x04`: `PAYLOAD_TYPE_ADVERT` (Node Identity advertisement)
  - `0x05`: `PAYLOAD_TYPE_GRP_TXT` (Group channel text message)
  - `0x06`: `PAYLOAD_TYPE_GRP_DATA` (Group binary datagram)
  - `0x07`: `PAYLOAD_TYPE_ANON_REQ` (Anonymous query to repeater/server)
- **Bits 6–7 (`Version`)**:
  - `0x00` (`0b00`): Protocol v1

### 2.2 Path Length Byte
- **Bits 0–5**: Hop count ($0 \dots 63$).
- **Bits 6–7**: Path hash size minus 1:
  - `0b00`: 1-byte hash
  - `0b01`: 2-byte hash (`0x40` for 0 hops)
  - `0b10`: 3-byte hash

---

## 3. Node Advertisements (`PAYLOAD_TYPE_ADVERT`)

Header: `(0x04 << 2) | 0x01 = 0x11` (v1, Advert, Flood).
Path byte: `0x40` (2-byte path hash mode, 0 hops).

### 3.1 Payload Layout (100+ Bytes)
```text
+-----------------------+-------------------+--------------------+------------------------+
| Public Key (32 Bytes) | Timestamp (4B LE) | Signature (64B)   | App Data (Variable)    |
+-----------------------+-------------------+--------------------+------------------------+
```

1. **Public Key**: Ed25519 public key of the transmitting node.
2. **Timestamp**: 32-bit little-endian Unix epoch seconds.
3. **Signature**: Ed25519 digital signature generated over:
   `Message = Public Key (32B) + Timestamp (4B) + App Data (N Bytes)`
4. **App Data**:
   - Byte 0 Flags:
     - Bits 0–3: Type (`1` = Chat, `2` = Repeater, `4` = Sensor)
     - Bit 4 (`0x10`): `ADV_LATLON_MASK` (if set, 8 bytes of coordinates follow)
     - Bit 7 (`0x80`): `ADV_NAME_MASK` (remainder of payload is UTF-8 node name)
   - If `ADV_LATLON_MASK`:
     - 4 Bytes: `latitude * 1,000,000` (int32 little-endian)
     - 4 Bytes: `longitude * 1,000,000` (int32 little-endian)
   - Remainder: Node name string (up to 32 bytes).

---

## 4. Why Stock MeshCore Nodes Reject Ambient OTA Time

In MeshCore firmware (`Mesh.cpp` and `BaseChatMesh.cpp`):
1. **Verification Gate**: Any `PAYLOAD_TYPE_ADVERT` packet under 100 bytes or with an invalid Ed25519 signature is immediately dropped as `"incomplete"` or `"forged"`.
2. **Untrusted Ambient Clock**: Even when an advertisement is cryptographically verified:
   ```cpp
   from->last_advert_timestamp = timestamp;
   from->lastmod = getRTCClock()->getCurrentTime();
   ```
   The remote timestamp is saved *only* to protect against packet replay attacks (`timestamp <= from->last_advert_timestamp`). The node's own RTC (`getRTCClock()->setCurrentTime()`) is **never** updated.
3. **Design Philosophy**: MeshCore treats over-the-air ambient timestamps as untrusted to prevent time-jacking, replay attacks, and clock desynchronization. Only a companion host (`CMD_SET_DEVICE_TIME`), onboard GPS, or an authenticated admin DM command (`clock sync`) can alter a node's clock.

---

## 5. Proposed Architecture for Trusted Mesh Time Beacons

If building a custom MeshCore network that uses dedicated Stratum-1 time beacons:

### 5.1 Time Server Implementation
1. Store a fixed Ed25519 seed in NVS flash.
2. Generate the 32-byte public key and 64-byte private key using `ed25519-donna`.
3. In each beacon burst, package current GPS epoch seconds and sign `(pub_key + timestamp + app_data)`.
4. Broadcast at `910.525 MHz`, SF7, BW 62.5 kHz, 32-symbol preamble.

### 5.2 Remote Client Firmware Patch
In the remote sensor / repeater firmware (`BaseChatMesh.cpp`):
1. Define a trusted authority public key list (whitelist):
   ```cpp
   static const uint8_t TRUSTED_TIME_SERVER_PUBKEY[32] = { ... };
   ```
2. In `onAdvertRecv()`:
   ```cpp
   if (memcmp(id.pub_key, TRUSTED_TIME_SERVER_PUBKEY, 32) == 0) {
       uint32_t curr = getRTCClock()->getCurrentTime();
       // Only update if local clock is unset (e.g. < 2024 epoch) or drift is reasonable
       if (curr < 1700000000UL || abs((int32_t)(timestamp - curr)) > 2) {
           getRTCClock()->setCurrentTime(timestamp);
           MESH_DEBUG_PRINTLN("Clock updated from trusted Stratum-1 time beacon");
       }
   }
   ```

---

## 6. Public Channel Group Text Alternative

If automated text announcements are desired on the MeshCore public chat screen:
- **Packet Type**: `PAYLOAD_TYPE_GRP_TXT` (`0x05`)
- **Header**: `0x15` (`(0x05 << 2) | 0x01`)
- **Path Length**: `0x40` (2-byte hash mode, 0 hops)
- **Built-in Public Channel Key**:
  - Base64: `izOH6cXN6mrJ5e26oRXNcg==`
  - Hex: `8b 33 87 e9 c5 cd ea 6a c9 e5 ed ba a1 15 cd 72` (16 bytes)
- **Channel Hash**: `0x11` (first byte of SHA256 of 16-byte key)
- **Encryption**: AES-128-ECB of plaintext padded to 16 bytes.
- **HMAC**: First 2 bytes of HMAC-SHA256 over ciphertext with 32-byte key (16-byte key zero-padded).
- **Plaintext Format**:
  `[timestamp: 4B LE] [type: 1B = 0x00] [Time-Beacon: <Message Text>\0]`
- **Result**: Automatically decrypts on any MeshCore companion and displays as a chat message in the "Public" channel.
