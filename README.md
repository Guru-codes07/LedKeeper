# LedKeeper

> **A lightweight Linux daemon that keeps keyboard LED states persistent.**

LedKeeper is a lightweight Linux utility written in **pure C** that keeps keyboard RGB lighting enabled on keyboards whose RGB is controlled by the **Scroll Lock LED**.

Following the Unix philosophy, LedKeeper is designed to do one thing well. It automatically detects the correct Scroll Lock LED device, monitors its state, and restores it whenever the Linux kernel disables it.

Originally developed for **Fedora Linux** running **Hyprland (Wayland)**, LedKeeper is intended to work on any compatible keyboard that uses the Scroll Lock LED to control RGB lighting.

---

## The Problem

Some keyboards, control their RGB lighting through the **Scroll Lock LED**.

On Linux, especially under **Wayland** compositors such as **Hyprland**, the kernel resets the Scroll Lock LED whenever another key is pressed. As a result, the keyboard RGB lighting immediately turns off.

Under **X11**, utilities like `xset` could manipulate keyboard LED states. Since Wayland provides no equivalent userspace interface, keeping the RGB enabled becomes difficult.

LedKeeper solves this by continuously monitoring the Scroll Lock LED and restoring it whenever the kernel turns it off.

---

# Features

- Written in **pure C**
- Lightweight and efficient
- Follows the Unix philosophy
- Minimal CPU and memory usage
- Automatically detects the correct Scroll Lock LED device
- No hardcoded LED paths
- Optional manual LED path override using `--led-path`
- Compatible with keyboards that use the Scroll Lock LED for RGB control
- Can run as a background daemon
- Installable as a **systemd user service**
- Clean shutdown on `SIGINT` and `SIGTERM`
- Modular and maintainable codebase
- MIT Licensed

---

# Project Structure

```text
LedKeeper/
├── src/
│   ├── main.c            # Entry point
│   ├── led.c             # LED detection and control
│   ├── led.h
│   ├── daemon.c          # Daemon lifecycle and signal handling
│   ├── daemon.h
│   ├── logger.c          # Logging utilities
│   └── logger.h
│
├── systemd/
│   └── ledkeeper.service # User systemd service
│
├── config/
│   └── ledkeeper.conf.example  # Example configuration file
│
├── tests/
│   └── led_test.c        # LED test program
│
├── Makefile              # Build system
├── README.md             # Project documentation
├── LICENSE               # MIT License
└── .gitignore
```

---

# Requirements

- Linux
- GCC
- Make
- systemd (optional)
- A compatible keyboard whose RGB lighting is controlled by the Scroll Lock LED

---

# Building

Clone the repository:

```bash
git clone https://github.com/Guru-codes07/LedKeeper.git
cd LedKeeper
```

Build using the provided Makefile:

```bash
make
```

Or compile manually:

```bash
gcc -Wall -Wextra -O2 src/*.c -o ledkeeper
```

---

# Installation

Install the binary:

```bash
sudo make install
```

Install the provided systemd user service:

```bash
mkdir -p ~/.config/systemd/user

cp systemd/ledkeeper.service ~/.config/systemd/user/

systemctl --user daemon-reload

systemctl --user enable ledkeeper.service

systemctl --user start ledkeeper.service
```

Verify the service:

```bash
systemctl --user status ledkeeper.service
```

---

# Usage

```
ledkeeper [OPTIONS]
```

### Run as a background daemon

```bash
ledkeeper --daemon
```

Runs LedKeeper continuously in the background.

---

### Turn the LED on once

```bash
ledkeeper --once
```

Turns the detected Scroll Lock LED on once and exits.

---

### Show daemon status

```bash
ledkeeper --status
```

Displays the current daemon and LED status.

---

### List detected LEDs

```bash
ledkeeper --list-leds
```

Lists all compatible Scroll Lock LED devices detected under:

```text
/sys/class/leds/
```

---

### Override the detected LED

```bash
ledkeeper --led-path /sys/class/leds/input3::scrolllock/brightness
```

Useful for systems where automatic detection is not desired.

---

### Show version

```bash
ledkeeper --version
```

Displays version information.

---

### Show help

```bash
ledkeeper --help
```

Displays all available command-line options.

---

# Configuration

LedKeeper automatically detects the appropriate Scroll Lock LED device, so no configuration is required for most systems.

An example configuration file is provided:

```text
config/ledkeeper.conf.example
```

This can be used as a starting point for custom configurations if manual overrides are needed.

---

# How It Works

At startup, LedKeeper scans:

```text
/sys/class/leds/
```

for Scroll Lock LED devices.

After detecting a compatible device, it monitors the LED state. Whenever the Linux kernel resets the LED, LedKeeper immediately restores it, allowing compatible keyboards to keep their RGB lighting enabled.

---

# Tested On

- Fedora Linux
- Hyprland (Wayland)
- My Zebronics keyboard using Scroll Lock LED controlled RGB lighting

---

# License

This project is licensed under the **MIT License**.

Copyright (c) 2026 **Guru Prasad Mishra**

See the [LICENSE](LICENSE) file for complete license information.

---

## Author

**Guru Prasad Mishra**

Built to solve a simple but surprisingly annoying Linux problem—keeping a keyboard glowing under Wayland.
