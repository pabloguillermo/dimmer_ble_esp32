# Changelog

Todas as mudanças notáveis neste projeto estão documentadas neste arquivo.

O formato é baseado em [Keep a Changelog](https://keepachangelog.com/),
e este projeto segue [Semantic Versioning](https://semver.org/).

## [1.0.0] - 2026-08-18

### Added
- Firmware inicial para controle de dimmer BLE em ESP32
- Suporte a Bluetooth Low Energy (BLE) com autenticação
- Sistema de persistência usando Preferences
- Controle físico via 3 botões (mais, menos, liga/desliga)
- Calibração dinâmica de PWM
- Notificações BLE em tempo real
- Segurança com reinicialização em falha de autenticação
- Documentação completa e exemplos
- CI/CD com GitHub Actions
- Estrutura profissional do projeto

### Features
- Protocolo BLE com UUIDs fixos
- Comandos: autenticação, potência, liga/desliga, modo
- Salvamento automático de configurações
- Modo debug com logs seriais
- Suporte a múltiplas placas ESP32

## [Planejado] - Próximas Versões

### Em Desenvolvimento
- [ ] Aplicativo mobile para iOS/Android
- [ ] Interface web para configuração
- [ ] Suporte a múltiplos dimmers em rede
- [ ] Scheduling/automação de horários
- [ ] Sincronização com Home Assistant
- [ ] Modo low-power/sleep

### Considerado
- Gerador de código QR para emparelhamento rápido
- Logs de histórico de uso
- API REST

---

**Maintainer**: Pablo Guillermo  
**Repositório**: https://github.com/pabloguillermo/dimmer_ble_esp32
