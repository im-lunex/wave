#!/bin/sh
#
# Use this script to run your program LOCALLY.
#
# Note: Changing this script WILL NOT affect how CodeCrafters runs your program.
#
# Learn more: https://codecrafters.io/program-interface

set -e # Exit early if any commands fail

# Copied from .codecrafters/compile.sh
#
# - Edit this to change how your program compiles locally
# - Edit .codecrafters/compile.sh to change how your program compiles remotely
(
  cd "$(dirname "$0")" # Ensure compile steps are run within the repository directory
  # If vcpkg.cmake exists or VCPKG_ROOT is set, use it. Otherwise, build normally.
  if [ -n "$VCPKG_ROOT" ] && [ -f "$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" ]; then
    cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"
  else
    # Try normal build. If it fails due to cache, clean and retry.
    cmake -B build -S . || (rm -rf build && cmake -B build -S .)
  fi
  cmake --build ./build
)

# Run the program
exec "$(dirname "$0")/build/shell" "$@"
