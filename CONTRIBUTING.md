# Guia de Contribuição

Obrigado por considerar contribuir para o projeto Dimmer BLE ESP32! 🎉

## Como Contribuir

### Reportar Bugs

1. Verifique se o bug já foi reportado em [Issues](https://github.com/pabloguillermo/dimmer_ble_esp32/issues)
2. Se for novo, abra uma issue com:
   - Título descritivo
   - Descrição clara do problema
   - Passos para reproduzir
   - Resultado esperado vs. resultado atual
   - Versão da placa ESP32 e ambiente

### Sugerir Melhorias

- Abra uma [Issue](https://github.com/pabloguillermo/dimmer_ble_esp32/issues) com o rótulo `enhancement`
- Descreva a motivação e caso de uso
- Explique a solução proposta

### Submeter Código

1. **Fork** o repositório
2. Crie uma branch com nome descritivo:
   ```bash
   git checkout -b feature/sua-feature
   # ou
   git checkout -b fix/seu-bugfix
   ```
3. Faça commits claros:
   ```bash
   git commit -m "Descrição clara da mudança"
   ```
4. Teste suas mudanças:
   ```bash
   pio run
   pio run -t upload
   ```
5. Push na sua branch:
   ```bash
   git push origin feature/sua-feature
   ```
6. Abra um **Pull Request** descrevendo as mudanças

## Padrões de Código

- Use indentação de 2 espaços
- Comente código complexo em português/inglês
- Siga o estilo do código existente
- Evite linhas com mais de 100 caracteres
- Use nomes descritivos para variáveis

## Processo de Review

- Todas as PRs serão revisadas antes de merge
- Pode ser necessário fazer ajustes
- Feedback será fornecido de forma construtiva

## Licença

Ao contribuir, você concorda que suas contribuições estão licenciadas sob a mesma licença MIT do projeto.

---

Dúvidas? Abra uma issue ou entre em contato!
