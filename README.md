# KFW ESP32 - Multi-Tool Pentesting Firmware

![KFW Banner](https://img.shields.io/badge/ESP32-Pentesting-orange?style=for-the-badge)
![Version](https://img.shields.io/badge/Version-1.0%20Stable-brightgreen?style=for-the-badge)
![License](https://img.shields.io/badge/License-MIT-blue?style=for-the-badge)

**KFW** (KalFirmWare) is a comprehensive ESP32-based multi-tool firmware for wireless security testing, RFID experimentation, and infrared control.

## 🚀 Quick Start

### Flash Online (Recommended)
Visit the **[KFW Web Flasher](https://labubuahhginger.github.io/kfwflasher.github.io)** to flash your ESP32 directly from your browser — no installation required!

### Manual Installation
1. Download the latest release from [GitHub Releases](https://github.com/labubuahhginger/kfw-esp32/releases)
2. Flash using Arduino IDE or PlatformIO
3. Connect to your ESP32 and enjoy!

## ✨ Features

### 📡 Wi-Fi Tools
- **Beacon Spam** — Flood Wi-Fi scanners with fake networks
- **Deauther** — Disconnect clients from access points
- **Probe Sniffer** — Capture probe requests from devices
- **Beacon Sniffer** — Monitor beacon frames
- **Channel Graph** — Visualize Wi-Fi channel usage
- **KFWGotchi** — Automated WPA2 handshake capture with Tamagotchi-style UI

### 📻 Bluetooth (BLE)
- **AppleJuice** — iOS device spam (AirPods, Apple TV, Keyboards)
- Customizable BLE advertising packets

### 🔴 Infrared (IR)
- **IR Receiver** — Capture remote control codes
- **IR Transmitter** — Replay captured codes
- **TV-B-Gone** — Universal TV power-off (20+ brands)

### 🔐 RFID / NFC
- **Read** — Read MIFARE Classic cards
- **Dump** — Full memory dump with key bruteforce
- **Format** — Reset cards to factory state
- **Emulate** — Clone UID

### 📁 File Manager
- **LittleFS Browser** — View and manage stored files
- **WebUI** — Browser-based file manager with Material Design 3
- **PCAP Export** — Download captured handshakes

### ⚙️ System
- **Adjustable TX Power** — Wi-Fi and BLE transmission power control
- **Brightness Control** — OLED display brightness adjustment
- **Serial Console** — Live debug output via WebUI

## 🛠️ Hardware Requirements

| Component | Model | Notes |
|-----------|-------|-------|
| Microcontroller | ESP32 Dev Module | Any ESP32 with 4MB+ flash |
| Display | SSD1306 OLED | 128x64, I2C |
| RFID Reader | MFRC522 | SPI interface |
| IR Receiver | TSOP38238 | GPIO 15 |
| IR Transmitter | IR LED | GPIO 13 |
| Buttons | Tactile switches | GPIO 32 (NEXT), GPIO 33 (OK) |

### Wiring Diagram
```
ESP32          →  Component
---------------------------------
GPIO 21 (SDA)  →  OLED SDA
GPIO 22 (SCL)  →  OLED SCL
GPIO 5  (SS)   →  MFRC522 SDA
GPIO 18 (SCK)  →  MFRC522 SCK
GPIO 19 (MISO) →  MFRC522 MISO
GPIO 23 (MOSI) →  MFRC522 MOSI
GPIO 27        →  MFRC522 RST
GPIO 15        →  IR Receiver
GPIO 13        →  IR LED (+ 330Ω resistor)
GPIO 32        →  NEXT Button (Pull-up)
GPIO 33        →  OK Button (Pull-up)
```

## 📖 Usage

### Navigation
- **NEXT Button** — Cycle through menu items
- **OK Button** — Select / Confirm

### WebUI Access
1. Go to **Wi-Fi → Start AP**
2. Connect to your home Wi-Fi (configured in code)
3. Note the IP address shown on screen
4. Open browser and navigate to the IP
5. Manage files, view console output, upload/download

### Capturing Handshakes (KFWGotchi)
1. **Wi-Fi → KFWGotchi**
2. Select a channel or let it auto-scan
3. Wait for a target to appear
4. Device automatically deauths and captures EAPOL packets
5. `.pcap` files saved to LittleFS
6. Download via WebUI

## 🎨 UI Design

KFW features a **Bruce-inspired** interface with:
- Clean header bars
- Scrollable menus with icons (Main Menu only)
- Status bar with button hints
- Material Design 3 WebUI (warm orange theme)

## 📦 Dependencies

```cpp
Adafruit_GFX
Adafruit_SSD1306
MFRC522
IRremoteESP8266
NimBLE-Arduino
TJpg_Decoder
LittleFS (built-in)
WiFi (built-in)
WebServer (built-in)
```

## ⚠️ Legal Disclaimer

**KFW is intended for authorized security testing and educational purposes only.**

- Only use on networks you own or have explicit permission to test
- Unauthorized access to computer networks is illegal
- The authors are not responsible for misuse of this software
- Check your local laws before using Wi-Fi deauth or jamming features

## 🤝 Contributing

Contributions are welcome! Please:
1. Fork the repository
2. Create a feature branch
3. Submit a pull request

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Credits

- **Created by:** [@labubuahhginger](https://github.com/labubuahhginger)
- **Inspired by:** Bruce, Flipper Zero, ESP32 Marauder
- **Libraries:** Adafruit, ESP32 Community, NimBLE

## 📞 Support

- **Issues:** [GitHub Issues](https://github.com/labubuahhginger/kfw-esp32/issues)
- **Web Flasher:** [labubuahhginger.github.io/kfwflasher.github.io](https://labubuahhginger.github.io/kfwflasher.github.io)

---

**Made with 🔥 by [@labubuahhginger](https://github.com/labubuahhginger)**
