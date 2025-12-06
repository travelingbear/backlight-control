#!/bin/bash

set -e

VERSION="1.0.0"
RELEASE_URL="https://github.com/username/backlight-control/releases/download/v${VERSION}/backlight-control-${VERSION}.tar.gz"

echo "Installing Backlight Control ${VERSION}..."

# Download and extract
cd /tmp
wget -O backlight-control.tar.gz "$RELEASE_URL"
tar -xzf backlight-control.tar.gz
cd "backlight-control-${VERSION}"

# Install
./install-complete.sh

# Cleanup
cd /
rm -rf "/tmp/backlight-control-${VERSION}" /tmp/backlight-control.tar.gz

echo "Installation complete!"