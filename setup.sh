#!/bin/bash

# --- Configuration ---
APPLICATION_NAME="ziga-pomodoro"  # Make sure this matches your executable name
VERSION="v1.0.0" # You might want to dynamically get this from the archive name later
INSTALL_DIR="$HOME/.local/bin" # User-local directory for executables
DESKTOP_ENTRY_DIR="$HOME/.local/share/applications" # User-local desktop entries

ARCHIVE_NAME="$APPLICATION_NAME-$VERSION-linux-x64.tar.gz" # Adjust if your archive naming is different

# --- Script Start ---

echo "Setting up $APPLICATION_NAME..."

# 1. Check if archive exists
if [ ! -f "$ARCHIVE_NAME" ]; then
  echo "Error: Archive '$ARCHIVE_NAME' not found in the current directory."
  echo "Please make sure the setup script and the archive are in the same directory."
  exit 1
fi

# 2. Create install directory if it doesn't exist
mkdir -p "$INSTALL_DIR"

# 3. Extract the archive
echo "Extracting archive..."
tar -xzf "$ARCHIVE_NAME"

# 4. Move the executable to the install directory
echo "Moving executable..."
if [ -f "./$APPLICATION_NAME" ]; then # Assuming executable is extracted directly in current dir
  mv "./$APPLICATION_NAME" "$INSTALL_DIR"
  chmod +x "$INSTALL_DIR/$APPLICATION_NAME" # Make sure it's executable
else
  echo "Error: Executable '$APPLICATION_NAME' not found in the extracted archive."
  echo "Please check the contents of your archive."
  exit 1
fi

# 5. Create desktop entry (for GUI applications)
if which update-desktop-database >/dev/null 2>&1; then # Check if desktop entry tools are available
  echo "Creating desktop entry..."
  mkdir -p "$DESKTOP_ENTRY_DIR"
  DESKTOP_FILE="$DESKTOP_ENTRY_DIR/$APPLICATION_NAME.desktop"

  cat <<EOF > "$DESKTOP_FILE"
[Desktop Entry]
Type=Application
Name=$APPLICATION_NAME
GenericName=Pomodoro Timer # You can customize this
Comment=A simple Pomodoro timer application
Exec=$INSTALL_DIR/$APPLICATION_NAME
Icon= # Path to an icon file if you have one (optional)
Terminal=false # Set to true if it's a terminal application
Categories=Utility; # Or other relevant categories
EOF
  chmod +x "$DESKTOP_FILE" # Make it executable (though not strictly necessary)
  update-desktop-database "$DESKTOP_ENTRY_DIR" # Update desktop database
fi

echo "$APPLICATION_NAME setup complete!"
echo "You can run it from your application menu or by typing '$APPLICATION_NAME' in the terminal."
echo "Executable installed in: $INSTALL_DIR"
echo "Desktop entry created in: $DESKTOP_ENTRY_DIR (if applicable)"

exit 0
