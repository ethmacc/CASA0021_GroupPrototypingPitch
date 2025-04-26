# HotStone - a CASA0021: Group Prototype and Pitch project

<img src="https://github.com/user-attachments/assets/9073ca0b-1672-47e7-8c66-f298b7448cce" width="600"/>

*Share the healing sense of touch with loved ones, from anywhere.*

## Table of Contents

- [Abstract](#Abstract)
- [Reasoning](#Reasoning)
- [How to Use](#how-to-use)
- [Design Journey](#how-to-use)
    - [Physical Design](#physical-design)
    - [Electrical Prototyping](#electrical-prototyping)
    - [Custom PCB Design](#custom-pcb-design)
- [Lessons Learned](#lessons-learned)
- [Future Additions](#future-additions)
- [Support](#support)
- [Appendix](#appendix)

## Abstract
HotStone is a prototype device that allows two users to bond over a shared experience of warmth across the distance. It is primarily aimed at users who, for one reason or another, find themselves in a long-distance relationship / friendship with another person or family member. Utilizing Internet-of-Things (IoT) technology, it is possible to create an interaction where one user putting their hand on their device creates a warm sensation for the other person when they touch the other paired device and vice versa.

## Reasoning

There have been a number of attempts to create devices that connect people over the miles, such as Frebble by Frederic Petrignani (https://www.kickstarter.com/projects/396691740/hold-hands-online-when-you-miss-someone). However, we believe we are the first to successfully demonstrate a prototype device that is able to transfer warmth to another user over the internet.

Link it here

## How to Use

## Design Journey

### Physical Design

#### Sphere

#### Travel Mug

#### A Study in Form

### Software and Electrical Prototyping

Basic Component Selection

How did we select these items, flow off what Ethan adds for his physical design parts.

Add research on how to create heat, pumping current through the 100 Ohm resistors

Screenshots of the math.
What is the required current and voltage to heat the resistors

Initial attempts to power through the MCU

Switching to the MOSFET

Document the MOSFET issues. Specifically what happned when Vg > Vgs.
add the photos from the chat history with Ethan.

Walk through the usage of Nichrome Wire as a heating element

The switch to the heating pads to simplify the device.

Pivot to Software

What libraries are we using
copy the docs for Platform.IO usage to the appendix and reference it

Talk about ESPNow, and briefly explain the original idea for using a mobile app like the competitor.

MAC address issues can be referenced within lessons learned.

Add a refined diagram of the FSM

Explain the logic of the heating timers and such

Add a brief about how the software was originally split up


### Custom PCB Design

## Lessons Learned

- PLA is pretty decent for conducting heat
- Where to place the load in a circuit, as opposed to the gate voltage from a MOSFET
- Ergonomics of the hand
- How to create heat through resistance
- NiChrome Wire
- Capacitive Touch Sensors
- Using metallic silver to cut out light

## Future Additions

- Look into different materials
    - Ceramics?
    - Metals?
    - Stones?
- Move away from heat pad to Nichrome again
    - Embeddeding Nichrome into a ceramic
- In-built battery vs external power source
- Switch to Wi-Fi or BLE to expand the range of connectivity
    - Wi-Fi means create a web portal accessible by QR code to connect to local networks.
    - BLE means creating a phone app to connect and communicate over distances.
- More testing into the form design
    - Smaller design? More Palm-able?
- Custom PCB with a smaller form factor
    - Integrate LEDs into the design.
    - Better Diffusion of color?
- Multi-color support?
- Research into alternatives from aluminum foil.
    - Double the Nichrome wire as the capacitor?


## Support

## Appendix

### Heating Element Research

### Circuit Analysis

### MOSFET
