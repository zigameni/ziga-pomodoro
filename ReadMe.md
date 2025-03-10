# Ziga-Pomodoro (work in progress)

A minimalist and customizable Pomodoro timer application built with Qt.

![Screenshot of Ziga-Pomodoro](/images/img.png)

## Description

Ziga-Pomodoro is a simple, distraction-free Pomodoro timer designed to help you stay focused and productive. It
implements the Pomodoro Technique, cycling between work intervals and short/long breaks. The application is designed to
be unobtrusive, always staying on top of your other windows, and offers a range of customization options to fit your
workflow and preferences.

## Features

* **Frameless and Always-on-Top Window:** Stays visible without cluttering your workspace.
* **Customizable Timer Durations:**  Adjust work, short break, and long break durations to your liking through the
  settings menu.
* **Visual and Sound Notifications:**
    * **Desktop Notifications:** Receive notifications when work or break sessions are completed (configurable).
    * **Sound Notifications:**  Plays a sound when a timer cycle finishes (configurable, allows custom sound file
      selection).
* **Customizable Appearance:**
    * **Font Size and Color:**  Adjust the timer display font to your preferred size and color.
    * **Text Shadow:** Enable or disable a text shadow with customizable offset, blur, and color for improved
      readability.
    * **Mode-Based Styling:** The timer display visually changes color to indicate work and break modes.
* **Session Tracking (Database Integration):**
    * Records completed Pomodoro work sessions and break sessions locally for personal progress tracking.
    * Tracks session start times and durations.
* **Skip Break Feature:**  Option to skip the current break interval and move to the next work session.
* **System Tray Icon:**  Provides application status in the system tray and delivers desktop notifications.
* **Settings Window:**  Access a dedicated settings window to configure all application preferences.
* **Draggable Window:**  Freely move the timer window around your screen.
* **Start/Pause, Stop, and Skip Controls:** Easy-to-use buttons that appear on hover for controlling the timer.
* **Close Button:**  Discreet close button for quickly exiting the application.

## Getting Started

### Prerequisites

* **Qt Framework:** You need to have the Qt Framework (version 5 or 6 recommended) installed on your system. Ensure you
  have the necessary Qt modules (like `QtWidgets`, `QtGui`, `QtMultimedia`, `QtSystemTrayIcon`, `QtSql`). You can
  download Qt from the [official Qt website](https://www.qt.io/downloads).
* **C++ Compiler:**  A C++ compiler compatible with Qt (like GCC, Clang, or MSVC).
* **Build System:**  `qmake` or CMake (typically comes with Qt).

### Building the Application

1. **Clone the repository** (if you are distributing via a repository):
   ```bash
   git clone ... 
   cd ziga-pomodoro
   ```

2. **Navigate to the project directory** in your terminal.

3. **Run `qmake` or use CMake** to generate the build files.

      ```

4. **Build the project** using your system's build tool.

   ...

### Project Structure

```
ziga-pomodoro/
├── src/
│   ├── main.cpp
│   ├── timerwindow.cpp
│   ├── timerwindow.h
│   ├── timer.cpp         *(Assumed to exist based on includes)*
│   ├── timer.h           *(Assumed to exist based on includes)*
│   ├── settings.cpp      *(Assumed to exist based on includes)*
│   ├── settings.h        *(Assumed to exist based on includes)*
│   ├── mainwindow.cpp    *(Assumed to exist based on includes)*
│   ├── mainwindow.h      *(Assumed to exist based on includes)*
│   ├── databasemanager.cpp *(Assumed to exist based on includes)*
│   ├── databasemanager.h   *(Assumed to exist based on includes)*
│   ├── ...
├── resources/
│   ├── icons/
│   │   └── tomato.png
│   ├── sounds/
│   │   └── alarm.wav 
│   └── styles/
│       └── ... (style sheets if any)
├── CMakeLists.txt        *(If using CMake)*
└── readme.md
```

## Usage

1. **Run the Ziga-Pomodoro application.**
2. The timer window will appear on your screen, staying always on top. Initially, only the timer display and a close
   button are visible.
3. **Hover over the timer window** to reveal the control buttons:
    * **Start/Pause Button:**  Starts or pauses the timer. The icon toggles to reflect the timer state.
    * **Stop Button:** Resets the timer to the beginning of the current mode (work or break).
    * **Skip Break Button:** (Enabled during break sessions) Skips the current break and immediately starts the next
      work session.
    * **Settings Button:** Opens the settings window to customize application behavior.
4. **Use the Settings window** to adjust:
    * Work, Short Break, and Long Break durations.
    * Font size and color for the timer display.
    * Text shadow settings (enable/disable, offset, blur, color).
    * Enable/disable desktop notifications and sound notifications.
    * Select a custom sound file for notifications.
5. **Close the application** using the "×" button in the top-right corner of the timer window.

## Configuration

All configurations are managed through the **Settings window**, accessible via the settings button on the timer window.
Settings are saved automatically.

## Database

Ziga-Pomodoro utilizes a local database to store records of your Pomodoro work sessions and break sessions. This allows
for basic tracking of your time and productivity.

## Contributing

Contributions are welcome! If you have suggestions, bug reports, or want to contribute code, please feel free to open
issues or pull requests on [your repository link - *replace with your repository link if applicable*].

## License

[MIT License] - See the `LICENSE` file for details.

## Acknowledgments

* This application was built using the (Qt5) [Qt Framework](https://www.qt.io/).

