#!/bin/bash

# Get backlight status from system config
if [ -f "/etc/backlight-control.conf" ]; then
    ENABLED=$(grep "enabled=" /etc/backlight-control.conf | cut -d= -f2)
else
    ENABLED=1  # Default to enabled if no config
fi

# Get current backlight level
LEVEL=$(backlight-ctl get 2>/dev/null)

# Choose display based on status
if [ "$ENABLED" = "1" ]; then
    DISPLAY="⌨A"
else
    DISPLAY="⌨M"
fi

# Show in top bar
echo "$DISPLAY"

echo "---"

# Menu items
if [ "$ENABLED" = "1" ]; then
    echo "Disable Auto Mode | bash='pkexec backlight-ctl disable' terminal=false refresh=true"
else
    echo "Enable Auto Mode | bash='pkexec backlight-ctl enable' terminal=false refresh=true"
fi

echo "---"
echo "Configuration | bash='pkexec env DISPLAY=\$DISPLAY WAYLAND_DISPLAY=\$WAYLAND_DISPLAY XDG_RUNTIME_DIR=\$XDG_RUNTIME_DIR backlight-config' terminal=false"
echo "Status | bash='backlight-ctl status' terminal=true"