# Keyboard Backlight Control

**Version 1.0.0**

A lightweight, efficient daemon for automatic keyboard backlight control on Linux laptops. Automatically manages backlight based on keyboard and touchpad activity with power-aware timeouts.

## Features

- **Power-aware timeouts** - Different settings for battery vs AC power
- **Activity detection** - Monitors laptop keyboard and touchpad only (ignores external devices)
- **Gradual dimming** - Smooth transition from bright → dim → off on AC power
- **Easy configuration** - CLI and GUI configuration tools
- **Minimal resource usage** - Only 1.7MB RAM, near-zero CPU when idle
- **Systemd integration** - Runs as system service, starts on boot
- **Manual control** - Set backlight levels manually when needed

## Quick Install

```bash
git clone https://github.com/travelingbear/backlight-control.git
cd backlight-control
./install-service.sh
```

That's it! The service will start automatically and run on every boot.

### Optional: Desktop Integration

Add to applications menu:
```bash
sudo tee /usr/share/applications/backlight-control.desktop > /dev/null << 'EOF'
[Desktop Entry]
Name=Backlight Control
Comment=Keyboard backlight configuration
Exec=backlight-config
Icon=preferences-desktop-keyboard
Type=Application
Categories=Settings;System;
StartupNotify=true
EOF
sudo update-desktop-database
```

## Requirements

- Linux laptop with keyboard backlight (tested on ThinkPad)
- Root access for installation
- systemd-based distribution

## Usage

### Manual Control
```bash
backlight-ctl set 2        # Set to bright
backlight-ctl set 1        # Set to dim  
backlight-ctl set 0        # Turn off
backlight-ctl get          # Show current level
```

### Configuration
```bash
# Command line
backlight-ctl config battery_timeout 45
backlight-ctl config ac_timeout 180
backlight-ctl config ac_default_level 2
backlight-ctl config battery_default_level 1

# GUI configuration
backlight-config

# Control panel  
backlight-control-panel

# Desktop entry (if installed)
# Search "Backlight Control" in Activities
```

### Service Management
```bash
sudo systemctl status backlight-control    # Check status
sudo systemctl restart backlight-control   # Restart service
journalctl -u backlight-control -f         # View logs
```

## Configuration Options

| Setting | Description | Range | Default |
|---------|-------------|-------|---------|
| `battery_timeout` | Seconds before turning off on battery | 5-300 | 30 |
| `ac_timeout` | Seconds before turning off on AC power | 10-600 | 120 |
| `ac_default_level` | Brightness level on AC power | 0-2 | 2 |
| `battery_default_level` | Brightness level on battery | 0-2 | 1 |

## How It Works

1. **Activity Detection**: Monitors i8042 interrupts (laptop keyboard/touchpad only)
2. **Smart Timeouts**: Uses shorter timeouts on battery to save power
3. **Gradual Dimming**: On AC with level 2, dims gradually (2→1→0) over timeout period
4. **Auto-reload**: Configuration changes apply immediately without restart

## Files

- `backlight-ctl` - Main binary
- `backlight-ctl.c` - Source code
- `backlight-control.service` - Systemd service
- `backlight-config` - GUI configuration tool
- `backlight-control-panel` - GUI control panel
- `install-service.sh` - Installation script
- `Makefile` - Build system

## Building from Source

```bash
make clean
make
sudo make install
```

## Uninstall

```bash
sudo systemctl stop backlight-control
sudo systemctl disable backlight-control
sudo rm -f /etc/systemd/system/backlight-control.service
sudo rm -f /usr/local/bin/backlight-ctl
sudo rm -f /usr/local/bin/backlight-config
sudo rm -f /usr/local/bin/backlight-control-panel
sudo rm -f /usr/share/man/man1/backlight-ctl.1
sudo rm -f /etc/udev/rules.d/90-backlight.rules
sudo rm -f /etc/sudoers.d/backlight-control
sudo rm -f /usr/share/applications/backlight-control.desktop
sudo systemctl daemon-reload
sudo update-desktop-database
```

## Troubleshooting

**Service not starting:**
```bash
sudo systemctl status backlight-control
journalctl -u backlight-control
```

**Permission issues:**
```bash
# Check if user is in video group
groups
# Add user to video group if needed
sudo usermod -a -G video $USER
# Log out and back in
```

**Manual testing:**
```bash
# Test daemon directly
sudo backlight-ctl daemon --debug
```

## Performance

- **Memory usage**: ~1.7 MB RAM
- **CPU usage**: Near-zero when idle
- **Battery impact**: Minimal (optimized for laptop use)
- **Startup time**: Instant

## Compatibility

Tested on:
- ThinkPad laptops with `tpacpi::kbd_backlight`
- Ubuntu/Debian-based distributions
- systemd-based systems

Should work on most Linux laptops with keyboard backlight support.

## License

Open source - feel free to modify and distribute.

## Contributing

1. Fork the repository
2. Make your changes
3. Test thoroughly
4. Submit a pull request

## Support

- Check logs: `journalctl -u backlight-control -f`
- Manual page: `man backlight-ctl`
- Debug mode: `sudo backlight-ctl daemon --debug`