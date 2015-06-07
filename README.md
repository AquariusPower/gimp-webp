## gimp-webp

This plugin provides [Gimp](https://developers.google.com/speed/webp/) with the ability to load and export WebP images. During export, a dialog is presented that provides access to alpha channel and image quality settings. The plugin is designed to run on all platforms currently supported by the Gimp.

### Building

In order to build this plugin, you will need the following tools and libraries installed:

 - [CMake 2.8.9+](http://www.cmake.org/)
 - Gimp 2.6+ development files (`libgimp2.0-dev` on Ubuntu)
 - Webp 0.4+ development files (`libwebp-dev` on Ubuntu)

The build process consists of:

    mkdir build
    cd build
    cmake ..
    make

### Installation

[TODO]
