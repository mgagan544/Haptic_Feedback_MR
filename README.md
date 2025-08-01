#  Solenoid Heartbeat Feedback System (Meta Quest Pro Compatible)

This project simulates a heartbeat using a solenoid controlled via Unity OVR interaction (e.g., Meta Quest Pro), ESP8266 (WiFi communication), and Arduino (solenoid driver). The solenoid "lub-dubs" in response to hand interactions in a Unity scene, offering haptic feedback synced with heart-like pulses.

---

##  Repository Contents

| File/Folder Name         | Description |
|--------------------------|-------------|
| `espforardu.ino`         | ESP8266 sketch — receives HTTP commands from Unity and transmits to Arduino via serial |
| `ardu_sole_heartbeat.ino`| Arduino sketch — receives serial commands and controls solenoid pulse rhythm |
| `SolenoidTrigger.cs`     | Unity script — detects trigger zone collisions and signals `StartPulse()` / `StopPulse()` |
| `SolenoidClient.cs`      | Unity script — sends HTTP GET requests (`ENABLE` / `DISABLE`) to ESP IP when triggered |
| `*.png` (schematic)      | Optional PCB or hardware wiring schematic for your ESP-Arduino-solenoid system |

---

##  System Architecture

-[Unity (Meta Quest Pro / OVR Simulator)] HTTP (GET /enable /disable)
-[ESP8266 (espforardu.ino)] Serial UART (TX/RX) [Arduino Uno/Nano (ardu_sole_heartbeat.ino)] Digital Output
-[Solenoid Driver Circuit]
-[Solenoid] <-- 24V Power


---

##  How to Run the System

> Please follow each step exactly — skipping one can cause malfunction or even hardware damage.

###  Hardware Setup

<img width="1330" height="986" alt="image" src="https://github.com/user-attachments/assets/bc1ea3d0-7443-4120-86b8-b3acdf15fee9" />


-  **Power Supply**: Use a 24V DC regulated power supply to power the solenoid. Do **NOT** power it from USB.
-  **WiFi**: Ensure **all devices (PC, ESP8266)** are connected to the **2.4 GHz WiFi band**.
-  **ESP8266 IP**: Assign a **static IP** to the ESP8266 in your router settings. This IP should match the one used in `SolenoidClient.cs`.
-  **Serial Connections**:
  - **ESP TX (GPIO 1)** → **Arduino RX (D2)**
  - **ESP RX (GPIO 3)** ← **Arduino TX (D3)**
-  **Remove TX/RX wires before uploading code** to ESP or Arduino — reattach after flashing.

---

###  Flashing Instructions

#### Upload to ESP8266 (`espforardu.ino`)
- Uses `ESP8266WebServer` to serve `/enable` and `/disable` endpoints.
- For serial transmission, uses `Serial.begin(9600)`.

####  Upload to Arduino (`ardu_sole_heartbeat.ino`)
- Reads commands (`ENABLE`, `DISABLE`) via `Serial.readStringUntil('\n')`.
- Pulses solenoid in `lub-dub` pattern using digitalWrite and delays.

---

###  Unity Configuration

- Attach `SolenoidTrigger.cs` to a trigger collider (e.g., invisible capsule in the scene).
- Assign correct **left and right controller tags** in the Inspector.
- Attach `SolenoidClient.cs` and input the **static IP** of your ESP8266.

**Pulse Behavior Logic:**
- `OnTriggerEnter`: Boolean `isControllerInside = true` → sends `ENABLE`.
- `OnTriggerExit`: Boolean `isControllerInside = false` → sends `DISABLE`.
- The solenoid pulses continuously only while the controller is inside.

---

##  Important Notes (Must Read)

 **Power First, Then Run**:
- Connect 24V power supply **before running** the system.

 **WiFi Network**:
- Always use **2.4 GHz WiFi**. ESP8266 doesn't support 5 GHz.

 **Static IP Setup**:
- Configure the ESP to have a **static IP** either manually in code or from the router DHCP settings.

 **TX/RX Caution**:
- **Disconnect** TX/RX lines **before flashing firmware**, or the board may fail to upload.

 **Testing without Meta Quest**:
- You can use Unity’s **OVR Simulator** for testing before deploying to Meta Quest Pro.

 **Serial Debugging**:
- Use Serial Monitor (baud: 9600) to check if ESP sends proper commands.
- If the serial monitor shows `UNKNOWN COMMAND`, check wiring or string formatting.

---

##  Troubleshooting

| Problem | Solution |
|--------|----------|
| Solenoid pulses erratically | Add small delay or debounce logic in Arduino |
| Solenoid doesn’t stop | Ensure "DISABLE" command is fully received and parsed |
| ESP doesn't respond | Check WiFi connection and if static IP is configured properly |
| Upload fails | Remove TX/RX wires before uploading code |
| Nothing happens | Check 24V power, grounding, serial baud match, and solenoid circuit |

---

##  Testing Tips

- Test `http://<ESP-IP>/enable` and `http://<ESP-IP>/disable` in a browser — if working, ESP is set.
- Test Unity trigger interaction by moving hands in and out of the collider.
- You should see `Lub-dub started.` and `Lub-dub stopped.` in Unity logs.

---

##  Optional: PCB and Circuit Reference

Check included `.png` schematic to see how the ESP, Arduino, resistors, transistor, diode, and solenoid are connected.  
For example:
- Use a **10k pull-down resistor** on gate of MOSFET to ensure it's OFF by default.
- Use **flyback diode** across solenoid terminals to protect from voltage spikes.
- Ensure solenoid power circuit and logic ground are shared.

---

##  Future Improvements

- Add debounce on serial input in Arduino
- Switch to **MQTT** protocol for more robust communication (optional)
- Add heartbeat BPM control via Unity slider or OVR UI

---

