# Contributing Guide

Thank you for considering contributing to the Dimmer BLE ESP32 project! 🎉

## How to Contribute

### Report Bugs

1. Check if the bug has already been reported in [Issues](https://github.com/pabloguillermo/dimmer_ble_esp32/issues)
2. If not, open a new issue with:
   - Descriptive title
   - Clear problem description
   - Steps to reproduce
   - Expected vs. actual behavior
   - ESP32 board version and environment details

### Suggest Improvements

- Open an [Issue](https://github.com/pabloguillermo/dimmer_ble_esp32/issues) with the `enhancement` label
- Describe the motivation and use case
- Explain the proposed solution

### Submit Code

1. **Fork** the repository
2. Create a branch with a descriptive name:
   ```bash
   git checkout -b feature/your-feature
   # or
   git checkout -b fix/your-bugfix
   ```
3. Make clear commits:
   ```bash
   git commit -m "Clear description of changes"
   ```
4. Test your changes:
   ```bash
   pio run
   pio run -t upload
   ```
5. Push to your branch:
   ```bash
   git push origin feature/your-feature
   ```
6. Open a **Pull Request** describing the changes

## Code Standards

- Use 2-space indentation
- Comment complex code in English
- Follow existing code style
- Avoid lines longer than 100 characters
- Use descriptive variable names

## Review Process

- All PRs will be reviewed before merging
- Adjustments may be required
- Feedback will be constructive

## License

By contributing, you agree that your contributions are licensed under the same GNU General Public License v3.0 as the project.

## Licença

Ao contribuir, você concorda que suas contribuições estão licenciadas sob a mesma GNU General Public License v3.0 do projeto.

---

Dúvidas? Abra uma issue ou entre em contato!
