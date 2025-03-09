libconops
=========

A simple library to facilitate the implementation of state machines through transition tables.

Building
--------

    Currently, the library supports both CMake and Meson. How to compile the library using them can be seen below.

Meson
~~~~~
    To build the library with Meson, run the following command on the repo's root::

        meson setup build
        meson compile -C build

CMake
~~~~~
    To build the library with CMake, run the following command on the repo's root::

        cmake -S . -B build
        cmake --build build
