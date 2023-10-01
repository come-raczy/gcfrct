# gcfrct
Guided Collimation For Ritchey-Chretien Telescopes

# Installation


## Requirements

This is a GTK4 application, using gtkmm, and CMake as a build system. The
following packages are required:

* CMake >= 3.18
* g++ >=10.1.0
* gtkmm >= 4.0

Note: on WSL, the following is needed:

    export $(dbus-launch)

Otherwise, there will be a Gtk Warning:

    Unable to acquire session bus: Error spawning command line “dbus-launch --autolaunch=2186e8f014214f2daf4116d80067e609 --binary-syntax --close-stderr”: Child process exited with code 1

## Build

From the source directory:

    mkdir build
    cd build
    cmake ..
    make

## Install

Simply copy the resuting executable `gcfrct` in the destination directory.
The executable bundles all resources required to run the application.

# Usage

TBD

