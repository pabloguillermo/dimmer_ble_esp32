# Dimmer BLE

Projeto de dimmer controlado por Bluetooth BLE para placas ESP32 WROOM32.

## Estrutura

- `src/main.cpp` - firmware principal
- `platformio.ini` - configuração do projeto PlatformIO
- `README.md` - documentação do projeto

## Hardware alvo

- ESP32 WROOM32
- GPIO 32: saída PWM do dimmer
- GPIO 34: botão "mais"
- GPIO 13: botão "menos"
- GPIO 26: botão liga/desliga

## Como usar no PlatformIO

1. Abra a pasta no VS Code.
2. Se ainda não estiver instalado, instale a extensão PlatformIO IDE.
3. No menu do PlatformIO, clique em "Build".
4. Para gravar: "Upload".
5. Para monitorar serial: "Device Monitor".

## Comandos úteis

```bash
pio run
pio run -t upload
pio device monitor
```

## Observações

- O firmware usa o BLE integrado do ESP32.
- A configuração de persistência salva a potência e estado em `Preferences`.
- O projeto foi migrado do formato antigo de Arduino para a estrutura padrão do PlatformIO.
