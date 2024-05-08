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

- simple-presence-service

## Build in docker 

```
docker run -itv /home/andrei/src:/home/andrei/src lora bash

cd /home/andrei/src/simple-presence-service
mkdir -p build
cd /home/andrei/src/simple-presence-service/build
rm *;rm -r CMakeFiles/
cmake ..
make
./simple-presence-service -?
```

### Commit docker

```
docker ps -a
docker commit stoic_ramanujan
docker images
docker tag c30cb68a6443 lora
# Remove closed containers
docker rm $(docker ps -qa --no-trunc --filter "status=exited")
```

### Deploy

#### Prepare destination directory on target machine

Before first deploy create directory:

```shell
ssh lora.commandus.com
mkdip -p ~/simple-presence-service
```

Stop service first if already running:

```
ssh lora.commandus.com
cd ~/simple-presence-service
pkill simple-presence-service
```

#### Strip and deploy

```
cd /home/andrei/src/simple-presence-service/build
sudo chown andrei:andrei *
strip simple-presence-client simple-presence-service
scp simple-presence-client simple-presence-service andrei@lora.commandus.com:~/simple-presence-service/
```

Install dependencies:
```
sudo apt install libuv1
```

#### Run service

```
cd ~/simple-presence-service
./simple-presence-service -d
```

or run as daemon using systemd.

First copy scripts to the target host:

```
scp -r systemd/ andrei@lora.commandus.com:~/simple-presence-service/
```

then run copy.sh script:
```
ssh lora.commandus.com
cd ~/simple-presence-service
cd systemd
sudo ./copy.sh

sudo systemctl start simple-presence.service
systemctl status simple-presence.service
sudo systemctl daemon-reload
```
