# Ziga Pomodoro 🍅

[![GitHub Release](https://img.shields.io/github/v/release/zigameni/ziga-pomodoro?style=flat-square)](https://github.com/zigameni/ziga-pomodoro/releases)
[![License](https://img.shields.io/badge/license-MIT-blue.svg?style=flat-square)](LICENSE)

A minimalist Qt-based Pomodoro timer with productivity tracking

![Application Screenshot](/images/img.png)

## Features ✨

### Core Functionality

- 🕒 Customizable Pomodoro intervals (work/break)
- 🔔 Visual & sound notifications
- 📊 Session tracking with local database
- ⏭️ Skip break functionality
- 🐧 **Linux-first design** (AppImage package)

### Customization

- 🎨 Adjustable font size/color
- 🌑 Text shadow customization
- 🖼️ Mode-based color themes
- 🔧 Settings window for preferences

### UX Enhancements

- 🔲 Frameless always-on-top window
- 🖱️ Draggable interface
- 🗑️ Discrete close button
- 📌 System tray integration

---

## Installation 💻

### Linux (AppImage)

```bash
wget https://github.com/zigameni/ziga-pomodoro/releases/download/v1.0.0/Ziga-Pomodoro-1.0.0-x86_64.AppImage
chmod +x Ziga-Pomodoro-*.AppImage
./Ziga-Pomodoro-*.AppImage
```

### From Source

**Prerequisites:**

- Qt 5
- C++17 compiler
- CMake 3.12+

**Build:**

```bash
git clone https://github.com/zigameni/ziga-pomodoro.git
cd ziga-pomodoro
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

---

## Inspiration 💡

Developed with [YAPA2](https://github.com/YetAnotherPomodoroApp/YAPA-2) in mind - bringing similar productivity features
to Linux users.

---

## License 📄

MIT License - See [LICENSE](LICENSE) for details.

---
Built for me, but sharing is caring ❤️