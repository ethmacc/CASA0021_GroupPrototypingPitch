# HotStone - a CASA0021: Group Prototype and Pitch project

<img src="https://github.com/user-attachments/assets/9073ca0b-1672-47e7-8c66-f298b7448cce" width="600"/>

*Share the healing sense of touch with loved ones, from anywhere.*

## Table of Contents

- [What is HotStone?](#what-is-hotstone)
- [Getting Started](#getting-started)
    - [Setup](#setup)
    - [Libraries](#libraries-used)
    - [Running the App](#running-the-application)
- [Physical Build](#physical-build)
    - [Materials](#materials)
    - [Enclosure](#enclosure)
    - [Circuit](#circuit)
- [Future Updates](#future-updates)
- [Further Questions](#Further-Questions)
- [Developers](#developers)

## What is HotStone?

HotStone is a prototype device that allows two users to bond over a shared experience of warmth across the distance. It is primarily aimed at users who, for one reason or another, find themselves in a long-distance relationship / friendship with another person or family member. Utilizing Internet-of-Things (IoT) technology, it is possible to create an interaction where one user putting their hand on their device creates a warm sensation for the other person when they touch the other paired device and vice versa.

There have been a number of attempts to create devices that connect people over the miles, such as [Frebble by Frederic Petrignani](https://www.kickstarter.com/projects/396691740/hold-hands-online-when-you-miss-someone). However, we believe we are the first to successfully demonstrate a prototype device that is able to transfer warmth to another user over the internet.

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

- [Adafruit NeoPixel](https://docs.arduino.cc/libraries/adafruit-neopixel/): to Adjust colors
- [Capacitive Sensor](https://docs.arduino.cc/libraries/capacitivesensor/): to Register a Users Touch

### Running the Application

Once the user has saved the MAC Address and capacitive touch threshold value, flash the code to the microcontroller in PlatformIO.

## Physical build

### Materials

You will need the following materials for this build:
- A 3D printer and filament
- Protoboard or stripboard for the circuit prototype build
- Wire
- FQP30N06L MOSFET
- 220K Ohm resistor
- Aluminium foil
- 8-LED Neopixel LED strip
- USB male cable
- Node MCU
- Pin headers (male and female)
- Heating pads, available on Amazon from multiple sources

### Enclosure

<img src="https://github.com/user-attachments/assets/bb88337b-866e-4be9-89dd-9279ab106c00" width="600" />

The 3D models for the enclosure can be found in ```Modeling/3D_models```. Use these to print out a copy of the enclosure to house the electronics in. We recommend printing the top shell of the enclosure in a solid material and the bottom part in translucent to allow the LED light to diffuse out.

You can adapt the 3D models to fit any protoboard size and configuration, but for the easiest solution, we recommend taking dimensions from the dummy protoboard in the 3D model files.

### Circuit

The circuit diagram for the device is shown below:

![thumbnail_image (1)](https://github.com/user-attachments/assets/3e6a947c-aa7c-4336-838b-0b1f66a238b3)

As well as the schematic for the layout of the protoboard:

![thumbnail_image](https://github.com/user-attachments/assets/236896b1-2056-4afd-a0a5-91ffbce2017c)

Using these, it is possible to reconstruct the protoboard arrangement shown below, that fits into the device compactly:

<img src="https://github.com/user-attachments/assets/e201b6ef-1a8e-4f32-85f7-3a14473ebf89" width="600"/>

<img src="https://github.com/user-attachments/assets/8312087c-e96c-4603-b33b-90e838355cfa" width="600" />

Note that the IMU was replaced by a capative sensor for a more natural human interaction, using the [Capacitive Sensor arduino library](https://docs.arduino.cc/libraries/capacitivesensor/):

<img src="https://github.com/user-attachments/assets/5de029b1-a5ec-4dd4-ae9e-6868a2725343" width="600" />

The main code in ```software/hotstone/src/main.cpp``` describes which pins the capacitive sensor should be soldered to. A 220k Ohm resistor was used for this sub-circuit (tested with an ESP8266) and aluminium foil was used for the sesnor plate, but these can be adjusted to your liking:

<img src="https://github.com/user-attachments/assets/16a029a0-b9c3-43cc-ba8d-eea6cc87670d" width="600" />

More information on capacitive sensor circuits can be found on the web, such as [this Instructables link](https://www.instructables.com/How-To-Use-Touch-Sensors-With-Arduino/)

## Future Updates

- Further study into material selection
- Further study into NiChrome Wire as a heating element
- Convert prototype board to PCB
- Convert ESP-NOW to WiFi
- Add QR Code or NFC tag to link users to a page to add WiFi details or add multiple devices to the network

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
