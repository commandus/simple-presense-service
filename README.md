# Simple presense service

## Requisites

### Linux

- CMake or Autotools/Automake
- C++ compiler and development tools bundle

### Windows

- CMake
- Visual Studio
- vcpkg

### ESP32

- SDK idf tools or
- Visual Studio Code with Espressif IDF plugin

#### Tools

Make sure you have automake or CMake installed:

```
apt install autoconf libtool build-essential 
```

or 

```
apt install cmake build-essential 
```

## Build

- autotools
- CMake

### Autotools

First install dependencies (see below) and then configure and make project using Autotools:

```
cd simple-presense-service
./autogen.sh
./configure
make
```

### CMake

Configure by default: 
```
cd simple-presense-service
mkdir build
cd build
cmake ..
make
```

#### clang instead of gcc

Export CC and CXX environment variables points to specific compiler binaries.
For instance, you can use Clang instead of gcc:

```
cd simple-presense-service
mkdir build
cd build
export CC=/usr/bin/clang;export CXX=/usr/bin/clang++;cmake ..
make
```

### Windows

- Visual Studio with C++ profile installed
- CMake
- vcpkg

### ESP32

SDK

Install:
```
cd ~/esp/esp-idf
./install.sh
```

Set up environment
```
cd ~/esp/esp-idf
. ./export.sh
```

Build

In "idf.py menuconfig" step go to 

- Compiler options, Optimization Level, select "Optimize for size (-Os)"
- Component config, Log output, Default log verbosity, Select "No output"
- Component config, ESP-MQTT Configurations, disable all

Save sdkconfig file and quit.

```
cd ~/src/simple-presense-service
idf_get
idf.py fullclean
idf.py menuconfig
idf.py build
...
```

Flash
```
idf.py flash -p /dev/ttyUSB0
```

Visual Studio Code
- Press F1; select ESP-IDF: Set Espressif device target; select lorawan-storage; select ESP32; select ESP32 chip (via ESP USB bridge)
- Press F1; select ESP-IDF: Build your project
- Press F1; select ESP-IDF: Flush your project

### Dependencies

- libuv

```
sudo apt install libuv1-dev
```

## Usage

Executables:

- simple-presense-service
