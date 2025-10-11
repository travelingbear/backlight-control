#!/bin/bash

# Package script for distribution

VERSION="1.0.0"
PACKAGE_NAME="backlight-control-${VERSION}"

echo "Creating distribution package..."

# Create package directory
mkdir -p dist/${PACKAGE_NAME}

# Copy essential files
cp backlight-ctl.c dist/${PACKAGE_NAME}/
cp Makefile dist/${PACKAGE_NAME}/
cp backlight-control.service dist/${PACKAGE_NAME}/
cp backlight-ctl.1 dist/${PACKAGE_NAME}/
cp install-service.sh dist/${PACKAGE_NAME}/
cp backlight-config dist/${PACKAGE_NAME}/
cp backlight-control-panel dist/${PACKAGE_NAME}/
cp README.md dist/${PACKAGE_NAME}/

# Make scripts executable
chmod +x dist/${PACKAGE_NAME}/install-service.sh
chmod +x dist/${PACKAGE_NAME}/backlight-config
chmod +x dist/${PACKAGE_NAME}/backlight-control-panel

# Create tarball
cd dist
tar -czf ${PACKAGE_NAME}.tar.gz ${PACKAGE_NAME}/
cd ..

echo "Package created: dist/${PACKAGE_NAME}.tar.gz"
echo ""
echo "Distribution contents:"
echo "  ✓ Source code and build system"
echo "  ✓ Systemd service configuration"
echo "  ✓ Installation script"
echo "  ✓ GUI configuration tools"
echo "  ✓ Man page documentation"
echo "  ✓ Complete README"
echo ""
echo "Users can install with:"
echo "  tar -xzf ${PACKAGE_NAME}.tar.gz"
echo "  cd ${PACKAGE_NAME}"
echo "  ./install-service.sh"