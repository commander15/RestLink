#!/bin/bash
set -e

QT_VERSION=6.9.0
INSTALLER_VERSION=4.9

INSTALLER="qt-online-installer-linux-x64-${INSTALLER_VERSION}.0.run"
INSTALL_DIR=/opt/Qt

QT_COMPONENT="qt${QT_VERSION}-essentials-dev"

echo 'Downloading installer...'
wget "https://download.qt.io/archive/online_installers/${INSTALLER_VERSION}/${INSTALLER}"

chmod +x $INSTALLER

./$INSTALLER \
  --platform minimal \
  --root "$INSTALL_DIR" \
  --accept-licenses \
  --accept-messages \
  --accept-obligations \
  --confirm-command \
  --default-answer \
  install $QT_COMPONENT
