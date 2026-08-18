# Dimmer BLE ESP32

![License](https://img.shields.io/badge/license-MIT-green)
![Platform](https://img.shields.io/badge/platform-ESP32-blue)

Firmware profissional para controle de dimmer por Bluetooth Low Energy (BLE) em placas ESP32. Sistema de autenticação integrado, persistência de dados e controle via botões físicos.

## 📋 Descrição

Este projeto implementa um dimmer inteligente (controlador de potência PWM) com interface BLE. Permite controlar a intensidade de iluminação/carga via aplicativo mobile ou botões físicos na placa, com autenticação por senha e salvamento automático de configurações.

### ✨ Funcionalidades

- **Controle BLE**: Comunicação bidirecional via Bluetooth Low Energy
- **Autenticação**: Sistema de autenticação com senha configurável
- **Persistência**: Salvamento automático de estado e configurações usando Preferences
- **Controle Físico**: 3 botões para ajuste manual (mais, menos, liga/desliga)
- **PWM Dinâmico**: Calibração avançada de potência com múltiplos modos
- **Notificações**: Feedback em tempo real dos estados via BLE
- **Segurança**: Reinicialização automática em caso de falha de autenticação

## 🔧 Configuração de Hardware

### Pinagem (ESP32 WROOM32)

| Função | GPIO | Tipo |
|--------|------|------|
| Saída PWM Dimmer | 32 | Digital Output |
| Botão Mais | 34 | Digital Input |
| Botão Menos | 13 | Digital Input |
| Botão Liga/Desliga | 26 | Digital Input |

### Requisitos

- **Placa**: ESP32 WROOM32 ou compatível
- **Tensão**: 5V USB ou fonte externa
- **Periféricos**: 3 botões tacteis, resistores pull-up (opcional)

## 📦 Estrutura do Projeto

```
dimmer_ble_esp32/
├── src/
│   └── main.cpp          # Firmware principal
├── include/              # Headers customizados (vazio por padrão)
├── lib/                  # Bibliotecas locais (vazio por padrão)
├── test/                 # Testes (estrutura PlatformIO)
├── platformio.ini        # Configuração de compilação
├── .gitignore           # Git ignore rules
└── README.md            # Este arquivo
```

## 🚀 Como Compilar e Gravar

### Pré-requisitos

- VS Code instalado
- Extensão PlatformIO IDE instalada
- Drivers USB CH340/CP2102 (para upload via serial)

### Passos

1. **Clonar o repositório**
   ```bash
   git clone https://github.com/pabloguillermo/dimmer_ble_esp32.git
   cd dimmer_ble_esp32
   ```

2. **Abrir no VS Code**
   ```bash
   code .
   ```

3. **Compilar**
   - Menu PlatformIO → Build, ou
   ```bash
   pio run
   ```

4. **Gravar na placa**
   - Menu PlatformIO → Upload, ou
   ```bash
   pio run -t upload
   ```

5. **Monitorar serial (debug)**
   - Menu PlatformIO → Device Monitor, ou
   ```bash
   pio device monitor
   ```

## 📡 Protocolo BLE

### UUIDs Fixos

```
Service UUID:        6E400001-B5A3-F393-E0A9-E50E24DCCA9E
RX Characteristic:   6E400002-B5A3-F393-E0A9-E50E24DCCA9E
TX Characteristic:   6E400003-B5A3-F393-E0A9-E50E24DCCA9E
```

### Comandos Suportados

| Comando | Formato | Descrição | Exemplo |
|---------|---------|-----------|---------|
| Autenticação | `p<senha>` | Envia senha (4 dígitos) | `p1234` |
| Potência | `c<0-100>` | Define porcentagem | `c75` |
| Liga/Desliga | `s<0\|1>` | 0=desliga, 1=liga | `s1` |
| Modo | `t<L\|M>` | Linear ou Máximo | `tL` |

**Nota**: Requer autenticação antes de executar comandos.

## 🔐 Autenticação

- Senha padrão: `1234` (configurável via Preferences)
- Senha mestra: `1234` (sempre funciona se nenhuma foi configurada)
- Falha de autenticação: Reinicia a placa automaticamente

## 📊 Variáveis de Estado Persistentes

As seguintes informações são salvas automaticamente e recuperadas ao reiniciar:

- Porcentagem de potência (0-100)
- Estado (ligado/desligado)
- Modo de operação (Linear/Máximo)
- Senha cadastrada

## 🛠️ Desenvolvimento

### Estrutura de Código

- **setup()**: Inicialização de GPIO, BLE e persistência
- **loop()**: Processamento de botões, timers e sincronização
- **calculaPWM()**: Função de mapeamento não-linear de PWM
- **Callbacks BLE**: Processamento de comandos via Bluetooth

### Compilar com Debug

Descomente em `platformio.ini`:
```ini
build_flags = -DCORE_DEBUG_LEVEL=3
```

## 📝 Licença

Este projeto está licenciado sob a licença MIT. Veja [LICENSE](LICENSE) para detalhes.

## 👤 Autor

Pablo Guillermo - [GitHub](https://github.com/pabloguillermo)

## 🐛 Reportar Problemas

Encontrou um bug? Abra uma [issue](https://github.com/pabloguillermo/dimmer_ble_esp32/issues) no GitHub.

## 📚 Referências

- [Documentação ESP32](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/)
- [PlatformIO Docs](https://docs.platformio.org/)
- [BLE Specification](https://www.bluetooth.com/specifications/specs/)

---

**Última atualização**: Agosto 2026
