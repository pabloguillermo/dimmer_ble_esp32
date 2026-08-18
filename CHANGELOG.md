# Changelog

All notable changes to this project are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/),
and this project adheres to [Semantic Versioning](https://semver.org/).

## [1.0.0] - 2026-08-18

### Added
- Initial firmware for BLE dimmer control on ESP32
- Bluetooth Low Energy (BLE) support with authentication
- Persistence system using Preferences
- Physical control via 3 buttons (increase, decrease, power)
- Dynamic PWM calibration
- Real-time BLE notifications
- Security with automatic restart on authentication failure
- Complete documentation and examples
- CI/CD with GitHub Actions
- Professional project structure

### Features
- BLE protocol with fixed UUIDs
- Commands: authentication, power level, on/off, mode
- Automatic configuration saving
- Debug mode with serial logs
- Support for multiple ESP32 boards

## [Planned] - Future Versions

### In Development
- [ ] Mobile app for iOS/Android
- [ ] Web interface for configuration
- [ ] Multi-dimmer network support
- [ ] Scheduling/automation features
- [ ] Home Assistant integration
- [ ] Low-power/sleep mode

### Considered
- QR code generator for quick pairing
- Usage history logging
- REST API

---

**Maintainer**: Pablo Guillermo  
**Repository**: https://github.com/pabloguillermo/dimmer_ble_esp32
