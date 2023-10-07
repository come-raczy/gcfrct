# gcfrct
Guided Collimation For Ritchey-Chretien Telescopes

# Installation


## Requirements

This is a GTK4 application, using gtkmm, and CMake as a build system. The
following packages are required:

* CMake >= 3.18
* g++ >=10.1.0
* gtkmm >= 4.0
* gtest >= 1.12
* libxml2-utils >= 2.9

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

Optionally, run the tests after building (from the build directory):

    ctest

Note: when a conda environment is activated, CMake will select the version of
gtest from conda. This will fail because of typeinfo configuration. It is
essential to deactivate the conda environment before executing CMake so that
the correct version of gtest is selected.

## Install

Simply copy the resuting executable `gcfrct` in the destination directory.
The executable bundles all resources required to run the application.

# Usage

TBD

