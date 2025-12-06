# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.1.0] - 2025-01-02

### Added
- Status bar integration with `backlight-status-bar.sh`
- Complete installation script (`install-complete.sh`) that includes status bar
- Automatic Argos extension support for GNOME users
- Release packaging system with `create-release.sh`
- System-wide status bar script installation

### Changed
- Updated installation process to include status bar by default
- Improved Makefile with status bar installation targets
- Enhanced documentation with status bar usage instructions

### Features
- Status bar shows ⌨A (auto mode) or ⌨M (manual mode)
- Click menu with toggle mode, configuration, and status options
- Compatible with Argos extension and other status bar systems
- Distributable release packages (.tar.gz and .zip)

## [1.0.0] - 2025-01-02

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