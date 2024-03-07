![Microchip logo](https://raw.githubusercontent.com/wiki/Microchip-MPLAB-Harmony/Microchip-MPLAB-Harmony.github.io/images/microchip_logo.png)
![Harmony logo small](https://raw.githubusercontent.com/wiki/Microchip-MPLAB-Harmony/Microchip-MPLAB-Harmony.github.io/images/microchip_mplab_harmony_logo_small.png)

# Microchip MPLAB® Harmony 3 Release Notes

## Harmony 3 Smart Energy G3 v1.0.0

### New Features

Smart Energy G3 repository contains the code that implements the G3 Stack as defined in the Standard Specification from the [G3-Alliance](https://g3-alliance.com/)
Provided Modules are:
- G3 libraries for the different G3 Stack layers:
  - ADP library
  - LOADng library
  - PLC MAC library
  - RF MAC library
- MAC-G3-ADP Net interface. Interface between the TCP/IP stack and the G3 Stack.
- APIs and Wrappers to G3 libraries.
- G3 Bootstrap Protocol (LBP) implementation.
- Serialization layers to access both ADP and MAC layers from an external Host.
- PAL (Physical Abstraction Layer) for:
  - PLC. Provides abstraction between G3 PLC MAC and Microchip implementation of G3 PLC Driver.
  - RF. Provides abstraction between G3 RF MAC and Microchip implementation of G3 RF Driver.

### Known Issues

- None

### Development Tools

- [MPLAB® X IDE v6.20](https://www.microchip.com/mplab/mplab-x-ide)
- [MPLAB® XC32 C/C++ Compiler v4.35](https://www.microchip.com/mplab/compilers)
- MPLAB® X IDE plug-ins:
  - MPLAB® Code Configurator 5.5.0 or higher

### Notes

- This is the first Release of the G3 Stack for Microchip MPLAB® Harmony 3 platform.
