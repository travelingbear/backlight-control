#!/bin/bash

set -e

# Get version
VERSION=$(cat VERSION)
RELEASE_NAME="backlight-control-${VERSION}"
RELEASE_DIR="releases/${RELEASE_NAME}"

echo "Creating release package for version ${VERSION}..."

# Clean and build
make clean
make

# Create release directory
mkdir -p releases
rm -rf "${RELEASE_DIR}"
mkdir -p "${RELEASE_DIR}"

# Copy files
cp backlight-ctl "${RELEASE_DIR}/"
cp backlight-config "${RELEASE_DIR}/"
cp backlight-control-panel "${RELEASE_DIR}/"
cp backlight-status-bar.sh "${RELEASE_DIR}/"
cp backlight-control.service "${RELEASE_DIR}/"
cp backlight-control.policy "${RELEASE_DIR}/"
cp backlight-ctl.1 "${RELEASE_DIR}/"
cp install-service.sh "${RELEASE_DIR}/"
cp install-complete.sh "${RELEASE_DIR}/"
cp README.md "${RELEASE_DIR}/"
cp LICENSE "${RELEASE_DIR}/"
cp VERSION "${RELEASE_DIR}/"
cp CHANGELOG.md "${RELEASE_DIR}/"

# Make scripts executable
chmod +x "${RELEASE_DIR}/install-service.sh"
chmod +x "${RELEASE_DIR}/install-complete.sh"
chmod +x "${RELEASE_DIR}/backlight-status-bar.sh"

# Create archive
cd releases
tar -czf "${RELEASE_NAME}.tar.gz" "${RELEASE_NAME}"
zip -r "${RELEASE_NAME}.zip" "${RELEASE_NAME}"

echo "Release packages created:"
echo "  releases/${RELEASE_NAME}.tar.gz"
echo "  releases/${RELEASE_NAME}.zip"
echo ""
echo "Installation instructions:"
echo "  tar -xzf ${RELEASE_NAME}.tar.gz"
echo "  cd ${RELEASE_NAME}"
echo "  ./install-complete.sh"