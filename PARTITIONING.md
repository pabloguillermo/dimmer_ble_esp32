# Technical Documentation - ESP32 Partitioning

## Overview

The `partitions.csv` file defines how the ESP32's Flash memory (4MB) is divided. The current configuration has been optimized to **maximize memory available for the program** (firmware).

## Partition Structure

### Flash Memory Map (4MB = 4194304 bytes)

```
0x000000 ┌─────────────────┐
         │    Bootloader   │  64 KB (0x10000)
0x010000 ├─────────────────┤
         │   APP (Main)    │  2.5 MB (0x280000) [app0] **ACTIVE**
0x290000 ├─────────────────┤
         │     SPIFFS      │  1.4375 MB (0x170000) [minimized]
0x400000 └─────────────────┘
```

## Detailed Partitions

| Partition | Type   | Subtype  | Offset   | Size       | Description |
|-----------|--------|----------|----------|------------|-------------|
| **nvs** | data | nvs | 0x9000 | 0x5000 (20 KB) | Preferences storage (passwords, configuration) |
| **app0** | app | factory | 0x10000 | 0x280000 (2.5 MB) | **Main firmware (NO OTA)** |
| **spiffs** | data | spiffs | 0x290000 | 0x170000 (1.44 MB) | File system (minimized) |

## Applied Optimizations

### 1. **Maximum Memory for Program**
- APP0 partition increased to **2.5 MB** (maximum without OTA)
- SPIFFS partition minimized (1.44 MB)
- NVS limited to 20 KB (sufficient for ~100 preferences)

### 2. **Compilation Flags** (platformio.ini)

```ini
build_flags = 
  -Os           # Size optimization (reduces .elf up to 30%)
  -DNDEBUG      # Disable assertions in release
```

### 3. **Build Configuration**

```ini
board_build.extra_flags = 
  -DBOARD_HAS_PSRAM=0  # Disable PSRAM (not used in this project)
```

### 4. **Serial Monitor**

```ini
monitor_filters = esp32_exception_decoder  # Decodes crashes
```

## Comparison: Before vs After

### Before (default.csv)
```
APP: 1.19 MB
SPIFFS: 1.90 MB
OTA: Enabled (2 slots)
```

### After (partitions.csv) - NO OTA
```
APP: 2.5 MB (+110%, +1.31 MB)
SPIFFS: 1.44 MB (sufficient for data)
OTA: Disabled (maximum space)
```

## Expected Results

✅ **Code space: 2.5 MB** (previously 1.19 MB)  
✅ **Gain: +1.31 MB (+110%)** for firmware  
✅ **Compilation optimizations** further reduce binary size  
✅ **Data persistence** guaranteed (NVS + SPIFFS)  
✅ **No OTA** (no reserved slot for automatic updates)  

## How to Use

### Standard build
```bash
pio run                # Compile for app0
pio run -t upload      # Upload to board
```

### Check firmware size
```bash
pio run -t buildfs     # Compile filesystem
ls -lh .pio/build/esp32dev/firmware.bin
```

### Serial monitor with decoder
```bash
pio device monitor
```

## Important Notes

⚠️ **No OTA**: This project **does not support OTA updates** (over-the-air). To update firmware:
- Connect via USB
- Use `pio run -t upload`
- Machine with serial terminal required

✅ **If OTA is needed in the future**:
See the "Alternative OTA Configuration" section below.

## Alternative Configuration with OTA

If OTA updates are needed in the future, use this partitioning:

```csv
# Partition with OTA support (reduced APP space)
nvs,      data,  nvs,     0x9000,  0x5000,
otadata,  data,  ota,     0xe000,  0x2000,
app0,     app,   ota_0,   0x10000, 0x180000,
app1,     app,   ota_1,   0x190000, 0x180000,
spiffs,   data,  spiffs,  0x310000, 0xF0000,
```

**Trade-off**: APP = 1.5 MB per slot (vs. 2.5 MB without OTA)

## References

- [ESP32 Partition Table](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-guides/partition-tables.html)
- [PlatformIO Build Configuration](https://docs.platformio.org/en/latest/platforms/espressif32.html)
- [ESP32 Memory Layout](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/system/memory_management.html)
