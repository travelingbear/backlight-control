#!/bin/bash

set -e

echo "Installing Backlight Control with Status Bar..."

# Install main service
./install-service.sh

# Install status bar script
echo "Installing status bar script..."
sudo cp backlight-status-bar.sh /usr/local/bin/
sudo chmod +x /usr/local/bin/backlight-status-bar.sh

# Create argos directory if it doesn't exist
mkdir -p ~/.config/argos

# Install status bar for current user
cp backlight-status-bar.sh ~/.config/argos/backlight.5s.sh
chmod +x ~/.config/argos/backlight.5s.sh

echo "Installation complete!"
echo ""
echo "Status bar installed to: ~/.config/argos/backlight.5s.sh"
echo "System copy available at: /usr/local/bin/backlight-status-bar.sh"
echo ""
echo "If using GNOME with Argos extension, the status bar should appear automatically."
echo "For other status bar systems, use: /usr/local/bin/backlight-status-bar.sh"