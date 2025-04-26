# Introduction

The goal of this project was to create a working miniature desktop display inspired by the 'Macrodata Refinement' computers featured in Apple TV's *Severance*. This build consists of two key aspects: the **software** and the **hardware design**, both outlined below.

---
---

# I. The Software

#### Objectives

  - Generate a randomized **number grid** with grouped 'bad numbers'.  
  - Animate and style the **interface** to match the aesthetics of the show. 

## a. The Number Grid  

> **GENERATE**  
  - A *N×N* grid is created.  
  - Each cell is filled with a random integer between 0-9.  

> **MAP**  
  - A Perlin noise map assigns smooth gradient values to the grid.  
  - This ensures numbers are naturally grouped rather than randomly scattered.  

> **THRESHOLD**  
  - Each cell's value is compared to a set threshold.  
  - Values above the threshold are marked as 'bad'/'scary' numbers.  

> **GROUP**  
  - 'Bad' numbers are grouped based on proximity to one another.  
  - At random intervals, a group visible in the viewport will be activated:  
    - If hovered over, they become 'super active', extending their active time and appearing agitated.  
    - If clicked, the group is **'refined'**, animating into a pre-determined bin and resetting as no longer 'bad'.  

## b. The Interface
- A moving Perlin noise map offsets each number (vertically or horizontally).  
- Activated bad groups expand / contract / jitter.
- Numbers scale-up based on cursor position.
- Refined groups animate into bins, with a percent bar keeping track of your progress.  

#### Additional Features:
- The application is cross-compatible, allowing compilation for both Linux and Raspberry Pi (ARM).
- A *'debug mode'* reveals various settings (which can be saved / loaded from disk as JSON).
- An *'idle mode'* can be enabled to display the Lumon logo screensaver.
- The user can navigate the full grid using arrow and zoom keys.

---
---

# II. Hardware Design

#### Objectives

  - Reconstruct the computer housing from the show as a printable 3D model.
  - Design the interior to support a RaspberryPi and LCD screen 

## Final Design
- The computer's outer shell was modeled in Blender from the ground up, closely following reference images from the show to capture the original look.
- The design was then optimized for 3D printing.
  - The structure was made modular, allowing for easy assembly and disassembly.
  - Each part includes a flat printing surface, enabling clean prints without the need for supports.
  - Tolerances were incorporated to ensure a snug fit for all components.
- Finally, the interior was equipped with an adjustable Raspberry Pi mount.
  - The mounting bracket can move laterally, and the RPi forward, allowing for fine-tuned alignment of the LCD screen with the front panel.

# Lumon MDR Auto-Start Setup

This guide explains how to set up your Lumon MDR application to automatically start when the Raspberry Pi boots up.

## Installation

1. Make sure your application is compiled and working correctly first
2. Run the installation script to set up auto-start:

```bash
chmod +x install_autostart.sh
./install_autostart.sh
```

3. Reboot your Raspberry Pi:

```bash
sudo reboot
```

## What the Setup Does

The installation script:

1. Creates a systemd service that automatically starts the application when the Pi boots
2. Disables screen blanking and screensavers
3. Configures the application to run in full-screen mode

## Manual Setup (if needed)

If you need to manually set up the auto-start:

1. Copy the systemd service file:
   ```bash
   sudo cp lumonmdr.service /lib/systemd/system/
   sudo chmod 644 /lib/systemd/system/lumonmdr.service
   sudo systemctl daemon-reload
   sudo systemctl enable lumonmdr.service
   ```

2. Disable screen blanking:
   ```bash
   mkdir -p ~/.config/lxsession/LXDE-pi
   echo "@xset s off" > ~/.config/lxsession/LXDE-pi/autostart
   echo "@xset -dpms" >> ~/.config/lxsession/LXDE-pi/autostart
   echo "@xset s noblank" >> ~/.config/lxsession/LXDE-pi/autostart
   ```

## Troubleshooting

If the application doesn't start automatically after reboot:

1. Check the service status:
   ```bash
   sudo systemctl status lumonmdr.service
   ```

2. Check the system logs:
   ```bash
   sudo journalctl -u lumonmdr.service
   ```

3. Make sure the path to your executable is correct in the service file
