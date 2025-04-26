#!/bin/bash

# Make script executable
chmod +x "$0"

echo "Setting up Lumon MDR to start automatically on boot..."

# Create a copy of the service file in the systemd directory
sudo cp lumonmdr.service /lib/systemd/system/

# Set proper permissions
sudo chmod 644 /lib/systemd/system/lumonmdr.service

# Enable and start the service
sudo systemctl daemon-reload
sudo systemctl enable lumonmdr.service

# Disable screen blanking and screensaver
echo "Disabling screen blanking and screensaver..."
mkdir -p ~/.config/lxsession/LXDE-pi
echo "@xset s off" > ~/.config/lxsession/LXDE-pi/autostart
echo "@xset -dpms" >> ~/.config/lxsession/LXDE-pi/autostart
echo "@xset s noblank" >> ~/.config/lxsession/LXDE-pi/autostart

# Also disable screen blanking in /boot/config.txt
if ! grep -q "consoleblank=0" /boot/config.txt; then
    echo "# Disable console blanking" | sudo tee -a /boot/config.txt
    echo "consoleblank=0" | sudo tee -a /boot/config.txt
fi

# Disable the cursor
if ! grep -q "disable_splash=1" /boot/config.txt; then
    echo "# Disable boot splash" | sudo tee -a /boot/config.txt
    echo "disable_splash=1" | sudo tee -a /boot/config.txt
fi

# Hide the mouse cursor when inactive
if ! grep -q "xserver-command=X -nocursor" /etc/lightdm/lightdm.conf; then
    sudo sed -i 's/^\[Seat:\*\]/[Seat:*]\nxserver-command=X -nocursor/' /etc/lightdm/lightdm.conf
fi

echo "Setup complete! The Lumon MDR application will now start automatically when the Raspberry Pi boots."
echo "Please reboot your Raspberry Pi to apply these changes: sudo reboot" 