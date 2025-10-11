#!/bin/bash

echo "Installing Backlight Control Service..."

# Build and install the binary
make clean && make
sudo cp backlight-ctl /usr/local/bin/
sudo chmod +x /usr/local/bin/backlight-ctl

# Create udev rule for backlight access
sudo tee /etc/udev/rules.d/90-backlight.rules > /dev/null << 'EOF'
# Allow users in video group to control keyboard backlight
SUBSYSTEM=="leds", KERNEL=="*kbd_backlight*", GROUP="video", MODE="0664"
EOF

# Add user to video group
sudo usermod -a -G video $USER

# Install systemd service
sudo cp backlight-control.service /etc/systemd/system/
sudo systemctl daemon-reload
sudo systemctl enable backlight-control.service

# Install man page
sudo cp backlight-ctl.1 /usr/share/man/man1/
sudo mandb -q

# Install GUI tools
sudo cp backlight-config /usr/local/bin/
sudo cp backlight-control-panel /usr/local/bin/
sudo chmod +x /usr/local/bin/backlight-config
sudo chmod +x /usr/local/bin/backlight-control-panel

# Create sudoers rule for service management (no password needed)
sudo tee /etc/sudoers.d/backlight-control > /dev/null << 'EOF'
# Allow users to manage backlight service without password
%video ALL=(root) NOPASSWD: /bin/systemctl start backlight-control
%video ALL=(root) NOPASSWD: /bin/systemctl stop backlight-control
%video ALL=(root) NOPASSWD: /bin/systemctl restart backlight-control
%video ALL=(root) NOPASSWD: /bin/systemctl reload backlight-control
%video ALL=(root) NOPASSWD: /bin/systemctl status backlight-control
EOF

# Reload udev rules
sudo udevadm control --reload-rules
sudo udevadm trigger

# Start the service
sudo systemctl start backlight-control.service

echo "Installation complete!"
echo ""
echo "Service Status:"
sudo systemctl status backlight-control.service --no-pager -l
echo ""
echo "The service will now:"
echo "  ✓ Start automatically on boot"
echo "  ✓ Run in background without manual intervention"
echo "  ✓ Reload configuration when you change settings"
echo ""
echo "Configuration:"
echo "  • CLI: backlight-ctl config <setting> <value>"
echo "  • GUI: ./backlight-control-panel"
echo "  • Manual control: backlight-ctl set <0|1|2>"
echo ""
echo "Service management:"
echo "  • Status: sudo systemctl status backlight-control"
echo "  • Restart: sudo systemctl restart backlight-control"
echo "  • Logs: journalctl -u backlight-control -f"