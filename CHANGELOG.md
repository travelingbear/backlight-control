# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.0] - 2024-10-11

### Added
- Initial release of keyboard backlight control daemon
- Automatic backlight management with i8042 interrupt monitoring
- Power-aware timeout system (different settings for battery vs AC)
- Gradual dimming feature on AC power (2→1→0 transition)
- CLI control interface with `backlight-ctl` command
- GUI configuration wizard (`backlight-config`)
- GUI control panel (`backlight-control-panel`)
- Systemd service integration with auto-start capability
- Configuration persistence with hot-reload support
- Complete installation script (`install-service.sh`)
- Man page documentation
- Desktop integration support
- MIT license

### Features
- Activity detection for laptop keyboard and touchpad only
- Configurable timeouts (5-300s battery, 10-600s AC)
- Three brightness levels (0=off, 1=low, 2=high)
- Minimal resource usage (1.7MB RAM, near-zero CPU when idle)
- Automatic service reload on configuration changes
- Comprehensive error handling and logging

### Compatibility
- ThinkPad laptops with `tpacpi::kbd_backlight` interface
- Ubuntu/Debian-based distributions
- systemd-based Linux systems
- Requires zenity for GUI tools