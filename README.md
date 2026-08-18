# Dimmer BLE ESP32

![License](https://img.shields.io/badge/license-GPL--3.0-green)
![Platform](https://img.shields.io/badge/platform-ESP32-blue)

Professional firmware for Bluetooth Low Energy (BLE) dimmer control on ESP32 boards. Features integrated authentication, data persistence, and physical button control.

## 📋 Description

This project implements an intelligent dimmer (PWM power controller) with BLE interface. It allows controlling light intensity/load via mobile app or physical buttons on the board, with password authentication and automatic configuration saving.

### ✨ Features

- **BLE Control**: Bidirectional communication via Bluetooth Low Energy
- **Authentication**: Configurable password-based authentication system
- **Persistence**: Automatic state and configuration saving using Preferences
- **Physical Control**: 3 buttons for manual adjustment (increase, decrease, on/off)
- **Dynamic PWM**: Advanced power calibration with multiple modes
- **Notifications**: Real-time state feedback via BLE
- **Security**: Automatic restart on authentication failure

## 🔧 Hardware Configuration

### Pin Assignment (ESP32 WROOM32)

| Function | GPIO | Type |
|----------|------|------|
| Dimmer PWM Output | 32 | Digital Output |
| Increase Button | 34 | Digital Input |
| Decrease Button | 13 | Digital Input |
| Power Button | 26 | Digital Input |

### Requirements

- **Board**: ESP32 WROOM32 or compatible
- **Power**: 5V USB or external supply
- **Peripherals**: 3 tactile buttons, pull-up resistors (optional)

## 📦 Project Structure

```
dimmer_ble_esp32/
├── src/
│   └── main.cpp          # Main firmware
├── include/              # Custom headers (empty by default)
├── lib/                  # Local libraries (empty by default)
├── test/                 # Tests (PlatformIO structure)
├── platformio.ini        # Build configuration
├── .gitignore            # Git ignore rules
└── README.md             # This file
```

## 🚀 Compilation and Upload

### Prerequisites

- VS Code installed
- PlatformIO IDE extension installed
- USB drivers CH340/CP2102 (for serial upload)

### Steps

1. **Clone the repository**
   ```bash
   git clone https://github.com/pabloguillermo/dimmer_ble_esp32.git
   cd dimmer_ble_esp32
   ```

2. **Open in VS Code**
   ```bash
   code .
   ```

3. **Build**
   - PlatformIO menu → Build, or
   ```bash
   pio run
   ```

4. **Upload to board**
   - PlatformIO menu → Upload, or
   ```bash
   pio run -t upload
   ```

5. **Monitor serial (debug)**
   - PlatformIO menu → Device Monitor, or
   ```bash
   pio device monitor
   ```

## 📡 BLE Protocol

### Fixed UUIDs

```
Service UUID:        6E400001-B5A3-F393-E0A9-E50E24DCCA9E
RX Characteristic:   6E400002-B5A3-F393-E0A9-E50E24DCCA9E
TX Characteristic:   6E400003-B5A3-F393-E0A9-E50E24DCCA9E
```

### Supported Commands

| Command | Format | Description | Example |
|---------|--------|-------------|---------|
| Authentication | `p<password>` | Send password (4 digits) | `p1234` |
| Power Level | `c<0-100>` | Set percentage | `c75` |
| Power | `s<0\|1>` | 0=off, 1=on | `s1` |
| Mode | `t<L\|M>` | Linear or Maximum | `tL` |

**Note**: Requires authentication before executing commands.

## 🔐 Authentication

- Default password: `1234` (configurable via Preferences)
- Master password: `1234` (always works if no password is set)
- Authentication failure: Board automatically restarts

## 📊 Persistent State Variables

The following information is automatically saved and recovered on restart:

- Power percentage (0-100)
- State (on/off)
- Operation mode (Linear/Maximum)
- Registered password

## 🛠️ Development

### Code Structure

- **setup()**: GPIO, BLE, and persistence initialization
- **loop()**: Button processing, timers, and synchronization
- **calculaPWM()**: Non-linear PWM mapping function
- **BLE Callbacks**: Bluetooth command processing

### Build with Debug

Uncomment in `platformio.ini`:
```ini
build_flags = -DCORE_DEBUG_LEVEL=3
```

## 📝 License

This project is licensed under the GNU General Public License v3.0. See [LICENSE](LICENSE) for details.

## 👤 Author

Pablo Guillermo - [GitHub](https://github.com/pabloguillermo)

## 🐛 Report Issues

Found a bug? Open an [issue](https://github.com/pabloguillermo/dimmer_ble_esp32/issues) on GitHub.

## 📚 References

- [ESP32 Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/)
- [PlatformIO Docs](https://docs.platformio.org/)
- [BLE Specification](https://www.bluetooth.com/specifications/specs/)

---

**Last Updated**: August 2026

## 🐛 Reportar Problemas

Encontrou um bug? Abra uma [issue](https://github.com/pabloguillermo/dimmer_ble_esp32/issues) no GitHub.

## 📚 Referências

- [Documentação ESP32](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/)
- [PlatformIO Docs](https://docs.platformio.org/)
- [BLE Specification](https://www.bluetooth.com/specifications/specs/)

---

**Última atualização**: Agosto 2026
