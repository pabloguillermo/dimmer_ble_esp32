# Documentação Técnica - Particionamento ESP32

## Visão Geral

O arquivo `partitions.csv` define como a memória Flash (4MB) do ESP32 é dividida. A configuração atual foi otimizada para **maximizar a memória disponível para o programa** (firmware).

## Estrutura de Partições

### Flash Memory Map (4MB = 4194304 bytes)

```
0x000000 ┌─────────────────┐
         │    Bootloader   │  64 KB (0x10000)
0x010000 ├─────────────────┤
         │   APP (Main)    │  2.5 MB (0x280000) [app0] **ATIVO**
0x290000 ├─────────────────┤
         │     SPIFFS      │  1.4375 MB (0x170000) [minimizado]
0x400000 └─────────────────┘
```

## Partições Detalhadas

| Partição | Tipo   | Subtipo  | Offset   | Tamanho    | Descrição |
|----------|--------|----------|----------|------------|-----------|
| **nvs** | data | nvs | 0x9000 | 0x5000 (20 KB) | Armazenamento de preferências (senhas, configurações) |
| **app0** | app | factory | 0x10000 | 0x280000 (2.5 MB) | **Firmware principal (SEM OTA)** |
| **spiffs** | data | spiffs | 0x290000 | 0x170000 (1.44 MB) | Sistema de arquivos (minimizado) |

## Otimizações Aplicadas

### 1. **Memória Máxima para Programa**
- Partição APP0 aumentada para **1.25 MB** (máximo possível com OTA)
- Partição SPIFFS reduzida ao mínimo (1.44 MB)
- NVS limitado a 20 KB (suficiente para ~100 preferências)

### 2. **Flags de Compilação** (platformio.ini)

```ini
build_flags = 
  -Os           # Otimização para tamanho (reduz .elf até 30%)
  -DNDEBUG      # Desativa assert() em release
```

### 3. **Configurações de Build**

```ini
board_build.extra_flags = 
  -DBOARD_HAS_PSRAM=0  # Desativa PSRAM (não usado neste projeto)
```

### 4. **Monitor Serial**

```ini
monitor_filters = esp32_exception_decoder  # Decodifica crashes
```

## Comparação: Antes vs Depois

### Antes (default.csv)
```
APP: 1.19 MB
SPIFFS: 1.90 MB
OTA: Habilitado (2 slots)
```

### Depois (partitions.csv) - SEM OTA
```
APP: 2.5 MB (+110%, +1.31 MB)
SPIFFS: 1.44 MB (suficiente para dados)
OTA: Desabilitado (máximo espaço)
```

## Resultado Esperado

✅ **Espaço de código: 2.5 MB** (antes era 1.19 MB)  
✅ **Ganho: +1.31 MB (+110%)** para firmware  
✅ **Otimizações de compilação** reduzem ainda mais o binário final  
✅ **Persistência de dados** garantida (NVS + SPIFFS)  
✅ **Sem OTA** (não há slot reservada para atualizações automáticas)  

## Como Usar

### Build padrão
```bash
pio run                # Compila para app0
pio run -t upload      # Grava na placa
```

### Verificar tamanho do firmware
```bash
pio run -t buildfs     # Compila filesystem
ls -lh .pio/build/esp32dev/firmware.bin
```

### Monitor serial com decoder
```bash
pio device monitor
```

## Notas Importantes

⚠️ **Sem OTA**: Este projeto **não suporta atualizações OTA** (over-the-air). Para atualizar o firmware:
- Conecte via USB
- Use `pio run -t upload`
- Máquina com terminal serial necessária

✅ **Se precisar de OTA no futuro**:
Veja a seção "Configuração Alternativa com OTA" abaixo.

## Configuração Alternativa com OTA

Se no futuro precisar de atualizações OTA, use este particionamento:

```csv
# Partição com suporte a OTA (espaço APP reduzido)
nvs,      data,  nvs,     0x9000,  0x5000,
otadata,  data,  ota,     0xe000,  0x2000,
app0,     app,   ota_0,   0x10000, 0x180000,
app1,     app,   ota_1,   0x190000, 0x180000,
spiffs,   data,  spiffs,  0x310000, 0xF0000,
```

**Trade-off**: APP = 1.5 MB cada slot (vs. 2.5 MB sem OTA)

## Referências

- [ESP32 Partition Table](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-guides/partition-tables.html)
- [PlatformIO Build Configuration](https://docs.platformio.org/en/latest/platforms/espressif32.html)
- [ESP32 Memory Layout](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/system/memory_management.html)
