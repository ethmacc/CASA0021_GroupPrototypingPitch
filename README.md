# Hot Stone

## CASA 0021: Group Prototype and Pitch

*Share the healing sense of touch with loved ones, from anywhere.*

## Table of Contents

- [What is HotStone?](#what-is-hotstone)
- [Getting Started](#getting-started)
    - [Setup](#setup)
    - [Libraries](#libraries-used)
    - [Running the App](#running-the-application)
- [Further Questions](#Further-Questions)
- [Developers](#developers)


## What is HotStone?

HotStone is an internet-connected pair of smart heating devices designed to help people who are physically apart, feel emotionally connected.

Inspired by the intimate gesture of holding hands, the device allows the transfer of physical warmth as a means of subtle, non-verbal communication over the miles. When one person places their hand on the device, their partner’s paired device receives the visual cue of a pulsing light and starts to warm up. When their partner then places their hand on their own device, the first device also gently warms up, creating a shared experience.

## Getting Started

### Setup
To reproduce this project, users will need to first set up [PlatformIO on VS Code](https://platformio.org/install/ide?install=vscode). Once the user has the software directory on their local machine, and opened on PlatformIO, the register, and calibration process can begin. Within the `include` directory, create a file named `macAddr.h` this file can maintain the unique MAC addresses of the partner devices, as well as the unique threshold value for the capacitive sensor. This file will require the following format:
```
#pragma once

#include <stdint.h>

#define DEVICE_B

#ifdef DEVICE_A   // Device B Address
   static const uint8_t MAC_ADDR_LIST[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
   static const uint8_t DEVICE_THRESHOLD = xxx;

#endif
#ifdef DEVICE_B   // Device A Address
   static const uint8_t MAC_ADDR_LIST[] = {0x07, 0x08, 0x09, 0x10, 0x11, 0x12};
   static const uint8_t DEVICE_THRESHOLD = xxx;
#endif

```

For each device the user will need to find their unique MAC Address and replace the values within the above arrays. This can be done by running using the following script on your device:

```
uint8_t baseMac[6];
esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, baseMac);
if (ret == ESP_OK) {
  Serial.printf("%02x:%02x:%02x:%02x:%02x:%02x\n",
                baseMac[0], baseMac[1], baseMac[2],
                baseMac[3], baseMac[4], baseMac[5]);
} else {
  Serial.println("Failed to read MAC address");
}
``` 

Once the MAC addresses have been set, we can set the threshold values. With the device plugged into a computer, open the serial port monitor, and place your hand on the device. The serial port should print out the capacitive values. Select a sensible value between the capacitive values when the hand is not on the device, as opposed to the values after the hand has been placed on the device. This value will replace the `xxx` within the `macAddr.h` file. With this code flashed on both devices, the user is ready to use the device normally.

### Libraries Used

- Adafruit NeoPixel: to Adjust colors
- Capacitive Sense: to Register a Users Touch

### Running the Application

Once the user has saved the MAC Address and capacitive touch threshold value, flash the code to the microcontroller in PlatformIO.

## Further Questions

Please reach out to the developers of this project, or add an issue to this repo.

## Developers

### Prototype and Software
Vineeth Kirandumkara: [@vineethk96](https://github.com/vineethk96)

### Product Design
Ethan Low: [@ethmacc](https://github.com/ethmacc)

### PCB Design
Donghao Zhang: [@wudaozhiying1](https://github.com/wudaozhiying1)

### Media
Yue Zhu: [@XLunaXX07](https://github.com/XLunaXX07)